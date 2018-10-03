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
#include "colorcorrectiongraphicsscene.h"
#include <QDebug>

ColorCorrectionGraphicsScene::ColorCorrectionGraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    privPixmapItem = new QGraphicsPixmapItem();
}

ColorCorrectionGraphicsScene::~ColorCorrectionGraphicsScene()
{
    delete privPixmapItem;
}

QGraphicsPixmapItem *ColorCorrectionGraphicsScene::pixmapItem()
{
    return privPixmapItem;
}

void ColorCorrectionGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint position = event->scenePos().toPoint();

    QImage image = privPixmapItem->pixmap().toImage();
    if (image.valid(position)) { // The click was isued on the image
        emit pixmapClicked(QColor(image.pixel(position)));
    }
}
