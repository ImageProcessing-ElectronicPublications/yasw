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
#ifndef FILTERCONTAINER_H
#define FILTERCONTAINER_H

#include <QTabWidget>
#include <QMap>
#include <QVariant>
#include <QString>
#include <QtXml/QDomDocument>
#include "basefilter.h"
#include "abstractfilterwidget.h"
#include "scalefilter.h"

class FilterContainer : public QTabWidget
{
    Q_OBJECT
public:
    FilterContainer(QWidget * parent = 0);
    ~FilterContainer();

    QMap<QString, QVariant> getSettings();
    void setSettings(QMap<QString, QVariant> settings);
    void settings2Dom(QDomDocument &doc, QDomElement &imageElement, QMap<QString, QVariant> settings);
    QMap<QString, QVariant> dom2Settings(QDomElement &imageElement);
    QPixmap getResultImage();
    QString currentFilter();
    void setImage(QPixmap pixmap);

public slots:
    void tabChanged(int index);
    void setSelectionColor(QColor color);
    void setBackgroundColor(QColor color);
    void setDisplayUnit(QString unit);
    void setDPI(int dpi);

private:
    QList<BaseFilter *> tabToFilter;
    int oldIndex = 0; //stores the last selected index, at init = first tab

signals:
    // Propagates changes to global configuration paramters
    void selectionColorChanged(QColor color);
    void backgroundColorChanged(QColor color);
    void displayUnitChanged(QString unit);
    void dpiChanged(int dpi);
};
#endif // FILTERCONTAINER_H
