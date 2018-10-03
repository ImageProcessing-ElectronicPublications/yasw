/*
 * Copyright (C) 2014 Robert Chéramy (robert@cheramy.net)
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

#include "constants.h"

QStringList Constants::displayUnits = QStringList() << "pixel"
                                                    << "milimeter"
                                                    << "inch";
QStringList Constants::dpiList = QStringList() << "150"
                                               << "300"
                                               << "600";

qreal const Constants::milimeterPerInch = 25.4;     // 1 inch = 25,4 mm


QStringList Constants::horizontalAlignment = QStringList() << "Left"
                                                  << "Center"
                                                  << "Right";
QStringList Constants::verticalAlignment = QStringList() << "Top"
                                                  << "Center"
                                                  << "Bottom";

QRegExp Constants::trailZero = QRegExp("0+$");
QRegExp Constants::trailDot = QRegExp("\\.$");


QString Constants::float2String(qreal n, int precision)
{
    QString str = QString::number(n, 'f', precision);
    str.remove(trailZero);
    str.remove(trailDot);

    return str;
}
