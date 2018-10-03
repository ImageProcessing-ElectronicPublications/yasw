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

#include <QFileInfo>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QDebug>
#include <QProgressDialog>

#include "imagetablewidget.h"
#include "constants.h"

#include "ui_imagetablewidget.h"

/* FIXME: I am very unhapy with the design of this ImageTableWidget. This is a dirty
   hack that has to be rewritten. It is a lot of work that noone sees but has to be
   rewritten before nice features like drag&drop comme in play.
   Layout has to be separated from Data, items have to become a dedicated widget
   (ideas: filename under image, display source image or preview, infos with tooltip...)
*/

ImageTableWidget::ImageTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageTableWidget)
{
    ui->setupUi(this);

    connect(ui->images, SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
            this, SLOT(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)));

    filterContainer = NULL;

    itemCount[leftSide] = 0;
    itemCount[rightSide] = 0;

    ui->images->setHorizontalHeaderLabels(QStringList() << "Left" << "Right");
}

ImageTableWidget::~ImageTableWidget()
{
    delete ui;
}

void ImageTableWidget::setFilterContainer(FilterContainer *container)
{
    if (filterContainer) {
        Q_ASSERT (false); // Changing filterContainer. This should never occur !
    }

    filterContainer = container;
}



void ImageTableWidget::currentItemChanged(QTableWidgetItem *newItem, QTableWidgetItem *previousItem)
{
    if (!filterContainer) {
        // This should never happen, but if so, do nothing.
        return;
    }

    QMap<QString, QVariant> settings;
    QMap<QString, QVariant> oldSettings;
    QString filterID = "";

    if (previousItem) {
        settings = filterContainer->getSettings();
        oldSettings = previousItem->data(ImagePreferences).toMap();
        // settings changed? Save them!
        if (settings != oldSettings) {
            previousItem->setData(ImagePreferences, settings);
        }
    }

    if (newItem) {
        // NOTE: setting the image an setting the settings results in recaluling twice the image
        // There might be a performance improvement here.
        filterContainer->setImage(QPixmap(newItem->data(ImageFileName).toString()));
        filterContainer->setSettings(newItem->data(ImagePreferences).toMap());
    } else {
        // FIXME: can this happen?
        qDebug() << "ImageTableWidget::currentItemChanged to an empty item";
        filterContainer->setImage(QPixmap());
        // Reset Filter Settings as no image is selected
        filterContainer->setSettings(QMap<QString, QVariant>());
    }
}

/** \brief Slot called from the UI to add an one or many images */
void ImageTableWidget::insertImage()
{
    QFileInfo fi;
    QString imageFileName;
    bool moveSelectionDown = false;
    ImageTableWidget::ImageSide side;

    int column = ui->images->currentColumn();
    if (column == 1)
        side = ImageTableWidget::rightSide;
    else // defaults to left
        side = ImageTableWidget::leftSide;

    // if there is a selected item, save its settings.
    QTableWidgetItem *currentImage = ui->images->currentItem();
    if (currentImage) {
        // Save current settings: call the currentItemChanged slot newItem = previusItem = currentItem
        currentItemChanged(currentImage, currentImage);
    }

    if (lastDir.length() == 0)
        lastDir = QDir::currentPath();


    QStringList images = QFileDialog::getOpenFileNames(this,
                        tr("Choose images"),
                        lastDir,
                        // FIXME: scaling down for *.png does not work as good as for jpg
                        // FIXME: export fron png images don not work. Deactiving png for now.
//                        tr("Images (*.jpg *.png);;All files (* *.*)"));
                        tr("Images (*.jpg);;All files (* *.*)"));

    int progress = 0;
    int numberImages = images.length();
    if (numberImages == 0) // No image to load
        return;

    QProgressDialog progressDialog(tr("Loading images..."), "Abort", 0, numberImages);
    progressDialog.setWindowModality(Qt::WindowModal);

    foreach (imageFileName, images) {
        // Update Progress Dialog
        progressDialog.setValue(progress);
        progress++;
        if (progressDialog.wasCanceled()) {
            // Allready loaded image can't be undone.
            // Load settings/image for the current item.
            currentItemChanged(ui->images->currentItem(), NULL);
            return;
        }
        // Insert the image
        if (moveSelectionDown) {
            // Append images after the previous insertion.
            // Insert the first image depending on current selection, so do nothing.
            ui->images->setCurrentCell(ui->images->currentRow() + 1, ui->images->currentColumn());
        }
        addImage(imageFileName, side);
        moveSelectionDown = true;
    }



    // Load settings/image for the current item.
    currentItemChanged(ui->images->currentItem(), NULL);

    // Close progressDialog
    progressDialog.setValue(numberImages);


    // Saves path of the last loaded image.
    if (imageFileName.length() > 0) {
        fi = QFileInfo(imageFileName);
        lastDir = fi.absolutePath();
    }

}

/* Inserts an image at the current selection on the given side and selects it.
*/
void ImageTableWidget::addImage(QString fileName, ImageTableWidget::ImageSide side, QMap<QString, QVariant> settings)
{
    QTableWidgetItem *item;
    QTableWidgetItem *currentItem;
    QFileInfo fi(fileName);
    QPixmap icon;
    int currentRow;

    //NOTE: loading all icons at this time implies waiting time for the user.
    //Alternatives: put some "waiting" dialog or do ICON-loading in the background
    icon = QPixmap(fileName).scaledToWidth(100);
    item = new QTableWidgetItem(QIcon(icon),
                        fi.fileName());
    item->setData(ImageFileName, fileName);
    item->setData(ImagePreferences, settings);
    item->setToolTip(fileName);

    currentItem = ui->images->currentItem();
    if (currentItem && ui->images->currentColumn() == side) {
        // Insert before current item
        currentRow = ui->images->currentRow();
    } else {
        // Insert at the End
        currentRow = itemCount[side];
    }

    insertItem(item, currentRow, side);

    // Select the inserted item
    ui->images->setCurrentItem(item);
}

/** \brief Inserts an item at row and side */
void ImageTableWidget::insertItem(QTableWidgetItem *item, int row, int side)
{
    int i;

    /* If the position is outside the possible possition, modifiy it to be the best
     * available value */
    if (side < 0)
        side = 0;
    if (side > 1)
        side = 1;
    if (row < 0)
        row = 0;
    if (row >= itemCount[side])
        row = itemCount[side];

    // Adjust Table size if necessary
    if (itemCount[side] >=  ui->images->rowCount()) {
        ui->images->setRowCount(itemCount[side] + 1);
    }

    // move all items after position one step down
    for (i = itemCount[side]; i > row; i--) {
        ui->images->setItem(i, side, ui->images->takeItem(i - 1, side));
    }

    ui->images->setItem(row, side, item);
    itemCount[side] = itemCount[side] + 1;
}

/** \brief Appends an image at the end of the Table

*/
void ImageTableWidget::appendImageToSide(QString fileName,
                                         ImageTableWidget::ImageSide side,
                                         QMap<QString, QVariant> settings)
{
    QTableWidgetItem *item;
    QPixmap icon;
    QFileInfo fi(fileName);

    icon = QPixmap(fileName).scaledToWidth(100);
    item = new QTableWidgetItem(QIcon(icon),
                        fi.fileName());
    // Adjust Table size if necessary
    if (itemCount[side] >=  ui->images->rowCount()) {
        ui->images->setRowCount(itemCount[side] + 1);
    }
    ui->images->setItem(itemCount[side], side, item);
    item->setData(ImageFileName, fileName);
    item->setData(ImagePreferences, settings);
    item->setToolTip(fileName);
    itemCount[side] = itemCount[side] + 1;
}

void ImageTableWidget::insertEmptyImage()
{
    ImageTableWidget::ImageSide side;

    int column = ui->images->currentColumn();
    if (column == 1)
        side = ImageTableWidget::rightSide;
    else // defaults to left
        side = ImageTableWidget::leftSide;

    addImage("", side);
}


void ImageTableWidget::imageDown()
{
    int side = ui->images->currentColumn();
    if (side < 0 || side > 1)
        return;

    int currentRow = ui->images->currentRow();
    if (currentRow + 1 > itemCount[side] - 1) {
        return;
    }
    QTableWidgetItem *itemToDown = ui->images->takeItem(currentRow, side);
    QTableWidgetItem *itemToUp = ui->images->takeItem(currentRow + 1, side);
    ui->images->setItem(currentRow +1, side, itemToDown);
    ui->images->setItem(currentRow, side, itemToUp);
    ui->images->setCurrentItem(itemToDown);
}

void ImageTableWidget::imageUp()
{
    int side = ui->images->currentColumn();
    if (side < 0 || side > 1)
        return;

    int currentRow = ui->images->currentRow();
    if (currentRow < 1 || currentRow >= itemCount[side]) {
        return;
    }
    QTableWidgetItem *itemToUp = ui->images->takeItem(currentRow, side);
    QTableWidgetItem *itemToDown = ui->images->takeItem(currentRow -1, side);
    ui->images->setItem(currentRow - 1, side, itemToUp);
    ui->images->setItem(currentRow, side, itemToDown);
    ui->images->setCurrentItem(itemToUp);
}

void ImageTableWidget::moveImageLeft()
{
    int side = ui->images->currentColumn();
    if (side != 1) { // nothing to move
        return;
    }
    int otherSide = 1 - side;

    int currentRow = ui->images->currentRow();
    if (currentRow < 0 || currentRow >= itemCount[side]) {
        return;
    }

    QTableWidgetItem *itemToMove = takeItem(currentRow, side);
    insertItem(itemToMove, currentRow, otherSide);
    // Select the moved item
    ui->images->setCurrentItem(itemToMove);
}

void ImageTableWidget::moveImageRight()
{
    int side = ui->images->currentColumn();
    if (side != 0) { // nothing to move
        return;
    }
    int otherSide = 1 - side;

    int currentRow = ui->images->currentRow();
    if (currentRow < 0 || currentRow >= itemCount[side]) {
        return;
    }

    QTableWidgetItem *itemToMove = takeItem(currentRow, side);
    insertItem(itemToMove, currentRow, otherSide);
    // Select the moved item
    ui->images->setCurrentItem(itemToMove);
}

void ImageTableWidget::selectPreviousImage()
{
    int side = ui->images->currentColumn();
    int row = ui->images->currentRow();

    row = row - 1;

    /* If the position is outside the possible possition, modifiy it to be the best
     * available value */
    if (side < 0)
        side = 0;
    if (side > 1)
        side = 1;
    if (row < 0)
        row = 0;
    if (row >= itemCount[side])
        row = itemCount[side] - 1;

    ui->images->setCurrentCell(row, side);
}

void ImageTableWidget::selectNextImage()
{
    int side = ui->images->currentColumn();
    int row = ui->images->currentRow();

    row = row + 1;

    /* If the position is outside the possible possition, modifiy it to be the best
     * available value */
    if (side < 0)
        side = 0;
    if (side > 1)
        side = 1;
    if (row < 0)
        row = 0;
    if (row >= itemCount[side])
        row = itemCount[side] - 1;

    ui->images->setCurrentCell(row, side);}

void ImageTableWidget::selectRightImage()
{
    int side = ui->images->currentColumn();
    int row = ui->images->currentRow();

    side = 1;

    /* If the position is outside the possible possition, modifiy it to be the best
     * available value */
    if (row < 0)
        row = 0;
    if (row >= itemCount[side])
        row = itemCount[side] - 1;

    ui->images->setCurrentCell(row, side);
}

void ImageTableWidget::selectLeftImage()
{
    int side = ui->images->currentColumn();
    int row = ui->images->currentRow();

    side = 0;

    /* If the position is outside the possible possition, modifiy it to be the best
     * available value */
    if (row < 0)
        row = 0;
    if (row >= itemCount[side])
        row = itemCount[side] - 1;

    ui->images->setCurrentCell(row, side);
}

QTableWidgetItem * ImageTableWidget::takeItem(int row, int side)
{
    int i;
    QTableWidgetItem *item;

    if (side < 0 || side > 1)
        return NULL; //Nothing to delete
    int otherSide = 1 - side;

    if (row >=0 && row >= itemCount[side]) {
        return NULL; //Nothing to delete
    }

    item = ui->images->takeItem(row, side);
    for (i = row; i < itemCount[side] - 1; i++) {
        ui->images->setItem(i, side,
                            ui->images->takeItem(i + 1, side));
    }
    itemCount[side]--;
    if (itemCount[side] >= itemCount[otherSide]) {
        ui->images->setRowCount(itemCount[side]);
    }

    // if last column removed, scroll one up
    if (row == itemCount[side] && row > 0) {
        ui->images->setCurrentCell(row - 1, side);
    }

    // redraw
    currentItemChanged(ui->images->currentItem(), NULL);

    return item;
}




void ImageTableWidget::removeSelected()
{
    int currentRow = ui->images->currentRow();
    int currentColumn = ui->images->currentColumn();

    delete takeItem(currentRow, currentColumn);
}

void ImageTableWidget::saveProjectParameters(QDomDocument &doc, QDomElement &parent)
{
    int row;
    QTableWidgetItem *item;
    QDomElement imageElement;
    QMap<QString, QVariant> settings;

    // Save the settings of current filter before saving
    settings = filterContainer->getSettings();
    item = ui->images->currentItem();
    if (item)
        item->setData(ImagePreferences, settings);
    settings.clear();

    for (row = 0; row < itemCount[leftSide]; row++) {
        item = ui->images->item(row, leftSide);

        // Save the image attributes
        imageElement = doc.createElement("image");
        imageElement.setAttribute("side", "left");
        imageElement.setAttribute("filename", item->data(ImageFileName).toString());
        parent.appendChild(imageElement);

        // Save the filter settings
        settings = item->data(ImagePreferences).toMap();
        filterContainer->settings2Dom(doc, imageElement, settings);

    }
    for (row = 0; row < itemCount[rightSide]; row++) {
        item = ui->images->item(row, rightSide);

        // Save the image attributes
        imageElement = doc.createElement("image");
        imageElement.setAttribute("side", "right");
        imageElement.setAttribute("filename", item->data(ImageFileName).toString());
        parent.appendChild(imageElement);

        // Save the filter settings
        settings = item->data(ImagePreferences).toMap();
        filterContainer->settings2Dom(doc, imageElement, settings);
    }




}

// This function loads the parameter from XML (DOM) int yasw
bool ImageTableWidget::loadProjectParameters(QDomElement &rootElement)
{
    QDomElement imageElement;
    QString sideString;
    ImageTableWidget::ImageSide side;
    int progress = 1;
    QString filename;


    clear();

    // Load all Images
    int numberImages = rootElement.elementsByTagName("image").size();

    QProgressDialog progressDialog("Loading project...", "Abort", 1, numberImages);
    progressDialog.setWindowModality(Qt::WindowModal);

    QMap<QString, QVariant> settings;

    imageElement = rootElement.firstChildElement("image");
    while (!imageElement.isNull()) {
        // update progress dialog
        progressDialog.setValue(progress);
        if (progressDialog.wasCanceled()) {
            return false;
        }
        // load image
        sideString = imageElement.attribute("side");
        if (sideString == "left") {
            side = leftSide;
        } else if (sideString == "right") {
            side = rightSide;
        } else {
            // Project file is not OK => cancel Loading
            // FIXME: signal the error to the User?
            progressDialog.cancel();
            return false;
        }
        filename = imageElement.attribute("filename");
        // FIXME: settings
        settings = filterContainer->dom2Settings(imageElement);
        appendImageToSide(filename, side, settings);
        imageElement = imageElement.nextSiblingElement("image");
        progress++;
    }
    progressDialog.setValue(numberImages);
    return true;
}

// Empty the widget from all images;
void ImageTableWidget::clear()
{
    //FIXME: is memory cleared?
    ui->images->setRowCount(0);
    itemCount[leftSide] = 0;
    itemCount[rightSide] = 0;
}

void ImageTableWidget::exportToFolder(QString folder)
{
    int row;
    QTableWidgetItem *currentItem = ui->images->currentItem();
    QPixmap pixmap;
    QString filename;

    int progress = 0;
    int maxProgress = itemCount[leftSide] + itemCount[rightSide];
    QProgressDialog progressDialog(QString("Exporting to folder %2...").arg(folder), "Abort", 0, maxProgress);
    progressDialog.setWindowModality(Qt::WindowModal);

    for (row = 0; row < itemCount[leftSide]; row++) {
        // Update Process Dialog
        progressDialog.setValue(progress);
        progress++;
        if (progressDialog.wasCanceled()) {
            ui->images->setCurrentItem(currentItem);
            return;
        }
        // Export image
        ui->images->setCurrentCell(row, leftSide);
        pixmap = filterContainer->getResultImage();
        filename = QString("%1/image_%2_Left.jpg").arg(folder).arg(row+1, 3, 10, QChar('0'));
        pixmap.save(filename);
    }
    for (row = 0; row < itemCount[rightSide]; row++) {
        // Update Process Dialog
        progressDialog.setValue(progress);
        progress++;
        if (progressDialog.wasCanceled()) {
            ui->images->setCurrentItem(currentItem);
            return;
        }
        // Export image
        ui->images->setCurrentCell(row, rightSide);
        pixmap = filterContainer->getResultImage();
        filename = QString("%1/image_%2_Right.jpg").arg(folder).arg(row+1, 3, 10, QChar('0'));
        pixmap.save(filename);
    }

    progressDialog.setValue(maxProgress);
    ui->images->setCurrentItem(currentItem);
}

void ImageTableWidget::exportToPdf(QString pdfFile, int DPI)
{
    int row;
    qreal w = 0;
    qreal h = 0;
    bool firstPage = true;
    QTableWidgetItem *currentItem = ui->images->currentItem();
    QPixmap pixmap;
    QPainter painter;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setFullPage(true);
    printer.setOutputFileName(pdfFile);
    // This seems to have no effect. Setting it doesn't hurt...
    printer.setResolution(DPI);

    int progress = 0;
    int maxProgress = qMax(itemCount[leftSide], itemCount[rightSide]);
    QProgressDialog progressDialog(QString("Exporting to %2...").arg(pdfFile), "Abort", 0, maxProgress);
    progressDialog.setWindowModality(Qt::WindowModal);

    for (row = 0; row < qMax(itemCount[leftSide], itemCount[rightSide]); row++) {
        // Update Process Dialog
        progressDialog.setValue(progress);
        progress++;
        if (progressDialog.wasCanceled()) {
            painter.end();
            ui->images->setCurrentItem(currentItem);
            return;
        }
        // Export pages
        // handle both side with one peace of code. side values: 0 = leftSide, 1 = rightSide
        for (int side = 0; side <= 1; side++) {
            if (row < itemCount[side]) {       // is one item available at this row?
                ui->images->setCurrentCell(row, side);
                pixmap = filterContainer->getResultImage();

                // as QPrinter does not handle DPI right, we have set the paper size in inches.
                w = (qreal) pixmap.width() / DPI;
                h = (qreal) pixmap.height() / DPI;
                printer.setPaperSize(QSizeF(w, h), QPrinter::Inch);

                // we don't need a new page for the first page or we would have a blank page
                if (firstPage == true) {
                    firstPage = false;
                    painter.begin(&printer);
                } else {
                    printer.newPage();
                }

                painter.drawPixmap(printer.pageRect(), pixmap);
            }
        }
    }

    painter.end();

    progressDialog.setValue(maxProgress);
    ui->images->setCurrentItem(currentItem);
}




void ImageTableWidget::on_btnPropagateFollowingSameSide_clicked()
{
    QMap<QString, QVariant> filterSettings;
    int row = ui->images->currentRow();
    int side = ui->images->currentColumn();

    QMap<QString, QVariant> settings = filterContainer->getSettings();
    QString filterID = filterContainer->currentFilter();
    for (int i = row; i < itemCount[side]; i++) {
        filterSettings = ui->images->item(i, side)->data(ImagePreferences).toMap();
        filterSettings[filterID] = settings[filterID];
        ui->images->item(i, side)->setData(ImagePreferences, filterSettings);
    }
}


void ImageTableWidget::on_btnPropagateAllSameSide_clicked()
{
    QMap<QString, QVariant> filterSettings;
    int side = ui->images->currentColumn();

    QMap<QString, QVariant> settings = filterContainer->getSettings();
    QString filterID = filterContainer->currentFilter();
    for (int i = 0; i < itemCount[side]; i++) {
        filterSettings = ui->images->item(i, side)->data(ImagePreferences).toMap();
        filterSettings[filterID] = settings[filterID];
        ui->images->item(i, side)->setData(ImagePreferences, filterSettings);
    }
}

void ImageTableWidget::on_btnPropagateAll_clicked()
{
    QMap<QString, QVariant> filterSettings;

    for (int side=0; side <= 1; side++) {
        QMap<QString, QVariant> settings = filterContainer->getSettings();
        QString filterID = filterContainer->currentFilter();
        for (int i = 0; i < itemCount[side]; i++) {
            filterSettings = ui->images->item(i, side)->data(ImagePreferences).toMap();
            filterSettings[filterID] = settings[filterID];
            ui->images->item(i, side)->setData(ImagePreferences, filterSettings);
        }
    }
}

