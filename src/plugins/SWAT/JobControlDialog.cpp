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

#include <SettingManager/SettingManager.h>
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

    ui->txtProcessesFitler->setText("mpirun|srun|orterun");

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

#ifdef QT_DEBUG
    ui->txtLaunchString->setText("/opt/stat/bin/orterun -np 2 /opt/stat/share/STAT/examples/bin/mpi_ringtopo");
#endif

    readSettings();

    on_btnSearchProcesses_clicked();
}

JobControlDialog::~JobControlDialog()
{
    writeSettings();

    if(m_Options) {
        delete m_Options;
        m_Options = NULL;
    }

    delete ui;
}

void JobControlDialog::writeSettings()
{
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT");

    settingManager.setValue("JobControlDialog/WindowSize", size());
    settingManager.setValue("JobControlDialog/WindowPosition", pos());

    settingManager.endGroup();
}


void JobControlDialog::readSettings()
{
    IAdapter *adapter = ConnectionManager::instance().currentAdapter();
    if(!adapter) {
        throw tr("No adapter found");
    }

    // Get settings from SettingManager and populate form
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT");

    //TODO: Set the window sizes appropriately
    resize( settingManager.value("JobControlDialog/WindowSize", size()).toSize() );
    move( settingManager.value("JobControlDialog/WindowPosition", pos()).toPoint() );


    QString processFilter = settingManager.value("attach/processFilter", ui->txtProcessesFitler->text()).toString();
    ui->txtProcessesFitler->setText(processFilter);


    QString remoteShellText = settingManager.value("remote/shell", ui->cmbRemoteShell->itemText(0)).toString();
    int remoteShellIndex = ui->cmbRemoteShell->findText(remoteShellText, Qt::MatchExactly);
    if(remoteShellIndex >= 0) {
        ui->cmbRemoteShell->setCurrentIndex(remoteShellIndex);
    } else {
        ui->cmbRemoteShell->setEditText(remoteShellText);
    }

#if 0
    ui->txtRemoteHost->setText(settingManager.value("remote/host", adapter->remoteHost()).toString());
#else
    ui->txtRemoteHost->setText(settingManager.value("remote/host", "localhost").toString());
#endif

    ui->chkWithThreads->setChecked(settingManager.value("sample/withThreads", false).toBool());

    QString sampleTypeText = settingManager.value("sample/type", ui->cmbSampleType->itemText(0)).toString();
    int indexSampleType = ui->cmbSampleType->findText(sampleTypeText, Qt::MatchExactly);
    if(indexSampleType >= 0) {
        ui->cmbSampleType->setCurrentIndex(indexSampleType);
    } else {
        ui->cmbSampleType->setCurrentIndex(0);
    }

    ui->txtRetries->setValue(settingManager.value("sample/retries", 5).toInt());

    ui->txtRetryFrequency->setValue(settingManager.value("sample/retryFrequency", 10).toInt());


    QString topologyTypeText = settingManager.value("topology/type", ui->cmbTopologyType->itemText(0)).toString();
    int topologyTypeIndex = ui->cmbTopologyType->findText(topologyTypeText, Qt::MatchExactly);
    if(topologyTypeIndex >= 0) {
        ui->cmbTopologyType->setCurrentIndex(topologyTypeIndex);
    } else {
        ui->cmbTopologyType->setCurrentIndex(0);
    }

    ui->txtTopology->setText(settingManager.value("topology/topology", "").toString());

    ui->txtNodes->setText(settingManager.value("topology/nodes", "").toString());

    ui->chkShareNodes->setChecked(settingManager.value("topology/shareNodes", false).toBool());

    //TODO: Get the default from the hardware
    ui->txtProcsPerNode->setValue(settingManager.value("topology/procsPerNode", 8).toInt());


    ui->txtDaemonPath->setText(settingManager.value("paths/daemon", adapter->defaultToolDaemonPath()).toString());

    ui->txtFilterPath->setText(settingManager.value("paths/filter", adapter->defaultFilterPath()).toString());


    ui->chkLogFrontEnd->setChecked(settingManager.value("logging/frontEnd", false).toBool());

    ui->chkLogBackEnd->setChecked(settingManager.value("logging/backEnd", false).toBool());

    ui->txtLogPath->setText(settingManager.value("logging/path", (QDesktopServices::storageLocation(QDesktopServices::DataLocation))).toString());

    QString verbosityTypeText = settingManager.value("logging/verbosity", ui->cmbVerbosityType->itemText(0)).toString();
    int verbosityTypeIndex = ui->cmbVerbosityType->findText(verbosityTypeText, Qt::MatchExactly);
    if(verbosityTypeIndex >= 0) {
        ui->cmbVerbosityType->setCurrentIndex(verbosityTypeIndex);
    } else {
        ui->cmbVerbosityType->setCurrentIndex(0);
    }

    ui->chkDebugBackEnds->setChecked(settingManager.value("logging/debugBackEnd", false).toBool());


    settingManager.endGroup();
}


void JobControlDialog::setType(Types type)
{
    m_Type = type;
    int index = 0;
    switch(type) {
      case Type_Attach:
        setWindowTitle(tr("Attach to Job Dialog"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Attach"));
        ui->tabLaunch->setEnabled(false);
        ui->tabAttach->setEnabled(true);


        index = ui->tabWidget->indexOf(ui->tabLaunch);
        if(index >= 0) {
            ui->tabWidget->removeTab(index);
        }

        index = ui->tabWidget->indexOf(ui->tabAttach);
        if(index < 0) {
            ui->tabWidget->insertTab(0, ui->tabAttach, tr("Attach"));
        }

        ui->tabWidget->setCurrentWidget(ui->tabAttach);
        break;

      case Type_Launch:
        setWindowTitle(tr("Launch Job Dialog"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Launch"));
        ui->tabAttach->setEnabled(false);
        ui->tabLaunch->setEnabled(true);

        index = ui->tabWidget->indexOf(ui->tabAttach);
        if(index >= 0) {
            ui->tabWidget->removeTab(index);
        }

        index = ui->tabWidget->indexOf(ui->tabLaunch);
        if(index < 0) {
            ui->tabWidget->insertTab(0, ui->tabLaunch, tr("Launch"));
        }

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

            attachOptions->pid = selectedItems.at(0)->data(Qt::UserRole + 1).toULongLong(&convertedOkay);
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

    QString filter = ui->txtProcessesFitler->text();

    QString program = "/bin/ps";
    QStringList arguments;
    arguments << "w" << "w" << "x" << "o" << "pid,command";

    QByteArray output;
    if(remoteHost == "localhost") {
        QProcess process;
        process.start(program, arguments);

        if(!process.waitForStarted()) {
            return;
        }

        if(!process.waitForFinished()) {
            return;
        }

        output = process.readAll();

    } else {
        //TODO: Get remote host processes
        //QString command = QString("%1 %2 ps wx").arg(remoteShell, remoteHost);
    }


    QStringList lines = QString(output).split('\n');

    QMap<quint64, QString> processes;

    static const QRegExp rxPid("\\s*PID");
    static const QRegExp rxCommand("COMMAND\\s*$");

    int commandIndex = 0;
    foreach(QString line, lines) {
        if(line.contains(rxPid)) {
            commandIndex = line.indexOf(rxCommand);
            continue;
        }

        bool okay = false;
        QString trimmed = line.trimmed();
        quint64 pid = trimmed.left(trimmed.indexOf(' ')).toULongLong(&okay);

        if(okay) {
            processes.insert(pid, line.right(line.count() - commandIndex));
        }
    }

    ui->lstProcesses->clear();

    const QRegExp rxFilter = QRegExp(filter, Qt::CaseInsensitive);
    foreach(quint64 pid, processes.keys()) {
        if(!processes.value(pid).contains(rxFilter)) {
            continue;
        }

        QListWidgetItem *item = new QListWidgetItem(processes.value(pid));
        item->setData(Qt::UserRole + 1, QVariant(pid));
        ui->lstProcesses->addItem(item);
    }
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
