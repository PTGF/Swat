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

namespace Plugins {
namespace CompiledAdapter {

FrontEnd::FrontEnd(QObject *parent) :
    QObject(parent),
    SWATFrontEnd(NULL)
{
}

FrontEnd::~FrontEnd()
{
    if(SWATFrontEnd) {
        SWATFrontEnd->shutDown();
    }
}

/*****************/

void FrontEnd::attachAndSpawnDaemons(quint64 pid, QString remoteNode)
{
    StatError_t value = SWATFrontEnd->attachAndSpawnDaemons((unsigned int)pid, remoteNode.toLocal8Bit().data());
    if(value != STAT_OK) {
        throw tr("FrontEnd::attachAndSpawnDaemons returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::launchAndSpawnDaemons(QString remoteNode, bool isStatBench)
{
    StatError_t value = SWATFrontEnd->launchAndSpawnDaemons(remoteNode.toLocal8Bit().data(), isStatBench);
    if(value != STAT_OK) {
        throw tr("FrontEnd::launchAndSpawnDaemons returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::launchMrnetTree(Topologies topologyType,
                               QString topologySpecification,
                               QString nodeList,
                               bool blocking,
                               bool shareAppNodes,
                               bool isStatBench)
{
    StatError_t value = SWATFrontEnd->launchMrnetTree((StatTopology_t)topologyType,
                                                      topologySpecification.toLocal8Bit().data(),
                                                      nodeList.toLocal8Bit().data(),
                                                      blocking,
                                                      shareAppNodes,
                                                      isStatBench);

    if(value != STAT_OK) {
        throw tr("FrontEnd::launchMrnetTree returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }

}

void FrontEnd::connectMrnetTree(bool blocking, bool isStatBench)
{
    StatError_t value = SWATFrontEnd->connectMrnetTree(blocking, isStatBench);
    if(value != STAT_OK) {
        throw tr("FrontEnd::connectMrnetTree returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::attachApplication(bool blocking)
{
    StatError_t value = SWATFrontEnd->attachApplication(blocking);
    if(value != STAT_OK) {
        throw tr("FrontEnd::attachApplication returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::pause(bool blocking)
{
    StatError_t value = SWATFrontEnd->pause(blocking);
    if(value != STAT_OK) {
        throw tr("FrontEnd::pause returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::resume(bool blocking)
{
    StatError_t value = SWATFrontEnd->resume(blocking);
    if(value != STAT_OK) {
        throw tr("FrontEnd::resume returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
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
    StatError_t value = SWATFrontEnd->sampleStackTraces((StatSample_t)sampleType,
                                                        withThreads,
                                                        clearOnSample,
                                                        (unsigned int)nTraces,
                                                        (unsigned int)traceFrequency,
                                                        (unsigned int)nRetries,
                                                        (unsigned int)retryFrequency,
                                                        blocking,
                                                        variableSpecification.toLocal8Bit().data());
    if(value != STAT_OK) {
        throw tr("FrontEnd::sampleStackTraces returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::gatherLastTrace(bool blocking)
{
    StatError_t value = SWATFrontEnd->gatherLastTrace(blocking);
    if(value != STAT_OK) {
        throw tr("FrontEnd::gatherLastTrace returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::gatherTraces(bool blocking)
{
    StatError_t value = SWATFrontEnd->gatherTraces(blocking);
    if(value != STAT_OK) {
        throw tr("FrontEnd::gatherTraces returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::detachApplication(bool blocking)
{
    StatError_t value = SWATFrontEnd->detachApplication(blocking);
    if(value != STAT_OK) {
        throw tr("FrontEnd::detachApplication returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::detachApplication(qptrdiff stopList, quint64 stopListSize, bool blocking)
{
    StatError_t value = SWATFrontEnd->detachApplication((int *)stopList, (int)stopListSize, blocking);
    if(value != STAT_OK) {
        throw tr("FrontEnd::detachApplication returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::shutDown()
{
    SWATFrontEnd->shutDown();
    SWATFrontEnd = NULL;
}

/*****************/


void FrontEnd::startLog(LogTypes logType, QString logOutDir)
{
    StatError_t value = SWATFrontEnd->startLog((StatLog_t)logType, logOutDir.toLocal8Bit().data());
    if(value != STAT_OK) {
        throw tr("FrontEnd::startLog returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::receiveAck(bool blocking)
{
    StatError_t value = SWATFrontEnd->receiveAck(blocking);
    if(value != STAT_OK) {
        throw tr("FrontEnd::receiveAck returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}

void FrontEnd::setToolDaemonExe(const QString toolDaemonExe)
{
    StatError_t value = SWATFrontEnd->setToolDaemonExe(toolDaemonExe.toLocal8Bit().data());
    if(value != STAT_OK) {
        throw tr("FrontEnd::setToolDaemonExe returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}
QString FrontEnd::toolDaemonExe() const
{
    return QString(SWATFrontEnd->getToolDaemonExe());
}

void FrontEnd::setOutDir(const QString outDir)
{
    StatError_t value = SWATFrontEnd->setOutDir(outDir.toLocal8Bit().data());
    if(value != STAT_OK) {
        throw tr("FrontEnd::setOutDir returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}
QString FrontEnd::outDir() const
{
    return QString(SWATFrontEnd->getOutDir());
}

void FrontEnd::setFilePrefix(const QString filePrefix)
{
    StatError_t value = SWATFrontEnd->setFilePrefix(filePrefix.toLocal8Bit().data());
    if(value != STAT_OK) {
        throw tr("FrontEnd::setFilePrefix returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}
QString FrontEnd::filePrefix() const
{
    return QString(SWATFrontEnd->getFilePrefix());
}

void FrontEnd::setProcsPerNode(quint64 procsPerNode)
{
    SWATFrontEnd->setProcsPerNode((unsigned int)procsPerNode);
}
quint64 FrontEnd::procsPerNode() const
{
    return static_cast<quint64>(SWATFrontEnd->getProcsPerNode());
}

void FrontEnd::setFilterPath(const QString filterPath)
{
    StatError_t value = SWATFrontEnd->setFilterPath(filterPath.toLocal8Bit().data());
    if(value != STAT_OK) {
        throw tr("FrontEnd::setFilterPath returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}
QString FrontEnd::filterPath() const
{
    return QString(SWATFrontEnd->getFilterPath());
}

void FrontEnd::addLauncherArgv(const QString launcherArg)
{
    StatError_t value = SWATFrontEnd->addLauncherArgv(launcherArg.toLocal8Bit().data());
    if(value != STAT_OK) {
        throw tr("FrontEnd::addLauncherArgv returned error from SWAT_FrontEnd:\n %1").arg(convert(convert(value)));
    }
}
QStringList FrontEnd::launcherArgv() const
{
    return QStringList(*SWATFrontEnd->getLauncherArgv());
}

void FrontEnd::setVerbosity(Verbosity verbose)
{
    SWATFrontEnd->setVerbose((StatVerbose_t)verbose);
}
FrontEnd::Verbosity FrontEnd::verbosity() const
{
    return static_cast<Verbosity>(SWATFrontEnd->getVerbose());
}

/*****************/

bool FrontEnd::isRunning() const
{
    return SWATFrontEnd->isRunning();
}

quint64 FrontEnd::launcherPid() const
{
    return static_cast<quint64>(SWATFrontEnd->getLauncherPid());
}

quint64 FrontEnd::numApplProcs() const
{
    return static_cast<quint64>(SWATFrontEnd->getNumApplProcs());
}

quint64 FrontEnd::numApplNodes() const
{
    return static_cast<quint64>(SWATFrontEnd->getNumApplNodes());
}

QString FrontEnd::remoteNode() const
{
    return QString(SWATFrontEnd->getRemoteNode());
}

QString FrontEnd::lastErrorMessage() const
{
    return QString(SWATFrontEnd->getLastErrorMessage());
}

QString FrontEnd::lastDotFilename() const
{
    return QString(SWATFrontEnd->getLastDotFilename());
}

QString FrontEnd::applExe() const
{
    return QString(SWATFrontEnd->getApplExe());
}

QString FrontEnd::installPrefix() const
{
    return QString(SWATFrontEnd->getInstallPrefix());
}

int FrontEnd::version()
{
    int *value = NULL;
    SWATFrontEnd->getVersion(value);

    int version = *value;
    return version;
}

/**********/


FrontEnd::Error FrontEnd::convert(StatError_t error)
{
    return (Error)error;
}

QString FrontEnd::convert(Error error)
{
    switch(error) {
    case Error_OK:
        return tr("OK");
        break;
    case Error_System:
        return tr("System error");
        break;
    case Error_MRNet:
        return tr("MRNet error");
        break;
    case Error_FilterLoad:
        return tr("FilterLoad error");
        break;
    case Error_GraphLib:
        return tr("GraphLib error");
        break;
    case Error_Allocate:
        return tr("Allocate error");
        break;
    case Error_Attach:
        return tr("Attach error");
        break;
    case Error_Detach:
        return tr("Detach error");
        break;
    case Error_Send:
        return tr("Send error");
        break;
    case Error_Sample:
        return tr("Sample error");
        break;
    case Error_Terminate:
        return tr("Terminate error");
        break;
    case Error_File:
        return tr("File error");
        break;
    case Error_LaunchMON:
        return tr("LaunchMON error");
        break;
    case Error_ARG:
        return tr("ARG error");
        break;
    case Error_Version:
        return tr("Version error");
        break;
    case Error_NotLaunched:
        return tr("NotLaunched error");
        break;
    case Error_NotAttached:
        return tr("NotAttached error");
        break;
    case Error_NotConnected:
        return tr("NotConnected error");
        break;
    case Error_NoSamples:
        return tr("NoSamples error");
        break;
    case Error_Warning:
        return tr("Warning error");
        break;
    case Error_LogMessage:
        return tr("LogMessage Error");
        break;
    case Error_StdOut:
        return tr("StdOut error");
        break;
    case Error_Verbosity:
        return tr("Verbosity error");
        break;
    case Error_Stackwalker:
        return tr("Stackwalker error");
        break;
    case Error_Pause:
        return tr("Pause error");
        break;
    case Error_Resume:
        return tr("Resume error");
        break;
    case Error_Daemon:
        return tr("Daemon error");
        break;
    case Error_ApplicationExited:
        return tr("ApplicationExited error");
        break;
    case Error_PendingAck:
        return tr("PendingAck error");
        break;
    default:
        return tr("Unknown error");
        break;
    }
}



} // namespace CompiledAdapter
} // namespace Plugins
