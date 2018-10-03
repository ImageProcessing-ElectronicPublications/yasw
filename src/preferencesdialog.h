#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QColor>
#include <QSettings>

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
    
private slots:
    void on_selectionColorButton_clicked();

    void on_backgroundColorButton_clicked();

private:
    void setSelectionColor(QColor color);
    void setBackgroundColor(QColor color);

    Ui::PreferencesDialog *ui;
    QColor selectionColor;
    QSettings *settings = NULL;
    QColor backgroundColor;

signals:
    void selectionColorChanged(QColor color);
    void backgroundColorChanged(QColor color);

};

#endif // PREFERENCESDIALOG_H
