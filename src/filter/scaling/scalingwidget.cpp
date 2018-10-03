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
    ui->leftMargin->setValidator(doubleValidator);
    ui->rightMargin->setValidator(doubleValidator);
    ui->bottomMargin->setValidator(doubleValidator);
    ui->topMargin->setValidator(doubleValidator);
    ui->pageWidth->setValidator(doubleValidator);
    ui->pageHeight->setValidator(doubleValidator);
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

double ScalingWidget::imagePixelHeight()
{
    return ui->pixelImageHeight->text().toDouble();
}

double ScalingWidget::imagePixelWidth()
{
    return ui->pixelImageWidth->text().toDouble();
}

double ScalingWidget::pagePixelHeight()
{
    return ui->pixelPageHeight->text().toDouble();
}

double ScalingWidget::pagePixelWidth()
{
    return ui->pixelPageWidth->text().toDouble();
}

qreal ScalingWidget::pageMilimeterHeight()
{
    return ui->millimeterPageHeight->text().toDouble();
}

qreal ScalingWidget::pageMilimeterWidth()
{
    return ui->millimeterPageWidth->text().toDouble();
}

double ScalingWidget::leftMargin()
{
    return(enteredSizeToPixel(ui->leftMargin->text().toDouble()));
}

double ScalingWidget::topMargin()
{
    return(enteredSizeToPixel(ui->topMargin->text().toDouble()));
}


// Fixme: this should be an enum, not a string!
enum ScalingWidget::PageLayout ScalingWidget::layout()
{
    if (ui->marginLayout->isChecked())
        return MarginLayout;
    if (ui->pageLayout->isChecked())
        return PageLayout;
//    if (ui->noMarginLayout->isChecked()) // this is default
    return NoMarginLayout;
}

ScalingWidget::HAlignment ScalingWidget::hAlignment()
{
    return (ScalingWidget::HAlignment)ui->horizontalAlignment->currentIndex();
}

ScalingWidget::VAlignment ScalingWidget::vAlignment()
{
    return (ScalingWidget::VAlignment)ui->verticalAlignment->currentIndex();
}

QMap<QString, QVariant> ScalingWidget::getSettings()
{
    QMap<QString, QVariant> settings;

    settings["imageWidth"] = ui->imageWidth->text().toDouble();
    settings["imageHeight"] = ui->imageHeight->text().toDouble();
    settings["DPI"] = ui->dpi->currentText().toDouble();
    settings["unit"] = ui->unit->currentIndex();
    settings["layout"] = layout();
    settings["leftMargin"] = ui->leftMargin->text().toDouble();
    settings["rightMargin"] = ui->rightMargin->text().toDouble();
    settings["topMargin"] = ui->topMargin->text().toDouble();
    settings["bottomMargin"] = ui->bottomMargin->text().toDouble();
    settings["horizontalAlignment"] = ui->horizontalAlignment->currentIndex();
    settings["verticalAlignment"] = ui->verticalAlignment->currentIndex();
    settings["pageWidth"] = ui->pageWidth->text().toDouble();
    settings["pageHeight"] = ui->pageHeight->text().toDouble();

    return settings;

}

void ScalingWidget::setSettings(QMap<QString, QVariant> settings)
{
    ui->imageWidth->setText(settings["imageWidth"].toString());
    ui->imageHeight->setText(settings["imageHeight"].toString());
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
    ui->leftMargin->setText(settings["leftMargin"].toString());
    ui->rightMargin->setText(settings["rightMargin"].toString());
    ui->topMargin->setText(settings["topMargin"].toString());
    ui->bottomMargin->setText(settings["bottomMargin"].toString());
    ui->pageWidth->setText(settings["pageWidth"].toString());
    ui->pageHeight->setText(settings["pageHeight"].toString());
    if (settings.contains("horizontalAlignment")) {
        ui->horizontalAlignment->setCurrentIndex(settings["horizontalAlignment"].toInt());
    } else {
        ui->horizontalAlignment->setCurrentIndex(1);
    }
    if (settings.contains("verticalAlignment")) {
        ui->verticalAlignment->setCurrentIndex(settings["verticalAlignment"].toInt());
    } else {
        ui->verticalAlignment->setCurrentIndex(1);
    }
    if (settings["layout"].toInt() == MarginLayout) {
        ui->marginLayout->setChecked(true);
    } else if (settings["layout"].toInt() ==  PageLayout) {
        ui->pageLayout->setChecked(true);
    } else { // "no margin" or wrong parameter
        ui->noMarginLayout->setChecked(true);
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
    const qreal millimeterPerInch = 25.4;     // 1 inch = 25,4 mm

    qreal dpi = ui->dpi->currentText().toDouble();
    if (dpi > 0) {
        // calculate the factor into pixel
        switch (ui->unit->currentIndex()) {
            case 0:      // pixel -> do nothing
                factor = 1;
                break;
            case 1:    // millimeter
                factor = 1 / millimeterPerInch * dpi;
                break;
            case 2:     // inches
                factor = dpi;
                break;
            default:
            // This step is never reached.
            Q_ASSERT (false);
        }

        // Sizes in Pixel
        qreal imageWidth = ui->imageWidth->text().toDouble() * factor;
        qreal imageHeight = ui->imageHeight->text().toDouble() * factor;
        qreal leftMargin = ui->leftMargin->text().toDouble() * factor;
        qreal rightMargin = ui->rightMargin->text().toDouble() * factor;
        qreal bottomMargin = ui->bottomMargin->text().toDouble() * factor;
        qreal topMargin = ui->topMargin->text().toDouble() * factor;
        qreal specifiedPageWidth = ui->pageWidth->text().toDouble() * factor;
        qreal specifiedPageHeight = ui->pageHeight->text().toDouble() * factor;

        qreal pageWidth, pageHeight;
        if (ui->noMarginLayout->isChecked()) {
            pageWidth = imageWidth;
            pageHeight = imageHeight;
        }
        if (ui->marginLayout->isChecked()) {
            pageWidth = imageWidth + leftMargin + rightMargin;
            pageHeight = imageHeight + topMargin + bottomMargin;
        }
        if (ui->pageLayout->isChecked()) {
            pageWidth = specifiedPageWidth;
            pageHeight = specifiedPageHeight;
        }

        //image
        ui->pixelImageWidth->setText(QString::number(imageWidth, 'f', 1));
        ui->pixelImageHeight->setText(QString::number(imageHeight, 'f', 1));
        ui->inchImageWidth->setText(QString::number(imageWidth / dpi, 'f', 1));
        ui->inchImageHeight->setText(QString::number(imageHeight / dpi, 'f', 1));
        ui->millimeterImageWidth->setText(QString::number(imageWidth / dpi * millimeterPerInch, 'f', 1));
        ui->millimeterImageHeight->setText(QString::number(imageHeight / dpi * millimeterPerInch, 'f', 1));
        //page
        ui->pixelPageWidth->setText(QString::number(pageWidth, 'f', 1));
        ui->pixelPageHeight->setText(QString::number(pageHeight, 'f', 1));
        ui->inchPageWidth->setText(QString::number(pageWidth / dpi, 'f', 1));
        ui->inchPageHeight->setText(QString::number(pageHeight / dpi, 'f', 1));
        ui->millimeterPageWidth->setText(QString::number(pageWidth / dpi * millimeterPerInch, 'f', 1));
        ui->millimeterPageHeight->setText(QString::number(pageHeight / dpi * millimeterPerInch, 'f', 1));
    } else {
        ui->pixelImageWidth->clear();
        ui->pixelImageHeight->clear();
        ui->inchImageWidth->clear();
        ui->inchImageHeight->clear();
        ui->millimeterImageWidth->clear();
        ui->millimeterImageHeight->clear();
        ui->leftMargin->clear();
        ui->rightMargin->clear();
        ui->bottomMargin->clear();
        ui->topMargin->clear();
        ui->pageWidth->clear();
        ui->pageHeight->clear();
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
        case 2:     // inches
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

    QString calcImageWidth, calcImageHeight;
    qreal imageWidth = ui->imageWidth->text().toDouble();
    qreal imageWeight = ui->imageHeight->text().toDouble();
    calcImageWidth = QString::number(imageWidth * factor);
    calcImageHeight = QString::number(imageWeight * factor);
    ui->imageWidth->setText(calcImageWidth);
    ui->imageHeight->setText(calcImageHeight);

    qreal value;
    QString string;
    value = ui->leftMargin->text().toDouble();
    string = QString::number(value * factor);
    ui->leftMargin->setText(string);
    value = ui->rightMargin->text().toDouble();
    string = QString::number(value * factor);
    ui->rightMargin->setText(string);
    value = ui->topMargin->text().toDouble();
    string = QString::number(value * factor);
    ui->topMargin->setText(string);
    value = ui->bottomMargin->text().toDouble();
    string = QString::number(value * factor);
    ui->bottomMargin->setText(string);
    value = ui->pageWidth->text().toDouble();
    string = QString::number(value * factor);
    ui->pageWidth->setText(string);
    value = ui->pageHeight->text().toDouble();
    string = QString::number(value * factor);
    ui->pageHeight->setText(string);

    lastUnitIndex = index;

}



qreal ScalingWidget::enteredSizeToPixel(qreal size)
{
    qreal factor;
    const qreal millimeterPerInch = 25.4;     // 1 inch = 25,4 mm

    qreal dpi = ui->dpi->currentText().toDouble();
    if (dpi == 0)
        return 0;

    // calculate the factor into pixel
    switch (ui->unit->currentIndex()) {
        case 0:      // pixel -> do nothing
            factor = 1;
            break;
        case 1:    // millimeter
            factor = 1 / millimeterPerInch * dpi;
            break;
        case 2:     // inches
            factor = dpi;
            break;
        default:
        // This step is never reached.
        Q_ASSERT (false);
    }

    return size * factor;
}
