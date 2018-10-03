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
#include "dekeystoning.h"
#include <QDebug>
#include <QColor>

Dekeystoning::Dekeystoning(QObject *parent)
{
    widget = new DekeystoningWidget();
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

/** \brief Returns a universal name for this filter.

 This identifier is unique for the filter. It can be used to identify the
 filter used (for example in configuration files)
 */
QString Dekeystoning::getIdentifier()
{
    return QString("Dekeystoning");
}

/*! \brief Retruns the localised name of the filter */
QString Dekeystoning::getName()
{
    return tr("Dekeystoning");
}

/** \brief Gets the settings from this filter
*/
QMap<QString, QVariant> Dekeystoning::getSettings()
{
    QMap<QString, QVariant> settings = widget->getSettings();

    settings["enabled"] = filterEnabled;
    return settings;
}

/** \brief set this filter's settings
 **/
void Dekeystoning::setSettings(QMap<QString, QVariant> settings)
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

void Dekeystoning::settings2Dom(QDomDocument &doc, QDomElement &imageElement, QMap<QString, QVariant> settings)
{
    QDomElement filter = doc.createElement(getIdentifier());
    imageElement.appendChild(filter);

    QDomElement pointElement;
    QPointF point;

    QString corner;
    QStringList cornerNames;
    int i;

    // Iterate through cornerNames to save all 4 Positions
    cornerNames << "topLeftCorner" << "topRightCorner" << "bottomRightCorner" << "bottomLeftCorner";
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

QMap<QString, QVariant> Dekeystoning::dom2Settings(QDomElement &filterElement)
{
    QMap<QString, QVariant> settings;
    QStringList cornerNames;
    QString corner;
    int i;
    QDomElement cornerElement;

    // Iterate through cornerNames to save all 4 Positions
    cornerNames << "topLeftCorner" << "topRightCorner" << "bottomRightCorner" << "bottomLeftCorner";


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

QImage Dekeystoning::filter(QImage inputImage)
{
    if (!filterEnabled)
        return inputImage;

    QPolygonF polygon = widget->polygon();

    QTransform transformMatrix;
    /* it might not be possbible to calculate a treansformation matrix */
    if (!QTransform::quadToSquare(polygon, transformMatrix)) {
        qDebug() << "No transformation exists for this";
        return QImage();
    }

    /* As transformMatrix transforms the polygon to a unit square (1px * 1px), we have
     * to scale it back to the size of our rectangle selection. We use the mean size of
     * the Rectangle as a reference. */
    qreal width  = widget->meanWidth();
    qreal height = widget->meanHeight();
    QTransform scaleMatrix = QTransform::fromScale(width, height);

    QTransform transformationMatrix = transformMatrix * scaleMatrix;

    return inputImage.transformed(transformationMatrix);

}

