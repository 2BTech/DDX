################################################################################
##                         DATA DISPLAY APPLICATION X                         ##
##                            2B TECHNOLOGIES, INC.                           ##
##                                                                            ##
## The DDX is free software: you can redistribute it and/or modify it under   ##
## the terms of the GNU General Public License as published by the Free       ##
## Software Foundation, either version 3 of the License, or (at your option)  ##
## any later version.  The DDX is distributed in the hope that it will be     ##
## useful, but WITHOUT ANY WARRANTY; without even the implied warranty of     ##
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General  ##
## Public License for more details.  You should have received a copy of the   ##
## GNU General Public License along with the DDX.  If not, see                ##
## <http://www.gnu.org/licenses/>.                                            ##
##                                                                            ##
##  For more information about the DDX, check out the 2B website or GitHub:   ##
##       <http://twobtech.com/DDX>       <https://github.com/2BTech/DDX>      ##
################################################################################


#-------------------------------------------------
#
# Project created by QtCreator 2015-04-29T16:35:09
#
#-------------------------------------------------

QT       += core \
			network \
			serialport \
			bluetooth \
			widgets \ # For tray icons & messages
			testlib  # For temporary testing

QT       -= gui

TARGET = DDX-daemon
CONFIG   += c++11
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    daemon.cpp \
    inlet.cpp \
    path.cpp \
    module.cpp \
    modules/genmod.cpp \
    modules/examplemodule.cpp \
    modules/module_register.cpp \
    network.cpp \
    settings.cpp \
    logger.cpp \
    modules/exampleinlet.cpp \
    remdev.cpp \
    netdev.cpp \
    pathmanager.cpp

HEADERS += \
    constants.h \
    ../NoGit/private_constants.h \
    daemon.h \
    inlet.h \
    path.h \
    module.h \
    data.h \
    modules/genmod.h \
    modules/examplemodule.h \
    network.h \
    settings.h \
    logger.h \
    modules/exampleinlet.h \
    remdev.h \
    netdev.h \
    pathmanager.h \
    rapidjson.h

DISTFILES += \
    DaemonCommandLineArguments.txt \
    DevNotes.txt \
    DataFlow.txt \
    RemoteManagementProtocol.md

RESOURCES += res/resources.qrc

# Release build optimizations
QMAKE_CFLAGS_RELEASE -= -O
QMAKE_CFLAGS_RELEASE -= -O1
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE *= -O3
QMAKE_CXXFLAGS_RELEASE *= -O3
#QMAKE_CXXFLAGS_RELEASE *= -Os

