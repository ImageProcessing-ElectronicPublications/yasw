#include "scalingwidget.h"
#include "ui_scalingwidget.h"
#include <QDebug>
#include <QString>

ScalingWidget::ScalingWidget(QWidget *parent) :
    AbstractFilterWidget(parent),
    ui(new Ui::ScalingWidget)
{
    ui->setupUi(this);

    doubleValidator = new QDoubleValidator;
    doubleValidator->setBottom(0);
    doubleValidator->setDecimals(3);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->imageHeight->setValidator(doubleValidator);
    ui->imageWidth->setValidator(doubleValidator);
    ui->dpi->setValidator(doubleValidator);
}

ScalingWidget::~ScalingWidget()
{
    delete ui;
    delete doubleValidator;
}

void ScalingWidget::setPixmap(QPixmap pixmap)
{
    inputPixmap = pixmap;
    if (!pixmap.isNull()
            && ui->imageWidth->text().length() == 0
            && ui->imageHeight->text().length() == 0) {
        ui->imageWidth->setText(QString::number(pixmap.width()));
        ui->imageHeight->setText(QString::number(pixmap.height()));
    }
    if (!preview()) {
        ui->view->setPixmap(pixmap);
    }
}

void ScalingWidget::setPreview(QPixmap pixmap)
{
    previewPixmap = pixmap;
    if (preview())
        ui->view->setPixmap(pixmap);
}

bool ScalingWidget::preview()
{
    return ui->preview->isChecked();
}

double ScalingWidget::imageHeight()
{
    return ui->imageHeight->text().toDouble();
}

double ScalingWidget::imageWidth()
{
    return ui->imageWidth->text().toDouble();
}

QMap<QString, QVariant> ScalingWidget::getSettings()
{
    QMap<QString, QVariant> settings;

    settings["imageWidth"] = ui->imageWidth->text().toDouble();
    settings["imageHeight"] = ui->imageHeight->text().toDouble();
    settings["DPI"] = ui->dpi->currentText().toDouble();
    settings["unit"] = ui->unit->currentIndex();

    return settings;

}

void ScalingWidget::setSettings(QMap<QString, QVariant> settings)
{
    if (settings.contains("imageWidth")
            && settings["imageWidth"].canConvert(QVariant::Double)
            && settings["imageWidth"].toDouble() != 0) {
        ui->imageWidth->setText(settings["imageWidth"].toString());
    } else {
        ui->imageWidth->clear();
    }
    if (settings.contains("imageHeight")
            && settings["imageHeight"].canConvert(QVariant::Double)
            && settings["imageHeight"].toDouble() != 0) {
        ui->imageHeight->setText(settings["imageHeight"].toString());
    } else {
        ui->imageHeight->clear();
    }
    if (settings.contains("DPI")
            && settings["DPI"].canConvert(QVariant::Double)) {
        QString DPIValue = settings["DPI"].toString();
        int index = ui->dpi->findText(DPIValue);
        if (index == -1) {          // This is no default value
            ui->dpi->addItem(DPIValue);
            index = ui->dpi->count();
        }
        ui->dpi->setCurrentIndex(index);
    } else {
        ui->dpi->setCurrentIndex(1);
    }
    if (settings.contains("unit")
            && settings["unit"].canConvert(QVariant::Int)) {
        lastUnitIndex = settings["unit"].toInt();
        ui->unit->setCurrentIndex(settings["unit"].toInt());
    } else {
        lastUnitIndex = 1;
        ui->unit->setCurrentIndex(1);
    }

    slotPropertyChanged();
}

void ScalingWidget::setBackgroundColor(QColor color)
{
    ui->view->setBackgroundBrush(QBrush(color));
}

void ScalingWidget::on_preview_toggled(bool checked)
{
    if (checked)
        ui->view->setPixmap(previewPixmap);
    else
        ui->view->setPixmap(inputPixmap);
}

/** \brief Slot to call when a parameter of the filter changes

 This is an agregator for all changed signals. It has multiple functions:
    - display calculated width and height of the resulting image.
    - emit a signel signalPropertyChanged, wich is used by the Scaling class to
      recalculate the resulting image.
 */
void ScalingWidget::slotPropertyChanged()
{
    qreal factor;
    QString calcUnitName;
    const qreal millimeterPerInch = 25.4;     // 1 inch = 25,4 mm

    qreal dpi = ui->dpi->currentText().toDouble();
    if (dpi > 0) {
        switch (ui->unit->currentIndex()) {
            case 0:      // pixel -> display mm
                calcUnitName = " mm";
                // size_mm = size_pix/dpi * inch2mm
                factor = 1 / dpi * millimeterPerInch;
                break;
            case 1:    // millimeter -> display pixels
                calcUnitName = " pixel";
                factor = 1 / millimeterPerInch * dpi;
                break;
            case 2:     // inches -> display pixels
                calcUnitName = " pixel";
                factor = dpi;
                break;
            default:
            // This step is never reached.
            Q_ASSERT (false);
        }

        QString calcWidth, calcHeight;
        qreal width = ui->imageWidth->text().toDouble();
        qreal height = ui->imageHeight->text().toDouble();
        calcWidth = QString::number(width * factor) + calcUnitName;
        calcHeight = QString::number(height * factor) + calcUnitName;
        ui->calculatedWidth->setText(calcWidth);
        ui->calculatedHeight->setText(calcHeight);
    } else {
        ui->calculatedWidth->clear();
        ui->calculatedHeight->clear();
    }

    emit signalPropertyChanged();
}

/** \brief Update the size displayed when the Unit changes
*/
void ScalingWidget::on_unit_currentIndexChanged(int index)
{
    qreal dpi = ui->dpi->currentText().toDouble();
    const qreal millimeterPerInch = 25.4;     // 1 inch = 25,4 mm

    if (lastUnitIndex == index || dpi == 0) {
        return;
    }

    qreal factor;

    /* First calculate the factor to convert into pixel */
    switch (lastUnitIndex) {
        case 0:      // pixel -> do nothing
            factor = 1;
            break;
        case 1:    // millimeter
            factor = 1 / millimeterPerInch * dpi;
            break;
        case 2:     // inches -> display pixels
            factor = dpi;
            break;
        default:
        // This step is never reached.
        Q_ASSERT (false);
    }


    /* Then update the factor to convert to the new Unit */
    switch (index) {
        case 0:      // pixel -> do nothing
            // factor = factor * 1;
            break;
        case 1:    // millimeter
            factor = factor / dpi * millimeterPerInch;
            break;
        case 2:     // inches
            factor = factor / dpi;
            break;
        default:
        // This step is never reached.
        Q_ASSERT (false);
    }

    QString calcWidth, calcHeight;
    qreal width = ui->imageWidth->text().toDouble();
    qreal height = ui->imageHeight->text().toDouble();
    calcWidth = QString::number(width * factor);
    calcHeight = QString::number(height * factor);
    ui->imageWidth->setText(calcWidth);
    ui->imageHeight->setText(calcHeight);

    lastUnitIndex = index;

}
