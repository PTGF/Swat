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

    QUuid launch(const LaunchOptions &options);
    QUuid attach(const AttachOptions &options);
    void reAttach(const QUuid &id);
    void detach(const QUuid &id);
    void pause(const QUuid &id);
    void resume(const QUuid &id);
    void sample(const SampleOptions &options, const QUuid &id);
    void sampleMultiple(const SampleOptions &options, const QUuid &id);

    const QString &defaultFilterPath() const;
    const QString &defaultToolDaemonPath() const;
    const QString &installPath() const;
    const QString &outputPath() const;

public slots:
    void cancel(const QUuid &id);

protected:
    struct OperationProgress {
        OperationProgress(float value = 0, float scale = 1) { this->value = value; this->scale = scale; }
        float value;
        float scale;
    };

    void launch(const LaunchOptions &options, const QUuid &id);
    void attach(const AttachOptions &options, const QUuid &id);
    STAT_FrontEnd *setupFrontEnd(const Options &options, const QUuid &id);
    void launchMRNet(const TopologyOptions &options, const QUuid &id);

    void attachApplication(const QUuid &id);

    void sample(const SampleOptions &options, const QUuid &id, OperationProgress &operationProgress);
    void sampleMultiple(const SampleOptions &options, const QUuid &id, OperationProgress &operationProgress);

    void preSampleRunWait(const quint64 &runTimeWait, const QUuid &id, OperationProgress &operationProgress);
    void sampleOne(const SampleOptions &options, const QUuid &id, OperationProgress &operationProgress);


    StatError_t waitAck(STAT_FrontEnd *frontEnd);

    STAT_FrontEnd *getFrontEnd(const QUuid &id = QUuid());

    QString errorToString(StatError_t error);

    bool isAttached(const QUuid &id) { return m_attached.contains(id); }
    bool isRunning(const QUuid &id)
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
