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
#include <ConnectionManager/ConnectionManager.h>

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

    Core::MainWindow::MainWindow &mainWindow = Core::MainWindow::MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
        if(action->text() == tr("File")) {

            //! \todo We really need to rely on the ActionManager to do this.
            QAction *before = NULL;
            foreach(QAction *item, action->menu()->actions()) {
                if(item->priority() == QAction::LowPriority) {
                    before = item;
                }
            }
        }
    }
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
        IAdapter *adapter = ConnectionManager::instance().currentAdapter();

        if(!adapter) {
            //TODO: display error
            return;
        }

        IAdapter::AttachOptions *options = (IAdapter::AttachOptions*)dialog->getOptions();

        if(options) {
            QProgressDialog *dlg = new QProgressDialog(this, Qt::Dialog);
            dlg->setRange(0,100);
            connect(adapter, SIGNAL(progress(int)), dlg, SLOT(setValue(int)));
            connect(adapter, SIGNAL(progressMessage(QString,QUuid)), dlg, SLOT(setLabelText(QString)));

            try {
                adapter->attach(*options);
            } catch(QString err) {
                Core::MainWindow::MainWindow::instance().notify(tr("Error while attaching: %1").arg(err), Core::MainWindow::NotificationWidget::Critical);
            }

            dlg->deleteLater();
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
        IAdapter *adapter = ConnectionManager::instance().currentAdapter();

        if(!adapter) {
            //TODO: display error
            return;
        }

        IAdapter::LaunchOptions *options = (IAdapter::LaunchOptions*)dialog->getOptions();

        if(options) {
            QProgressDialog *dlg = new QProgressDialog(this, Qt::Dialog);
            dlg->setRange(0,100);
            connect(adapter, SIGNAL(progress(int)), dlg, SLOT(setValue(int)));
            connect(adapter, SIGNAL(progressMessage(QString,QUuid)), dlg, SLOT(setLabelText(QString)));

            try {
                adapter->launch(*options);
            } catch(QString err) {
                Core::MainWindow::MainWindow::instance().notify(tr("Error while launching: %1").arg(err), Core::MainWindow::NotificationWidget::Critical);
            }

            dlg->deleteLater();
        }
    }

    dialog->deleteLater();

    setStyleSheet(currentStyleSheet);  // Reset the stylesheet
}


} // namespace SWAT
} // namespace Plugins
