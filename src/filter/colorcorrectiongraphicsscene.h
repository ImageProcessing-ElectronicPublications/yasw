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
#ifndef COLORCORRECTIONGRAPHICSSCENE_H
#define COLORCORRECTIONGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QColor>
#include <QGraphicsSceneMouseEvent>

class ColorCorrectionGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ColorCorrectionGraphicsScene(QObject *parent = 0);
    ~ColorCorrectionGraphicsScene();
    QGraphicsPixmapItem *pixmapItem();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
signals:
    void pixmapClicked(QColor color);
private:
    QGraphicsPixmapItem *privPixmapItem = NULL;
    
};

#endif // COLORCORRECTIONGRAPHICSSCENE_H
