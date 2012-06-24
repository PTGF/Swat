/*!
   \file CompiledAdapterPlugin.cpp
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

#include "CompiledAdapterPlugin.h"

#include <PluginManager/PluginManager.h>

using namespace Plugins::SWAT;

namespace Plugins {
namespace CompiledAdapter {

/*! \namespace Plugins::CompiledAdapter
    \brief Contains the CompiledAdapterPlugin.
 */

/*! \class CompiledAdapterPlugin
    \version 0.1.dev
    \brief

    \par Depends on Plugins:
         SWAT

    \todo Document this more explicitly.
 */

/*!
   \fn CompiledAdapterPlugin::CompiledAdapterPlugin()
   \brief Constructor.
 */
CompiledAdapterPlugin::CompiledAdapterPlugin(QObject *parent = 0) :
    QObject(parent)
{
    m_Name = "CompiledAdapter";
    m_Version = "0.1.dev";
    m_Dependencies.append( Core::PluginManager::Dependency("SWAT", "^0\\.1.*$") );
}

/*!
   \fn CompiledAdapterPlugin::~CompiledAdapterPlugin()
   \brief Destructor.
 */
CompiledAdapterPlugin::~CompiledAdapterPlugin()
{
}

/*!
   \fn CompiledAdapterPlugin::initialize()
   \brief
   \returns true if successful
 */
bool CompiledAdapterPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)
    Q_UNUSED(err)

    try {
        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        pluginManager.addObject(&m_CompiledAdapter);
    } catch(...) {
        return false;
    }

    return true;
}

/*!
   \fn CompiledAdapterPlugin::shutdown()
   \brief
 */
void CompiledAdapterPlugin::shutdown()
{
}

/*!
   \fn CompiledAdapterPlugin::name()
   \brief
   \returns name of plugin
 */
QString CompiledAdapterPlugin::name()
{
    return m_Name;
}

/*!
   \fn CompiledAdapterPlugin::version()
   \brief
   \returns version of plugin
 */
QString CompiledAdapterPlugin::version()
{
    return m_Version;
}

/*!
   \fn CompiledAdapterPlugin::dependencies()
   \brief
   \returns list of dependecies
 */
QList<Core::PluginManager::Dependency> CompiledAdapterPlugin::dependencies()
{
    return m_Dependencies;
}

} // namespace CompiledAdapter
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::CompiledAdapter::CompiledAdapterPlugin)
