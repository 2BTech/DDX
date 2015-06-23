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
			serialport \
			bluetooth \
			widgets # For tray icons & messages

QT       -= gui

TARGET = DDX-daemon
CONFIG   += c++11
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    daemon.cpp \
    inlet.cpp \
    outlet.cpp \
    path.cpp

HEADERS += \
    ../DDX-gui/constants.h \
    ../NoGit/private_constants.h \
    daemon.h \
    inlet.h \
    outlet.h \
    path.h

DISTFILES += \
    DaemonCommandLineArguments.txt \
    DevNotes.txt \
    DataFlow.txt

RESOURCES += res/resources.qrc
