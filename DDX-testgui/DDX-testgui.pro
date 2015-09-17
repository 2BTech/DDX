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
# Project created by QtCreator 2015-04-29T16:34:41
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DDX-gui
CONFIG   += c++11
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    remdev.cpp \
    devmgr.cpp \
    netdev.cpp \
    testdev.cpp \
	network.cpp

HEADERS  += mainwindow.h \
    remdev.h \
    devmgr.h \
    rapidjson_using.h \
    netdev.h \
    testdev.h \
	network.h \
    ddxrpc.h \
    testgui_constants.h

DISTFILES +=

# Windows: download SSL from https://slproweb.com/products/Win32OpenSSL.html, copy "lib" folder to out-of-git location
win32 {
	LIBS += -L"$$PWD/../NoGit/OpenSSL-Win32" -lssleay32
}

macx {
	#error(SSL not installed)
}

INCLUDEPATH += "$$PWD/../rapidjson/include/rapidjson"
