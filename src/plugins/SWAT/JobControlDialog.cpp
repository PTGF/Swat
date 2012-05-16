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

#include "JobControlDialog.h"
#include "ui_JobControlDialog.h"

#include <ConnectionManager/ConnectionManager.h>

#ifdef QT_DEBUG
    #include <QtDebug>
#endif

namespace Plugins {
namespace SWAT {

JobControlDialog::JobControlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JobControlDialog),
    m_Options(NULL),
    m_Type(Type_Attach)
{
    ui->setupUi(this);

    this->setStyleSheet(QString());

#if(QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
    //Placeholder text wasn't added until Qt4.7
    ui->txtProcessesFitler->setPlaceholderText(tr("Filter"));
    ui->txtLaunchString->setPlaceholderText(tr("Launch String"));
    ui->txtTopology->setPlaceholderText(tr("Topology Specification"));
    ui->txtNodes->setPlaceholderText(tr("Node List"));
#endif

    setType(Type_Attach);

    ui->cmbTopologyType->clear();
    ui->cmbTopologyType->insertItem(0, tr("User"), IAdapter::Topology_User);
    ui->cmbTopologyType->insertItem(0, tr("Fan Out"), IAdapter::Topology_FanOut);
    ui->cmbTopologyType->insertItem(0, tr("Depth"), IAdapter::Topology_Depth);
    ui->cmbTopologyType->insertItem(0, tr("Automatic"), IAdapter::Topology_Auto);
    ui->cmbTopologyType->setCurrentIndex(0);

    ui->cmbVerbosityType->clear();
    ui->cmbVerbosityType->insertItem(0, tr("Full"), IAdapter::Verbose_Error | IAdapter::Verbose_StdOut);
    ui->cmbVerbosityType->insertItem(0, tr("Standard"), IAdapter::Verbose_StdOut);
    ui->cmbVerbosityType->insertItem(0, tr("Error"), IAdapter::Verbose_Error);
    ui->cmbVerbosityType->setCurrentIndex(0);

    ui->cmbSampleType->clear();
    ui->cmbSampleType->insertItem(0, tr("Function and Line"), IAdapter::Sample_FunctionAndLine);
    ui->cmbSampleType->insertItem(0, tr("Function and PC"), IAdapter::Sample_FunctionAndPC);
    ui->cmbSampleType->insertItem(0, tr("Function Only"), IAdapter::Sample_FunctionNameOnly);
    ui->cmbSampleType->setCurrentIndex(0);

    ui->txtLogPath->setText(QDesktopServices::storageLocation(QDesktopServices::DataLocation));


    IAdapter *adapter = ConnectionManager::instance().currentAdapter();
    if(!adapter) {
        throw tr("No adapter found");
    }
    ui->txtFilterPath->setText(adapter->defaultFilterPath());
    ui->txtDaemonPath->setText(adapter->defaultToolDaemonPath());

#if 0
    ui->txtRemoteHost->setText(adapter->remoteHost());
#else
    ui->txtRemoteHost->setText("localhost");
#endif

#ifdef QT_DEBUG
    ui->txtLaunchString->setText("/opt/stat/bin/orterun -np 2 /opt/stat/share/STAT/examples/bin/mpi_ringtopo");
#endif
}

JobControlDialog::~JobControlDialog()
{
    if(m_Options) {
        delete m_Options;
        m_Options = NULL;
    }

    delete ui;
}

void JobControlDialog::setType(Types type)
{
    m_Type = type;
    switch(type) {
      case Type_Attach:

        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Attach"));

        ui->tabLaunch->setEnabled(false);
        ui->tabAttach->setEnabled(true);
        ui->tabWidget->setCurrentWidget(ui->tabAttach);

        break;

      case Type_Launch:

        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Launch"));

        ui->tabAttach->setEnabled(false);
        ui->tabLaunch->setEnabled(true);
        ui->tabWidget->setCurrentWidget(ui->tabLaunch);

        break;
    }
}

void JobControlDialog::accept()
{
    if(m_Options) {
        delete m_Options;
        m_Options = NULL;
    }

    QString errorStyleSheet = "background-color: #FFEEEE;";

    bool convertedOkay;
    uint errorsFound = 0;

    IAdapter::Options *options = NULL;

    if(m_Type == Type_Attach) {
        IAdapter::AttachOptions *attachOptions = new IAdapter::AttachOptions();

        QList<QListWidgetItem*> selectedItems = ui->lstProcesses->selectedItems();
        if(selectedItems.count() != 1) {
            ui->lstProcesses->setToolTip("A process must be selected");
            ui->lstProcesses->setStyleSheet(errorStyleSheet);
            ui->tabWidget->setCurrentWidget(ui->tabAttach);
            errorsFound++;
        } else {
            ui->lstProcesses->setToolTip(QString());
            ui->lstProcesses->setStyleSheet(QString());

            attachOptions->pid = selectedItems.at(0)->data(Qt::UserRole).toULongLong(&convertedOkay);
            if(!convertedOkay) {
                throw tr("Conversion of PID value failed");
            }
        }

        options = attachOptions;

    } else {
        IAdapter::LaunchOptions *launchOptions = new IAdapter::LaunchOptions();

        if(ui->txtLaunchString->text().isEmpty() || ui->txtLaunchString->text().split(" ", QString::SkipEmptyParts).count() <= 1) {
            ui->txtLaunchString->setToolTip(tr("A valid launch string must be supplied"));
            ui->txtLaunchString->setStyleSheet(errorStyleSheet);
            ui->tabWidget->setCurrentWidget(ui->tabLaunch);
            errorsFound++;
        } else {
            ui->txtLaunchString->setToolTip(QString());
            ui->txtLaunchString->setStyleSheet(QString());
            launchOptions->args = ui->txtLaunchString->text().split(" ", QString::SkipEmptyParts);
        }

        options = launchOptions;
    }


    if(ui->txtDaemonPath->text().isEmpty() || !QFile::exists(ui->txtDaemonPath->text())) {
        ui->txtDaemonPath->setToolTip(tr("A valid daemon path must be supplied"));
        ui->txtDaemonPath->setStyleSheet(errorStyleSheet);
        ui->tabWidget->setCurrentWidget(ui->tabAdvanced);
        errorsFound++;
    } else {
        ui->txtDaemonPath->setToolTip(QString());
        ui->txtDaemonPath->setStyleSheet(QString());
        options->toolDaemonPath = ui->txtDaemonPath->text();
    }

    if(ui->txtFilterPath->text().isEmpty() || !QFile::exists(ui->txtFilterPath->text())) {
        ui->txtFilterPath->setToolTip(tr("A valid daemon path must be supplied"));
        ui->txtFilterPath->setStyleSheet(errorStyleSheet);
        ui->tabWidget->setCurrentWidget(ui->tabAdvanced);
        errorsFound++;
    } else {
        ui->txtFilterPath->setToolTip(QString());
        ui->txtFilterPath->setStyleSheet(QString());
        options->filterPath = ui->txtFilterPath->text();
    }

    if(ui->txtLogPath->text().isEmpty() || !QFile::exists(ui->txtLogPath->text())) {
        ui->txtLogPath->setToolTip(tr("A valid log path must be supplied"));
        ui->txtLogPath->setStyleSheet(errorStyleSheet);
        ui->tabWidget->setCurrentWidget(ui->tabAdvanced);
        errorsFound++;
    } else {
        ui->txtLogPath->setToolTip(QString());
        ui->txtLogPath->setStyleSheet(QString());
        options->logPath = ui->txtLogPath->text();
    }

    options->logFlags = IAdapter::Log_None;
    if(ui->chkLogFrontEnd->isChecked()) {
        options->logFlags |= IAdapter::Log_FrontEnd;
    }
    if(ui->chkLogBackEnd->isChecked()) {
        options->logFlags |= IAdapter::Log_BackEnd;
    }


    options->verboseFlags = ui->cmbVerbosityType->itemData(ui->cmbVerbosityType->currentIndex()).toInt(&convertedOkay);
    if(!convertedOkay) {
        options->verboseFlags = IAdapter::Verbose_None;
    }

    options->debugFlags = IAdapter::Debug_None;
    if(ui->chkDebugBackEnds->isChecked()) {
        options->debugFlags |= IAdapter::Debug_BackEnd;
    }

    int topologyType = ui->cmbTopologyType->itemData(ui->cmbTopologyType->currentIndex()).toInt(&convertedOkay);
    if(convertedOkay) {
        options->topologyType = (IAdapter::TopologyType)topologyType;
    } else {
        options->topologyType = IAdapter::Topology_Auto;
    }

    if(options->topologyType == IAdapter::Topology_User) {
        if(ui->txtTopology->text().isEmpty()) {
            ui->txtTopology->setToolTip(tr("A valid topology must be supplied if the 'user' topology is selected"));
            ui->txtTopology->setStyleSheet(errorStyleSheet);
            ui->tabWidget->setCurrentWidget(ui->tabTopology);
            errorsFound++;
        } else {
            ui->txtTopology->setToolTip(QString());
            ui->txtTopology->setStyleSheet(QString());
            options->topologySpecification = ui->txtTopology->text();
        }

    }

    options->nodeList = ui->txtNodes->text().split(" ", QString::SkipEmptyParts);

    options->processesPerNode = ui->txtProcsPerNode->value();

    options->shareApplicationNodes = ui->chkShareNodes->isChecked();

    int sampleType = ui->cmbSampleType->itemData(ui->cmbSampleType->currentIndex()).toInt(&convertedOkay);
    if(convertedOkay) {
        options->sampleType = (IAdapter::SampleType)sampleType;
    } else {
        options->sampleType = IAdapter::Sample_FunctionNameOnly;
    }

    options->withThreads = ui->chkWithThreads->isChecked();

    options->retryCount = ui->txtRetries->value();

    options->retryFrequency = ui->txtRetryFrequency->value();

    //options->clearOnSample = ;

    //options->traceCount = ;

    //options->traceFrequency = ;

    //options->runTimeBeforeSample = ;


    if(ui->cmbRemoteShell->currentText().isEmpty()) {
        ui->cmbRemoteShell->setToolTip(tr("A remote shell must be supplied"));
        ui->cmbRemoteShell->setStyleSheet(errorStyleSheet);
        ui->tabWidget->setCurrentWidget(ui->tabRemote);
        errorsFound++;
    } else {
        ui->cmbRemoteShell->setToolTip(QString());
        ui->cmbRemoteShell->setStyleSheet(QString());
        options->remoteShell = ui->cmbRemoteShell->currentText();
    }

    if(ui->txtRemoteHost->text().isEmpty()) {
        ui->txtRemoteHost->setToolTip(tr("A valid remote host must be supplied"));
        ui->txtRemoteHost->setStyleSheet(errorStyleSheet);
        ui->tabWidget->setCurrentWidget(ui->tabRemote);
        errorsFound++;
    } else {
        ui->txtRemoteHost->setToolTip(QString());
        ui->txtRemoteHost->setStyleSheet(QString());
        options->remoteHost = ui->txtRemoteHost->text();
    }


    if(errorsFound > 0) {
        return;
    }

    m_Options = options;
    QDialog::accept();
}

int JobControlDialog::exec(Types type)
{
    if(m_Options) {
        delete m_Options;
        m_Options = NULL;
    }

    setType(type);
    return QDialog::exec();
}

IAdapter::Options *JobControlDialog::getOptions()
{
    return m_Options;
}

void JobControlDialog::on_btnSearchProcesses_clicked()
{

    QString remoteShell = ui->cmbRemoteShell->currentText();
    QString remoteHost = ui->txtRemoteHost->text();

    QString command = QString("%1 %2 ps wx").arg(remoteShell, remoteHost);

    //TODO:


}

void JobControlDialog::on_btnDaemonPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Back-end daemon executable"), ui->txtDaemonPath->text());
    if(!fileName.isEmpty()) {
        ui->txtDaemonPath->setText(fileName);
    }
}

void JobControlDialog::on_btnFilterPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("MRNet filter shared object"), ui->txtFilterPath->text());
    if(!fileName.isEmpty()) {
        ui->txtFilterPath->setText(fileName);
    }
}

void JobControlDialog::on_btnLogPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Log file directory"), ui->txtLogPath->text());
    if(!path.isEmpty()) {
        ui->txtLogPath->setText(path);
    }
}

} // namespace SWAT
} // namespace Plugins
