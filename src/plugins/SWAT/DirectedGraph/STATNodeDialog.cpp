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

#include "STATNodeDialog.h"
#include "ui_STATNodeDialog.h"

#include "STATWidget.h"
#include "STATNode.h"

namespace Plugins {
namespace DirectedGraph {

STATNodeDialog::STATNodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::STATNodeDialog)
{
    ui->setupUi(this);

    ui->grpLeafTasks->setVisible(false);
    ui->grpTotalTasks->setVisible(false);

    connect(this, SIGNAL(finished(int)), this, SLOT(onFinished()));
}

void STATNodeDialog::onFinished()
{
    // Deselect the node so the user can reselect the same node again without effort
    m_Node->setSelected(false);
}


STATNodeDialog::~STATNodeDialog()
{
    delete ui;
}

void STATNodeDialog::setNode(STATNode *node)
{
    if(node == m_Node) {
        return;
    }

    m_Node = node;

    ui->grpLeafTasks->setVisible(false);
    ui->grpTotalTasks->setVisible(false);

    if(!m_Node) {
        return;
    }

    ui->btnCollapse->setChecked(m_Node->isCollapsed());

    ui->txtStackFrame->setText(m_Node->label());

    ui->btnViewSource->setEnabled(!m_Node->sourceFile().isEmpty());

    //TODO: Leaf Tasks

    if(!m_Node->processCount().isEmpty() || !m_Node->processList().count()) {
        ui->grpTotalTasks->setVisible(true);

        bool okay = false;
        QString taskTitle = tr("Unknown Total Tasks");
        qulonglong taskCount = m_Node->processCount().toULongLong(&okay);
        if(okay) {
            if(taskCount == 1) {
                taskTitle = tr("%L1 Total Task").arg(taskCount);
            } else {
                taskTitle = tr("%L1 Total Tasks").arg(taskCount);
            }
        }
        ui->grpTotalTasks->setTitle(taskTitle);

        ui->txtTotalTasks->setText(m_Node->processList().join(", "));
    }
}

STATNode *STATNodeDialog::node()
{
    return m_Node;
}


void STATNodeDialog::on_btnCollapse_toggled(bool checked)
{
    if(!m_Node) { return; }

    if(DirectedGraphWidget *view = qobject_cast<DirectedGraphWidget *>(parent())) {
        checked ? view->doCollapse(m_Node) : view->doExpand(m_Node);
    }

}

void STATNodeDialog::on_btnCollapseDepth_clicked()
{
    if(!m_Node) { return; }

    if(DirectedGraphWidget *view = qobject_cast<DirectedGraphWidget *>(parent())) {
        view->doCollapseDepth(m_Node->nodeDepth());
    }

}

void STATNodeDialog::on_btnFocus_clicked()
{
    if(!m_Node) { return; }

    if(STATWidget *view = qobject_cast<STATWidget *>(parent())) {
        view->doFocus(m_Node);
    }

}

void STATNodeDialog::on_btnViewSource_clicked()
{
    if(!m_Node) {
        return;
    }

    if(STATWidget *view = qobject_cast<STATWidget *>(parent())) {
        view->openSourceFile(m_Node->sourceFile(), m_Node->sourceLine());
    }
}

} // namespace DirectedGraph
} // namespace Plugins
