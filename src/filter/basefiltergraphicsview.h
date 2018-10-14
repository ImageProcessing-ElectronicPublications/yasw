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
#ifndef BASEFILTERGRAPHICSVIEW_H
#define BASEFILTERGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class BaseFilterGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    BaseFilterGraphicsView(QWidget *parent);
    ~BaseFilterGraphicsView();
    void setPixmap(const QPixmap pixmap);
protected:
    void wheelEvent(QWheelEvent *event);
    QGraphicsScene *scene = NULL;
    QGraphicsPixmapItem *pixmapItem = NULL;
};

#endif // BASEFILTERGRAPHICSVIEW_H
