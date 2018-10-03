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
#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QColor>
#include <QSettings>
#include <QDomDocument>
#include <QDomElement>
#include <QKeyEvent>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();
    void setSettings(QSettings *newSettings);
    QString displayUnit();
    void setDPI(int newDpi);
    int DPI();

    // save YASW into XML
    void saveProjectParameters(QDomDocument &doc, QDomElement &rootElement);
    // load XML int YASW
    bool loadProjectParameters(QDomElement &rootElement);
    void keyPressEvent(QKeyEvent *evt);

public slots:
    void dpiFormChanged();
    
private slots:
    void on_selectionColorButton_clicked();
    void on_backgroundColorButton_clicked();
    void on_unit_currentIndexChanged(const QString &unit);
    void on_dpi_editTextChanged(const QString &stringDPI);


private:
    void setSelectionColor(QColor color);
    void setBackgroundColor(QColor color);
    void setDisplayUnit(QString unit);


    Ui::PreferencesDialog *ui;
    QColor selectionColor;
    QSettings *settings = NULL;
    QColor backgroundColor;

    int dpi = 0;

signals:
    void selectionColorChanged(QColor color);
    void backgroundColorChanged(QColor color);
    void displayUnitChanged(QString unit);
    void dpiChanged(int dpi);
};

#endif // PREFERENCESDIALOG_H
