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
#include "colorcorrectiongraphicsview.h"
#include <QImage>
#include <QDebug>
#include <QWheelEvent>


ColorCorrectionGraphicsView::ColorCorrectionGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    scene = new ColorCorrectionGraphicsScene();
    setScene(scene);

    pixmapItem = scene->pixmapItem();
    scene->addItem(pixmapItem);

    // forward the signal
    connect (scene, SIGNAL(pixmapClicked(QColor)),
             this, SLOT(colorFromScene(QColor)));
}

ColorCorrectionGraphicsView::~ColorCorrectionGraphicsView()
{
    delete scene;
}

// FIXME: I would have prefered to inheritate this method from BaseFilterGraphicsView...
void
ColorCorrectionGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        double factor = pow(1.125, numSteps);
        scale(factor, factor);
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

// Must reimplement as scene is another Class.
void ColorCorrectionGraphicsView::setPixmap(const QPixmap pixmap)
{
    scene->setSceneRect(pixmap.rect());
    pixmapItem->setPixmap(pixmap);

    /* Zoom the QGraphicsView to fit the new Pixmap */
    fitInView(pixmapItem, Qt::KeepAspectRatio);
}

void ColorCorrectionGraphicsView::colorFromScene(QColor color)
{
    emit pixmapClicked(color);
}


