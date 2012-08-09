/*!
   \file CompiledAdapter.cpp
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

#include "CompiledAdapter.h"

#include <MainWindow/MainWindow.h>

#ifdef COMPILEDADAPTER_DEBUG
#  include <QtDebug>
#endif

using namespace Plugins::SWAT;

namespace Plugins {
namespace CompiledAdapter {

/*! \class CompiledAdapter
    \version 0.1.dev
    \brief SWAT FrontEnd adapter compiled directly with the libraries (STAT_FrontEnd.h interface)
 */

CompiledAdapter::CompiledAdapter(QObject *parent) :
    IAdapter(parent)
{
    setObjectName("CompiledAdapter");

//    setenv("prefix", "/opt/stat", true);
//    setenv("exec_prefix", "/opt/stat", true);
//    setenv("PATH", "$PATH:/usr/bin", true);
//    setenv("STAT_LMON_REMOTE_LOGIN", "/usr/bin/rsh", true);
//    setenv("STAT_XPLAT_RSH", "/usr/bin/rsh", true);
//    setenv("STAT_MRNET_COMM_PATH", "/opt/stat/bin/mrnet_commnode", true);
//    setenv("STAT_LMON_LAUNCHMON_ENGINE_PATH", "/opt/stat/bin/launchmon", true);
//    setenv("STAT_MRNET_DEBUG_LOG_DIRECTORY", "/dev/null", true);
//    setenv("STAT_CONNECT_TIMEOUT", "600", true);
//    setenv("LMON_FE_ENGINE_TIMEOUT", "600", true);

//    setenv("MRNET_DEBUG_LEVEL", "5", true);

    // Get default values from the FrontEnd and cache them in this object
    STAT_FrontEnd *frontEnd = new STAT_FrontEnd();
    m_DefaultFilterPath = QString(frontEnd->getFilterPath());
    m_DefaultToolDaemonPath = QString(frontEnd->getToolDaemonExe());
    m_InstallPath = QString(frontEnd->getInstallPrefix());
    m_OutputPath = QString(frontEnd->getOutDir());
    delete frontEnd;
}

CompiledAdapter::~CompiledAdapter()
{
    // Detach and shutdown any front ends
    foreach(QUuid id, m_frontEnds.keys()) {
        try {
            if(isAttached(id)) {
                detach(id);
            }
        } catch(...) { /* ignore */ }  //TODO: should probably log errors

        try {
            if(m_frontEnds.contains(id)) {
                getFrontEnd(id)->shutDown();
            }
        } catch(...) { /* ignore */ }  //TODO: should probably log errors

        m_running.removeAll(id);
        m_attached.removeAll(id);
    }

    // Delete all of the FrontEnd objects
    qDeleteAll(m_frontEnds);
}

QUuid CompiledAdapter::launch(LaunchOptions options)
{
    QUuid id = QUuid::createUuid();
    launch(options, id);
    return id;
}

void CompiledAdapter::launch(LaunchOptions options, QUuid id)
{
    StatError_t statError;

    emit launching(id);
    emit progress(1, id);

    emit progressMessage("Starting Front End", id);
    STAT_FrontEnd *frontEnd = setupFrontEnd(options, id);

//    options.args.pop_front();  // STATGUI.py just ignores the 'Launcher Exe' argument
    foreach(QString arg, options.args) {
        frontEnd->addLauncherArgv(arg.toLocal8Bit().data());
    }
    emit progress(5, id);

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::launchAndSpawnDaemons()";
    Thread::sleep(100);
#endif
    emit progressMessage("Launch Daemons", id);
    if(options.remoteHost == "localhost") {
        statError = frontEnd->launchAndSpawnDaemons();
    } else {
        statError = frontEnd->launchAndSpawnDaemons(options.remoteHost.toLocal8Bit().data());
    }
    if(statError != STAT_OK) {
        throw tr("Failed to launch daemons: %1").arg(frontEnd->getLastErrorMessage());
    }
    emit progress(10, id);

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "CompiledAdapter::launchMRNet()";
    Thread::sleep(100);
#endif
    emit progressMessage("Connect to Daemons", id);
    launchMRNet(options, id);
    emit progress(15, id);

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "CompiledAdapter::attachApplication()";
    Thread::sleep(100);
#endif
    emit progressMessage("Attach to Application", id);
    attachApplication(id);
    emit progress(20, id);

    options.traceCount = 1;
    options.traceFrequency = 1;
    OperationProgress operationProgress(30, 0.7);
    sample(options, id, operationProgress);
    emit progress(100, id);

    m_attached.append(id);
    m_running.append(id);
    emit launched(id);
}

QUuid CompiledAdapter::attach(AttachOptions options)
{
    QUuid id = QUuid::createUuid();
    attach(options, id);
    return id;
}

void CompiledAdapter::attach(AttachOptions options, QUuid id)
{
    StatError_t statError;

    emit attaching(id);

    emit progressMessage("Starting Front End", id);
    STAT_FrontEnd *frontEnd = setupFrontEnd(options, id);
    emit progress(5, id);

    emit progressMessage("Launch Daemons", id);
    if(options.remoteHost == "localhost") {
        statError = frontEnd->attachAndSpawnDaemons(options.pid);
    } else {
        statError = frontEnd->attachAndSpawnDaemons(options.pid, options.remoteHost.toLocal8Bit().data());
    }
    if(statError != STAT_OK) {
        throw tr("Failed to launch daemons: %1").arg(frontEnd->getLastErrorMessage());
    }
    emit progress(10, id);

    emit progressMessage("Connect to Daemons", id);
    launchMRNet(options, id);
    emit progress(15, id);

    emit progressMessage("Attach to Application", id);
    attachApplication(id);
    emit progress(20, id);

    options.traceCount = 1;
    options.traceFrequency = 1;
    OperationProgress operationProgress(30, 0.7);
    sample(options, id, operationProgress);
    emit progress(100, id);

    m_attached.append(id);
    emit attached(id);
}

STAT_FrontEnd *CompiledAdapter::setupFrontEnd(Options options, QUuid id)
{
    StatError_t statError;
    STAT_FrontEnd *frontEnd = getFrontEnd(id);

    m_reattachOptions[id] = options;  // Save for possible later reattach

    statError = frontEnd->setToolDaemonExe(options.toolDaemonPath.toLocal8Bit().data());
    if(statError != STAT_OK) {
        throw tr("STAT_FrontEnd::setToolDaemonExe() returned error: %1").arg(frontEnd->getLastErrorMessage());
    }

    statError = frontEnd->setFilterPath(options.filterPath.toLocal8Bit().data());
    if(statError != STAT_OK) {
        throw tr("STAT_FrontEnd::setFilterPath() returned error: %1").arg(frontEnd->getLastErrorMessage());
    }

    if(options.logFlags > 0) {
        StatLog_t logType = STAT_LOG_NONE;
        if(options.logFlags | Log_FrontEnd && options.logFlags | Log_BackEnd) {
            logType = STAT_LOG_ALL;
        } else if(options.logFlags | Log_FrontEnd) {
            logType = STAT_LOG_FE;
        } else if(options.logFlags | Log_BackEnd) {
            logType = STAT_LOG_BE;
        }

        statError = frontEnd->startLog(logType, options.logPath.toLocal8Bit().data());
        if(statError != STAT_OK) {
            throw tr("Failed to start log: %1").arg(frontEnd->getLastErrorMessage());
        }
    }

    if(options.verboseFlags | Verbose_Error && options.verboseFlags | Verbose_StdOut) {
        frontEnd->setVerbose(STAT_VERBOSE_FULL);
    } else if(options.verboseFlags | Verbose_Error) {
        frontEnd->setVerbose(STAT_VERBOSE_ERROR);
    } else if(options.verboseFlags | Verbose_StdOut) {
        frontEnd->setVerbose(STAT_VERBOSE_STDOUT);
    }

    if(options.debugFlags & Debug_BackEnd) {
        setenv("LMON_DEBUG_BES", "1", true);
    } else {
        unsetenv("LMON_DEBUG_BES");
    }

    frontEnd->setProcsPerNode(options.processesPerNode);

    return frontEnd;
}

void CompiledAdapter::reAttach(QUuid id)
{
    if(!isAttached(id)) {
        throw tr("Unable to reattach; already attached!");
    }

    if(!m_reattachOptions.contains(id)) {
        throw tr("Unable to reattach; options not found in cache.");
    }


    IAdapter::AttachOptions *attachOptions = dynamic_cast<IAdapter::AttachOptions*>(&(m_reattachOptions[id]));
    if(attachOptions) {
        attach(*attachOptions, id);
        return;
    }

    IAdapter::LaunchOptions *launchOptions = dynamic_cast<IAdapter::LaunchOptions*>(&(m_reattachOptions[id]));
    if(launchOptions) {
        launch(*launchOptions, id);
        return;
    }

    throw tr("Unable to reattach; valid options not found in cache.");
}

void CompiledAdapter::detach(QUuid id)
{
    if(!m_frontEnds.contains(id)) {
        throw tr("CompiledAdapter::resume() frontEnd was null! This is not a valid value.");
    }

    STAT_FrontEnd *frontEnd = getFrontEnd(id);

    StatError_t statError;

    if(!isAttached(id)) {
        throw tr("Unable to detach; not already attached.");
    }

    detaching(id);

    if((statError = frontEnd->detachApplication(NULL, 0, false)) != STAT_OK) {
        throw tr("Failed to detach from application: %1").arg(frontEnd->getLastErrorMessage());
    }

    if((statError = waitAck(frontEnd)) != STAT_OK) {
        throw tr("Failed to resume application: %1").arg(frontEnd->getLastErrorMessage());
    }

    m_frontEnds.remove(id);
    frontEnd->shutDown();
    delete frontEnd;

    m_running.removeAll(id);
    m_attached.removeAll(id);
    emit detached(id);
}

void CompiledAdapter::pause(QUuid id)
{
    if(!m_frontEnds.contains(id)) {
        throw tr("CompiledAdapter::pause() frontEnd was null! This is not a valid value.");
    }

    STAT_FrontEnd *frontEnd = getFrontEnd(id);

    StatError_t statError;

    // Can't pause if we're not running
    if(!frontEnd->isRunning()) {
        return;
    }

    emit pausing(id);

    if((statError = frontEnd->pause()) != STAT_OK) {
        throw tr("Failed to pause application: %1").arg(frontEnd->getLastErrorMessage());
    }

    if((statError = waitAck(frontEnd)) != STAT_OK) {
        throw tr("Failed to pause application: %1").arg(frontEnd->getLastErrorMessage());
    }

    m_running.removeAll(id);
    emit paused(id);
}

void CompiledAdapter::resume(QUuid id)
{
    //TODO: Should we just reattach if possible?
    if(!m_frontEnds.contains(id)) {
        throw tr("CompiledAdapter::resume() frontEnd was null! This is not a valid value.");
    }

    STAT_FrontEnd *frontEnd = getFrontEnd(id);

    StatError_t statError;

    // Can't resume if we're already running
    if(frontEnd->isRunning()) {
        return;
    }

    emit resuming(id);

    if((statError = frontEnd->resume()) != STAT_OK) {
        throw tr("Failed to resume application: %1").arg(frontEnd->getLastErrorMessage());
    }

    if((statError = waitAck(frontEnd)) != STAT_OK) {
        throw tr("Failed to resume application: %1").arg(frontEnd->getLastErrorMessage());
    }

    m_running.append(id);
    emit resumed(id);
}

void CompiledAdapter::sample(SampleOptions options, QUuid id)
{
    OperationProgress operationProgress;
    sample(options, id, operationProgress);
}

void CompiledAdapter::sample(SampleOptions options, QUuid id, OperationProgress &operationProgress)
{
    if(!m_frontEnds.contains(id)) {
        throw tr("CompiledAdapter::sample() frontEnd was null! This is not a valid value.");
    }

    // Store the progress scale, we're going to be messing with it for sub-operations
    float operationProgressScale = operationProgress.scale;

    // Run the application for the specified period before we begin
    operationProgress.scale = 0.05 * operationProgressScale;  // (((our steps) * (our scale)) / (sub-op scale))
    preSampleRunWait(options.runTimeBeforeSample, id, operationProgress);


    // Rescale the progress for the following operations
    operationProgress.scale = 0.85 * operationProgressScale;

    options.traceCount = 1;
    options.traceFrequency = 1;
    sampleOne(options, id, operationProgress);
}

void CompiledAdapter::sampleMultiple(SampleOptions options, QUuid id)
{
    OperationProgress operationProgress;
    sampleMultiple(options, id, operationProgress);
}

void CompiledAdapter::sampleMultiple(SampleOptions options, QUuid id, OperationProgress &operationProgress)
{
    if(!m_frontEnds.contains(id)) {
        throw tr("CompiledAdapter::sampleMultiple() frontEnd was null! This is not a valid value.");
    }

    StatError_t statError;

    // Store the progress scale, we're going to be messing with it for sub-operations
    float operationProgressScale = operationProgress.scale;

    // Run the application for the specified period before we begin
    operationProgress.scale = 0.05 * operationProgressScale;  // (((our steps) * (our scale)) / (sub-op scale))
    preSampleRunWait(options.runTimeBeforeSample, id, operationProgress);

    // Rescale the progress for the following operations
    operationProgress.scale = 0.85 * operationProgressScale / options.traceCount;

    SampleOptions tempOptions = options;
    tempOptions.traceCount = 1;
    tempOptions.traceFrequency = 0;
    for(quint64 i=0; i < options.traceCount; ++i) {
        if(i == 1) tempOptions.clearOnSample = false;
        sampleOne(tempOptions, id, operationProgress);
    }


    emit progressMessage("Gather Stack Traces", id);
    STAT_FrontEnd *frontEnd = getFrontEnd(id);
    if((statError = frontEnd->gatherTraces(false)) != STAT_OK) {
        throw tr("Failed to gather stack traces:\n%1").arg(frontEnd->getLastErrorMessage());
    }

    if((statError = waitAck(frontEnd)) != STAT_OK) {
        throw tr("Failed to gather stack traces: %1").arg(frontEnd->getLastErrorMessage());
    }
    operationProgress.value += operationProgressScale * 5;
    emit progress(operationProgress.value, id);

    emit progressMessage("Render Stack Traces", id);
    QFileInfo fileInfo(frontEnd->getLastDotFilename());
    if(!fileInfo.exists()) {
        throw tr("File does not exist: '%1'").arg(fileInfo.absoluteFilePath());
    }

    QFile file(fileInfo.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw tr("Failed to open file: '%1'").arg(fileInfo.absoluteFilePath());
    }

    emit sampled(QString(file.readAll()), id);
    file.close();

    operationProgress.value += operationProgressScale * 43;
    emit progress(operationProgress.value, id);
}

/*! \fn CompiledAdapter::preSampleRunWait()
    \brief Helper function for sample operations where a required wait period occurs
    \param id Unique ID associated with the FrontEnd
    \param runTimeWait time in seconds to run the application before returning
    \param operationProgress
 */
void CompiledAdapter::preSampleRunWait(quint64 runTimeWait, QUuid id, OperationProgress &operationProgress)
{
    float operationProgressValue = operationProgress.value;

    if(runTimeWait > 0) {
        if(!isRunning(id)) {
            resume(id);
        }

        emit progressMessage("Waiting for Pre-Sample Run Time", id);

        QTime start = QTime::currentTime().addSecs(runTimeWait);

        static const int progressSteps = 10;
        QTime progressNotify = QTime::currentTime().addMSecs((runTimeWait * 1000) / progressSteps);

        while(QTime::currentTime() < start) {
            if(QTime::currentTime() > progressNotify) {
                progressNotify = QTime::currentTime().addMSecs((runTimeWait * 1000) / progressSteps);
                operationProgress.value += operationProgress.scale * 10;
                emit progress(operationProgress.value, id);
            }

            QApplication::processEvents();
            Thread::sleep(5);
        }
    }

    // Ensure that we always finish at 100% for this operation
    operationProgress.value = operationProgressValue + operationProgress.scale * 100;
    emit progress(operationProgress.value, id);
}

void CompiledAdapter::sampleOne(SampleOptions options, QUuid id, OperationProgress &operationProgress)
{
    if(!m_frontEnds.contains(id)) {
        throw tr("CompiledAdapter::sampleOne() frontEnd was null! This is not a valid value.");
    }

    STAT_FrontEnd *frontEnd = getFrontEnd(id);

    float operationProgressScale = operationProgress.scale;

    StatError_t statError;

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::pause()";
    Thread::sleep(100);
#endif
    if(frontEnd->isRunning()) {
        if((statError = frontEnd->pause()) != STAT_OK) {
            throw tr("Failed to pause application during sample: %1").arg(frontEnd->getLastErrorMessage());
        }
    }
    emit paused(id);

    operationProgress.value += operationProgressScale * 7;
    emit progress(operationProgress.value, id);

    emit progressMessage("Sample Stack Trace", id);
    StatSample_t sampleType = STAT_FUNCTION_NAME_ONLY;
    if(options.sampleType == Sample_FunctionAndPC) {
        sampleType = STAT_FUNCTION_AND_PC;
    } else if(options.sampleType == Sample_FunctionAndLine) {
        sampleType = STAT_FUNCTION_AND_LINE;
    }

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::sampleStackTraces()";
    Thread::sleep(100);
#endif
    //! \note The compiler warning for the string conversion is in the STAT_FrontEnd header; not our issue
    statError = frontEnd->sampleStackTraces(sampleType, options.withThreads, options.clearOnSample,
                                            options.traceCount, options.traceFrequency,
                                            options.retryCount, options.retryFrequency,
                                            false, const_cast<char *>("NULL"));
    if(statError != STAT_OK) {
        throw tr("Failed to sample stack trace: %1").arg(frontEnd->getLastErrorMessage());
    }


#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::waitAck()";
    Thread::sleep(100);
#endif
    if((statError = waitAck(frontEnd)) != STAT_OK) {
        throw tr("Failed to sample stack trace: %1").arg(frontEnd->getLastErrorMessage());
    }
    operationProgress.value += operationProgressScale * 36;
    emit progress(operationProgress.value, id);

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::gatherLastTrace()";
    Thread::sleep(100);
#endif
    emit progressMessage("Gather Stack Trace", id);
    if((statError = frontEnd->gatherLastTrace(false)) != STAT_OK) {
        throw tr("Failed to gather stack trace: %1").arg(frontEnd->getLastErrorMessage());
    }

    if((statError = waitAck(frontEnd)) != STAT_OK) {
        throw tr("Failed to gather stack trace: %1").arg(frontEnd->getLastErrorMessage());
    }
    operationProgress.value += operationProgressScale * 14;
    emit progress(operationProgress.value, id);

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::getLastDotFilename()";
    Thread::sleep(100);
#endif
    emit progressMessage("Render Stack Trace", id);
    QFileInfo fileInfo(frontEnd->getLastDotFilename());
    if(!fileInfo.exists()) {
        throw tr("File does not exist: '%1'").arg(fileInfo.absoluteFilePath());
    }

    QFile file(fileInfo.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw tr("Failed to open file: '%1'").arg(fileInfo.absoluteFilePath());
    }

    emit sampled(QString(file.readAll()), id);
    file.close();

    operationProgress.value += operationProgressScale * 43;
    emit progress(operationProgress.value, id);
}

/*! \fn CompiledAdapter::launchMRNet()
    \param options Topology options
    \param id Unique ID associated with SWAT FrontEnd
 */
void CompiledAdapter::launchMRNet(TopologyOptions options, QUuid id) {
    if(!m_frontEnds.contains(id)) {
        throw tr("CompiledAdapter::launchMRNet() frontEnd was null! This is not a valid value.");
    }

    STAT_FrontEnd *frontEnd = getFrontEnd(id);

    StatError_t statError;

    StatTopology_t topologyType = STAT_TOPOLOGY_AUTO;
    if(options.topologyType == Topology_Depth) {
        topologyType = STAT_TOPOLOGY_DEPTH;
    } else if(options.topologyType == Topology_FanOut) {
        topologyType = STAT_TOPOLOGY_FANOUT;
    } else if(options.topologyType == Topology_User) {
        topologyType = STAT_TOPOLOGY_USER;
    }

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::launchMrnetTree()";
    Thread::sleep(100);
#endif
    statError = frontEnd->launchMrnetTree(topologyType,
                                          options.topologySpecification.toLocal8Bit().data(),
                                          options.nodeList.join(" ").toLocal8Bit().data(),
                                          false,
                                          options.shareApplicationNodes);
    if(statError != STAT_OK) {
        throw tr("Failed to launch MRNet tree: %1").arg(frontEnd->getLastErrorMessage());
    }

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::connectMrnetTree()";
    Thread::sleep(100);
#endif
    while((statError = frontEnd->connectMrnetTree(false)) == STAT_PENDING_ACK) {
        QApplication::processEvents();
        Thread::sleep(5);
    }
    if(statError != STAT_OK) {
        throw tr("Failed to connect MRNet tree: %1").arg(frontEnd->getLastErrorMessage());
    }

}

/*! \fn CompiledAdapter::attachApplication()
    \param id Unique ID associated with SWAT FrontEnd
 */
void CompiledAdapter::attachApplication(QUuid id)
{
    if(!m_frontEnds.contains(id)) {
        throw tr("CompiledAdapter::attachApplication() frontEnd was null! This is not a valid value.");
    }

    STAT_FrontEnd *frontEnd = getFrontEnd(id);

    StatError_t statError;

#ifdef COMPILEDADAPTER_DEBUG
    Thread::sleep(100);
    qDebug() << "STAT_FrontEnd::attachApplication()";
    Thread::sleep(100);
#endif
    statError = frontEnd->attachApplication(false);
    if(statError != STAT_OK) {
        throw tr("Failed to attach application: %1").arg(frontEnd->getLastErrorMessage());
    }

    if((statError = waitAck(frontEnd)) != STAT_OK) {
        throw tr("Failed to attach application: %1").arg(frontEnd->getLastErrorMessage());
    }
}

/*! \fn CompiledAdapter::waitAck()
    \brief Helper function that waits for a FrontEnd to do an operation while the GUI continues to be responsive.
 */
StatError_t CompiledAdapter::waitAck(STAT_FrontEnd *frontEnd)
{
    StatError_t statError;
    while((statError = frontEnd->receiveAck(false)) == STAT_PENDING_ACK) {
        QApplication::processEvents();
        Thread::sleep(5);
    }
    return statError;
}

/*! \fn CompiledAdapter::getFrontEnd()
    \param id Unique ID
    \returns STAT_FrontEnd object associated with unique ID
 */
STAT_FrontEnd *CompiledAdapter::getFrontEnd(QUuid id)
{
    if(id.isNull()) {
        throw tr("CompiledAdapter::getFrontEnd() ID not valid");
    }

    if(m_frontEnds.contains(id)) {
        return m_frontEnds[id];
    }

    STAT_FrontEnd *frontEnd = new STAT_FrontEnd;
    m_frontEnds.insert(id, frontEnd);
    return frontEnd;
}

/*! \fn CompiledAdapter::errorToString()
    \brief Converts STAT error enum value to QString for debug display
    \param StatError_t
    \returns string describing error
 */
QString CompiledAdapter::errorToString(StatError_t error)
{
    switch(error) {
    case STAT_OK:
        return tr("OK");
        break;
    case STAT_SYSTEM_ERROR:
        return tr("System error");
        break;
    case STAT_MRNET_ERROR:
        return tr("MRNet error");
        break;
    case STAT_FILTERLOAD_ERROR:
        return tr("FilterLoad error");
        break;
    case STAT_GRAPHLIB_ERROR:
        return tr("GraphLib error");
        break;
    case STAT_ALLOCATE_ERROR:
        return tr("Allocate error");
        break;
    case STAT_ATTACH_ERROR:
        return tr("Attach error");
        break;
    case STAT_DETACH_ERROR:
        return tr("Detach error");
        break;
    case STAT_SEND_ERROR:
        return tr("Send error");
        break;
    case STAT_SAMPLE_ERROR:
        return tr("Sample error");
        break;
    case STAT_TERMINATE_ERROR:
        return tr("Terminate error");
        break;
    case STAT_FILE_ERROR:
        return tr("File error");
        break;
    case STAT_LMON_ERROR:
        return tr("LaunchMON error");
        break;
    case STAT_ARG_ERROR:
        return tr("ARG error");
        break;
    case STAT_VERSION_ERROR:
        return tr("Version error");
        break;
    case STAT_NOT_LAUNCHED_ERROR:
        return tr("NotLaunched error");
        break;
    case STAT_NOT_ATTACHED_ERROR:
        return tr("NotAttached error");
        break;
    case STAT_NOT_CONNECTED_ERROR:
        return tr("NotConnected error");
        break;
    case STAT_NO_SAMPLES_ERROR:
        return tr("NoSamples error");
        break;
    case STAT_WARNING:
        return tr("Warning error");
        break;
    case STAT_LOG_MESSAGE:
        return tr("LogMessage Error");
        break;
    case STAT_STDOUT:
        return tr("StdOut error");
        break;
    case STAT_VERBOSITY:
        return tr("Verbosity error");
        break;
    case STAT_STACKWALKER_ERROR:
        return tr("Stackwalker error");
        break;
    case STAT_PAUSE_ERROR:
        return tr("Pause error");
        break;
    case STAT_RESUME_ERROR:
        return tr("Resume error");
        break;
    case STAT_DAEMON_ERROR:
        return tr("Daemon error");
        break;
    case STAT_APPLICATION_EXITED:
        return tr("ApplicationExited error");
        break;
    case STAT_PENDING_ACK:
        return tr("PendingAck error");
        break;
    default:
        return tr("Unknown error");
        break;
    }
}

const QString CompiledAdapter::defaultFilterPath() const
{
    return m_DefaultFilterPath;
}

const QString CompiledAdapter::defaultToolDaemonPath() const
{
    return m_DefaultToolDaemonPath;
}
const QString CompiledAdapter::installPath() const
{
    return m_InstallPath;
}
const QString CompiledAdapter::outputPath() const
{
    return m_OutputPath;
}

void CompiledAdapter::cancel(QUuid id)
{
    emit canceling(id);
    emit progressMessage(tr("Canceling"), id);
    emit progress(99, id);

    //TODO: cancel operation

    emit canceled(id);
    emit progressMessage(tr("Canceled"), id);
    emit progress(100, id);
}

} // namespace CompiledAdapter
} // namespace Plugins
