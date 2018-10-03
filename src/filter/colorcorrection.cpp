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
#include "colorcorrection.h"
#include <QImage>
#include <QDebug>

ColorCorrection::ColorCorrection(QObject *parent) : BaseFilter(parent)
{
    widget = new ColorCorrectionWidget();
    filterWidget = widget;

    connect(widget, SIGNAL(parameterChanged()),
            this, SLOT(widgetParameterChanged()));

    if (parent) {
        /* Connect slots to the filtercontainer */
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

QString ColorCorrection::getIdentifier()
{
    return QString("colorcorrection");
}

QString ColorCorrection::getName()
{
    return tr("Color Correction");
}

QMap<QString, QVariant> ColorCorrection::getSettings()
{
    QMap<QString, QVariant> settings;

    QColor color;

    color = widget->whitePoint();
    settings["whitepoint"] = color.name();

    color = widget->blackPoint();
    settings["blackpoint"] = color.name();

    settings["enabled"] = filterEnabled;

    return settings;
}

void ColorCorrection::setSettings(QMap<QString, QVariant> settings)
{
    loadingSettings = true;

    QColor color;

    if (settings.contains("whitepoint"))
        color.setNamedColor(settings["whitepoint"].toString());
    else
        color = Qt::white;
    widget->setWhitePoint(color);

    if (settings.contains("blackpoint"))
        color.setNamedColor(settings["blackpoint"].toString());
    else
        color = Qt::black;
    widget->setBlackPoint(color);

    if (settings.contains("enabled"))
        enableFilter(settings["enabled"].toBool());
    else
        enableFilter("true");

    mustRecalculate = true;
    loadingSettings = false;
}

void ColorCorrection::settings2Dom(QDomDocument &doc, QDomElement &parent, QMap<QString, QVariant> settings)
{
    QDomElement filter = doc.createElement(getIdentifier());
    parent.appendChild(filter);
    if (settings.contains("whitepoint"))
        filter.setAttribute("whitepoint", settings["whitepoint"].toString());
    else
        filter.setAttribute("whitepoint", "#FFFFFF");

    if (settings.contains("blackpoint"))
        filter.setAttribute("blackpoint", settings["blackpoint"].toString());
    else
        filter.setAttribute("blackpoint", "#000000");


    if (settings.contains("enabled"))
        filter.setAttribute("enabled", settings["enabled"].toBool());
    else
        filter.setAttribute("enabled", true);
}

QMap<QString, QVariant> ColorCorrection::dom2Settings(QDomElement &filterElement)
{
    QMap<QString, QVariant> settings;

    settings["whitepoint"] = filterElement.attribute("whitepoint", "#FFFFFF");
    settings["blackpoint"] = filterElement.attribute("blackpoint", "#000000");

    return settings;
}


/* Scales every pixel value of the image so that it matches the choosen White and Black points.
 *  For every color (here red) whe have:
 *  - an intensity "red"
 *  - the red component of the white point "redWhite"
 *  - the red component of the white point "redBlack"
 *  - the new intensity "redNew"
 * So we just need to scale the range redBlack..redWhite to 0..255, points under redBlack are set to 0, points over redWhite to 255:
 *   redNew = red * 255 / (redWhite - redBlack) - redBlack) (plus min and max)
 *      NOTE: to optimise, we use redDelta = (redWhite - redBlack); dividing by 255 may be contra-productive
 *            as it needs the use of real values.
 *
 * NOTE: performance improvements might be possible (use of scanline() or preview a scaled image)
 * NOTE: move this code to an ImageManipulation class?
 */
QImage ColorCorrection::filter(QImage inputImage)
{
    if (!filterEnabled)
        return inputImage;

    QImage outputImage(inputImage.width(), inputImage.height(), QImage::Format_ARGB32_Premultiplied);

    int x, y; // coordinates in the image for the for() loops
    QRgb pixelColor;
    int redNew, redWhite, redBlack, redDelta;
    int greenNew, greenWhite, greenBlack, greenDelta;
    int blueNew, blueWhite, blueBlack, blueDelta;

    QColor whitePoint = widget->whitePoint();
    QColor blackPoint = widget->blackPoint();

    // Optimisation: Storing everything static in seperate values to avoid needless calls while computing.
    redWhite = whitePoint.red();
    greenWhite = whitePoint.green();
    blueWhite = whitePoint.blue();
    redBlack = blackPoint.red();
    greenBlack = blackPoint.green();
    blueBlack = blackPoint.blue();
    // as we divide through xxxDelta, it must at least be 1.
    redDelta = qMax(1, redWhite - redBlack);
    greenDelta = qMax(1, greenWhite - greenBlack);
    blueDelta = qMax(1, blueWhite - blueBlack);
    int imageWidth = inputImage.width();
    int imageHeight = inputImage.height();

    for (x = 0; x < imageWidth; x++) {
        for (y = 0; y < imageHeight; y++) {
            pixelColor = inputImage.pixel(x,y);
            redNew =   qMax(0, qMin(255, qRed(pixelColor)   * 255 / redDelta   - redBlack));
            greenNew = qMax(0, qMin(255, qGreen(pixelColor) * 255 / greenDelta - greenBlack));
            blueNew =  qMax(0, qMin(255, qBlue(pixelColor)  * 255 / blueDelta  - blueBlack));
            outputImage.setPixel(x, y, qRgb(redNew, greenNew, blueNew));
        }
    }
    return outputImage;
}
