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

#ifndef IMAGETABLEWIDGET_H
#define IMAGETABLEWIDGET_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QtXml/QDomDocument>
#include "filtercontainer.h"

namespace Ui {
class ImageTableWidget;
}

class ImageTableWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ImageTableWidget(QWidget *parent = 0);
    ~ImageTableWidget();
    void setFilterContainer(FilterContainer *container);
    // save YASW into XML
    void saveProjectParameters(QDomDocument &doc, QDomElement &parent);
    // load XML int YASW
    bool loadProjectParameters(QDomElement &rootElement);
    void clear();
    void exportToFolder(QString folder);
    void exportToPdf(QString pdfFile, int DPI);

public slots:
    void currentItemChanged(QTableWidgetItem *newItem, QTableWidgetItem *previousItem);
    void insertImage();
    void insertEmptyImage();
    void imageUp();
    void imageDown();
    void removeSelected();
    void moveImageLeft();
    void moveImageRight();
    void selectPreviousImage();
    void selectNextImage();
    void selectRightImage();
    void selectLeftImage();

private:
    Ui::ImageTableWidget *ui;
    enum ImageSide { leftSide, rightSide };
    FilterContainer *filterContainer;
    QString lastDir = "";
    // stores the last row for left and right images
    int itemCount[2];

    void addImage(QString fileName, enum ImageSide side,
                  QMap<QString, QVariant> settings = QMap<QString, QVariant> ());
    void appendImageToSide(QString fileName, ImageTableWidget::ImageSide side,
                           QMap<QString, QVariant> settings);
    enum ImageTableUserRoles { ImagePreferences = Qt::UserRole,
                              ImageFileName
                            };
    void addClicked(ImageTableWidget::ImageSide side);
    QTableWidgetItem * takeItem(int row, int side);
    void insertItem(QTableWidgetItem * item, int row, int side);

private slots:
    void on_btnPropagateFollowingSameSide_clicked();
    void on_btnPropagateAllSameSide_clicked();
    void on_btnPropagateAll_clicked();
};

#endif // IMAGETABLEWIDGET_H
