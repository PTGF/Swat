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

#ifndef PLUGINS_SWAT_IADAPTER_H
#define PLUGINS_SWAT_IADAPTER_H

#include <QtCore>
#include "ConnectionManagerLibrary.h"

namespace Plugins {
namespace SWAT {

/*! \class IAdapter
    \version 0.1.dev
    \brief API definition to a SWAT FrontEnd adapter
 */
class CONNECTIONMANAGER_EXPORT IAdapter : public QObject
{
    Q_OBJECT

public:
    /*! \enum IAdapter::LogFlag
        \brief Log flags
     */
    enum LogFlag {
        Log_None     = 0,
        Log_FrontEnd = 1,
        Log_BackEnd  = 2
    };

    /*! \enum IAdapter::VerboseFlag
        \brief Verbosity flags
     */
    enum VerboseFlag {
        Verbose_None = 0,
        Verbose_Error  = 1,
        Verbose_StdOut = 2
    };

    /*! \enum IAdapter::DebugFlag
        \brief Debuging location flags
     */
    enum DebugFlag {
        Debug_None = 0,
        Debug_FrontEnd = 1,
        Debug_BackEnd = 2
    };

    /*! \enum IAdapter::SampleTypes
        \brief Sample types available
     */
    enum SampleType {
        Sample_FunctionNameOnly = 0,
        Sample_FunctionAndPC    = 1,
        Sample_FunctionAndLine  = 2
    };

    /*! \enum IAdapter::TopologyType
        \brief MRNet topology types available
     */
    enum TopologyType {
        Topology_Depth  = 0,
        Topology_FanOut = 1,
        Topology_User   = 2,
        Topology_Auto   = 3
    };

    /*******************************/
    /*! \brief Options required for sample, launch, and attach operations
        \sa IAdapter::launch() IAdapter::attach() IAdapter::sample()
     */
    struct SampleOptions {
        SampleType sampleType;
        bool withThreads;
        bool clearOnSample;

        quint64 retryCount;
        quint64 retryFrequency;
        quint64 traceCount;
        quint64 traceFrequency;

        quint64 runTimeBeforeSample;
    };

    /*! \brief MRNet topology options
        \sa IAdapter::launch() IAdapter::attach()
     */
    struct TopologyOptions {
        TopologyType topologyType;
        QString topologySpecification;

        QStringList nodeList;
        quint64 processesPerNode;
        bool shareApplicationNodes;
    };

    /*! \brief Options required to launch and attach operations
        \sa IAdapter::launch() IAdapter::attach()
     */
    struct RemoteHostOptions {
        QString remoteShell;
        QString remoteHost;
    };

    /*! \brief Options required most operations
        \sa IAdapter::launch() IAdapter::attach() IAdapter::sample()
     */
    struct Options : TopologyOptions, SampleOptions, RemoteHostOptions {
        virtual ~Options() {}  // for RTTI

        QString toolDaemonPath;
        QString filterPath;

        QString logPath;
        int logFlags;       /*! \sa IAdapter::LogFlag     */
        int verboseFlags;   /*! \sa IAdapter::VerboseFlag */
        int debugFlags;     /*! \sa IAdapter::DebugFlag   */
    };

    /*! \brief Options required to attach
        \sa IAdapter::attach()
     */
    struct AttachOptions : Options {
        quint64 pid;
    };

    /*! \brief Options required to launch
        \sa IAdapter::launch()
     */
    struct LaunchOptions : Options {
        QStringList args;
    };

    /*******************************/

    explicit IAdapter(QObject *parent = 0);


    /*! \fn IAdapter::launch()
        \brief Launch a job and attach to it
        \returns Unique ID associated with FrontEnd
     */
    virtual QUuid launch(LaunchOptions options) = 0;

    /*! \fn CompiledAdapter::attach()
        \brief Attach to an already running job process
        \returns Unique ID associated with FrontEnd
     */
    virtual QUuid attach(AttachOptions options) = 0;

    /*! \fn CompiledAdapter::reAttach()
        \brief
        \returns
     */
    virtual void reAttach(QUuid id) = 0;

    /*! \fn CompiledAdapter::detach()
        \brief
        \returns
     */
    virtual void detach(QUuid id) = 0;

    /*! \fn CompiledAdapter::pause()
        \brief
        \returns
     */
    virtual void pause(QUuid id) = 0;

    /*! \fn CompiledAdapter::resume()
        \brief
        \returns
     */
    virtual void resume(QUuid id) = 0;

    /*! \fn CompiledAdapter::sample()
        \brief
        \returns
     */
    virtual void sample(SampleOptions options, QUuid id) = 0;

    /*! \fn CompiledAdapter::sampleMultiple()
        \brief
        \returns
     */
    virtual void sampleMultiple(SampleOptions options, QUuid id) = 0;



    virtual const QString defaultFilterPath() const = 0;
    virtual const QString defaultToolDaemonPath() const = 0;
    virtual const QString installPath() const = 0;
    virtual const QString outputPath() const = 0;

public slots:
    virtual void cancel(QUuid id) = 0;

signals:
    /*! \fn CompiledAdapter::progress()
        \brief Emitted when progress has been made on an operation
        \param progress Amount of progress the operation has made
        \param id Unique ID of the associated FrontEnd
     */
    void progress(int progress, QUuid id);

    /*! \fn CompiledAdapter::progressMessage()
        \brief Emitted when progress has been made on an operation
        \param progress String describing current step in progress
        \param id Unique ID of the associated FrontEnd
     */
    void progressMessage(QString progress, QUuid id);

    /*! \fn CompiledAdapter::launching()
        \brief Emitted when a launch operation is initiated
        \param id Unique ID of the associated FrontEnd
     */
    void launching(QUuid id);

    /*! \fn CompiledAdapter::launched()
        \brief Emitted when a launch operation has finished
        \param id Unique ID of the associated FrontEnd
     */
    void launched(QUuid id);

    /*! \fn CompiledAdapter::attaching()
        \brief Emitted when an attach operation is initiated
        \param id Unique ID of the associated FrontEnd
     */
    void attaching(QUuid id);

    /*! \fn CompiledAdapter::attached()
        \brief Emitted when an attach operation has finished
        \param id Unique ID of the associated FrontEnd
     */
    void attached(QUuid id);

    /*! \fn CompiledAdapter::detaching()
        \brief Emitted when a detach operation is initiated
        \param id Unique ID of the associated FrontEnd
     */
    void detaching(QUuid id);

    /*! \fn CompiledAdapter::detached()
        \brief Emitted when a detach operation has finished
        \param id Unique ID of the associated FrontEnd
     */
    void detached(QUuid id);

    /*! \fn CompiledAdapter::pausing()
        \brief Emitted when a pause operation is initiated
        \param id Unique ID of the associated FrontEnd
     */
    void pausing(QUuid id);

    /*! \fn CompiledAdapter::paused()
        \brief Emitted when a pause operation has finished
        \param id Unique ID of the associated FrontEnd
     */
    void paused(QUuid id);

    /*! \fn CompiledAdapter::resuming()
        \brief Emitted when a resume operation is initiated
        \param id Unique ID of the associated FrontEnd
     */
    void resuming(QUuid id);

    /*! \fn CompiledAdapter::resumed()
        \brief Emitted when a resume operation has finished
        \param id Unique ID of the associated FrontEnd
     */
    void resumed(QUuid id);

    /*! \fn CompiledAdapter::sampling()
        \brief Emitted when a sample operation is initiated
        \param id Unique ID of the associated FrontEnd
     */
    void sampling(QUuid id);

    /*! \fn CompiledAdapter::sampled()
        \brief Emitted when a sample operation has finished
        \param filename Absolute path to file with stored content
        \param id Unique ID of the associated FrontEnd
     */
    void sampled(QString filename, QUuid id);

    /*! \fn CompiledAdapter::canceling()
        \brief Emitted when a cancel operation is initiated
        \param id Unique ID of the associated FrontEnd
     */
    void canceling(QUuid id);

    /*! \fn CompiledAdapter::canceled()
        \brief Emitted when a cancel operation has finished
        \param id Unique ID of the associated FrontEnd
     */
    void canceled(QUuid id);
};

} // namespace SWAT
} // namespace Plugins

//! We do this so that we can use the pointer in a QVariant
Q_DECLARE_METATYPE(Plugins::SWAT::IAdapter *)

Q_DECLARE_INTERFACE(Plugins::SWAT::IAdapter, "org.krellinst.swat.IAdapter/0.1")

#endif // PLUGINS_SWAT_IADAPTER_H
