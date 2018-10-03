#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include <QColorDialog>
#include <QColor>
#include <QPalette>
#include <QDebug>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    setSelectionColor(Qt::red);
    setBackgroundColor(Qt::lightGray);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

/* TODO: is this the right way to pass the settings? What I need is a reference */
void PreferencesDialog::setSettings(QSettings *newSettings)
{
    QColor color;
    settings = newSettings;

    color = settings->value("selectionColor").value<QColor>();
    if (!color.isValid()) {
        color = selectionColor;
    }
    setSelectionColor(color);

    color = settings->value("backgroundColor").value<QColor>();
    if (!color.isValid()) {
        color = backgroundColor;
    }
    setBackgroundColor(color);
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
    palette.setColor(QPalette::ButtonText, color);
    ui->selectionColorButton->setPalette(palette);

    if (settings) {
        settings->setValue("selectionColor", color);
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
    palette.setColor(QPalette::ButtonText, color);
    ui->backgroundColorButton->setPalette(palette);

    if (settings) {
        settings->setValue("backgroundColor", color);
    }

    emit(backgroundColorChanged(color));
}

void PreferencesDialog::on_backgroundColorButton_clicked()
{
    QColor color;

    color = QColorDialog::getColor(backgroundColor, this, tr("Choose a background color"));
    setBackgroundColor(color);
}
