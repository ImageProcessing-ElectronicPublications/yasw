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

#include "layoutfilter.h"
#include "constants.h"

#include <QDebug>
#include <QPainter>

LayoutFilter::LayoutFilter(QObject * parent) : BaseFilter(parent)
{
    widget = new LayoutWidget();
    filterWidget = widget;

    connect(widget, SIGNAL(parameterChanged()),
            this, SLOT(widgetParameterChanged()));

    if (parent) {
        /* Connect slots to the filtercontainer */
        connect(parent, SIGNAL(backgroundColorChanged(QColor)),
                widget, SLOT(setBackgroundColor(QColor)));
        connect(parent, SIGNAL(displayUnitChanged(QString)),
                this, SLOT(setDisplayUnit(QString)));
        connect(parent, SIGNAL(dpiChanged(int)),
                widget, SLOT(setDPI(int)));
    }

    // Connect seems only to work when applied to the inherited classes
    // I would have love to connect one for all in Basefilter...
    connect(widget, SIGNAL(enableFilterToggled(bool)),
            this, SLOT(enableFilterToggled(bool)));
    connect(widget, SIGNAL(previewChecked()),
            this, SLOT(previewChecked()));

}

QString LayoutFilter::getIdentifier()
{
    return QString("LayoutFilter");
}

QString LayoutFilter::getName()
{
    return tr("Layout");
}

/** \brief Gets the settings of the filter.

  As all the settings are maintained in the widget, this function just gets the setting from
  the widget and gives them back.
*/
QMap<QString, QVariant> LayoutFilter::getSettings()
{
    QMap<QString, QVariant> settings = widget->getSettings();
    settings["enabled"] = filterEnabled;

    return settings;
}

/** \brief Sets the settings for the filter.

  Gives the settings to the widget (were they are stored) and recalculate the produced Image.
 */

void LayoutFilter::setSettings(QMap<QString, QVariant> settings)
{
    loadingSettings = true;

    widget->setSettings(settings);
    if (settings.contains("enabled"))
        enableFilter(settings["enabled"].toBool());
    else
        enableFilter("true");

    mustRecalculate = true;
    loadingSettings = false;

    emit parameterChanged();
}

void LayoutFilter::settings2Dom(QDomDocument &doc, QDomElement &parent, QMap<QString, QVariant> settings)
{
    QDomElement filter = doc.createElement(getIdentifier());
    parent.appendChild(filter);

    QString attribute;
    QStringList attributeNames;
    int i;

    // Iterate through attributeNames to save all double
    attributeNames << "pxPageWidth" << "pxPageHeight";

    for (i = 0; i < attributeNames.size(); i++) {
        attribute = attributeNames.at(i);
        if (settings.contains(attribute)) {
            filter.setAttribute(attribute, Constants::float2String(settings[attribute].toDouble()));
        }
    }

    // Iterate through attributeNames to save all strings
    attributeNames.clear();
    attributeNames << "horizontalAlignement" << "verticalAlignement";

    for (i = 0; i < attributeNames.size(); i++) {
        attribute = attributeNames.at(i);
        if (settings.contains(attribute)) {
            filter.setAttribute(attribute, settings[attribute].toString());
        }
    }

    if (settings.contains("enabled"))
        filter.setAttribute("enabled", settings["enabled"].toBool());
    else
        filter.setAttribute("enabled", true);
}

QMap<QString, QVariant> LayoutFilter::dom2Settings(QDomElement &filterElement)
{
    QMap<QString, QVariant> settings;
    QString attribute;
    QStringList attributeNames;
    int i;
    QDomElement cornerElement;

    // Iterate through attributeNames to save all double
    attributeNames << "pxPageWidth" << "pxPageHeight";

    for (i = 0; i < attributeNames.size(); i++) {
        attribute = attributeNames.at(i);
        if (filterElement.hasAttribute(attribute)) {
            settings[attribute] = filterElement.attribute(attribute).toDouble();
        }
    }

    // Iterate through attributeNames to save all strings
    attributeNames << "horizontalAlignement" << "verticalAlignement";

    for (i = 0; i < attributeNames.size(); i++) {
        attribute = attributeNames.at(i);
        if (filterElement.hasAttribute(attribute)) {
            settings[attribute] = filterElement.attribute(attribute);
        }
    }

    settings["enabled"] = filterElement.attribute("enabled", "1").toInt();

    return settings;
}

void LayoutFilter::setDisplayUnit(QString unit)
{
    widget->setDisplayUnit(unit);
}

QImage LayoutFilter::filter(QImage inputImage)
{
    if (!filterEnabled)
        return inputImage;

    qreal pageWidth = widget->pagePixelWidth();
    qreal pageHeight = widget->pagePixelHeight();

    if (pageWidth == 0 || pageHeight == 0 || inputPixmap.isNull()) {
        return QImage();
    }

    qreal imageWidth = inputImage.width();
    qreal imageHeight = inputImage.height();

    qreal leftMargin = 0;
    qreal topMargin = 0;
    QString horizontalAlignement = widget->horizontalAlignement();
    QString verticalAlignement = widget->verticalAlignement();

    // indexOf returns -1 if the alignement is unknown. In this case, margin = 0;
    switch (Constants::horizontalAlignment.indexOf(horizontalAlignement)) {
    case Constants::LeftHAlignment:
        leftMargin = 0;
        break;
    case Constants::CenterHAlignment:
        leftMargin = qMax((pageWidth - imageWidth) / 2, 0.0);
        break;
    case Constants::RightHAlignment:
        leftMargin = qMax(pageWidth - imageWidth, 0.0);
        break;
    }
    switch (Constants::verticalAlignment.indexOf(verticalAlignement)) {
    case Constants::TopVAlignment:
        topMargin = 0;
        break;
    case Constants::CenterVAlignment:
        topMargin = qMax((pageHeight - imageHeight) / 2, 0.0);
        break;
    case Constants::BottomVAlignment:
        topMargin = qMax(pageHeight - imageHeight, 0.0);
        break;
    }

    QImage page = QImage(pageWidth, pageHeight, QImage::Format_ARGB32_Premultiplied);
    //NOTE: fill color could be a parameter. I do wait for user feedback ;-)
    page.fill(Qt::white);
    QPainter painter(&page);
    painter.drawImage(leftMargin, topMargin, inputImage);
    return page;
}
