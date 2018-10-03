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
#include <QColorDialog>
#include <QColor>
#include <QPalette>
#include <QDebug>
#include <QLineEdit>

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "constants.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    setSelectionColor(Qt::red);
    setBackgroundColor(Qt::lightGray);

    ui->unit->insertItems(0, Constants::displayUnits);
    setDisplayUnit("pixel");

    QIntValidator *dpiValidator = new QIntValidator();
    dpiValidator->setBottom(Constants::MIN_DPI);
    ui->dpi->setValidator(dpiValidator);

    connect(ui->dpi->lineEdit(), SIGNAL(editingFinished()),
            this, SLOT(dpiFormChanged()));

    ui->dpi->insertItems(0, Constants::dpiList);
    setDPI(Constants::DEFAULT_DPI);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::setSettings(QSettings *newSettings)
{
    QColor color;
    settings = newSettings;

    color = QColor(settings->value("selectionColor").toString());
    if (!color.isValid()) {
        color = selectionColor;
    }
    setSelectionColor(color);

    color = QColor(settings->value("backgroundColor").toString());
    if (!color.isValid()) {
        color = backgroundColor;
    }
    setBackgroundColor(color);

    QString unit = settings->value("displayUnit").toString();
    setDisplayUnit(unit);
}

QString PreferencesDialog::displayUnit()
{
    return ui->unit->currentText();
}

void PreferencesDialog::setDPI(int newDpi)
{
    if (newDpi <= Constants::MIN_DPI) // we only accept positive DPI
        return;

    dpi = newDpi;

    QString dpiString = QString::number(dpi, 'f', 0);
    if (Constants::dpiList.contains(dpiString)) {
        ui->dpi->setCurrentIndex(Constants::dpiList.indexOf(dpiString));
    } else {
        ui->dpi->setEditText(dpiString);
    }

    emit(dpiChanged(dpi));
}

int PreferencesDialog::DPI()
{
    return dpi;
}

void PreferencesDialog::saveProjectParameters(QDomDocument &doc, QDomElement &rootElement)
{
    QDomElement parameter = doc.createElement("global");
    parameter.setAttribute("DPI", dpi);
    rootElement.appendChild(parameter);
}

bool PreferencesDialog::loadProjectParameters(QDomElement &rootElement)
{
    QDomElement parameter = rootElement.firstChildElement("global");
    if (parameter.isNull())
        return false;
    setDPI(parameter.attribute("DPI", QString::number(Constants::DEFAULT_DPI, 'f', 0)).toInt());
    return true;
}

// We don't want the dialog to close when pressing enter or the user might not see that his DPI
// has not been taken into account
void PreferencesDialog::keyPressEvent(QKeyEvent *evt)
{
    if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
        return;
    QDialog::keyPressEvent(evt);
}

void PreferencesDialog::on_selectionColorButton_clicked()
{
    QColor color;

    color = QColorDialog::getColor(selectionColor, this, tr("Choose a color for visual selection"));
    setSelectionColor(color);
}

/** \brief Sets the Color for grafical selection feedback (example deykeystoning quadrilateral)

  This function emits a selectionColorChanged signal, so that the filtercontainer can
  transmit the information to the plugings, who should change the color used.
*/
void PreferencesDialog::setSelectionColor(QColor color)
{
    QPalette palette;

    if (!color.isValid()) {
        return;
    }

    selectionColor = color;
    palette.setColor(QPalette::Button, color);
    ui->selectionColorButton->setPalette(palette);

    if (settings) {
        settings->setValue("selectionColor", color.name());
    }

    emit(selectionColorChanged(color));
}

void PreferencesDialog::setBackgroundColor(QColor color)
{
    QPalette palette;

    if (!color.isValid()) {
        return;
    }

    backgroundColor = color;
    palette.setColor(QPalette::Button, color);
    ui->backgroundColorButton->setPalette(palette);

    if (settings) {
        settings->setValue("backgroundColor", color.name());
    }

    emit(backgroundColorChanged(color));
}

void PreferencesDialog::setDisplayUnit(QString unit)
{
    if (!Constants::displayUnits.contains(unit)) // unit unknown? Do nothing.
        return;

    ui->unit->setCurrentIndex(Constants::displayUnits.indexOf(unit));

    if (settings) {
        settings->setValue("displayUnit", unit);
    }
}

void PreferencesDialog::on_backgroundColorButton_clicked()
{
    QColor color;

    color = QColorDialog::getColor(backgroundColor, this, tr("Choose a background color"));
    setBackgroundColor(color);
}

void PreferencesDialog::on_unit_currentIndexChanged(const QString &unit)
{
    if (settings) {
        settings->setValue("displayUnit", unit);
    }

    emit(displayUnitChanged(unit));
}

void PreferencesDialog::on_dpi_editTextChanged(const QString &stringDPI)
{
    qreal dpi = stringDPI.toDouble();

    if (dpi < Constants::MIN_DPI) {
        ui->dpi->setStyleSheet("background-color: rgb(255, 0, 0);");
    } else {
        ui->dpi->setStyleSheet("");
    }
}

void PreferencesDialog::dpiFormChanged()
{
    int newDPI = ui->dpi->currentText().toInt();

    if (dpi == newDPI) // do nothing when nothing changes.
        return;

    if (newDPI < Constants::MIN_DPI) {  // We don't accept small DPI, but this should never happen (Validator on Form)
        return;
    }

    dpi = newDPI;

    emit dpiChanged(dpi);
}
