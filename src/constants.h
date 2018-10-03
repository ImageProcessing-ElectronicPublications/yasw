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
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QStringList>
// QDebug included centraly, so that it can be commented out.
#include <QDebug>

static const char VERSION[] = "0.6";


class Constants
{
public:
    static QStringList displayUnits;
    static QStringList dpiList;
    static qreal const milimeterPerInch ; //= 25.4;

    static int const MIN_DPI = 10;
    static int const DEFAULT_DPI = 300;

    // Constants for Layout Filter & Widget
    enum horizintalAlignmentEnum {LeftHAlignment, CenterHAlignment, RightHAlignment};
    enum verticalAlignmentEnum {TopVAlignment, CenterVAlignment, BottomVAlignment};
    static QStringList horizontalAlignment;
    static QStringList verticalAlignment;

    static QString float2String(qreal n, int precision = 2);

private:
    // Code optimisation: define these QRegExp once and loading a project is much faster.
    static QRegExp trailZero;
    static QRegExp trailDot;
};



#endif // CONSTANTS_H
