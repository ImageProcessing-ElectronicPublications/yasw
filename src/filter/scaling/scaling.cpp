/*
 * Copyright (C) 2012 Robert Chéramy (robert@cheramy.net)
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
#include "scaling.h"
#include <QDebug>

Scaling::Scaling(QObject * parent) : BaseFilter(parent)
{
    widget = new ScalingWidget();
    filterWidget = widget;

    //TODO: connect items changed to recalculate
    connect(widget, SIGNAL(signalPropertyChanged()),
            this, SLOT(recalculate()));

    if (parent) {
        /* Connect slots to the filtercontainer */
        connect(parent, SIGNAL(backgroundColorChanged(QColor)),
                widget, SLOT(setBackgroundColor(QColor)));
    }
}

AbstractFilterWidget *Scaling::getWidget()
{
    return widget;
}

QString Scaling::getIdentifier()
{
    return QString("scaling");
}

QString Scaling::getName()
{
    return tr("Scaling");
}

/** \brief Gets the settings of the filter.

  As all the settings are maintained in the widget, this function just gets the setting from
  the widget and gives them back.
*/
QMap<QString, QVariant> Scaling::getSettings()
{
    return widget->getSettings();
}

/** \brief Sets the settings for the filter.

  Gives the settings to the widget (were they are stored) and recalculate the produced Image.
 */

void Scaling::setSettings(QMap<QString, QVariant> settings)
{
    widget->setSettings(settings);

    recalculate();
}

void Scaling::recalculate()
{
    QSize outputSize = QSize(widget->imageWidth(),widget->imageHeight());
    outputPixmap = inputPixmap.scaled(outputSize);
    widget->setPreview(outputPixmap);
}
