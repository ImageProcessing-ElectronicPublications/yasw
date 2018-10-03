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
#include "basefilter.h"

/*! \class BaseFilter

  \brief The BaseFilter class is the model for all Filter applied on pages.

  It should be inherited by every Filter. The PageFilter class is functional but does nothing, and
  returns the pixmap unmodified.

  Some functions are definied as virtual in order to allow the calling class use a unique interface
  (all Filter can be stored as a "BaseFilter" Class).
*/

/*! \brief  Constructs a Filter
*/
BaseFilter::BaseFilter(QObject * parent) : QObject(parent)
{
    widget = new BaseFilterWidget();
    filterWidget = widget;
}

BaseFilter::~BaseFilter()
{
    delete widget;
}


/*! \brief Set input Page

  This function is called by the calling class to set the Pixmap to be "filtered".
*/
void BaseFilter::setImage(QPixmap pixmap)
{
    inputPixmap = pixmap;
    filterWidget->setPixmap(pixmap);
    recalculate();
}

/*! \brief Returns the transformed page

  @returns The transformed page, or a null Pixmap if no page is available
*/
QPixmap BaseFilter::getFilteredImage()
{
    return outputPixmap;
}

/*! \brief Gets the widget to display the filter

    The returned widget must not be freed, it is handled by the class destructor.
    @returns The Widget for this Filter.
  */
AbstractFilterWidget* BaseFilter::getWidget()
{
    return widget;
}

/** \brief Returns a universal name for this filter.

 This identifier is unique for the filter. It can be used to identify the
 filter used (for example in configuration files)
 */
QString BaseFilter::getIdentifier()
{
    return QString("BaseFilter");
}

/*! \brief Retruns the name of the filter */
QString BaseFilter::getName()
{
    return tr("Base Filer");
}

/*! \brief Recalculate the output Pixmap

    In the case of BaseFilter: does nothing as the filter does nothing.
*/
void BaseFilter::recalculate()
{
    outputPixmap = inputPixmap;
}

/*! \brief virtual function to get the Filter settings

    @returns A QMap of keys (QString) to settings (QVariant).
*/
QMap<QString, QVariant> BaseFilter::getSettings()
{
    /* As BaseFilter does nothing, there is nothing to save */
    return QMap<QString, QVariant>();
}

/** \brief virtual function do set the Filter settings.

  If a needed setting is not available in settings, a default has to be set.
*/
void BaseFilter::setSettings(QMap<QString, QVariant> /* settings */)
{
    /* Ignore settings, as there is nothing to set */
}




