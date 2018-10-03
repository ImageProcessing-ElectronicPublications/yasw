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
        Q_ASSERT ("Changing filterContainer. This should never occur !");
        disconnect(this, SIGNAL(pixmapChanged(QPixmap)),
                   filterContainer, SLOT(setImage(QPixmap)));
    }

    filterContainer = container;

    connect (this, SIGNAL(pixmapChanged(QPixmap)),
             filterContainer, SLOT(setImage(QPixmap)));
}



void ImageTableWidget::currentItemChanged(QTableWidgetItem *newItem, QTableWidgetItem *previousItem)
{
    QMap<QString, QVariant> settings;
    int fromRow, side, i;

    if (filterContainer && previousItem) {
        settings = filterContainer->getSettings();
        // settings changed? Save them!
        if (settings != previousItem->data(ImagePreferences).toMap()) {
            previousItem->setData(ImagePreferences, settings);

            // Propagate settings according to settings policy
            switch (ui->settingPolicy->currentIndex()) {
                case 1: // propagate to all following images in this side
                    fromRow = ui->images->row(previousItem);
                    side = ui->images->column(previousItem);
                    for (i = fromRow; i < itemCount[side]; i++) {
                        ui->images->item(i, side)->setData(ImagePreferences, settings);
                    }
                    break;
                case 2: // propagate to all images in this side
                    side = ui->images->column(previousItem);
                    for (i = 0; i < itemCount[side]; i++) {
                        ui->images->item(i, side)->setData(ImagePreferences, settings);
                    }
                    break;
                //case 0: do not propagate;
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
    int i, currentRow;

    //NOTE: loading all icons at this time implies waiting time for the user.
    //Alternatives: put some "waiting" dialog or do ICON-loading in the background
    icon = QPixmap(fileName).scaledToWidth(100);
    item = new QTableWidgetItem(QIcon(icon),
                        fi.fileName());

    // Adjust Table size if necessary
    if (itemCount[side] >=  ui->images->rowCount()) {
        ui->images->setRowCount(itemCount[side] + 1);
    }

    currentItem = ui->images->currentItem();
    if (currentItem && ui->images->currentColumn() == side) {
        // Insert before current item
        currentRow = ui->images->currentRow();
    } else {
        // Insert at the End
        currentRow = itemCount[side];
    }

    // move all items after current item one step down
    for (i = itemCount[side]; i > currentRow; i--) {
        ui->images->setItem(i, side, ui->images->takeItem(i - 1, side));
    }

    ui->images->setItem(currentRow, side, item);
    item->setData(ImageFileName, fileName);
    item->setData(ImagePreferences, settings);
    item->setToolTip(fileName);
    itemCount[side] = itemCount[side] + 1;

    // Select the inserted item
    ui->images->setCurrentItem(item);
    // scroll down to next item for order by multiple inserts?
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

void ImageTableWidget::removeSelected()
{
    int currentRow = ui->images->currentRow();
    int currentColumn = ui->images->currentColumn();
    if (currentColumn < 0 || currentColumn > 1)
        return;
    int otherSide = 1 - currentColumn;
    int i;

    if (currentRow >=0
            && currentRow >= itemCount[currentColumn])
        return; //Nothing to delete

    delete ui->images->takeItem(currentRow, currentColumn);
    for (i = currentRow; i < itemCount[currentColumn] - 1; i++) {
        ui->images->setItem(i, currentColumn,
                            ui->images->takeItem(i + 1, currentColumn));
    }
    itemCount[currentColumn]--;
    if (itemCount[currentColumn] >= itemCount[otherSide]) {
        ui->images->setRowCount(itemCount[currentColumn]);
    }

    // if last column removed, scroll one up
    if (currentRow == itemCount[currentColumn] && currentRow > 0) {
        ui->images->setCurrentCell(currentRow - 1, currentColumn);
    }

    // redraw
    currentItemChanged(ui->images->currentItem(), NULL);
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
            imageSize = filterContainer->getImageSize();
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
            imageSize = filterContainer->getImageSize();
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


