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
#include "basefiltergraphicsview.h"
#include <QWheelEvent>
#include <math.h>
#include <QDebug>

BaseFilterGraphicsView::BaseFilterGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    scene = new QGraphicsScene();
    setScene(scene);

    pixmapItem = new QGraphicsPixmapItem();
    scene->addItem(pixmapItem);
}

BaseFilterGraphicsView::~BaseFilterGraphicsView()
{
    delete scene; // this includes all items in the scene so pixmapItem must not be explicitly deleted.
}

//TODO: add another possibility to zoom (buttons, ctrl+-..).
void
BaseFilterGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        double factor = pow(1.125, numSteps);
        scale(factor, factor);
    }
    // If one would want to scroll horizintaly with Shift instead of Alt (Qt Default),
    // else if (event->modifiers().testFlag(Qt::ShiftModifier))
    // We should create a new Event with a changed modifier Qt::AltModifier
    // But this is Qt Default, so we stay by Qt Default (Alt+Scroll) for now.
    else {
        QGraphicsView::wheelEvent(event);
    }
}

void BaseFilterGraphicsView::setPixmap(const QPixmap pixmap)
{
    scene->setSceneRect(pixmap.rect());
    pixmapItem->setPixmap(pixmap);

    /* Zoom the QGraphicsView to fit the new Pixmap */
    fitInView(pixmapItem, Qt::KeepAspectRatio);
}
