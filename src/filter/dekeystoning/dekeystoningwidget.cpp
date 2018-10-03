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
#include "dekeystoningwidget.h"
#include "ui_dekeystoningwidget.h"

DekeystoningWidget::DekeystoningWidget(QWidget *parent) :
    AbstractFilterWidget(parent),
    ui(new Ui::DekeystoningWidget)
{
    ui->setupUi(this);
}

DekeystoningWidget::~DekeystoningWidget()
{
    delete ui;
}

void DekeystoningWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DekeystoningWidget::setPixmap(QPixmap pixmap)
{
    inputPixmap = pixmap;
    if (!preview())
        ui->view->setPixmap(pixmap);
}

void DekeystoningWidget::setPreview(QPixmap pixmap)
{
    previewPixmap = pixmap;
    if (preview())
        ui->view->setPixmap(pixmap);
}

qreal DekeystoningWidget::meanWidth()
{
    return ui->view->meanWidth();
}

qreal DekeystoningWidget::meanHeight()
{
    return ui->view->meanHeight();
}

QPolygonF DekeystoningWidget::polygon()
{
    return ui->view->polygon();
}

bool DekeystoningWidget::preview()
{
    return ui->preview->isChecked();
}

void DekeystoningWidget::on_preview_toggled(bool checked)
{
    if (checked) {
        ui->view->resetPolygonMoved();
        emit polygonChanged();
        ui->view->setPixmap(previewPixmap);
    } else {
        ui->view->setPixmap(inputPixmap);
    }
}

bool DekeystoningWidget::polygonMoved()
{
    return ui->view->polygonMoved();
}

void DekeystoningWidget::resetPolygonMoved()
{
    return ui->view->resetPolygonMoved();
}

/** \brief Get the filter settings

    NOTE: Currently does only save the polygon coordinates; one may want to save
    activated and preview.
*/
QMap<QString, QVariant> DekeystoningWidget::getSettings()
{
    return ui->view->getSettings();
}

/** \brief sets the filter settings (change polygon coordinates)

    If the settings are not present, sets default values

    NOTE: Currently does only save the polygon coordinates; one may want to load
    activated and preview settings (if present).
*/
void DekeystoningWidget::setSettings(QMap<QString, QVariant> settings)
{
    ui->view->setSettings(settings);
}

/** \brief Sets new Selection Color

  This does only forward the information, as this widget ist the only one able to call
  members functions of ui->view.
 */
void DekeystoningWidget::setSelectionColor(QColor color)
{
    ui->view->setSelectionColor(color);
}

void DekeystoningWidget::setBackgroundColor(QColor color)
{
    ui->view->setBackgroundBrush(QBrush(color));
}


