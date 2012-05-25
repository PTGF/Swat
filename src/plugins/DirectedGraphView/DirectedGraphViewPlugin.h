/*!
   \file DirectedGraphViewPlugin.cpp
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the StackWalker Analysis Tool (SWAT)
   Copyright (C) 2012-2012 Argo Navis Technologies, LLC
   Copyright (C) 2012-2012 University of Wisconsin

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by the
   Free Software Foundation; either version 2.1 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
   for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

   \section DESCRIPTION

 */

#ifndef DIRECTEDGRAPHVIEWPLUGIN_H
#define DIRECTEDGRAPHVIEWPLUGIN_H

#include <QtCore>
#include <QtGui>

#include <PluginManager/IPlugin.h>
#include <SWAT/ViewManager/IViewFactory.h>

namespace Plugins {
namespace DirectedGraphView {

class DirectedGraphViewPlugin :
        public QObject,
        public Core::PluginManager::IPlugin,
        public Plugins::SWAT::IViewFactory
{
    Q_OBJECT
    Q_INTERFACES(Core::PluginManager::IPlugin)
    Q_INTERFACES(Plugins::SWAT::IViewFactory)

public:
    DirectedGraphViewPlugin(QObject *parent = 0);

    /* IPlugin Interface */
    ~DirectedGraphViewPlugin();
    bool initialize(QStringList &args, QString *err);
    void shutdown();
    QString name();
    QString version();
    QList<Core::PluginManager::Dependency> dependencies();

    /* IViewFactory Interface */
    QString viewName();
    bool viewHandles(QAbstractItemModel *model);
    QAbstractItemView *viewWidget(QAbstractItemModel *model);
    bool viewHandlesFiles();
    QWidget *viewWidget(const QByteArray &content);

protected:
    QString m_Name;
    QString m_Version;
    QList<Core::PluginManager::Dependency> m_Dependencies;

};

} // namespace DirectedGraphView
} // namespace Plugins

#endif // DIRECTEDGRAPHVIEWPLUGIN_H
