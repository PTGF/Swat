/*!
   \file SWATWidget.h
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

#ifndef PLUGINS_SWAT_SWATWIDGET_H
#define PLUGINS_SWAT_SWATWIDGET_H

#include <QtCore>
#include <QtGui>

#include <PrettyWidgets/TabWidget.h>

#include "SWATLibrary.h"

namespace Plugins {
namespace SWAT {

class IAdapter;
class STATWidget;

namespace Ui {
    class SWATMainWidget;
}

class SWAT_EXPORT SWATMainWidget : public TabWidget
{
    Q_OBJECT

public:
    explicit SWATMainWidget(QWidget *parent = 0);
    ~SWATMainWidget();

public slots:
    void attachJob();
    void launchJob();

protected:

    enum StateType {
        State_Unknown = 0,
        State_Detached,
        State_Paused,
        State_Running
    };

    void tabInserted(int index);
    void tabRemoved(int index);
    void checkAdapterProgress(IAdapter *adapter);
    bool searchProcesses();

    StateType state(const QUuid &id);
    void setState(const QUuid &id, const StateType &state);

    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

protected slots:
    void tabTitleChanged();

    void CurrentAdapterChanged(IAdapter*,IAdapter*);

    void attaching(QUuid);
    void attached(QUuid);
    void launching(QUuid);
    void launched(QUuid);
    void progress(int, QUuid);
    void progressMessage(QString, QUuid);
    void cancelAttach();

    void sampled(QString filename, QUuid id);

    void sampling(QUuid id);
    void detaching(QUuid id);
    void pausing(QUuid id);
    void resuming(QUuid id);
    void canceling(QUuid id);

    void doReattach();
    void doDetach();
    void doPause();
    void doResume();
    void doSample();
    void doSampleMultiple();

    void loadTraceFile();
    void loadTraceFromFile(QString filename);

    void closeJob(int index = -1);

    void indexChanged(int index);

private:
    Ui::SWATMainWidget *ui;

    QAction *m_AttachJob;
    QAction *m_LaunchJob;
    QAction *m_LoadFile;
    QAction *m_CloseJob;

    QAction *m_Reattach;
    QAction *m_Detach;
    QAction *m_Pause;
    QAction *m_Resume;
    QAction *m_Sample;
    QAction *m_SampleMultiple;

    QToolBar *m_ToolBar;
    QToolBar *m_CommandsToolBar;

    QString m_StyleSheet;
    QList<QProgressDialog*> m_ProgressDialogs;

    QHash<QUuid, StateType> m_FrontEndStates;

};

} // namespace SWAT
} // namespace Plugins

#endif // PLUGINS_SWAT_SWATWIDGET_H
