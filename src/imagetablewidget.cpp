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

#include <QFileInfo>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QDebug>
#include "imagetablewidget.h"
#include "ui_imagetablewidget.h"

//TODO: comment this file, most comments from the old imagelistwidget shall match
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

    connect (this, SIGNAL(pixmapChanged(QPixmap)),
             filterContainer, SLOT(setImage(QPixmap)));
    // When a new filter is selected, propagate the changes to other images
    connect (filterContainer, SIGNAL(filterChanged(QString)),
             this, SLOT(filterChanged(QString)));
}



void ImageTableWidget::currentItemChanged(QTableWidgetItem *newItem, QTableWidgetItem *previousItem)
{
    QMap<QString, QVariant> settings;
    QMap<QString, QVariant> oldSettings;
    int fromRow, side, i;
    QString filterID = "";

    if (filterContainer && previousItem) {
        settings = filterContainer->getSettings();
        oldSettings = previousItem->data(ImagePreferences).toMap();
        // settings changed? Save them!
        if (settings != oldSettings) {
            previousItem->setData(ImagePreferences, settings);

            // Propagate settings according to settings policy
            switch (ui->settingImagePolicy->currentIndex()) {
            case 1: // propagate to all following images in this side
                fromRow = ui->images->row(previousItem);
                side = ui->images->column(previousItem);

                switch (ui->settingFilterPolicy->currentIndex()) {
                case 0: // propagate only selected filter
                    filterID = filterContainer->currentFilter();
                    for (i = fromRow; i < itemCount[side]; i++) {
                        oldSettings = ui->images->item(i, side)->data(ImagePreferences).toMap();
                        oldSettings[filterID] = settings[filterID];
                        ui->images->item(i, side)->setData(ImagePreferences, oldSettings);
                    }
                    break;
                case 1: // propagate all filter settings
                    for (i = fromRow; i < itemCount[side]; i++) {
                        ui->images->item(i, side)->setData(ImagePreferences, settings);
                    }
                    break;
                }
                break;
            case 2: // propagate to all images in this side
                side = ui->images->column(previousItem);
                switch (ui->settingFilterPolicy->currentIndex()) {
                case 0: // propagate only selected filter
                    filterID = filterContainer->currentFilter();
                    for (i = 0; i < itemCount[side]; i++) {
                        oldSettings = ui->images->item(i, side)->data(ImagePreferences).toMap();
                        oldSettings[filterID] = settings[filterID];
                        ui->images->item(i, side)->setData(ImagePreferences, oldSettings);
                    }
                    break;
                case 1: // propagate all filter settings
                    for (i = 0; i < itemCount[side]; i++) {
                        ui->images->item(i, side)->setData(ImagePreferences, settings);
                    }
                    break;
                }
                break;
//          default:
//          case 0: // do not propagate;
            }
        }
    }

    if (filterContainer && newItem) {
        filterContainer->setSettings(newItem->data(ImagePreferences).toMap());
    }

    if (newItem) {
        emit pixmapChanged(QPixmap(newItem->data(ImageFileName).toString()));
    } else {
        emit pixmapChanged(QPixmap());
        // Reset Filter Settings as no image is selected
        if (filterContainer)
            filterContainer->setSettings(QMap<QString, QVariant>());
    }
}

/* \brief Propagate settings to other images,
   if the settings changed and if there is such a policy activated */
void ImageTableWidget::filterChanged(QString oldFilterID)
{
    QMap<QString, QVariant> settings;
    QMap<QString, QVariant> oldSettings;
    QTableWidgetItem *item;
    int fromRow, side, i;

    settings = filterContainer->getSettings();
    item = ui->images->currentItem();
    if (item == NULL) { // There is no item under selection
        return;
    }
    oldSettings = item->data(ImagePreferences).toMap();

    if (settings[oldFilterID] == oldSettings[oldFilterID]) {
        // do nothing
        return;
    }

    // update old Settungs and save the changes
    oldSettings[oldFilterID] = settings[oldFilterID];
    item->setData(ImagePreferences, oldSettings);

    // and propagate them according to the settings policy
    switch (ui->settingImagePolicy->currentIndex()) {
    case 1: // propagate to all following images in this side
        fromRow = ui->images->row(item);
        side = ui->images->column(item);

        for (i = fromRow; i < itemCount[side]; i++) {
            oldSettings = ui->images->item(i, side)->data(ImagePreferences).toMap();
            oldSettings[oldFilterID] = settings[oldFilterID];
            ui->images->item(i, side)->setData(ImagePreferences, oldSettings);
        }
        break;
    case 2: // propagate to all images in this side
        side = ui->images->column(item);
        for (i = 0; i < itemCount[side]; i++) {
            oldSettings = ui->images->item(i, side)->data(ImagePreferences).toMap();
            oldSettings[oldFilterID] = settings[oldFilterID];
            ui->images->item(i, side)->setData(ImagePreferences, oldSettings);
        }
        break;
//    default:
//    case 0: // do not propagate;
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

    // Save current settings: call the currentItemChanged slot newItem = previusItem = currentItem
    currentItemChanged(ui->images->currentItem(), ui->images->currentItem());

    if (lastDir.length() == 0)
        lastDir = QDir::currentPath();


    QStringList images = QFileDialog::getOpenFileNames(this,
                        tr("Choose images"),
                        lastDir,
                        // FIXME: scaling down for *.png does not work as good as for jpg
                        // FIXME: export fron png images don not work. Deactiving png for now.
//                        tr("Images (*.jpg *.png);;All files (* *.*)"));
                        tr("Images (*.jpg);;All files (* *.*)"));

    foreach (imageFileName, images) {
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

    if (imageFileName.length() > 0) {
        fi = QFileInfo(imageFileName);
        lastDir = fi.absolutePath();
    }

}

/** \brief inserts an image at the current selection on the given side.

*/
// FIXME: handle fileName = "" (empty image)
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
}

/** \brief Inserts an item at row and side and selects it */
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

    // Select the inserted item
    ui->images->setCurrentItem(item);
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

    ui->images->setCurrentItem(item);
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



QMap<QString, QVariant> ImageTableWidget::getSettings()
{
    QMap<QString, QVariant> settings;
    QTableWidgetItem *item;
    int row;
    QString key;

    // Save the settings of current filter before saving
    settings = filterContainer->getSettings();
    item = ui->images->currentItem();
    if (item)
        item->setData(ImagePreferences, settings);
    settings.clear();

    for (row = 0; row < itemCount[leftSide]; row++) {
        item = ui->images->item(row, leftSide);
        key = QString("%1_Left_").arg(row, leftSide) + item->data(ImageFileName).toString();
        settings[key] = item->data(ImagePreferences).toMap();
    }
    for (row = 0; row < itemCount[rightSide]; row++) {
        item = ui->images->item(row, rightSide);
        key = QString("%1_Right_").arg(row, rightSide) + item->data(ImageFileName).toString();
        settings[key] = item->data(ImagePreferences).toMap();
    }

    return settings;
}

void ImageTableWidget::setSettings(QMap<QString, QVariant> settings)
{
    QString key;
    int row;
    QString sideString;
    ImageTableWidget::ImageSide side;
    QString filename;

    clear();

    foreach (key, settings.keys()) {
        row = key.section("_", 0, 0).toInt();
        sideString = key.section("_", 1, 1);
        if (sideString == "Left") {
            side = leftSide;
        } else {
            //NOTE: this is not nice, we should check if sideString == "Right"
            side = rightSide;
        }
        filename = key.section("_", 2);
        if (row >= 0 && filename.length() > 0) {
            appendImageToSide(filename, side, settings[key].toMap());
        }
    }
}

void ImageTableWidget::clear()
{
    //FIXME: is memory cleared?
    ui->images->setRowCount(0);
    itemCount[leftSide] = 0;
    itemCount[rightSide] = 0;
    ui->settingFilterPolicy->setCurrentIndex(0);
    ui->settingImagePolicy->setCurrentIndex(0);
}

void ImageTableWidget::exportToFolder(QString folder)
{
    int row;
    QTableWidgetItem *currentItem = ui->images->currentItem();
    QPixmap pixmap;
    QString filename;

    for (row = 0; row < itemCount[leftSide]; row++) {
        ui->images->setCurrentCell(row, leftSide);
        pixmap = filterContainer->getResultImage();
        filename = QString("%1/image_%2_Left.jpg").arg(folder).arg(row+1, 3, 10, QChar('0'));
        pixmap.save(filename);
    }
    for (row = 0; row < itemCount[rightSide]; row++) {
        ui->images->setCurrentCell(row, rightSide);
        pixmap = filterContainer->getResultImage();
        filename = QString("%1/image_%2_Right.jpg").arg(folder).arg(row+1, 3, 10, QChar('0'));
        pixmap.save(filename);
    }

    ui->images->setCurrentItem(currentItem);
}

void ImageTableWidget::exportToPdf(QString pdfFile)
{
    int row;
    bool firstPage = true;
    QTableWidgetItem *currentItem = ui->images->currentItem();
    QPixmap pixmap;
    QPainter painter;
    QMap<QString, QVariant> imageSize;

    QPrinter *printer = new QPrinter();
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setFullPage(true);
    printer->setOutputFileName(pdfFile);


    for (row = 0; row < qMax(itemCount[leftSide], itemCount[rightSide]); row++) {
        // left Side
        if (row < itemCount[leftSide]) {       // is one item availabla at this row?
            ui->images->setCurrentCell(row, leftSide);
            pixmap = filterContainer->getResultImage();

            /* set Paper size from the scaling Filter */
            imageSize = filterContainer->getPageSize();
            printer->setPaperSize(QSize(imageSize["size"].toSize()),
                                  static_cast<QPrinter::Unit>(imageSize["unit"].toInt()));

            // we don't need a new page for the first page or we would have a blank page
            if (firstPage == true) {
                firstPage = false;
                painter.begin(printer);
            } else {
                printer->newPage();
            }
            painter.drawPixmap(printer->pageRect(), pixmap);
        }
        // right side
        if (row < itemCount[rightSide]) {       // is one item availabla at this row?
            ui->images->setCurrentCell(row, rightSide);
            pixmap = filterContainer->getResultImage();

            /* set Paper size from the scaling Filter */
            imageSize = filterContainer->getPageSize();
            printer->setPaperSize(QSize(imageSize["size"].toSize()),
                                  static_cast<QPrinter::Unit>(imageSize["unit"].toInt()));

            // we don't need a new page for the first page or we would have a blank page
            if (firstPage == true) {
                firstPage = false;
                painter.begin(printer);
            } else {
                printer->newPage();
            }
            painter.drawPixmap(printer->pageRect(), pixmap);
        }
    }

    painter.end();
    delete(printer);

    ui->images->setCurrentItem(currentItem);
}




