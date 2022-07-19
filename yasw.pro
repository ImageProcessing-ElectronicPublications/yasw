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
QMAKE_CXXFLAGS += -Isrc -std=c++11
QMAKE_LDFLAGS += -lm
TARGET = yasw
TEMPLATE = app
QT += xml widgets printsupport
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/filter/basefilter.cpp \
    src/filter/basefiltergraphicsview.cpp \
    src/filter/basefilterwidget.cpp \
    src/filtercontainer.cpp \
    src/filter/dekeystoning/dekeystoningwidget.cpp \
    src/filter/dekeystoning/dekeystoning.cpp \
    src/filter/rotation/rotationwidget.cpp \
    src/filter/rotation/rotation.cpp \
    src/filter/abstractfilterwidget.cpp \
    src/filter/cropping/cropping.cpp \
    src/filter/cropping/croppingwidget.cpp \
    src/filter/cropping/croppinggraphicsview.cpp \
    src/filter/cropping/croppingcorner.cpp \
    src/filter/dekeystoning/dekeystoningline.cpp \
    src/filter/dekeystoning/dekeystoningcorner.cpp \
    src/imagetablewidget.cpp \
    src/filter/scalewidget.cpp \
    src/preferencesdialog.cpp \
    src/filter/colorcorrectionwidget.cpp \
    src/filter/colorcorrection.cpp \
    src/filter/dekeystoning/dekeystoninggraphicsview.cpp \
    src/filter/colorcorrectiongraphicsview.cpp \
    src/filter/colorcorrectiongraphicsscene.cpp \
    src/constants.cpp \
    src/filter/layoutfilter.cpp \
    src/filter/layoutwidget.cpp \
    src/filter/scalefilter.cpp
HEADERS += \
    src/mainwindow.h \
    src/filter/basefilter.h \
    src/filter/basefiltergraphicsview.h \
    src/filter/basefilterwidget.h \
    src/filtercontainer.h \
    src/filter/dekeystoning/dekeystoningwidget.h \
    src/filter/dekeystoning/dekeystoninggraphicsview.h \
    src/filter/dekeystoning/dekeystoning.h \
    src/filter/rotation/rotationwidget.h \
    src/filter/rotation/rotation.h \
    src/filter/abstractfilterwidget.h \
    src/filter/cropping/cropping.h \
    src/filter/cropping/croppingwidget.h \
    src/filter/cropping/croppinggraphicsview.h \
    src/filter/cropping/croppingcorner.h \
    src/filter/dekeystoning/dekeystoningline.h \
    src/filter/dekeystoning/dekeystoningcorner.h \
    src/imagetablewidget.h \
    src/filter/scalewidget.h \
    src/filter/layoutfilter.h \
    src/filter/layoutwidget.h \
    src/preferencesdialog.h \
    src/filter/colorcorrectionwidget.h \
    src/filter/colorcorrection.h \
    src/filter/colorcorrectiongraphicsview.h \
    src/filter/colorcorrectiongraphicsscene.h \
    src/constants.h \
    src/filter/scalefilter.h
FORMS += \
    src/mainwindow.ui \
    src/filter/basefilterwidget.ui \
    src/filter/dekeystoning/dekeystoningwidget.ui \
    src/filter/rotation/rotationwidget.ui \
    src/filter/cropping/croppingwidget.ui \
    src/imagetablewidget.ui \
    src/preferencesdialog.ui \
    src/filter/scalewidget.ui \
    src/filter/layoutwidget.ui \
    src/filter/colorcorrectionwidget.ui
INCLUDEPATH += \
    src/filter \
    src/filter/dekeystoning \
    src/filter/rotation \
    src/filter/cropping
RESOURCES += src/icons/icons.qrc

OTHER_FILES += \
    changelog.txt \
    install.txt \
    developpers.txt \
    LICENSE \
    README.md

# build dir
UI_DIR =      ./build
MOC_DIR =     ./build
RCC_DIR =     ./build
OBJECTS_DIR = ./build
mytarget.commands += $${QMAKE_MKDIR} ./build

static {
## Everything below takes effect when calling qmake with CONFIG += static
## thanks to Charles N. Burns, http://www.formortals.com/how-to-statically-link-qt-4/ for the help
## Update Qt5: Q_IMPORT_PLUGIN(qsvg) in main.c is not needed anymore; QT += svg neither
    QTPLUGIN += qsvg
    DEFINES += STATIC
    message ("This is a static build")
}

