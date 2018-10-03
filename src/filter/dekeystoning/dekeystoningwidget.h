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
#ifndef DEKEYSTONINGWIDGET_H
#define DEKEYSTONINGWIDGET_H

#include <QWidget>
#include "abstractfilterwidget.h"

namespace Ui {
    class DekeystoningWidget;
}

class DekeystoningWidget : public AbstractFilterWidget {
    Q_OBJECT
public:
    DekeystoningWidget(QWidget *parent = 0);
    ~DekeystoningWidget();
    void setPixmap(QPixmap pixmap);
    void setPreview(QPixmap pixmap);
    bool preview();
    qreal meanWidth();
    qreal meanHeight();
    QPolygonF polygon();
    bool polygonMoved();
    void resetPolygonMoved();
    QMap<QString, QVariant> getSettings();
    void setSettings(QMap <QString, QVariant> settings);
    void enableFilter(bool enable);


public slots:
    void setSelectionColor(QColor color);
    void setBackgroundColor(QColor color);
    void gvParameterChanged();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DekeystoningWidget *ui;

private slots:
    void on_preview_toggled(bool checked);
    void on_enable_toggled(bool checked);
};

#endif // DEKEYSTONINGWIDGET_H
