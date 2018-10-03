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
#include "rotation.h"
#include <QDebug>

Rotation::Rotation(QObject * parent) : BaseFilter(parent)
{
    widget = new RotationWidget();
    filterWidget = widget;
    connect(widget, SIGNAL(parameterChanged()), this, SLOT(widgetParameterChanged()));
    if (parent) {
        /* Connect slots to the filtercontainer */
        connect(parent, SIGNAL(backgroundColorChanged(QColor)),
                widget, SLOT(setBackgroundColor(QColor)));
    }

    // TODO: Connect seems only to work when applied to the inherited classes
    // I would have love to connect one for all in Basefilter...
    connect(widget, SIGNAL(enableFilterToggled(bool)),
            this, SLOT(enableFilterToggled(bool)));
    connect(widget, SIGNAL(previewChecked()),
            this, SLOT(previewChecked()));
}

/** \brief Returns a universal name for this filter.

 This identifier is unique for the filter. It can be used to identify the
 filter used (for example in configuration files)
 */
QString Rotation::getIdentifier()
{
    return QString("Rotation");
}

/** \brief Returns the localised name of this filter
 */
QString Rotation::getName()
{
    return tr("Rotation");
}

QImage Rotation::filter(QImage inputImage)
{
    if (filterEnabled) {
        rotationMatrix.reset();
        rotationMatrix.rotate(widget->rotation());
        return inputImage.transformed(rotationMatrix);
    } else {
        return inputImage;
    }
}

// Return the settings of the filter: Rotation Angle in Degrees and Enable Checkbox
QMap<QString, QVariant> Rotation::getSettings()
{
    QMap<QString, QVariant> settings;

    settings["rotation"] = widget->rotation();
    settings["enabled"] = filterEnabled;

    return settings;
}

/** \brief Set filter settings

  The only relevant Setting for the Filter is its rotation angle.
  todo: One may want to also save the preview checkbox.
  */
void Rotation::setSettings(QMap<QString, QVariant> settings)
{
    loadingSettings = true;

    if (settings.contains("rotation"))
        widget->setRotation(settings["rotation"].toInt());
    else
        widget->setRotation(0);

    if (settings.contains("enabled"))
        enableFilter(settings["enabled"].toBool());
    else
        enableFilter("true");

    mustRecalculate = true;
    loadingSettings = false;
    emit parameterChanged();
}

void Rotation::settings2Dom(QDomDocument &doc, QDomElement &parent, QMap<QString, QVariant> settings)
{
    QDomElement filter = doc.createElement(getIdentifier());
    parent.appendChild(filter);
    if (settings.contains("rotation"))
        filter.setAttribute("angle", settings["rotation"].toInt());
    else
        filter.setAttribute("angle", 0);

    if (settings.contains("enabled"))
        filter.setAttribute("enabled", settings["enabled"].toBool());
    else
        filter.setAttribute("enabled", true);
}

QMap<QString, QVariant> Rotation::dom2Settings(QDomElement &filterElement)
{
    QMap<QString, QVariant> settings;

    settings["rotation"] = filterElement.attribute("angle", "0").toInt();
    settings["enabled"] = filterElement.attribute("enabled", "1").toInt();

    return settings;
}

