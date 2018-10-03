/*
 * Copyright (C) 2012-2014 Robert Chéramy (robert@cheramy.net)
 *
 * This file is part of YASW (Yet Another Scan Wizard).
 *
 * YASW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YASW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YASW.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "filtercontainer.h"
#include "rotation.h"
#include "dekeystoning.h"
#include "cropping.h"
#include "scalefilter.h"
#include "colorcorrection.h"
#include "layoutfilter.h"

#include <QPrinter>
#include <QDebug>

/** \class FilterContainer
    \brief A customised QTabWidget to display the different filters.

    FilterContainer initialises each Filter.

    As QTabWidget::currentWidget only returns a QWidget and we need a FilterWidget and its virtual functions,
    we keep a Map between tabs indexes and FilterWidgets in tabToWidget.
    */


FilterContainer::FilterContainer( QWidget * parent)
    : QTabWidget(parent)
{
    // initialise the filters
    Rotation *rotationFilter = new Rotation(this);
    tabToFilter.append(rotationFilter);
    addTab(rotationFilter->getWidget(), rotationFilter->getName());

    Dekeystoning *dekeystoningFilter = new Dekeystoning(this);
    tabToFilter.append(dekeystoningFilter);
    addTab(dekeystoningFilter->getWidget(), dekeystoningFilter->getName());
    /* connect the filter to previous filter so it gets changes automaticaly */
    connect(rotationFilter, SIGNAL(parameterChanged()),
            dekeystoningFilter, SLOT(inputImageChanged()));
    dekeystoningFilter->setPreviousFilter(rotationFilter);

    Cropping *croppingFilter = new Cropping(this);
    tabToFilter.append(croppingFilter);
    addTab(croppingFilter->getWidget(), croppingFilter->getName());
    /* connect the filter to previous filter so it gets changes automaticaly */
    connect(dekeystoningFilter, SIGNAL(parameterChanged()),
            croppingFilter, SLOT(inputImageChanged()));
    croppingFilter->setPreviousFilter(dekeystoningFilter);

    ScaleFilter *scaleFilter = new ScaleFilter(this);
    tabToFilter.append(scaleFilter);
    addTab(scaleFilter->getWidget(), scaleFilter->getName());
    /* connect the filter to previous filter so it gets changes automaticaly */
    scaleFilter->setPreviousFilter(croppingFilter);
    connect(croppingFilter, SIGNAL(parameterChanged()),
            scaleFilter, SLOT(inputImageChanged()));

    LayoutFilter *layoutFilter = new LayoutFilter(this);
    tabToFilter.append(layoutFilter);
    addTab(layoutFilter->getWidget(), layoutFilter->getName());
    /* connect the filter to previous filter so it gets changes automaticaly */
    layoutFilter->setPreviousFilter(scaleFilter);
    connect(scaleFilter, SIGNAL(parameterChanged()),
            layoutFilter, SLOT(inputImageChanged()));

// Deactivation Color Corection for release 0.6: this ist not good enought for a release.
//    ColorCorrection *colorCorrection = new ColorCorrection(this);
//    tabToFilter.append(colorCorrection);
//    addTab(colorCorrection->getWidget(), colorCorrection->getName());
//    /* connect the filter to previous filter so it gets changes automaticaly */
//    colorCorrection->setPreviousFilter(layoutFilter);
//    connect(layoutFilter, SIGNAL(parameterChanged()),
//            colorCorrection, SLOT(inputImageChanged()));

    // get informed when a tab changed
    connect(this, SIGNAL(currentChanged(int)),
            this, SLOT(tabChanged(int)));
}

FilterContainer::~FilterContainer()
{
    int index;
    for (index = 0; index < tabToFilter.size(); index++) {
        delete tabToFilter[index];
    }
    tabToFilter.clear();
}

/* Sets the image to be worked on. */
void FilterContainer::setImage(QPixmap pixmap)
{
    // Settings the image on the fist filter results in recalculating the image for all filters,
    // as the each filter emits a parameterChanged signal, which is recieved by the next filter.
    tabToFilter[0]->setImage(pixmap);

    int currentTab = std::min (tabToFilter.size(), currentIndex());
    tabToFilter[currentTab]->refresh();
}

void FilterContainer::setSelectionColor(QColor color)
{
    emit(selectionColorChanged(color));
}

void FilterContainer::setBackgroundColor(QColor color)
{
    emit(backgroundColorChanged(color));
}

void FilterContainer::setDisplayUnit(QString unit)
{
    emit(displayUnitChanged(unit));
}

void FilterContainer::setDPI(int dpi)
{
    emit(dpiChanged(dpi));
}

void FilterContainer::tabChanged(int index)
{
    int currentTab = std::min (tabToFilter.size(), currentIndex());
    tabToFilter[currentTab]->refresh();
    oldIndex = index;
}

/*! \brief Get settings from the filters.

  Each Filter.getSettings will return its settings in a QMap<QString, QVariant>, which will be packed in another
  QMap (its type is QMap<QString, QMap<QString, QVariant> > ). The String Key is the value of Filter.getName().
  */
QMap<QString, QVariant> FilterContainer::getSettings()
{
    /* QVariant here is a QMap<QString, QVariant> */
    QMap<QString, QVariant> allSettings;
    BaseFilter *filter;


    foreach (filter, tabToFilter) {
        allSettings[filter->getIdentifier()] = filter->getSettings();
    }

    return allSettings;
}

/*! \brief Set settings to the filters.

    See also FilterContainer::getSettings
  */
void FilterContainer::setSettings(QMap<QString, QVariant> settings)
{
    QString filterName;
    BaseFilter *filter;
    bool filterSet;

    foreach (filter, tabToFilter) {
        filterSet = false;
        foreach (filterName, settings.keys()) {
            if (filterName == filter->getIdentifier()) {
                filter->setSettings(settings[filterName].toMap());
                filterSet = true;
            }
        }
        if (filterSet == false) {
            /* we did not found settings for this filter, reset settings */
            filter->setSettings(QMap<QString, QVariant>());
        }
    }

    int currentTab = std::min (tabToFilter.size(), currentIndex());
    tabToFilter[currentTab]->refresh();
}

/* Fills the parent with filter DomEmelents and ther parameters
   We have to provide the parameters as they are stored in the calling Class (ImageTableWidget)
   This function is called when serializing the settings into an XML file.
 */
void FilterContainer::settings2Dom(QDomDocument &doc, QDomElement &imageElement, QMap<QString, QVariant> settings)
{
    BaseFilter *filter;

    foreach (filter, tabToFilter) {
        if (settings.contains(filter->getIdentifier())) {
            filter->settings2Dom(doc, imageElement, settings[filter->getIdentifier()].toMap());
        }
    }
}

// Transforms the subtags from <image> into filter settings QMap.
// NOTE: It might be interesting to get ride of the QMap and only use DOM for the whole Settings.
QMap<QString, QVariant> FilterContainer::dom2Settings(QDomElement &imageElement)
{
    QDomElement filterElement;
    BaseFilter *filter;
    QMap<QString, QVariant> settings;

    filterElement = imageElement.firstChildElement();

    while (!filterElement.isNull()) {
        foreach (filter, tabToFilter) {
            if (filterElement.tagName() == filter->getIdentifier()) {
                settings[filter->getIdentifier()] = filter->dom2Settings(filterElement);
            }
        }
        filterElement = filterElement.nextSiblingElement();
    }
    return settings;
}

/** \brief Compute and return the resulting image above all filter
 */
QPixmap FilterContainer::getResultImage()
{
    int maxTab = tabToFilter.size() - 1;

    return tabToFilter[maxTab]->getOutputImage();
}

/** \brief Returns the identifiert of the current filter */
QString FilterContainer::currentFilter()
{
    return tabToFilter[currentIndex()]->getIdentifier();
}

