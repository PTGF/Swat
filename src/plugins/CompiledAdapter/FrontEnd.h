/*!
   \file FrontEnd.h
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

#ifndef FRONTEND_H
#define FRONTEND_H

#include <QObject>

namespace Plugins {
namespace CompiledAdapter {

class FrontEnd : public QObject
{
    Q_OBJECT
public:
    enum LogTypes {
        Log_FrontEnd,
        Log_BackEnd,
        Log_All,
        Log_None
    };

    enum SampleTypes {
        Sample_FunctionNameOnly,
        Sample_FunctionAndPC,
        Sample_FunctionAndLine
    };

    enum LaunchTypes {
        Launch_Launch,
        Launch_Attach
    };

    enum Verbosity {
        Verbose_Error,
        Verbose_StdOut,
        Verbose_Full
    };

    enum Topologies {
        Topology_Depth,
        Topology_FanOut,
        Topology_User,
        Topology_Auto
    };

    enum Error {
        Error_OK,
        Error_System,
        Error_MRNet,
        Error_FilterLoad,
        Error_GraphLib,
        Error_Allocate,
        Error_Attach,
        Error_Detach,
        Error_Send,
        Error_Sample,
        Error_Terminate,
        Error_File,
        Error_LaunchMON,
        Error_ARG,
        Error_Version,
        Error_NotLaunched,
        Error_NotAttached,
        Error_NotConnected,
        Error_NoSamples,
        Error_Warning,
        Error_LogMessage,
        Error_StdOut,
        Error_Verbosity,
        Error_Stackwalker,
        Error_Pause,
        Error_resume,
        Error_Daemon,
        Error_ApplicationExited,
        Error_PendingAck
    };

    explicit FrontEnd(QObject *parent = 0);
    ~FrontEnd();

    /* START Verbs */
    void attachAndSpawnDaemons(quint64 pid, QString remoteNode = QString());
    void launchAndSpawnDaemons(QString remoteNode = QString(), bool isStatBench = false);
    void launchMrnetTree(Topologies topologyType, QString topologySpecification, QString nodeList = QString(),
                          bool blocking = true, bool shareAppNodes = false, bool isStatBench = false);
    void connectMrnetTree(bool blocking = true, bool isStatBench = false);
    void attachApplication(bool blocking = true);
    void pause(bool blocking = true);
    void resume(bool blocking = true);
    void sampleStackTraces(SampleTypes sampleType, bool withThreads, bool clearOnSample, quint64 nTraces, quint64 traceFrequency,
                            quint64 nRetries, quint64 retryFrequency, bool blocking = true, QString variableSpecification = QString());
    void gatherLastTrace(bool blocking = true);
    void gatherTraces(bool blocking = true);
    void detachApplication(bool blocking = true);
    void detachApplication(qptrdiff stopList, quint64 stopListSize, bool blocking = true);
    void shutDown();
    /* END Verbs */


    /* START Properties */
    void startLog(LogTypes logType, QString logOutDir);
    void receiveAck(bool blocking = true);

    void setToolDaemonExe(const QString toolDaemonExe);
    QString toolDaemonExe() const;

    void setOutDir(const QString outDir);
    QString outDir() const;

    void setFilePrefix(const QString filePrefix);
    QString filePrefix() const;

    void setProcsPerNode(quint64 procsPerNode);
    quint64 procsPerNode() const;

    void setFilterPath(const QString filterPath);
    QString filterPath() const;

    void addLauncherArgv(const QString launcherArg);
    QStringList launcherArgv() const;

    void setVerbosity(Verbosity verbose);
    Verbosity verbosity() const;

    bool isRunning() const;
    quint64 launcherPid() const;
    quint64 numApplProcs() const;
    quint64 numApplNodes() const;
    QString remoteNode() const;
    QString lastErrorMessage() const;
    QString lastDotFilename() const;
    QString applExe() const;
    QString installPrefix() const;
    int version();
    /* END Properties */

signals:
    
public slots:
    
};

} // namespace CompiledAdapter
} // namespace Plugins

#endif // FRONTEND_H
