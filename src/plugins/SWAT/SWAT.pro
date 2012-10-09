# This file is part of the StackWalker Analysis Tool (SWAT)
# Copyright (C) 2012-2012 Argo Navis Technologies, LLC
# Copyright (C) 2012-2012 University of Wisconsin
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

include(../plugins.pri)

include(SourceView.pri)
include(QGraphViz.pri)
include(GraphLib.pri)


#TODO: Break this out into its own file
LIBS         += -L$$quote($${BUILD_PATH}/plugins/Welcome/$${DIR_POSTFIX}) -lWelcome$${LIB_POSTFIX}

CONFIG(debug, debug|release) {
  TARGET      = SWATD
} else {
  TARGET      = SWAT
}

SOURCES      += SWATPlugin.cpp \
                SWATWidget.cpp \
                AboutDialog.cpp \
                Settings/SettingPage.cpp \
                Welcome/WelcomeData.cpp \
                ConnectionManager/IAdapter.cpp \
                JobControlDialog.cpp \
                ConnectionManager/ConnectionManager.cpp \
                DirectedGraph/DirectedGraphWidget.cpp \
                DirectedGraph/DirectedGraphScene.cpp \
                DirectedGraph/DirectedGraphNode.cpp \
                DirectedGraph/DirectedGraphEdge.cpp \
                DirectedGraph/STATWidget.cpp \
                DirectedGraph/STATScene.cpp \
                DirectedGraph/STATNode.cpp \
                DirectedGraph/STATEdge.cpp \
                DirectedGraph/STATNodeDialog.cpp \
                DirectedGraph/SWATWidget.cpp \
                DirectedGraph/SWATScene.cpp \
                DirectedGraph/SWATNode.cpp \
                DirectedGraph/SWATEdge.cpp

HEADERS      += SWATPlugin.h \
                SWATWidget.h \
                AboutDialog.h \
                SWATLibrary.h \
                Settings/SettingPage.h \
                Welcome/WelcomeData.h \
                ConnectionManager/ConnectionManagerLibrary.h \
                ConnectionManager/IAdapter.h \
                JobControlDialog.h \
                ConnectionManager/ConnectionManager.h \
                DirectedGraph/DirectedGraphWidget.h \
                DirectedGraph/DirectedGraphScene.h \
                DirectedGraph/DirectedGraphNode.h \
                DirectedGraph/DirectedGraphEdge.h \
                DirectedGraph/STATWidget.h \
                DirectedGraph/STATScene.h \
                DirectedGraph/STATNode.h \
                DirectedGraph/STATEdge.h \
                DirectedGraph/STATNodeDialog.h \
                DirectedGraph/SWATWidget.h \
                DirectedGraph/SWATScene.h \
                DirectedGraph/SWATNode.h \
                DirectedGraph/SWATEdge.h

FORMS        += SWATWidget.ui \
                AboutDialog.ui \
                Settings/SettingPage.ui \
                JobControlDialog.ui \
                DirectedGraph/STATNodeDialog.ui

RESOURCES    += Resources/SWAT.qrc

QT           += gui xml

DEFINES      += SWAT_LIBRARY CONNECTIONMANAGER_LIBRARY VIEWMANAGER_LIBRARY

#debug: DEFINES     += DIRECTEDGRAPHWIDGET_DEBUG


OTHER_FILES += Welcome/WelcomeData.xml
win32: welcomeData.path = /swat/
else: welcomeData.path = /share/swat/gui/
welcomeData.files = Welcome/WelcomeData.xml
INSTALLS += welcomeData
