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

#ifndef LAYOUTWIDGET_H
#define LAYOUTWIDGET_H

#include <QWidget>
#include "abstractfilterwidget.h"
#include <QDoubleValidator>


namespace Ui {
class LayoutWidget;
}

class LayoutWidget : public AbstractFilterWidget
{
    Q_OBJECT
    
public:
    explicit LayoutWidget(QWidget *parent = 0);
    ~LayoutWidget();
    
    void setPixmap(QPixmap pixmap);
    void setPreview(QPixmap pixmap);
    bool preview();
    double pagePixelHeight();
    double pagePixelWidth();
    // NOTE: It might be cleaner to use an Enum referencing an array of char*...
    QString verticalAlignement();
    QString horizontalAlignement();

    QMap<QString, QVariant> getSettings();
    void setSettings(QMap <QString, QVariant> settings);
    void setDisplayUnit(QString unit);
    void enableFilter(bool enable);



public slots:
    void setBackgroundColor(QColor color);
    void setDPI(int newDpi);

private slots:
    void on_preview_toggled(bool checked);
    void on_pageWidth_editingFinished();
    void on_pageHeight_editingFinished();
    void on_pageWidth_textEdited(const QString &strValue);
    void on_pageHeight_textEdited(const QString &strValue);
    void on_horizontalAlignment_currentIndexChanged(int /*index*/);
    void on_verticalAlignment_currentIndexChanged(int /*index*/);

    void on_enable_toggled(bool checked);

private:
    void updateFormSizes();

    Ui::LayoutWidget *ui;
    QDoubleValidator *doubleValidator;
    int lastUnitIndex = 0;
    QString displayUnit;
    qreal factorPixeltoDisplayUnit = 1;

    qreal pxPageWidth = 0;
    qreal pxPageHeight = 0;
    qreal dpi = 1;
};

#endif // LAYOUTWIDGET_H
