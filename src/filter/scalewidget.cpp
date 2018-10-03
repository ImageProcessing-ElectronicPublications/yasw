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

#include "scalewidget.h"
#include "ui_scalewidget.h"
#include "constants.h"

#include <QDebug>
#include <QString>
#include <QtCore/qmath.h>

ScaleWidget::ScaleWidget(QWidget *parent) :
    AbstractFilterWidget(parent),
    ui(new Ui::ScaleWidget)
{
    ui->setupUi(this);


    // default value
    displayUnit = "pixel";

    doubleValidator = new QDoubleValidator;
    doubleValidator->setBottom(0.001);
    doubleValidator->setDecimals(3);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->imageHeight->setValidator(doubleValidator);
    ui->imageWidth->setValidator(doubleValidator);

    setDPI(Constants::DEFAULT_DPI);
}

ScaleWidget::~ScaleWidget()
{
    delete ui;
    delete doubleValidator;
}

void ScaleWidget::setPixmap(QPixmap pixmap)
{
    inputPixmap = pixmap;
    if (!pixmap.isNull()
            && pxImageWidth == 0
            && pxImageHeight == 0) {
        pxImageWidth = pixmap.width();
        pxImageHeight = pixmap.height();
    }
    updateFormSizes();
    if (!preview()) {
        ui->view->setPixmap(pixmap);
    }
}

void ScaleWidget::setPreview(QPixmap pixmap)
{
    previewPixmap = pixmap;
    if (preview())
        ui->view->setPixmap(pixmap);
}

bool ScaleWidget::preview()
{
    return ui->preview->isChecked();
}

double ScaleWidget::imagePixelHeight()
{
    return pxImageHeight;
}

double ScaleWidget::imagePixelWidth()
{
    return pxImageWidth;
}

QMap<QString, QVariant> ScaleWidget::getSettings()
{
    QMap<QString, QVariant> settings;

    settings["pxImageWidth"] = pxImageWidth;
    settings["pxImageHeight"] = pxImageHeight;
    return settings;
}

void ScaleWidget::setSettings(QMap<QString, QVariant> settings)
{
    pxImageWidth = settings["pxImageWidth"].toDouble();
    pxImageHeight = settings["pxImageHeight"].toDouble();

    // Update the form
    updateFormSizes();
}

void ScaleWidget::setDisplayUnit(QString unit)
{
    switch (Constants::displayUnits.indexOf(unit)) {
        case 0:     // pixel
            factorPixeltoDisplayUnit = 1;
            break;
        case 1:     // millimeter
            factorPixeltoDisplayUnit = Constants::milimeterPerInch / dpi;
            break;
        case 2:     // inches
            factorPixeltoDisplayUnit = 1 / dpi;
            break;
        default:
        // The unit provided is not valid. Do nothing.
        return;
    }
    displayUnit = unit;
    ui->lbUnitHeight->setText(unit);
    ui->lbUnitWidth->setText(unit);

    updateFormSizes();
}

void ScaleWidget::enableFilter(bool enable)
{
    ui->enable->setChecked(enable);
}

void ScaleWidget::setBackgroundColor(QColor color)
{
    ui->view->setBackgroundBrush(QBrush(color));
}

void ScaleWidget::on_preview_toggled(bool checked)
{
    if (checked) {
        // This does recalculate the output image if necessary and sets the preview Image.
        emit previewChecked();
    } else
        ui->view->setPixmap(inputPixmap);
}

// When a parameter is changed, the input and resulting image Sizes are recalculated with this function.
void ScaleWidget::updateFormSizes()
{
    QString newImageWidth, newImageHeight;
    newImageWidth = Constants::float2String(pxImageWidth * factorPixeltoDisplayUnit);
    newImageHeight = Constants::float2String(pxImageHeight * factorPixeltoDisplayUnit);

    ui->imageWidth->setText(newImageWidth);
    ui->imageHeight->setText(newImageHeight);
    ui->pixelImageWidth->setText(Constants::float2String(pxImageWidth));
    ui->pixelImageHeight->setText(Constants::float2String(pxImageHeight));
    ui->inchImageWidth->setText(Constants::float2String(pxImageWidth / dpi));
    ui->inchImageHeight->setText(Constants::float2String(pxImageHeight / dpi));
    ui->millimeterImageWidth->setText(
            Constants::float2String(pxImageWidth / dpi * Constants::milimeterPerInch));
    ui->millimeterImageHeight->setText(
                Constants::float2String(pxImageHeight / dpi * Constants::milimeterPerInch));

    int inputWidth = inputPixmap.width();
    int inputHeight = inputPixmap.height();
    ui->pixelInputWidth->setText(Constants::float2String(inputWidth));
    ui->pixelInputHeight->setText(Constants::float2String(inputHeight));
    ui->inchInputWidth->setText(Constants::float2String(inputWidth / dpi));
    ui->inchInputHeight->setText(Constants::float2String(inputHeight / dpi));
    ui->millimeterInputWidth->setText(
            Constants::float2String(inputWidth / dpi * Constants::milimeterPerInch));
    ui->millimeterInputHeight->setText(
                Constants::float2String(inputHeight / dpi * Constants::milimeterPerInch));


}

void ScaleWidget::setDPI(int newDpi)
{
    if (newDpi < Constants::MIN_DPI) // we only accept positive DPI
        return;

    if (dpi == newDpi) // do nothing when nothing changes.
        return;

    dpi = newDpi;

    switch (Constants::displayUnits.indexOf(displayUnit)) {
        case 0:     // pixel
            factorPixeltoDisplayUnit = 1;
            break;
        case 1:     // millimeter
            factorPixeltoDisplayUnit = Constants::milimeterPerInch / dpi;
            break;
        case 2:     // inches
            factorPixeltoDisplayUnit = 1 / dpi;
            break;
        // default:
        // Default will not happen, we check the unit before setting displayUnit
    }

    updateFormSizes();
    emit parameterChanged();
}

void ScaleWidget::on_imageWidth_editingFinished()
{
    pxImageWidth = ui->imageWidth->text().toDouble() / factorPixeltoDisplayUnit;

    updateFormSizes();
    emit parameterChanged();
}

void ScaleWidget::on_imageHeight_editingFinished()
{
    pxImageHeight = ui->imageHeight->text().toDouble() / factorPixeltoDisplayUnit;

    updateFormSizes();
    emit parameterChanged();
}

void ScaleWidget::on_imageWidth_textEdited(const QString &strValue)
{
    qreal width = strValue.toDouble();

    if (width <= 0.001) {
        ui->imageWidth->setStyleSheet("background-color: rgb(255, 0, 0);");
    } else {
        ui->imageWidth->setStyleSheet("");
    }
}

void ScaleWidget::on_imageHeight_textEdited(const QString &strValue)
{
    qreal height = strValue.toDouble();

    if (height <= 0.001) {
        ui->imageHeight->setStyleSheet("background-color: rgb(255, 0, 0);");
    } else {
        ui->imageHeight->setStyleSheet("");
    }}

void ScaleWidget::on_enable_toggled(bool checked)
{
    emit enableFilterToggled(checked);
}
