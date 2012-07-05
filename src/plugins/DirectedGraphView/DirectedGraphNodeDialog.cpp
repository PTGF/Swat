/*!
   \file
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the Parallel Tools GUI Framework (PTGF)
   Copyright (C) 2010-2011 Argo Navis Technologies, LLC

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

#include "DirectedGraphNodeDialog.h"
#include "ui_DirectedGraphNodeDialog.h"

DirectedGraphNodeDialog::DirectedGraphNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DirectedGraphNodeDialog)
{
    ui->setupUi(this);

    ui->grpLeafTasks->setVisible(false);
    ui->grpTotalTasks->setVisible(false);
}

DirectedGraphNodeDialog::~DirectedGraphNodeDialog()
{
    delete ui;
}

void DirectedGraphNodeDialog::setStackFrame(QString stackFrame)
{
    ui->txtStackFrame->setText(stackFrame);
}

void DirectedGraphNodeDialog::setLeafTasks(QString leafTasks)
{
    ui->grpLeafTasks->setVisible(true);

    //TODO: Set the group box text to the task count total
    ui->txtLeafTasks->setText(leafTasks);
}

void DirectedGraphNodeDialog::setTotalTasks(QString totalTaskCount, QString totalTasks)
{
    ui->grpTotalTasks->setVisible(true);

    QString taskTitle;

    bool okay;
    quint64 taskCount = totalTaskCount.toULongLong(&okay);
    if(okay) {
        if(taskCount == 1) {
            taskTitle = tr("%L1 Total Task").arg(taskCount);
        } else {
            taskTitle = tr("%L1 Total Tasks").arg(taskCount);
        }
    } else {
        taskTitle = "Unknown Total Tasks";
    }

    ui->grpTotalTasks->setTitle(taskTitle);
    ui->txtTotalTasks->setText(totalTasks);
}
