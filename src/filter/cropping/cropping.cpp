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
#include "cropping.h"

Cropping::Cropping(QObject *parent)
{
    widget = new CroppingWidget();
    filterWidget = widget;
    connect(widget, SIGNAL(parameterChanged()), this, SLOT(widgetParameterChanged()));

    if (parent) {
        /* Connect slots to the filtercontainer */
        connect(parent, SIGNAL(selectionColorChanged(QColor)),
                widget, SLOT(setSelectionColor(QColor)));
        connect(parent, SIGNAL(backgroundColorChanged(QColor)),
                widget, SLOT(setBackgroundColor(QColor)));
    }

    // Connect seems only to work when applied to the inherited classes
    // I would have love to connect one for all in Basefilter...
    connect(widget, SIGNAL(enableFilterToggled(bool)),
            this, SLOT(enableFilterToggled(bool)));
    connect(widget, SIGNAL(previewChecked()),
            this, SLOT(previewChecked()));
}

QImage Cropping::filter(QImage inputImage)
{
    if (filterEnabled) {
        QRect rectangle = widget->rectangle();

        return inputImage.copy(rectangle);
    } else {
        return inputImage;
    }
}

/** \brief Returns a universal name for this filter.

 This identifier is unique for the filter. It can be used to identify the
 filter used (for example in configuration files)
 */
QString Cropping::getIdentifier()
{
    return QString("Cropping");
}

/** \brief Returns the localised name of this Filter */
QString Cropping::getName()
{
    return tr("Cropping");
}

/** \brief Gets the settings from this filter
*/
QMap<QString, QVariant> Cropping::getSettings()
{
    QMap<QString, QVariant> settings = widget->getSettings();
    settings["enabled"] = filterEnabled;

    return settings;
}

/** \brief set this filter's settings
 **/
void Cropping::setSettings(QMap<QString, QVariant> settings)
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

void Cropping::settings2Dom(QDomDocument &doc, QDomElement &imageElement, QMap<QString, QVariant> settings)
{
    QDomElement filter = doc.createElement(getIdentifier());
    imageElement.appendChild(filter);

    QDomElement pointElement;
    QPointF point;

    QString corner;
    QStringList cornerNames;
    int i;

    // Iterate through cornerNames to save all Positions
    cornerNames << "bottomRightCorner" << "topLeftCorner";
    for (i = 0; i < cornerNames.size(); i++) {
        corner = cornerNames.at(i);
        if (settings.contains(corner)) {
            pointElement = doc.createElement(corner);
            point = settings[corner].toPoint();
            pointElement.setAttribute("x", point.x());
            pointElement.setAttribute("y", point.y());
            filter.appendChild(pointElement);
        }
    }

    if (settings.contains("enabled"))
        filter.setAttribute("enabled", settings["enabled"].toBool());
    else
        filter.setAttribute("enabled", true);
}

QMap<QString, QVariant> Cropping::dom2Settings(QDomElement &filterElement)
{
    QMap<QString, QVariant> settings;
    QStringList cornerNames;
    QString corner;
    int i;
    QDomElement cornerElement;

    // Iterate through cornerNames to save all Positions
    cornerNames << "bottomRightCorner" << "topLeftCorner";

    for (i = 0; i < cornerNames.size(); i++) {
        corner = cornerNames.at(i);

        cornerElement = filterElement.firstChildElement(corner);

        if (!cornerElement.isNull()) {
            settings[corner] = QPointF(cornerElement.attribute("x").toDouble(),
                                       cornerElement.attribute("y").toDouble());
        }
    }
    settings["enabled"] = filterElement.attribute("enabled", "1").toInt();

    return settings;
}



