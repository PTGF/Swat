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

#include "DirectedGraphViewPlugin.h"

#include <PluginManager/PluginManager.h>

#include "DirectedGraphView.h"

using namespace Plugins::SWAT;

namespace Plugins {
namespace DirectedGraphView {

/*! \namespace Plugins::DirectedGraphView
    \brief Contains the DirectedGraphViewPlugin.
 */

/*! \class DirectedGraphViewPlugin
    \version 0.1.dev
    \brief

    \par Depends on Plugins:
         SWAT

    \todo Document this more explicitly.
 */

/*!
   \fn DirectedGraphViewPlugin::DirectedGraphViewPlugin()
   \brief Constructor.
 */
DirectedGraphViewPlugin::DirectedGraphViewPlugin(QObject *parent) :
    QObject(parent)
{
    m_Name = "DirectedGraphView";
    m_Version = "0.1.dev";
    m_Dependencies.append( Core::PluginManager::Dependency("SWAT", "^0\\.1.*$") );
}

/*!
   \fn DirectedGraphViewPlugin::~DirectedGraphViewPlugin()
   \brief Destructor.
 */
DirectedGraphViewPlugin::~DirectedGraphViewPlugin()
{
}

/*!
   \fn DirectedGraphViewPlugin::initialize()
   \brief
   \returns true if successful
 */
bool DirectedGraphViewPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {

        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(this);

    } catch(...) {
        return false;
    }

    return true;
}

/*!
   \fn DirectedGraphViewPlugin::shutdown()
   \brief
 */
void DirectedGraphViewPlugin::shutdown()
{
}

/*!
   \fn DirectedGraphViewPlugin::name()
   \brief
   \returns name of plugin
 */
QString DirectedGraphViewPlugin::name()
{
    return m_Name;
}

/*!
   \fn DirectedGraphViewPlugin::version()
   \brief
   \returns version of plugin
 */
QString DirectedGraphViewPlugin::version()
{
    return m_Version;
}

/*!
   \fn DirectedGraphViewPlugin::dependencies()
   \brief
   \returns list of dependecies
 */
QList<Core::PluginManager::Dependency> DirectedGraphViewPlugin::dependencies()
{
    return m_Dependencies;
}

QString DirectedGraphViewPlugin::viewName()
{
    return tr("Directed Graph");
}

bool DirectedGraphViewPlugin::viewHandles(QAbstractItemModel *model)
{
    Q_UNUSED(model)
    return false;
}

QAbstractItemView *DirectedGraphViewPlugin::viewWidget(QAbstractItemModel *model)
{
    Q_UNUSED(model)
    return NULL;
}

bool DirectedGraphViewPlugin::viewHandlesFiles()
{
    return true;
}

QWidget *DirectedGraphViewPlugin::viewWidget(QByteArray content)
{
    QGraphVizScene *scene = new QGraphVizScene(QString(content));
    DirectedGraphView *view = new DirectedGraphView(scene);
    scene->setParent(view);
    view->setWindowTitle(tr("File"));
    return view;
}


} // namespace DirectedGraphView
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::DirectedGraphView::DirectedGraphViewPlugin)
