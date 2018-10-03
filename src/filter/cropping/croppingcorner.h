/*
 * Copyright (C) 2012 Robert Chéramy (robert@cheramy.net)
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

#ifndef CROPPINGCORNER_H
#define CROPPINGCORNER_H

#include <QGraphicsEllipseItem>

enum CornerType { TopLeftCorner, BottomRightCorner };

/*! Inherits QObject to be able to emit signals */
class CroppingCorner : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    CroppingCorner(QPoint position);
    bool getCornerMoved();
    void resetCornerMoved();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    QRect *rectangle;
    bool cornerMoved;
    const int diameter = 8;
signals:
    void signalCornerMoved();
};

#endif // CROPPINGCORNER_H
