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
#ifndef ABSTRACTFILTERWIDGET_H
#define ABSTRACTFILTERWIDGET_H

#include <QWidget>


/* An abstract class for all Filter Widgets.

  This abstract class describes the members a Filter Widget must have.
  It has to be inherited by all Filter Widgets.

  A Filter Widget must provide a way to
  setPixmap - set the input image (for displaying it)
  setPreview - set the output image (for previewing the result)
  preview - inform if the preview is active
  parameterChanged - inform if parameter have been changed in the widget
*/


class AbstractFilterWidget : public QWidget
{
    Q_OBJECT
public:
    AbstractFilterWidget(QWidget *parent = 0);
    // Sets the input pixmap
    virtual void setPixmap(QPixmap pixmap) = 0;
    // Sets the output pixmap for preview
    virtual void setPreview(QPixmap pixmap) = 0;
    // true if preview is active
    virtual bool preview() = 0;
    virtual void enableFilter(bool enable) = 0;
protected:
    QPixmap inputPixmap;
    QPixmap previewPixmap;
signals:
    void parameterChanged();
    void enableFilterToggled(bool checked);
    void previewChecked();

};

#endif // ABSTRACTFILTERWIDGET_H
