#ifndef CROPPINGWIDGET_H
#define CROPPINGWIDGET_H

#include "abstractfilterwidget.h"

namespace Ui {
    class CroppingWidget;
}

class CroppingWidget : public AbstractFilterWidget
{
    Q_OBJECT

public:
    explicit CroppingWidget(QWidget *parent = 0);
    ~CroppingWidget();
    void setPixmap(QPixmap pixmap);
    void setPreview(QPixmap pixmap);
    bool preview();
    QRect rectangle();
    bool rectangleMoved();
    void resetRectangleMoved();
    QMap<QString, QVariant> getSettings();
    void setSettings(QMap <QString, QVariant> settings);
    void enableFilter(bool enable);

public slots:
    void setSelectionColor(QColor color);
    void setBackgroundColor(QColor color);
    void gvParameterChanged();

private:
    Ui::CroppingWidget *ui;

private slots:
    void on_preview_toggled(bool checked);
    void on_enable_toggled(bool checked);
};

#endif // CROPPINGWIDGET_H
