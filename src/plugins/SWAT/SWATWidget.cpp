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

#include "SWATWidget.h"
#include "ui_SWATWidget.h"

#include <MainWindow/MainWindow.h>
#include <MainWindow/NotificationWidget.h>
#include <PluginManager/PluginManager.h>
#include <ConnectionManager/ConnectionManager.h>
#include <ViewManager/IViewFactory.h>

#include "JobControlDialog.h"

namespace Plugins {
namespace SWAT {

SWATWidget::SWATWidget(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::SWATWidget)
{
    ui->setupUi(this);

    /* Set up the stylesheet for periods when we have tabs */
    m_StyleSheet = styleSheet();

    setWindowTitle(QString("SWAT%1").arg(QChar(0x2122))); //Trademark
    setWindowIcon(QIcon(":/SWAT/app.gif"));

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
        if(action->text() == tr("File")) {

            m_AttachJob = new QAction(tr("Attach to Job"), this);
            m_AttachJob->setToolTip(tr("Attach SWAT to a running job"));
            m_AttachJob->setIcon(QIcon(":/SWAT/app.gif"));
            m_AttachJob->setIconVisibleInMenu(true);
            m_AttachJob->setVisible(false);
            m_AttachJob->setProperty("swat_menuitem", QVariant(1));
            connect(m_AttachJob, SIGNAL(triggered()), this, SLOT(attachJob()));

            m_LaunchJob = new QAction(tr("Launch a Job"), this);
            m_LaunchJob->setToolTip(tr("Launch job and attach SWAT"));
            m_LaunchJob->setIcon(QIcon(":/SWAT/app.gif"));
            m_LaunchJob->setIconVisibleInMenu(true);
            m_LaunchJob->setVisible(false);
            m_LaunchJob->setProperty("swat_menuitem", QVariant(1));
            connect(m_LaunchJob, SIGNAL(triggered()), this, SLOT(launchJob()));

            m_LoadFile = new QAction(tr("Load from File"), this);
            m_LoadFile->setToolTip(tr("Load SWAT output from saved file"));
            m_LoadFile->setIcon(QIcon(":/SWAT/app.gif"));
            m_LoadFile->setIconVisibleInMenu(true);
            m_LoadFile->setVisible(false);
            m_LoadFile->setProperty("swat_menuitem", QVariant(1));
            connect(m_LoadFile, SIGNAL(triggered()), this, SLOT(loadFile()));

            m_CloseJob = new QAction(tr("Close Job"), this);
            m_CloseJob->setToolTip(tr("Close current SWAT job"));
            m_CloseJob->setIcon(QIcon(":/SWAT/app.gif"));
            m_CloseJob->setIconVisibleInMenu(true);
            m_CloseJob->setVisible(false);
            m_CloseJob->setEnabled(false);
            m_CloseJob->setProperty("swat_menuitem", QVariant(1));
            connect(m_CloseJob, SIGNAL(triggered()), this, SLOT(closeJob()));


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
            } else {
                action->menu()->addAction(m_LaunchJob);
                action->menu()->addAction(m_AttachJob);
                action->menu()->addAction(m_LoadFile);
                action->menu()->addAction(m_CloseJob);
            }

        }
    }

    CurrentAdapterChanged(NULL, ConnectionManager::currentAdapter());
    connect(&ConnectionManager::instance(), SIGNAL(CurrentAdapterChanged(IAdapter*,IAdapter*)),
            this, SLOT(CurrentAdapterChanged(IAdapter*,IAdapter*)));
}

SWATWidget::~SWATWidget()
{
    delete ui;
}

void SWATWidget::tabInserted(int index)
{
    Q_UNUSED(index)

    if(this->count() < 2) {
        this->tabBar()->hide();
    } else {
        this->tabBar()->show();
    }

    /* Set the stylesheet to nothing if we have tabs, otherwise we'll run into display issues */
    if(count() <= 0) {
        setStyleSheet(m_StyleSheet);
    } else {
        setStyleSheet(QString());
    }

    m_CloseJob->setEnabled(count());
}

void SWATWidget::tabRemoved(int index)
{
    Q_UNUSED(index)

    if(this->count() < 2) {
        this->tabBar()->hide();
    } else {
        this->tabBar()->show();
    }

    /* Set the stylesheet to nothing if we have tabs, otherwise we'll run into display issues */
    if(count() <= 0) {
        setStyleSheet(m_StyleSheet);
    } else {
        setStyleSheet(QString());
    }

    m_CloseJob->setEnabled(count());
}

void SWATWidget::tabTitleChanged()
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

void SWATWidget::attachJob()
{
    // Stylesheet screws with the dialog
    QString currentStyleSheet = this->styleSheet();
    setStyleSheet(QString());

    //Prompt user for finding and attaching to a running job
    JobControlDialog *dialog = new JobControlDialog(this);

    if(dialog->exec(JobControlDialog::Type_Attach) == QDialog::Accepted &&
            dialog->getOptions() != NULL) {
        IAdapter *adapter = ConnectionManager::currentAdapter();

        if(!adapter) {
            //TODO: display error
            return;
        }

        IAdapter::AttachOptions *options = (IAdapter::AttachOptions*)dialog->getOptions();

        connect(adapter, SIGNAL(attaching(QUuid)), this, SLOT(attaching(QUuid)));

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

void SWATWidget::launchJob()
{
    // Stylesheet screws with the dialog
    QString currentStyleSheet = this->styleSheet();
    setStyleSheet(QString());

    //Prompt user for launching and attaching to a new job
    JobControlDialog *dialog = new JobControlDialog(this);

    if(dialog->exec(JobControlDialog::Type_Launch) == QDialog::Accepted &&
            dialog->getOptions() != NULL) {
        IAdapter *adapter = ConnectionManager::currentAdapter();

        if(!adapter) {
            //TODO: display error
            return;
        }

        IAdapter::LaunchOptions *options = (IAdapter::LaunchOptions*)dialog->getOptions();

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

void SWATWidget::CurrentAdapterChanged(IAdapter* from, IAdapter* to)
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
    }
}

void SWATWidget::checkAdapterProgress(IAdapter *adapter)
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
}

void SWATWidget::attaching(QUuid id)
{
    QProgressDialog *dlg = new QProgressDialog(this, Qt::Dialog);
    dlg->setProperty("id", QVariant(id.toString()));
    dlg->setProperty("adapter", qVariantFromValue(ConnectionManager::currentAdapter()));
    dlg->setRange(0,100);
    dlg->resize(dlg->width()*2, dlg->height());
    dlg->show();

    m_ProgressDialogs.append(dlg);

    connect(dlg, SIGNAL(canceled()), this, SLOT(cancelAttach()));
}

void SWATWidget::attached(QUuid id)
{
    for(quint16 i = 0; i < m_ProgressDialogs.count(); ++i) {
        QProgressDialog *dlg = m_ProgressDialogs.at(i);
        if(dlg->property("id").toString() != id) {
            continue;
        }

        IAdapter *progressAdapter = dlg->property("adapter").value<IAdapter*>();

        m_ProgressDialogs.removeAt(i);
        dlg->deleteLater();
        checkAdapterProgress(progressAdapter);

        break;
    }
}

void SWATWidget::progress(int progress, QUuid id)
{
    foreach(QProgressDialog *dlg, m_ProgressDialogs) {
        if(dlg->property("id").toString() == id) {
            dlg->setValue(progress);
        }
    }
}

void SWATWidget::progressMessage(QString progressMessage, QUuid id)
{
    foreach(QProgressDialog *dlg, m_ProgressDialogs) {
        if(dlg->property("id").toString() != id) {
            dlg->setWindowTitle(progressMessage);
        }
    }
}

void SWATWidget::cancelAttach()
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


void SWATWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(action->property("swat_menuitem").isValid()) {
            action->setVisible(true);
        }
    }
}

void SWATWidget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(action->property("swat_menuitem").isValid()) {
            action->setVisible(false);
        }
    }
}


void SWATWidget::sampled(QString content, QUuid id)
{
    loadFromContent(content.toLocal8Bit());
}

void SWATWidget::closeJob(int index)
{
    try {

        if(index < 0) {
            index = currentIndex();
        }


        QWidget *widget = this->widget(currentIndex());
        widget->close();
        removeTab(currentIndex());
        widget->deleteLater();

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to close job: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to close job."), NotificationWidget::Critical);
    }
}

void SWATWidget::loadFile()
{
    static QDir path = QDir::currentPath();

    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open SWAT file"),
                                                    path.absolutePath(),
                                                    tr("SWAT files (*.dot *.swat);;STAT files (*.dot)")
                                                    );

    if(!filename.isEmpty()) {
        path.setPath(QFileInfo(filename).absolutePath());

        loadFromFile(filename);
    }

}


void SWATWidget::loadFromFile(QString filename)
{
    QFileInfo fileInfo(filename);

    if(!fileInfo.exists()) {
        throw tr("File does not exist: '%1'").arg(fileInfo.absoluteFilePath());
    }

    QFile file(fileInfo.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly)) {
        throw tr("Failed to open file: '%1'").arg(fileInfo.absoluteFilePath());
    }

    QByteArray fileContent = file.readAll();

    file.close();

    loadFromContent(fileContent, fileInfo.completeBaseName());
}

void SWATWidget::loadFromContent(QByteArray content, QString title)
{
    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    mainWindow.setCurrentCentralWidget(this);

    if(QWidget *view = getView(content)) {
        if(!title.isEmpty()) {
            view->setWindowTitle(title);
        }

        int index = addTab(view, view->windowTitle());
        setCurrentIndex(index);
    }
}

QWidget *SWATWidget::getView(QByteArray content)
{
    try {

        Core::PluginManager::PluginManager &pluginManager = Core::PluginManager::PluginManager::instance();
        foreach(QObject *object, pluginManager.allObjects()) {
            IViewFactory *viewFactory = qobject_cast<IViewFactory *>(object);
            if(viewFactory) {
                if(viewFactory->viewHandlesFiles()) {
                    return viewFactory->viewWidget(content);
                }
            }
        }

    } catch(QString err) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to create view from content: %1").arg(err), NotificationWidget::Critical);
    } catch(...) {
        using namespace Core::MainWindow;
        MainWindow::instance().notify(tr("Failed to create view from content."), NotificationWidget::Critical);
    }

    return NULL;
}



} // namespace SWAT
} // namespace Plugins
