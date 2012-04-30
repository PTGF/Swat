/*!
   \file
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

#include "WelcomeData.h"

#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <PluginManager/PluginManager.h>
#include "SWATWidget.h"

using namespace Plugins::Welcome;

namespace Plugins {
namespace SWAT {

WelcomeData::WelcomeData(QObject *parent) :
    QObject(parent),
    m_WelcomeData("WelcomeData")
{
}

bool WelcomeData::initialize()
{
#ifdef WIN32
    QFileInfo fileInfo(QString("%1/swat/WelcomeData.xml").arg(QApplication::instance()->applicationDirPath()));
#else
    QFileInfo fileInfo(QString("%1/../share/swat/gui/WelcomeData.xml").arg(QApplication::instance()->applicationDirPath()));
#endif

    QString filePath;
    if(filePath.isEmpty()) {
        filePath = fileInfo.absoluteFilePath();
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QString error = tr("Could not open welcome data file: %1").arg(filePath);
        qWarning(error.toLatin1());
        throw error;
    }
    if (!m_WelcomeData.setContent(&file)) {
        file.close();
        QString error = tr("Could not use welcome data file after opening, possibly invalid text: %1").arg(filePath);
        qWarning(error.toLatin1());
        throw error;
    }
    file.close();

    return true;
}

QList<Link> WelcomeData::actions()
{
    QList<Link> list;

    Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
    QList<SWATWidget *> swatWidgets = pluginManager.getObjects<SWATWidget>();
    if(swatWidgets.count() == 1) {
        SWATWidget *swatWidget = swatWidgets.at(0);

        list.append(Link(tr("SWAT Attach"),
                         tr("attach to already running job"),
                         Link::Type_None, 128,
                         swatWidget, SLOT(attachJob())));

        list.append(Link(tr("SWAT Launch"),
                         tr("launch a new job and attach"),
                         Link::Type_None, 128,
                         swatWidget, SLOT(launchJob())));

    }

    return list;
}

QList<Link> WelcomeData::recent()
{
    QList<Link> list;

    list.append(Link(tr("/home/dane/STAT_results/mpi_ringtopo"), QString(), Link::Type_None, 128,
                     QUrl("stat:///home/dane/STAT_results/mpi_ringtopo")));
    list.append(Link(tr("/home/dane/STAT_results/mpi_ringtopo.0001"), QString(), Link::Type_None, 128,
                     QUrl("stat:///home/dane/STAT_results/mpi_ringtopo.0001")));
    list.append(Link(tr("/home/dane/STAT_results/mpi_ringtopo.0002"), QString(), Link::Type_None, 128,
                     QUrl("stat:///home/dane/STAT_results/mpi_ringtopo.0002")));

    return list;
}

QList<Link> WelcomeData::tutorials()
{
    return createUrlLinks("Tutorials");
}

QList<Link> WelcomeData::examples()
{
    return createUrlLinks("Examples");
}

QList<Link> WelcomeData::support()
{
    return createUrlLinks("Support");
}

QStringList WelcomeData::tipsAndTricks()
{
    QStringList list;

    QDomElement welcomeData = m_WelcomeData.firstChildElement("WelcomeData");
    if(welcomeData.isNull()) {
        return list;
    }

    QDomElement element = welcomeData.firstChildElement("TipsAndTricks");
    if(element.isNull()) {
        return list;
    }

    QDomElement tip = element.firstChildElement("Tip");
    while(!tip.isNull()) {
        list.append(tip.attribute("text"));
        tip = tip.nextSiblingElement(tip.tagName());
    }

    return list;
}

QList<QUrl> WelcomeData::latestNewsRss()
{
    QList<QUrl> list;

    QDomElement welcomeData = m_WelcomeData.firstChildElement("WelcomeData");
    if(welcomeData.isNull()) {
        return list;
    }

    QDomElement element = welcomeData.firstChildElement("LatestNews");
    if(element.isNull()) {
        return list;
    }

    QDomElement rss = element.firstChildElement("Rss");
    while(!rss.isNull()) {
        list.append(QUrl(rss.attribute("url")));
        rss = rss.nextSiblingElement(rss.tagName());
    }

    return list;
}

QList<QWidget *> WelcomeData::additionalTabs()
{
    QList<QWidget *> list;
    return list;
}

QList<Link> WelcomeData::createUrlLinks(const QString &elementName)
{
    QList<Link> list;

    QDomElement welcomeData = m_WelcomeData.firstChildElement("WelcomeData");
    if(welcomeData.isNull()) {
        return list;
    }

    QDomElement element = welcomeData.firstChildElement(elementName);
    if(element.isNull()) {
        return list;
    }

    QDomElement link = element.firstChildElement("Link");
    while(!link.isNull()) {
        list.append(createUrlLink(link));
        link = link.nextSiblingElement(link.tagName());
    }

    return list;
}

Link WelcomeData::createUrlLink(const QDomElement &link)
{
    QString title = link.attribute("title");
    QString description = link.attribute("description");

    QString typeString = link.attribute("type");
    Link::Type type = Link::Type_None;
    if(typeString == "text") {
        type = Link::Type_Text;
    } else if(typeString == "video") {
        type = Link::Type_Video;
    } else if(typeString == "web") {
        type = Link::Type_Web;
    }

    bool okay;
    int priority = link.attribute("priority", "128").toInt(&okay);
    if(!okay) priority = 128;

    QUrl url(link.attribute("url"));

    return Link(title, description, type, priority, url);
}


} // namespace SWAT
} // namespace Plugins
