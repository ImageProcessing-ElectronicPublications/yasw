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
#include <QPainter>

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

qreal Scaling::pageMilimeterHeight()
{
    return widget->pageMilimeterHeight();
}

qreal Scaling::pageMilimeterWidth()
{
    return widget->pageMilimeterWidth();
}

void Scaling::recalculate()
{
    qreal imageWidth = widget->imagePixelWidth();
    qreal imageHeight = widget->imagePixelHeight();
    qreal pageWidth = widget->pagePixelWidth();
    qreal pageHeight = widget->pagePixelHeight();
    enum ScalingWidget::PageLayout layout = widget->layout();
    qreal leftMargin, topMargin;

    if (imageWidth == 0 || imageHeight == 0 || inputPixmap.isNull()) {
        outputPixmap = QPixmap();
    } else {
        if (layout == ScalingWidget::MarginLayout) {
            leftMargin = widget->leftMargin();
            topMargin = widget->topMargin();
        } else if (layout == ScalingWidget::PageLayout) {
            // if the image is too big, reduce it.
            // NOTE: this shall decrease the DPI value in the UI.
            if (imageWidth > pageWidth) {
                imageHeight = imageHeight * pageWidth / imageWidth;
                imageWidth = pageWidth;
            }
            if (imageHeight > pageHeight) {
                imageWidth = imageWidth * pageHeight / imageHeight;
                imageHeight = pageHeight;
            }

            switch (widget->hAlignment()) {
            case ScalingWidget::LeftHAlignment:
                leftMargin = 0;
                break;
            case ScalingWidget::RightHAlignment:
                leftMargin = pageWidth - imageWidth;
                break;
            case ScalingWidget::CenterHAlignment:
            default:
                leftMargin = (pageWidth - imageWidth) / 2;
                break;
            }
            switch (widget->vAlignment()) {
            case ScalingWidget::TopVAlignment:
                topMargin = 0;
                break;
            case ScalingWidget::BottomVAlignment:
                topMargin = (pageHeight - imageHeight);
                break;
            case ScalingWidget::CenterVAlignment:
            default:
                topMargin = (pageHeight - imageHeight) / 2;
                break;
            }


        } else { // layout == noMarginLayout or error
            leftMargin = 0;
            topMargin = 0;
        }

        QSize outputImageSize = QSize(imageWidth, imageHeight);
        QPixmap scaledImage = inputPixmap.scaled(outputImageSize);
        QPixmap page = QPixmap(pageWidth, pageHeight);
        //NOTE: fill color could be a parameter. Wait for User feedback ;-)
        page.fill(Qt::white);
        QPainter painter(&page);
        painter.drawPixmap(leftMargin, topMargin, scaledImage);
        outputPixmap = page;
    }

    widget->setPreview(outputPixmap);
}
