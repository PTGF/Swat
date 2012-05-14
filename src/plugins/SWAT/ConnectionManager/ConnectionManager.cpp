/*!
   \file
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Parallel Tools GUI Framework (PTGF)
   Copyright (C) 2010-2011 Argo Navis Technologies, LLC

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

#include "ConnectionManager.h"

#include <PluginManager/PluginManager.h>
#include <SettingManager/SettingManager.h>

#include "IAdapter.h"

namespace Plugins {
namespace SWAT {

ConnectionManager &ConnectionManager::instance()
{
    static ConnectionManager instance;
    return instance;
}

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent)
{
    m_CurrentAdapter = NULL;
}

ConnectionManager::~ConnectionManager()
{
}

/*! \fn ConnectionManager::initialize()
    \brief Called when the GUI plugins are initializing
 */
bool ConnectionManager::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)

    using namespace Core;

    try {
        readSettings();

        PluginManager::PluginManager &pluginManager = PluginManager::PluginManager::instance();
        pluginManager.addObject(this);

        /* Check the object pool for anything we should manage */
        foreach(QObject *object, pluginManager.allObjects()) { pluginObjectRegistered(object); }
        connect(&pluginManager, SIGNAL(objectAdded(QObject*)), this, SLOT(pluginObjectRegistered(QObject*)));

    } catch(...) {
        if(err->isEmpty()) err->append("\n");
        err->append(tr("Could not initialize ConnectionManager"));
        return false;
    }

    return true;
}

/*! \fn ConnectionManager::shutdown()
    \brief Called when the system is shutting down
 */
void ConnectionManager::shutdown()
{
    writeSettings();
}

/*! \fn ConnectionManager::readSettings()
    \brief Reads settings for the ConnectionManager from the SettingManager
 */
void ConnectionManager::readSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT/ConnectionManager");

    // Read stored settings and setup the manager

    settingManager.endGroup();
}

/*! \fn ConnectionManager::writeSettings()
    \brief Writes settings for the ConnectionManager to the SettingManager
 */
void ConnectionManager::writeSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT/ConnectionManager");

    // Store manager settings

    settingManager.endGroup();
}

void ConnectionManager::pluginObjectRegistered(QObject *object)
{
    IAdapter *adapter = qobject_cast<IAdapter *>(object);
    if(adapter) m_CurrentAdapter = adapter;
}

IAdapter *ConnectionManager::currentAdapter()
{
    return m_CurrentAdapter;
}

} // namespace SWAT
} // namespace Plugins
