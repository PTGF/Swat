/*!
   \file SourceViewPlugin.cpp
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

#include "SourceViewPlugin.h"

#include <PluginManager/PluginManager.h>

#include "SourceView.h"

using namespace Plugins::SWAT;

namespace Plugins {
namespace SourceView {

/*! \namespace Plugins::SourceView
    \brief Contains the SourceViewPlugin.
 */

/*! \class SourceViewPlugin
    \version 0.1.dev
    \brief

    \par Depends on Plugins:
         SWAT

    \todo Document this more explicitly.
 */

/*!
   \fn SourceViewPlugin::SourceViewPlugin()
   \brief Constructor.
 */
SourceViewPlugin::SourceViewPlugin(QObject *parent) :
    QObject(parent)
{
    m_Name = "SourceView";
    m_Version = "0.1.dev";
    m_Dependencies.append( Core::PluginManager::Dependency("SWAT", "^0\\.1.*$") );
}

/*!
   \fn SourceViewPlugin::~SourceViewPlugin()
   \brief Destructor.
 */
SourceViewPlugin::~SourceViewPlugin()
{
}

/*!
   \fn SourceViewPlugin::initialize()
   \brief
   \returns true if successful
 */
bool SourceViewPlugin::initialize(QStringList &args, QString *err)
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
   \fn SourceViewPlugin::shutdown()
   \brief
 */
void SourceViewPlugin::shutdown()
{
}

/*!
   \fn SourceViewPlugin::name()
   \brief
   \returns name of plugin
 */
QString SourceViewPlugin::name()
{
    return m_Name;
}

/*!
   \fn SourceViewPlugin::version()
   \brief
   \returns version of plugin
 */
QString SourceViewPlugin::version()
{
    return m_Version;
}

/*!
   \fn SourceViewPlugin::dependencies()
   \brief
   \returns list of dependecies
 */
QList<Core::PluginManager::Dependency> SourceViewPlugin::dependencies()
{
    return m_Dependencies;
}

QString SourceViewPlugin::viewName()
{
    return tr("Source");
}

bool SourceViewPlugin::viewHandles(QAbstractItemModel *model)
{
    Q_UNUSED(model)
    return false;
}

QAbstractItemView *SourceViewPlugin::viewWidget(QAbstractItemModel *model)
{
    Q_UNUSED(model)
    return NULL;
}

bool SourceViewPlugin::viewHandlesFiles()
{
    return true;
}

QWidget *SourceViewPlugin::viewWidget(const QByteArray &content)
{
    return new SourceView(content);
}


} // namespace SourceView
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::SourceView::SourceViewPlugin)
