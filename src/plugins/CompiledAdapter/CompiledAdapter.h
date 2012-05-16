/*!
   \file CompiledAdapter.h
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

#ifndef COMPILEDADAPTER_H
#define COMPILEDADAPTER_H

#include <QtCore>
#include <QtGui>

#include <STAT_FrontEnd.h>
#include <SWAT/ConnectionManager/IAdapter.h>


namespace Plugins {
namespace CompiledAdapter {

// Only way to get a calling thread to sleep using Qt4.
class Thread : public QThread
{
public:
    static void sleep(unsigned long msecs)
    {
        if(msecs < 100) {
            QThread::msleep(msecs);
        } else {
            QTime time = QTime::currentTime();
            time.start();
            while((unsigned long)time.elapsed() < msecs) {
                QApplication::processEvents();
                int remain = msecs - time.elapsed();
                QThread::msleep((remain <= 10) ? remain : 10);
            }
        }
    }
};

class CompiledAdapter : public Plugins::SWAT::IAdapter
{
    Q_OBJECT
    Q_INTERFACES(Plugins::SWAT::IAdapter)

public:
    explicit CompiledAdapter(QObject *parent = 0);
    ~CompiledAdapter();

    QUuid launch(LaunchOptions options);
    QUuid attach(AttachOptions options);
    void reAttach(QUuid id);
    void detach(QUuid id);
    void pause(QUuid id);
    void resume(QUuid id);
    void sample(SampleOptions options, QUuid id);
    void sampleMultiple(SampleOptions options, QUuid id);

    const QString defaultFilterPath() const;
    const QString defaultToolDaemonPath() const;
    const QString installPath() const;
    const QString outputPath() const;

public slots:
    void cancel(QUuid id);

protected:
    struct OperationProgress {
        OperationProgress(float value = 0, float scale = 1) { this->value = value; this->scale = scale; }
        float value;
        float scale;
    };

    void launch(LaunchOptions options, QUuid id);
    void attach(AttachOptions options, QUuid id);
    STAT_FrontEnd *setupFrontEnd(Options options, QUuid id);
    void launchMRNet(TopologyOptions options, QUuid id);

    void attachApplication(QUuid id);

    void sample(SampleOptions options, QUuid id, OperationProgress &operationProgress);
    void sampleMultiple(SampleOptions options, QUuid id, OperationProgress &operationProgress);

    void preSampleRunWait(quint64 runTimeWait, QUuid id, OperationProgress &operationProgress);
    void sampleOne(SampleOptions options, QUuid id, OperationProgress &operationProgress);


    StatError_t waitAck(STAT_FrontEnd *frontEnd);

    STAT_FrontEnd *getFrontEnd(QUuid id = QUuid());

    QString errorToString(StatError_t error);

    bool isAttached(QUuid id) { return m_attached.contains(id); }
    bool isRunning(QUuid id)
    {
        if(!m_frontEnds.contains(id)) {
            return false;
        }
        return getFrontEnd(id)->isRunning();
    }

private:
    QHash<QUuid, STAT_FrontEnd*> m_frontEnds;

    QHash<QUuid, IAdapter::Options> m_reattachOptions;

    //! List of attached FrontEnds; push when attaching/launching; pop when detaching
    QList<QUuid> m_attached;

    //! List of running FrontEnds; push when running; pop when else
    QList<QUuid> m_running;

    QString m_DefaultFilterPath;
    QString m_DefaultToolDaemonPath;
    QString m_InstallPath;
    QString m_OutputPath;

};

} // namespace CompiledAdapter
} // namespace Plugins

#endif // COMPILEDADAPTER_H
