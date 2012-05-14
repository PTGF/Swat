/*!
   \file SWATPlugin.cpp
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

#ifdef QT_DEBUG
#  include <QtDebug>
#endif

#include "SWATPlugin.h"

#include <PluginManager/PluginManager.h>
#include <SettingManager/SettingManager.h>
#include <MainWindow/MainWindow.h>

#include <ConnectionManager/ConnectionManager.h>
#include <Settings/SettingPage.h>

#include "AboutDialog.h"
#include "SWATWidget.h"


namespace Plugins {
namespace SWAT {

/*! \namespace Plugins::SWAT
    \brief Contains the base plugin for the SWAT set of plugins.
 */

/*! \class SWATPlugin
    \version 0.1.dev
    \brief The base plugin for the SWAT set of plugins.

    \par Depends on Plugins:
         none
 */


SWATPlugin::SWATPlugin() :
    m_MainWidget(NULL),
    m_AboutPage(this)
{
    m_Name = "SWAT";
    m_Version = "0.1.dev";

    // If we're in production mode, show the splash screen!
#ifndef QT_DEBUG
    AboutDialog::splash();
#endif
}

SWATPlugin::~SWATPlugin()
{
}

bool SWATPlugin::initialize(QStringList &args, QString *err)
{
    Q_UNUSED(args)

    using namespace Core;

    try {

        readSettings();

        /*** We're a main plugin, so we need to make changes to the mainWindow,
             like the application icon and the title ***/
        MainWindow::MainWindow &mainWindow = MainWindow::MainWindow::instance();

        if(!m_MainWidget) {
            m_MainWidget = new SWATWidget(&mainWindow);
        }

        mainWindow.setWindowTitle(m_MainWidget->windowTitle());
        mainWindow.setWindowIcon(m_MainWidget->windowIcon());

        /*** Set our main widget in the main window ***/
        mainWindow.addCentralWidget(m_MainWidget, 32, QString("SWAT"), m_MainWidget->windowIcon());

        /*** Register our menu structure ***/
        foreach(QAction *action, mainWindow.menuBar()->actions()) {
            if(action->text() == tr("Help")) {
                m_AboutPage.setText(tr("About SWAT"));
                m_AboutPage.setToolTip(tr("Displays the SWAT about dialog"));
                connect(&m_AboutPage, SIGNAL(triggered()), this, SLOT(aboutDialog()));
                action->menu()->addAction(&m_AboutPage);
            }
        }

        /*** Initialize child objects ***/
        ConnectionManager::instance().initialize(args, err);

        /*** Register any objects with the plugin manager ***/
        PluginManager::PluginManager &pluginManager = PluginManager::PluginManager::instance();
        pluginManager.addObject(this);                         /* Register ourselves as an ISettingPageFactory */
        pluginManager.addObject(m_MainWidget);

        // This should be fault tollerant, as it's not mandatory to function properly
        try {
            m_WelcomeData.initialize();
            pluginManager.addObject(&m_WelcomeData);
        } catch(QString error) {
            if(!err) err = new QString();
            if(err->isEmpty()) err->append("\n");
            err->append(error);
        } catch(...) {
            if(!err) err = new QString();
            if(err->isEmpty()) err->append("\n");
            err->append(tr("Could not initialize SWAT plugin's welcome page data"));
        }

    } catch(...) {
        if(!err) err = new QString();
        if(err->isEmpty()) err->append("\n");
        err->append(tr("Could not initialize SWAT plugin"));
        return false;
    }

    return true;
}

void SWATPlugin::shutdown()
{
    writeSettings();
}

void SWATPlugin::aboutDialog()
{
    try {
        AboutDialog about;
        about.exec();
    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to open about dialog: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to open about dialog."), NotificationWidget::Critical);
    }
}


QString SWATPlugin::name()
{
    return m_Name;
}

QString SWATPlugin::version()
{
    return m_Version;
}

QList<Core::PluginManager::Dependency> SWATPlugin::dependencies()
{
    return m_Dependencies;
}

void SWATPlugin::readSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT");


    settingManager.endGroup();
}

void SWATPlugin::writeSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT");


    settingManager.endGroup();
}


/* BEGIN ISettingPageFactory */
QIcon SWATPlugin::settingPageIcon()
{
    return QIcon(":/SWAT/app.gif");
}

QString SWATPlugin::settingPageName()
{
    return tr("SWAT");
}

int SWATPlugin::settingPagePriority()
{
    return 128;
}

Core::SettingManager::ISettingPage *SWATPlugin::createSettingPage()
{
    return new SettingPage();
}
/* END ISettingPageFactory */


} // namespace SWAT
} // namespace Plugins

Q_EXPORT_PLUGIN(Plugins::SWAT::SWATPlugin)

