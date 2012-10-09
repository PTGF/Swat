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

#ifndef PLUGINS_DIRECTEDGRAPH_STATNODEDIALOG_H
#define PLUGINS_DIRECTEDGRAPH_STATNODEDIALOG_H

#include <QDialog>

namespace Plugins {
namespace DirectedGraph {

class STATNode;

namespace Ui {
class STATNodeDialog;
}

class STATNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit STATNodeDialog(QWidget *parent = 0);
    ~STATNodeDialog();

    void setNode(STATNode *node);
    STATNode *node();

protected:
//    void updateStackFrame();
//    void updateLeafTasks();
//    void setTotalTasks(QString totalTaskCount, QString totalTasks);

protected slots:
    void on_btnCollapse_toggled(bool checked);
    void on_btnCollapseDepth_clicked();
    void on_btnFocus_clicked();
    void on_btnViewSource_clicked();

    void onFinished();


private:
    Ui::STATNodeDialog *ui;
    STATNode *m_Node;

};

} // namespace DirectedGraph
} // namespace Plugins

#endif // PLUGINS_DIRECTEDGRAPH_STATNODEDIALOG_H
