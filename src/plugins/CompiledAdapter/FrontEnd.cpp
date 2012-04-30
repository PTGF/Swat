/*!
   \file FrontEnd.cpp
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

#include "FrontEnd.h"

#include <QStringList>

//#include <STAT_FrontEnd.h>

namespace Plugins {
namespace CompiledAdapter {

FrontEnd::FrontEnd(QObject *parent) :
    QObject(parent)
{
}

FrontEnd::~FrontEnd()
{
}

/*****************/

void FrontEnd::attachAndSpawnDaemons(quint64 pid, QString remoteNode)
{
}

void FrontEnd::launchAndSpawnDaemons(QString remoteNode, bool isStatBench)
{
}

void FrontEnd::launchMrnetTree(Topologies topologyType,
                                QString topologySpecification,
                                QString nodeList,
                                bool blocking,
                                bool shareAppNodes,
                                bool isStatBench)
{
}

void FrontEnd::connectMrnetTree(bool blocking, bool isStatBench)
{
}

void FrontEnd::attachApplication(bool blocking)
{
}

void FrontEnd::pause(bool blocking)
{
}

void FrontEnd::resume(bool blocking)
{
}

void FrontEnd::sampleStackTraces(SampleTypes sampleType,
                                 bool withThreads,
                                 bool clearOnSample,
                                 quint64 nTraces,
                                 quint64 traceFrequency,
                                 quint64 nRetries,
                                 quint64 retryFrequency,
                                 bool blocking,
                                 QString variableSpecification)
{
}

void FrontEnd::gatherLastTrace(bool blocking)
{
}

void FrontEnd::gatherTraces(bool blocking)
{
}

void FrontEnd::detachApplication(bool blocking)
{
}

void FrontEnd::detachApplication(qptrdiff stopList, quint64 stopListSize, bool blocking)
{
}

void FrontEnd::shutDown()
{
}

/*****************/


void FrontEnd::startLog(LogTypes logType, QString logOutDir)
{
}
void FrontEnd::receiveAck(bool blocking)
{
}

void FrontEnd::setToolDaemonExe(const QString toolDaemonExe)
{
}
QString FrontEnd::toolDaemonExe() const
{
    return QString();
}

void FrontEnd::setOutDir(const QString outDir)
{
}
QString FrontEnd::outDir() const
{
    return QString();
}

void FrontEnd::setFilePrefix(const QString filePrefix)
{
}
QString FrontEnd::filePrefix() const
{
    return QString();
}

void FrontEnd::setProcsPerNode(quint64 procsPerNode)
{
}
quint64 FrontEnd::procsPerNode() const
{
    return 0;
}

void FrontEnd::setFilterPath(const QString filterPath)
{
}
QString FrontEnd::filterPath() const
{
    return QString();
}

void FrontEnd::addLauncherArgv(const QString launcherArg)
{
}
QStringList FrontEnd::launcherArgv() const
{
    return QStringList();
}

void FrontEnd::setVerbosity(Verbosity verbose)
{
}
FrontEnd::Verbosity FrontEnd::verbosity() const
{
    return FrontEnd::Verbose_Error;
}

/*****************/

bool FrontEnd::isRunning() const
{
    return false;
}

quint64 FrontEnd::launcherPid() const
{
    return 0;
}

quint64 FrontEnd::numApplProcs() const
{
    return 0;
}

quint64 FrontEnd::numApplNodes() const
{
    return 0;
}

QString FrontEnd::remoteNode() const
{
    return QString();
}

QString FrontEnd::lastErrorMessage() const
{
    return QString();
}

QString FrontEnd::lastDotFilename() const
{
    return QString();
}

QString FrontEnd::applExe() const
{
    return QString();
}

QString FrontEnd::installPrefix() const
{
    return QString();
}

int FrontEnd::version()
{
    return 0;
}



} // namespace CompiledAdapter
} // namespace Plugins
