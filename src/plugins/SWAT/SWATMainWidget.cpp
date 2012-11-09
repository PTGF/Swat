/*!
   \file SWATWidget.cpp
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

#include "SWATMainWidget.h"
#include "ui_SWATMainWidget.h"

#include <MainWindow/MainWindow.h>
#include <MainWindow/NotificationWidget.h>
#include <SettingManager/SettingManager.h>
#include <PluginManager/PluginManager.h>
#include <ConnectionManager/ConnectionManager.h>

#include <DirectedGraph/STATWidget.h>
#include <DirectedGraph/SWATWidget.h>
#include <SourceView/ISourceViewFactory.h>

#include "JobControlDialog.h"

namespace Plugins {
namespace SWAT {

SWATMainWidget::SWATMainWidget(QWidget *parent) :
    TabWidget(parent),
    ui(new Ui::SWATMainWidget),
    m_AttachJob(NULL),
    m_LaunchJob(NULL),
    m_LoadFile(NULL),
    m_CloseJob(NULL),
    m_ToolBar(NULL),
    m_CommandsToolBar(NULL)
{
    ui->setupUi(this);

    /* Set up the stylesheet for periods when we have tabs */
    m_StyleSheet = styleSheet();

    setWindowTitle(QString("SWAT%1").arg(QChar(0x2122))); //Trademark
    setWindowIcon(QIcon(":/SWAT/app.gif"));

    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeJob(int)));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(indexChanged(int)));

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
        if(action->text() == tr("File")) {

            m_AttachJob = new QAction(tr("Attach to Job"), this);
            m_AttachJob->setToolTip(tr("Attach SWAT to a running job"));
            m_AttachJob->setIcon(QIcon(":/SWAT/attach.svg"));
            m_AttachJob->setIconVisibleInMenu(true);
            m_AttachJob->setVisible(false);
            m_AttachJob->setShortcut(QKeySequence::Refresh);
            m_AttachJob->setProperty("swat_menuitem", QVariant(1));
            connect(m_AttachJob, SIGNAL(triggered()), this, SLOT(attachJob()));

            m_LaunchJob = new QAction(tr("Launch a Job"), this);
            m_LaunchJob->setToolTip(tr("Launch job and attach SWAT"));
            m_LaunchJob->setIcon(QIcon(":/SWAT/launch.svg"));
            m_LaunchJob->setIconVisibleInMenu(true);
            m_LaunchJob->setVisible(false);
            m_LaunchJob->setShortcut(QKeySequence::New);
            m_LaunchJob->setProperty("swat_menuitem", QVariant(1));
            connect(m_LaunchJob, SIGNAL(triggered()), this, SLOT(launchJob()));

            m_LoadFile = new QAction(tr("Load from File"), this);
            m_LoadFile->setToolTip(tr("Load SWAT output from saved file"));
            m_LoadFile->setIcon(QIcon(":/SWAT/open.svg"));
            m_LoadFile->setIconVisibleInMenu(true);
            m_LoadFile->setVisible(false);
            m_LoadFile->setShortcut(QKeySequence::Open);
            m_LoadFile->setProperty("swat_menuitem", QVariant(1));
            connect(m_LoadFile, SIGNAL(triggered()), this, SLOT(loadTraceFile()));

            m_CloseJob = new QAction(tr("Close Job"), this);
            m_CloseJob->setToolTip(tr("Close current SWAT job"));
            m_CloseJob->setIcon(QIcon(":/SWAT/close.svg"));
            m_CloseJob->setIconVisibleInMenu(true);
            m_CloseJob->setVisible(false);
            m_CloseJob->setEnabled(false);
            m_CloseJob->setShortcut(QKeySequence::Close);
            m_CloseJob->setProperty("swat_menuitem", QVariant(1));
            connect(m_CloseJob, SIGNAL(triggered()), this, SLOT(closeJob()));


            m_ToolBar = new QToolBar(tr("SWAT"), this);
            m_ToolBar->setObjectName("SwatToolBar");
//            m_ToolBar->setIconSize(QSize(16,16));
            m_ToolBar->setFloatable(false);
            m_ToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
            m_ToolBar->addAction(m_AttachJob);
            m_ToolBar->addAction(m_LaunchJob);
            m_ToolBar->addAction(m_LoadFile);
            m_ToolBar->addAction(m_CloseJob);
            mainWindow.addToolBar(Qt::TopToolBarArea, m_ToolBar);
            m_ToolBar->hide();


            //! \todo We really need to rely on the ActionManager to do this.
            QAction *before = NULL;
            foreach(QAction *item, action->menu()->actions()) {
                if(item->priority() == QAction::LowPriority) {
                    before = item;
                }
            }

            if(before) {
                action->menu()->insertAction(before, m_LaunchJob);
                action->menu()->insertAction(before, m_AttachJob);
                action->menu()->insertAction(before, m_LoadFile);
                action->menu()->insertAction(before, m_CloseJob);
                action->menu()->insertSeparator(before)->setProperty("swat_menuitem", QVariant(1));
            } else {
                action->menu()->addAction(m_LaunchJob);
                action->menu()->addAction(m_AttachJob);
                action->menu()->addAction(m_LoadFile);
                action->menu()->addAction(m_CloseJob);
                action->menu()->addSeparator()->setProperty("swat_menuitem", QVariant(1));
            }


        } else if(action->text() == tr("Tools")) {
            m_Reattach = new QAction(QIcon(":/SWAT/reattach.svg"), tr("Re-Attach"), this);
            m_Reattach->setToolTip(tr("Attach to the last application STAT detached from"));
            m_Reattach->setProperty("swatWidget_menuitem",QVariant(1));
            m_Reattach->setEnabled(false);
            connect(m_Reattach, SIGNAL(triggered()), this, SLOT(doReattach()));

            m_Detach = new QAction(QIcon(":/SWAT/detach.svg"), tr("Detach"), this);
            m_Detach->setToolTip(tr("Detach from the application"));
            m_Detach->setProperty("swatWidget_menuitem",QVariant(1));
            m_Detach->setEnabled(false);
            connect(m_Detach, SIGNAL(triggered()), this, SLOT(doDetach()));

            m_Pause = new QAction(QIcon(":/SWAT/pause.svg"), tr("Pause"), this);
            m_Pause->setToolTip(tr("Pause the application"));
            m_Pause->setProperty("swatWidget_menuitem",QVariant(1));
            m_Pause->setEnabled(false);
            connect(m_Pause, SIGNAL(triggered()), this, SLOT(doPause()));

            m_Resume = new QAction(QIcon(":/SWAT/resume.svg"), tr("Resume"), this);
            m_Resume->setToolTip(tr("Resume the application"));
            m_Resume->setProperty("swatWidget_menuitem",QVariant(1));
            m_Resume->setEnabled(false);
            connect(m_Resume, SIGNAL(triggered()), this, SLOT(doResume()));

            m_Sample = new QAction(QIcon(":/SWAT/sample.svg"), tr("Sample"), this);
            m_Sample->setToolTip(tr("Gather a stack trace sample from the application"));
            m_Sample->setProperty("swatWidget_menuitem",QVariant(1));
            m_Sample->setEnabled(false);
            connect(m_Sample, SIGNAL(triggered()), this, SLOT(doSample()));

            m_SampleMultiple = new QAction(QIcon(":/SWAT/sampleMultiple.svg"), tr("Sample Multiple"), this);
            m_SampleMultiple->setToolTip(tr("Gather the merged stack traces accumulated over time"));
            m_SampleMultiple->setProperty("swatWidget_menuitem", QVariant(1));
            m_SampleMultiple->setEnabled(false);
            connect(m_SampleMultiple, SIGNAL(triggered()), this, SLOT(doSampleMultiple()));


            m_CommandsToolBar = new QToolBar(tr("Process Control"), this);
            m_CommandsToolBar->setObjectName("ProcessControlToolBar");
//            m_CommandsToolBar->setIconSize(QSize(16,16));
            m_CommandsToolBar->addAction(m_Reattach);
            m_CommandsToolBar->addAction(m_Detach);
            m_CommandsToolBar->addAction(m_Pause);
            m_CommandsToolBar->addAction(m_Resume);
            m_CommandsToolBar->addAction(m_Sample);
            m_CommandsToolBar->addAction(m_SampleMultiple);
            mainWindow.addToolBar(Qt::TopToolBarArea, m_CommandsToolBar);
            m_CommandsToolBar->hide();


            //! \todo We really need to rely on the ActionManager to do this.
            QAction *before = NULL;
            foreach(QAction *item, action->menu()->actions()) {
                if(item->priority() == QAction::LowPriority) {
                    before = item;
                }
            }

            if(before) {
                action->menu()->insertSeparator(before)->setProperty("swatWidget_menuitem", QVariant(1));
                action->menu()->insertAction(before, m_Reattach);
                action->menu()->insertAction(before, m_Detach);
                action->menu()->insertAction(before, m_Pause);
                action->menu()->insertAction(before, m_Resume);
                action->menu()->insertAction(before, m_Sample);
                action->menu()->insertAction(before, m_SampleMultiple);
                action->menu()->insertSeparator(before)->setProperty("swatWidget_menuitem", QVariant(1));
            } else {
                action->menu()->insertSeparator(before)->setProperty("swatWidget_menuitem", QVariant(1));
                action->menu()->addAction(m_Reattach);
                action->menu()->addAction(m_Detach);
                action->menu()->addAction(m_Pause);
                action->menu()->addAction(m_Resume);
                action->menu()->addAction(m_Sample);
                action->menu()->addAction(m_SampleMultiple);
                action->menu()->addSeparator()->setProperty("swatWidget_menuitem", QVariant(1));
            }


        }
    }

    CurrentAdapterChanged(NULL, ConnectionManager::currentAdapter());
    connect(&ConnectionManager::instance(), SIGNAL(CurrentAdapterChanged(IAdapter*,IAdapter*)),
            this, SLOT(CurrentAdapterChanged(IAdapter*,IAdapter*)));
}

SWATMainWidget::~SWATMainWidget()
{
    delete ui;
}

void SWATMainWidget::tabInserted(int index)
{
    Q_UNUSED(index)

    TabWidget::tabInserted(index);

    m_CloseJob->setEnabled(count());
}

void SWATMainWidget::tabRemoved(int index)
{
    Q_UNUSED(index)

    TabWidget::tabRemoved(index);

    m_CloseJob->setEnabled(count());
}

void SWATMainWidget::tabTitleChanged()
{
    try {

        QWidget *widget = qobject_cast<QWidget *>(QObject::sender());
        if(widget) {
            setTabText(indexOf(widget), widget->windowTitle());
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to change tab name: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to change tab name."), NotificationWidget::Critical);
    }
}


SWATMainWidget::StateType SWATMainWidget::state(const QUuid &id)
{
    return m_FrontEndStates.value(id, State_Unknown);
}

void SWATMainWidget::setState(const QUuid &id, const StateType &state)
{
    m_FrontEndStates[id] = state;

    QWidget *widget = currentWidget();
    if(!widget || !widget->property("id").isValid()) {
        m_CommandsToolBar->hide();
        return;
    }

    m_CommandsToolBar->show();

    if(widget->property("id").toString() == id) {
        m_Reattach->setEnabled(false);
        m_Detach->setEnabled(false);
        m_Pause->setEnabled(false);
        m_Resume->setEnabled(false);
        m_Sample->setEnabled(false);
        m_SampleMultiple->setEnabled(false);

        switch(state) {
          case State_Detached:
            m_Reattach->setEnabled(true);
            break;
          case State_Paused:
            m_Resume->setEnabled(true);
            m_Detach->setEnabled(true);
            m_Sample->setEnabled(true);
            m_SampleMultiple->setEnabled(true);
            break;
          case State_Running:
            m_Pause->setEnabled(true);
            m_Detach->setEnabled(true);
            m_Sample->setEnabled(true);
            m_SampleMultiple->setEnabled(true);
            break;
          case State_Unknown:
            break;
        }
    }
}


bool SWATMainWidget::searchProcesses()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT");
    QString filter = settingManager.value("attach/processFilter", "mpirun|srun|orterun").toString();
    settingManager.endGroup();

    QString program = "/bin/ps";
    QStringList arguments;
    arguments << "w" << "x";

    QByteArray output;

    {
        QProcess process;
        process.start(program, arguments);

        if(!process.waitForStarted()) {
            return false;
        }

        if(!process.waitForFinished()) {
            return false;
        }

        output = process.readAll();
    }

    QStringList lines = QString(output).split('\n');

    static const QRegExp rxPid("\\s*PID");
    static const QRegExp rxCommand("COMMAND\\s*$");
    QRegExp rxFilter = QRegExp(filter, Qt::CaseInsensitive);

    bool foundOne = false;
    int commandIndex = 0;
    foreach(QString line, lines) {
        if(line.contains(rxPid)) {
            commandIndex = line.indexOf(rxCommand);
            continue;
        }

        if(line.right(line.count() - commandIndex).contains(rxFilter)) {
            foundOne = true;
            break;
        }
    }

    return foundOne;
}

void SWATMainWidget::attachJob()
{
    // Stylesheet screws with the dialog
    QString currentStyleSheet = this->styleSheet();
    setStyleSheet(QString());

    //Prompt user for finding and attaching to a running job
    JobControlDialog *dialog = new JobControlDialog(this);

    if(dialog->exec(JobControlDialog::Type_Attach) == QDialog::Accepted &&
            dialog->options() != NULL) {
        IAdapter *adapter = ConnectionManager::currentAdapter();

        if(!adapter) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Error while attaching: No adapter!"), NotificationWidget::Critical);
            return;
        }

        IAdapter::AttachOptions *options = (IAdapter::AttachOptions*)dialog->options();

        if(options) {
            try {
                adapter->attach(*options);
            } catch(QString err) {
                using namespace Core::MainWindow;
                MainWindow::instance().notify(tr("Error while attaching: %1").arg(err), NotificationWidget::Critical);
            } catch(...) {
                using namespace Core::MainWindow;
                MainWindow::instance().notify(tr("Error while attaching"), NotificationWidget::Critical);
            }
        }
    }

    dialog->deleteLater();

    setStyleSheet(currentStyleSheet);  // Reset the stylesheet
}

void SWATMainWidget::launchJob()
{
    // Stylesheet screws with the dialog
    QString currentStyleSheet = this->styleSheet();
    setStyleSheet(QString());

    //Prompt user for launching and attaching to a new job
    JobControlDialog *dialog = new JobControlDialog(this);

    if(dialog->exec(JobControlDialog::Type_Launch) == QDialog::Accepted &&
            dialog->options() != NULL) {
        IAdapter *adapter = ConnectionManager::currentAdapter();

        if(!adapter) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Error while launching: No adapter!"), NotificationWidget::Critical);
            return;
        }

        IAdapter::LaunchOptions *options = (IAdapter::LaunchOptions*)dialog->options();

        if(options) {
            try {
                adapter->launch(*options);
            } catch(QString err) {
                using namespace Core::MainWindow;
                MainWindow::instance().notify(tr("Error while launching: %1").arg(err), NotificationWidget::Critical);
            } catch(...) {
                using namespace Core::MainWindow;
                MainWindow::instance().notify(tr("Error while launching"), NotificationWidget::Critical);
            }
        }
    }

    dialog->deleteLater();

    setStyleSheet(currentStyleSheet);  // Reset the stylesheet
}

void SWATMainWidget::CurrentAdapterChanged(IAdapter* from, IAdapter* to)
{
    checkAdapterProgress(from);

    if(to) {
        connect(to, SIGNAL(attaching(QUuid)),                   this, SLOT(attaching(QUuid)));
        connect(to, SIGNAL(launching(QUuid)),                   this, SLOT(attaching(QUuid)));
        connect(to, SIGNAL(attached(QUuid)),                    this, SLOT(attached(QUuid)));
        connect(to, SIGNAL(launched(QUuid)),                    this, SLOT(attached(QUuid)));
        connect(to, SIGNAL(progress(int,QUuid)),                this, SLOT(progress(int,QUuid)));
        connect(to, SIGNAL(progressMessage(QString,QUuid)),     this, SLOT(progressMessage(QString,QUuid)));
        connect(to, SIGNAL(sampled(QString,QUuid)),             this, SLOT(sampled(QString,QUuid)));

        connect(to, SIGNAL(sampling(QUuid)),                    this, SLOT(sampling(QUuid)));
        connect(to, SIGNAL(detaching(QUuid)),                   this, SLOT(detaching(QUuid)));
        connect(to, SIGNAL(pausing(QUuid)),                     this, SLOT(pausing(QUuid)));
        connect(to, SIGNAL(resuming(QUuid)),                    this, SLOT(resuming(QUuid)));
        connect(to, SIGNAL(canceling(QUuid)),                   this, SLOT(canceling(QUuid)));

        // Check for any running jobs that we can attach to
        static bool searchedProcesses = false;
        if(!searchedProcesses) {
            Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
            settingManager.beginGroup("Plugins/SWAT");
            bool searchProcs = settingManager.value("startup/searchProcesses", true).toBool();
            settingManager.endGroup();

            searchedProcesses = true;

            if(searchProcs && searchProcesses()) {
                attachJob();
            }
        }
    }
}

void SWATMainWidget::checkAdapterProgress(IAdapter *adapter)
{
    if(!adapter) {
        return;
    }

    // Skip checking if this is still the current adapter
    if(ConnectionManager::currentAdapter() == adapter) {
        return;
    }

    // Go ahead and disconnect from any new attachments
    disconnect(adapter, SIGNAL(attaching(QUuid)),               this, SLOT(attaching(QUuid)));
    disconnect(adapter, SIGNAL(launching(QUuid)),               this, SLOT(attaching(QUuid)));

    // Search to see if there are any other progress dialogs associated with this adapter
    foreach(QProgressDialog *dlg, m_ProgressDialogs) {
        if(dlg->property("adapter").value<IAdapter*>() != adapter) {
            continue;
        }

        return;
    }

    // If there are no progress dialogs for this adapter; disconnect us from it completely
    disconnect(adapter, SIGNAL(attached(QUuid)),                this, SLOT(attached(QUuid)));
    disconnect(adapter, SIGNAL(launched(QUuid)),                this, SLOT(attached(QUuid)));
    disconnect(adapter, SIGNAL(progress(int,QUuid)),            this, SLOT(progress(int,QUuid)));
    disconnect(adapter, SIGNAL(progressMessage(QString,QUuid)), this, SLOT(progressMessage(QString,QUuid)));

    disconnect(adapter, SIGNAL(sampling(QUuid)),                this, SLOT(sampling(QUuid)));
    disconnect(adapter, SIGNAL(detaching(QUuid)),               this, SLOT(detaching(QUuid)));
    disconnect(adapter, SIGNAL(pausing(QUuid)),                 this, SLOT(pausing(QUuid)));
    disconnect(adapter, SIGNAL(resuming(QUuid)),                this, SLOT(resuming(QUuid)));
    disconnect(adapter, SIGNAL(canceling(QUuid)),               this, SLOT(canceling(QUuid)));

}

void SWATMainWidget::attaching(QUuid id)
{
    if(id.isNull()) {
        return;
    }

    setState(id, State_Unknown);

    QProgressDialog *dlg = new QProgressDialog(this, Qt::Dialog);
    dlg->setProperty("id", QVariant(id.toString()));
    dlg->setProperty("adapter", qVariantFromValue(ConnectionManager::currentAdapter()));
    dlg->setRange(0,100);
    dlg->resize(dlg->width()*2, dlg->height());
    dlg->show();

    m_ProgressDialogs.append(dlg);

    connect(dlg, SIGNAL(canceling()), this, SLOT(cancelAttach()));
}

void SWATMainWidget::attached(QUuid id)
{
    if(id.isNull()) {
        return;
    }

    setState(id, State_Paused);

    for(quint16 i = 0; i < m_ProgressDialogs.count(); ++i) {
        QProgressDialog *dlg = m_ProgressDialogs.at(i);
        if(dlg->property("id").toString() != id) {
            continue;
        }

        IAdapter *progressAdapter = dlg->property("adapter").value<IAdapter*>();

        m_ProgressDialogs.removeAt(i);
        dlg->deleteLater();
        checkAdapterProgress(progressAdapter);
    }
}

void SWATMainWidget::progress(int progress, QUuid id)
{
    foreach(QProgressDialog *dlg, m_ProgressDialogs) {
        if(dlg->property("id").toString() == id) {
            dlg->setValue(progress);
        }
    }
}

void SWATMainWidget::progressMessage(QString progressMessage, QUuid id)
{
    foreach(QProgressDialog *dlg, m_ProgressDialogs) {
        if(dlg->property("id").toString() == id) {
            dlg->setWindowTitle(progressMessage);
        }
    }
}

void SWATMainWidget::cancelAttach()
{
    QProgressDialog *dlg = qobject_cast<QProgressDialog *>(QObject::sender());
    if(!dlg) {
        return;
    }


    IAdapter *adapter = ConnectionManager::currentAdapter();
    if(adapter) {
        try {
            adapter->cancel(dlg->property("id").toString());
        } catch(QString err) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Error while canceling: %1").arg(err), NotificationWidget::Critical);
        } catch(...) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Error while canceling"), NotificationWidget::Critical);
        }
    }

    dlg->close();
    m_ProgressDialogs.removeAll(dlg);
    checkAdapterProgress(dlg->property("adapter").value<IAdapter*>());
    dlg->deleteLater();
}


void SWATMainWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(action->property("swat_menuitem").isValid()) {
            action->setVisible(true);
        }
    }

    m_ToolBar->show();

    if(currentWidget() && currentWidget()->property("id").isValid()) {
        m_CommandsToolBar->show();
    }

    TabWidget::showEvent(event);
}

void SWATMainWidget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(action->property("swat_menuitem").isValid()) {
            action->setVisible(false);
        }
    }

    m_ToolBar->hide();
    m_CommandsToolBar->hide();

    TabWidget::hideEvent(event);
}



void SWATMainWidget::sampled(QString filename, QUuid id)
{
    try {

        loadTraceFromFile(filename);

        // Store the ID for later process control
        this->widget(currentIndex())->setProperty("id", QVariant(id.toString()));

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to open sample: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to open sample."), NotificationWidget::Critical);
    }

}

void SWATMainWidget::sampling(QUuid id)
{
    if(id.isNull()) {
        return;
    }

    setState(id, State_Paused);
}



void SWATMainWidget::detaching(QUuid id)
{
    if(id.isNull()) {
        return;
    }

    setState(id, State_Detached);
}

void SWATMainWidget::pausing(QUuid id)
{
    if(id.isNull()) {
        return;
    }

    setState(id, State_Paused);
}

void SWATMainWidget::resuming(QUuid id)
{
    if(id.isNull()) {
        return;
    }

    setState(id, State_Running);
}

void SWATMainWidget::canceling(QUuid id)
{
    if(id.isNull()) {
        return;
    }

    setState(id, State_Detached);
}


void SWATMainWidget::doReattach()
{
    QVariant varId = this->currentWidget()->property("id");
    if(!varId.isValid()) {
        return;
    }

    IAdapter *adapter = ConnectionManager::currentAdapter();
    if(!adapter) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while reattaching: No adapter!"), NotificationWidget::Critical);
        return;
    }

    try {
        adapter->reAttach(QUuid(varId.toString()));
    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while reattaching: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while reattaching"), NotificationWidget::Critical);
    }
}

void SWATMainWidget::doDetach()
{
    QVariant varId = this->currentWidget()->property("id");
    if(!varId.isValid()) {
        return;
    }

    IAdapter *adapter = ConnectionManager::currentAdapter();
    if(!adapter) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while detaching: No adapter!"), NotificationWidget::Critical);
        return;
    }

    try {
        adapter->detach(QUuid(varId.toString()));
    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while detaching: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while detaching"), NotificationWidget::Critical);
    }
}

void SWATMainWidget::doPause()
{
    QVariant varId = this->currentWidget()->property("id");
    if(!varId.isValid()) {
        return;
    }

    IAdapter *adapter = ConnectionManager::currentAdapter();
    if(!adapter) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while pausing: No adapter!"), NotificationWidget::Critical);
        return;
    }

    try {
        adapter->pause(QUuid(varId.toString()));
    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while pausing: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while pausing"), NotificationWidget::Critical);
    }
}

void SWATMainWidget::doResume()
{
    QVariant varId = this->currentWidget()->property("id");
    if(!varId.isValid()) {
        return;
    }

    IAdapter *adapter = ConnectionManager::currentAdapter();
    if(!adapter) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while resuming: No adapter!"), NotificationWidget::Critical);
        return;
    }

    try {
        adapter->resume(QUuid(varId.toString()));
    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while resuming: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Error while resuming"), NotificationWidget::Critical);
    }
}

void SWATMainWidget::doSample()
{
    QVariant varId = this->currentWidget()->property("id");
    if(!varId.isValid()) {
        return;
    }

    //Prompt user for sample preferences
    JobControlDialog *dialog = new JobControlDialog(this);

    if(dialog->exec(JobControlDialog::Type_Sample) == QDialog::Accepted &&
            dialog->options() != NULL) {
        IAdapter *adapter = ConnectionManager::currentAdapter();

        if(!adapter) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Error while sampling: No adapter!"), NotificationWidget::Critical);
            return;
        }

        IAdapter::SampleOptions *options = (IAdapter::SampleOptions*)dialog->options();

        if(options) {
            try {
                adapter->sample(*options, QUuid(varId.toString()));
            } catch(QString err) {
                using namespace Core::MainWindow;
                MainWindow::instance().notify(tr("Error while sampling: %1").arg(err), NotificationWidget::Critical);
            } catch(...) {
                using namespace Core::MainWindow;
                MainWindow::instance().notify(tr("Error while sampling"), NotificationWidget::Critical);
            }
        }
    }

    dialog->deleteLater();
}

void SWATMainWidget::doSampleMultiple()
{
    QVariant varId = this->currentWidget()->property("id");
    if(!varId.isValid()) {
        return;
    }

    //Prompt user for multiple sample preferences
    JobControlDialog *dialog = new JobControlDialog(this);

    if(dialog->exec(JobControlDialog::Type_SampleMultiple) == QDialog::Accepted &&
            dialog->options() != NULL) {
        IAdapter *adapter = ConnectionManager::currentAdapter();

        if(!adapter) {
            using namespace Core::MainWindow;
            MainWindow::instance().notify(tr("Error while multi-sampling: No adapter!"), NotificationWidget::Critical);
            return;
        }

        IAdapter::SampleOptions *options = (IAdapter::SampleOptions*)dialog->options();

        if(options) {
            try {
                adapter->sampleMultiple(*options, QUuid(varId.toString()));
            } catch(QString err) {
                using namespace Core::MainWindow;
                MainWindow::instance().notify(tr("Error while multi-sampling: %1").arg(err), NotificationWidget::Critical);
            } catch(...) {
                using namespace Core::MainWindow;
                MainWindow::instance().notify(tr("Error while multi-sampling"), NotificationWidget::Critical);
            }
        }
    }

    dialog->deleteLater();
}



void SWATMainWidget::closeJob(int index)
{
    try {

        if(index < 0) {
            index = currentIndex();
        }


        QWidget *widget = this->widget(index);

        QVariant varId = widget->property("id");
        if(varId.isValid()) {
            QUuid id = QUuid(varId.toString());
            if(state(id) != State_Detached) {
                doDetach();
            }
            m_FrontEndStates.remove(id);
        }

        widget->close();
        removeTab(index);
        widget->deleteLater();

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to close job: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to close job."), NotificationWidget::Critical);
    }
}

void SWATMainWidget::indexChanged(int index)
{
    QVariant varId = widget(index)->property("id");
    if(varId.isValid()) {
        QUuid id = QUuid(varId.toString());
        setState(id, state(id));
    }
}


void SWATMainWidget::loadTraceFile()
{
    static QDir path = QDir::currentPath();

    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open SWAT file"),
                                                    path.absolutePath(),
                                                    tr("SWAT files (*.grl *.dot);;STAT files (*.dot)")
                                                    );

    if(!filename.isEmpty()) {
        path.setPath(QFileInfo(filename).absolutePath());

        loadTraceFromFile(filename);
    }

}


void SWATMainWidget::loadTraceFromFile(QString filename)
{
    try {

        QFileInfo fileInfo(filename);

        if(!fileInfo.exists()) {
            throw tr("File does not exist: '%1'").arg(fileInfo.absoluteFilePath());
        }

        using namespace Core::MainWindow;
        MainWindow &mainWindow = MainWindow::instance();
        mainWindow.setCurrentCentralWidget(this);

        if(fileInfo.suffix().compare("dot") == 0) {
            QFile file(fileInfo.absoluteFilePath());
            if(!file.open(QIODevice::ReadOnly)) {
                throw tr("Failed to open file: '%1'").arg(fileInfo.absoluteFilePath());
            }
            QByteArray fileContent = file.readAll();
            file.close();

            Plugins::DirectedGraph::STATWidget *view = new Plugins::DirectedGraph::STATWidget(this);
            view->setContent(fileContent);

            view->setWindowFilePath(fileInfo.absoluteFilePath());
            view->setWindowTitle(fileInfo.completeBaseName());

            int index = addTab(view, view->windowTitle());
            setCurrentIndex(index);

        } else if(fileInfo.suffix().compare("grl") == 0) {
            Plugins::DirectedGraph::SWATWidget *view = new Plugins::DirectedGraph::SWATWidget(this);
            view->loadGraphLib(fileInfo.absoluteFilePath());

            view->setWindowFilePath(fileInfo.absoluteFilePath());
            view->setWindowTitle(fileInfo.completeBaseName());

            int index = addTab(view, view->windowTitle());
            setCurrentIndex(index);

        } else {
            throw tr("Unknown file type: %1").arg(fileInfo.absoluteFilePath());

        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to create view from file: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to create view from file."), NotificationWidget::Critical);
    }

}



} // namespace SWAT
} // namespace Plugins
