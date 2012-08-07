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

#include "SettingPage.h"
#include "ui_SettingPage.h"

#include <SettingManager/SettingManager.h>
#include <ConnectionManager/ConnectionManager.h>

namespace Plugins {
namespace SWAT {

/*!
   \class SettingPage
   \brief Widget that handles settings for the SWAT plugin.

   This page is produced by the SettingPageFactory, which is registered
   with the SettingManager in the Core set of libraries.

   \sa SWATPlugin, SettingPageFactory, Core::SettingManager
 */


SettingPage::SettingPage(QWidget *parent) :
    Core::SettingManager::ISettingPage(parent),
    ui(new Ui::SettingPage)
{
    ui->setupUi(this);

#if(QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
    //Placeholder text wasn't added until Qt4.7
    ui->txtTopology->setPlaceholderText(tr("Topology Specification"));
    ui->txtNodes->setPlaceholderText(tr("Node List"));
#endif

    ui->cmbSampleType->clear();
    ui->cmbSampleType->insertItem(0, tr("Function and Line"), IAdapter::Sample_FunctionAndLine);
    ui->cmbSampleType->insertItem(0, tr("Function and PC"), IAdapter::Sample_FunctionAndPC);
    ui->cmbSampleType->insertItem(0, tr("Function Only"), IAdapter::Sample_FunctionNameOnly);
    ui->cmbSampleType->setCurrentIndex(0);

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

    initialize();
}

SettingPage::~SettingPage()
{
    delete ui;
}

void SettingPage::initialize()
{
    IAdapter *adapter = ConnectionManager::instance().currentAdapter();
    if(!adapter) {
        throw tr("No adapter found");
    }

    // Get settings from SettingManager and populate form
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT");


    QString remoteShellText = settingManager.value("remote/shell", ui->cmbRemoteShell->itemText(0)).toString();
    int remoteShellIndex = ui->cmbRemoteShell->findText(remoteShellText, Qt::MatchExactly);
    if(remoteShellIndex >= 0) {
        ui->cmbRemoteShell->setCurrentIndex(remoteShellIndex);
    } else {
        ui->cmbRemoteShell->setEditText(remoteShellText);
    }

    ui->txtRemoteHost->setText(settingManager.value("remote/host", "localhost").toString());


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

void SettingPage::apply()
{
    // Persist changed settings to SettingManager
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT");


    settingManager.setValue("remote/shell", ui->cmbRemoteShell->currentText());
    settingManager.setValue("remote/host", ui->txtRemoteHost->text());

    settingManager.setValue("sample/withThreads", ui->chkWithThreads->isChecked());
    settingManager.setValue("sample/type", ui->cmbSampleType->currentText());
    settingManager.setValue("sample/retries", ui->txtRetries->value());
    settingManager.setValue("sample/retryFrequency", ui->txtRetryFrequency->value());

    settingManager.setValue("topology/type", ui->cmbTopologyType->currentText());
    settingManager.setValue("topology/topology", ui->txtTopology->text());
    settingManager.setValue("topology/nodes", ui->txtNodes->text());
    settingManager.setValue("topology/shareNodes", ui->chkShareNodes->isChecked());
    settingManager.setValue("topology/procsPerNode", ui->txtProcsPerNode->value());

    settingManager.setValue("paths/daemon", ui->txtDaemonPath->text());
    settingManager.setValue("paths/filter", ui->txtFilterPath->text());

    settingManager.setValue("logging/frontEnd", ui->chkLogFrontEnd->isChecked());
    settingManager.setValue("logging/backEnd", ui->chkLogBackEnd->isChecked());
    settingManager.setValue("logging/path", ui->txtLogPath->text());
    settingManager.setValue("logging/verbosity", ui->cmbVerbosityType->currentText());
    settingManager.setValue("logging/debugBackEnd", ui->chkDebugBackEnds->isChecked());


    settingManager.endGroup();
}

void SettingPage::reset()
{
    initialize();
}


void SettingPage::on_btnDaemonPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Back-end daemon executable"), ui->txtDaemonPath->text());
    if(!fileName.isEmpty()) {
        ui->txtDaemonPath->setText(fileName);
    }
}

void SettingPage::on_btnFilterPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("MRNet filter shared object"), ui->txtFilterPath->text());
    if(!fileName.isEmpty()) {
        ui->txtFilterPath->setText(fileName);
    }
}

void SettingPage::on_btnLogPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Log file directory"), ui->txtLogPath->text());
    if(!path.isEmpty()) {
        ui->txtLogPath->setText(path);
    }
}


}}
