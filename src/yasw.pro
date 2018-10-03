# Copyright (C) 2012 Robert Chéramy (robert@cheramy.net)
# 
# This file is part of YASW (Yet Another Scan Wizard).
# 
# YASW is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# YASW is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with YASW.  If not, see <http://www.gnu.org/licenses/>.

# Disable warning about non-static data member initializers.
# No idea if this works on other platforms, old platforms will probably do not support it.
QMAKE_CXXFLAGS += -std=c++11
TARGET = yasw
TEMPLATE = app
QT += xml
QT += widgets
QT += printsupport
SOURCES += main.cpp \
    mainwindow.cpp \
    filter/basefilter.cpp \
    filter/basefiltergraphicsview.cpp \
    filter/basefilterwidget.cpp \
    filtercontainer.cpp \
    filter/dekeystoning/dekeystoningwidget.cpp \
    filter/dekeystoning/dekeystoning.cpp \
    filter/rotation/rotationwidget.cpp \
    filter/rotation/rotation.cpp \
    filter/abstractfilterwidget.cpp \
    filter/cropping/cropping.cpp \
    filter/cropping/croppingwidget.cpp \
    filter/cropping/croppinggraphicsview.cpp \
    filter/cropping/croppingcorner.cpp \
    filter/dekeystoning/dekeystoningline.cpp \
    filter/dekeystoning/dekeystoningcorner.cpp \
    imagetablewidget.cpp \
    filter/scalewidget.cpp \
    preferencesdialog.cpp \
    filter/colorcorrectionwidget.cpp \
    filter/colorcorrection.cpp \
    filter/dekeystoning/dekeystoninggraphicsview.cpp \
    filter/colorcorrectiongraphicsview.cpp \
    filter/colorcorrectiongraphicsscene.cpp \
    constants.cpp \
    filter/layoutfilter.cpp \
    filter/layoutwidget.cpp \
    filter/scalefilter.cpp
HEADERS += mainwindow.h \
    filter/basefilter.h \
    filter/basefiltergraphicsview.h \
    filter/basefilterwidget.h \
    filtercontainer.h \
    filter/dekeystoning/dekeystoningwidget.h \
    filter/dekeystoning/dekeystoninggraphicsview.h \
    filter/dekeystoning/dekeystoning.h \
    filter/rotation/rotationwidget.h \
    filter/rotation/rotation.h \
    filter/abstractfilterwidget.h \
    filter/cropping/cropping.h \
    filter/cropping/croppingwidget.h \
    filter/cropping/croppinggraphicsview.h \
    filter/cropping/croppingcorner.h \
    filter/dekeystoning/dekeystoningline.h \
    filter/dekeystoning/dekeystoningcorner.h \
    imagetablewidget.h \
    filter/scalewidget.h \
    filter/layoutfilter.h \
    filter/layoutwidget.h \
    preferencesdialog.h \
    filter/colorcorrectionwidget.h \
    filter/colorcorrection.h \
    filter/colorcorrectiongraphicsview.h \
    filter/colorcorrectiongraphicsscene.h \
    constants.h \
    filter/scalefilter.h
FORMS += mainwindow.ui \
    filter/basefilterwidget.ui \
    filter/dekeystoning/dekeystoningwidget.ui \
    filter/rotation/rotationwidget.ui \
    filter/cropping/croppingwidget.ui \
    imagetablewidget.ui \
    preferencesdialog.ui \
    filter/scalewidget.ui \
    filter/layoutwidget.ui \
    filter/colorcorrectionwidget.ui
INCLUDEPATH += filter \
    filter/dekeystoning \
    filter/rotation \
    filter/cropping
RESOURCES += icons/icons.qrc

OTHER_FILES += \
    ../changelog.txt \
    ../install.txt \
    ../gpl.txt \
    ../developpers.txt \
    ../README

static {
## Everything below takes effect when calling qmake with CONFIG += static
## thanks to Charles N. Burns, http://www.formortals.com/how-to-statically-link-qt-4/ for the help
## Update Qt5: Q_IMPORT_PLUGIN(qsvg) in main.c is not needed anymore; QT += svg neither
    QTPLUGIN += qsvg
    DEFINES += STATIC
    message ("This is a static build")
}

