#ifndef SCALINGWIDGET_H
#define SCALINGWIDGET_H

#include <QWidget>
#include "abstractfilterwidget.h"
#include <QDoubleValidator>


namespace Ui {
class ScalingWidget;
}

class ScalingWidget : public AbstractFilterWidget
{
    Q_OBJECT
    
public:
    explicit ScalingWidget(QWidget *parent = 0);
    ~ScalingWidget();
    
    void setPixmap(QPixmap pixmap);
    void setPreview(QPixmap pixmap);
    bool preview();
    double imageHeight();
    double imageWidth();
    QMap<QString, QVariant> getSettings();
    void setSettings(QMap <QString, QVariant> settings);

public slots:
    void setBackgroundColor(QColor color);

private slots:
    void on_preview_toggled(bool checked);
    void slotPropertyChanged();

    void on_unit_currentIndexChanged(int index);

signals:
    void signalPropertyChanged();


private:
    Ui::ScalingWidget *ui;
    QDoubleValidator *doubleValidator;
    int lastUnitIndex = 0;
};

#endif // SCALINGWIDGET_H
