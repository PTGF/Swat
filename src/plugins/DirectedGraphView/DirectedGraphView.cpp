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

#include "DirectedGraphView.h"
#include "ui_DirectedGraphView.h"

#include <QGraphVizView.h>

#include <MainWindow/MainWindow.h>
#include <PluginManager/PluginManager.h>
#include <SourceView/ISourceViewFactory.h>

#include "DirectedGraphScene.h"
#include "DirectedGraphNode.h"
#include "DirectedGraphNodeDialog.h"

namespace Plugins {
namespace DirectedGraphView {

DirectedGraphView::DirectedGraphView(const QByteArray &content, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirectedGraphView),
    m_Scene(NULL),
    m_View(NULL),
    m_Id(QUuid::createUuid()),
    m_HideMPI(NULL)
{
    ui->setupUi(this);

    m_Scene = new DirectedGraphScene(QString(content));
    m_View = new QGraphVizView(m_Scene, this);
    m_Scene->setParent(m_View);

    setWindowTitle(tr("Directed Graph View"));

    ui->layStack->insertWidget(0, m_View);

    connect(m_Scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    m_UndoStack = new QUndoStack(this);

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
        if(action->text() == tr("Edit")) {
            QAction *undo = m_UndoStack->createUndoAction(this);
            undo->setIcon(QIcon(":/SWAT/app.gif"));
            undo->setProperty("swatView_menuitem", m_Id.toString());

            QAction *redo = m_UndoStack->createRedoAction(this);
            redo->setIcon(QIcon(":/SWAT/app.gif"));
            redo->setProperty("swatView_menuitem", m_Id.toString());

            //! \todo We really need to rely on the ActionManager to do this.
            QAction *before = NULL;
            foreach(QAction *item, action->menu()->actions()) {
                if(item->priority() == QAction::LowPriority) {
                    before = item;
                }
            }

            if(before) {
                action->menu()->insertAction(before, undo);
                action->menu()->insertAction(before, redo);
                action->menu()->insertSeparator(before)->setProperty("swatView_menuitem", m_Id.toString());
            } else {
                action->menu()->addAction(undo);
                action->menu()->addAction(redo);
                action->menu()->addSeparator()->setProperty("swatView_menuitem", m_Id.toString());
            }
        }

        if(action->text() == tr("Tools")) {
            m_HideMPI = new QAction(tr("Hide MPI Functions"), this);
            m_HideMPI->setToolTip(tr("Collapse all MPI functions in the current stack trace"));
            m_HideMPI->setIcon(QIcon(":/SWAT/app.gif"));
            m_HideMPI->setIconVisibleInMenu(true);
            m_HideMPI->setVisible(false);
            m_HideMPI->setProperty("swatView_menuitem", m_Id.toString());
            connect(m_HideMPI, SIGNAL(triggered()), this, SLOT(doHideMPI()));

            //! \todo We really need to rely on the ActionManager to do this.
            QAction *before = NULL;
            foreach(QAction *item, action->menu()->actions()) {
                if(item->priority() == QAction::LowPriority) {
                    before = item;
                }
            }

            if(before) {
                action->menu()->insertAction(before, m_HideMPI);
                action->menu()->insertSeparator(before)->setProperty("swatView_menuitem", m_Id.toString());
            } else {
                action->menu()->addAction(m_HideMPI);
                action->menu()->addSeparator()->setProperty("swatView_menuitem", m_Id.toString());
            }
        }
    }

 }

DirectedGraphView::~DirectedGraphView()
{
    delete ui;
}

QGraphVizView *DirectedGraphView::view()
{
    return m_View;
}

void DirectedGraphView::undo()
{
    m_UndoStack->undo();
}

void DirectedGraphView::redo()
{
    m_UndoStack->redo();
}



void DirectedGraphView::doCollapse(DirectedGraphNode *node)
{
    m_UndoStack->push(new CollapseNodeCommand(node, true));
}

void DirectedGraphView::doExpand(DirectedGraphNode *node)
{
    m_UndoStack->push(new CollapseNodeCommand(node, false));
}

void DirectedGraphView::doHideMPI()
{
    m_UndoStack->push(new HideMPICommand(this));
}




void DirectedGraphView::on_txtFilter_textChanged(const QString &filter)
{
    QRegExp rx = QRegExp(filter, Qt::CaseInsensitive, QRegExp::RegExp2);
    foreach(QGraphVizNode *gvNode, m_Scene->getNodes()) {
        if(DirectedGraphNode *node = dynamic_cast<DirectedGraphNode *>(gvNode)) {
            if(rx.indexIn(node->label()) >= 0) {
                node->setSelected(true);
            } else {
                node->setSelected(false);
            }
        }
    }
}

void DirectedGraphView::selectionChanged()
{
    if(m_Scene->selectedItems().count() == 1) {
        if(DirectedGraphNode *node = dynamic_cast<DirectedGraphNode *>(m_Scene->selectedItems().at(0))) {
            DirectedGraphNodeDialog *dlg = new DirectedGraphNodeDialog(this);
            dlg->setNode(node);
            dlg->exec();
        }
    }
}

void DirectedGraphView::openSourceFile(QString filename, int lineNumber)
{
    loadSourceFromFile(filename);
    //TODO: Set view line number
}

void DirectedGraphView::loadSourceFromFile(QString filename)
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

    loadSourceFromContent(fileContent, fileInfo.fileName());
}

void DirectedGraphView::loadSourceFromContent(QByteArray content, QString title)
{
    if(QPlainTextEdit *view = getSourceView(content)) {
        if(!title.isEmpty()) {
            view->setWindowTitle(title);
        }

        int index = ui->tabWidget->addTab(view, view->windowTitle());
        ui->tabWidget->setCurrentIndex(index);
    }
}

QPlainTextEdit *DirectedGraphView::getSourceView(const QString &content)
{
    try {

        using namespace Core::PluginManager;
        using namespace Plugins::SourceView;

        PluginManager &pluginManager = PluginManager::instance();

        foreach(QObject *object, pluginManager.allObjects()) {
            ISourceViewFactory *viewFactory = qobject_cast<ISourceViewFactory *>(object);
            if(viewFactory) {
                QPlainTextEdit *view = viewFactory->sourceViewWidget(content);
                view->setParent(this);
                return view;
            }
        }

    } catch(...) { }

    return NULL;
}


DirectedGraphNode *DirectedGraphView::rootNode()
{
    QList<QGraphVizNode *> nodes = m_Scene->getNodes();

    if(!nodes.isEmpty()) {
        DirectedGraphNode *root = qgraphicsitem_cast<DirectedGraphNode *>(nodes.at(0));
        return root;
    }

    return NULL;
}


void DirectedGraphView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(m_Id.toString() == action->property("swatView_menuitem").toString()) {
            action->setVisible(true);
        }
    }

    QWidget::showEvent(event);
}

void DirectedGraphView::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(action->property("swatView_menuitem").isValid() && (m_Id == action->property("swatView_menuitem").toString())) {
            action->setVisible(false);
        }
    }

    QWidget::hideEvent(event);
}




CollapseNodeCommand::CollapseNodeCommand(DirectedGraphNode *node, bool collapse) :
    m_Node(node),
    m_Collapse(collapse)
{
    if(collapse) {
        setText(QApplication::tr("Collapse node"));
    } else {
        setText(QApplication::tr("Expand node"));
    }
}

void CollapseNodeCommand::undo()
{
    m_Node->setCollapsed(!m_Collapse);
}

void CollapseNodeCommand::redo()
{
    m_Node->setCollapsed(m_Collapse);
}




HideMPICommand::HideMPICommand(DirectedGraphView *view, bool hide) :
    m_DirectedGraphView(view),
    m_Hide(hide)
{
    if(hide) {
        setText(QApplication::tr("Hide MPI Functions"));
    } else {
        setText(QApplication::tr("Show MPI Functions"));
    }
}

QStringList HideMPICommand::mpiFunctions()
{
    static const QStringList functionList =
       ( QStringList() << "mpi_file_iwrite_shared" << "mpi_info_set" << "mpio_request_c2f" << "mpi_file_open"
                       << "mpi_init" << "mpio_request_f2c" << "mpi_file_preallocate" << "mpi_init_thread"
                       << "mpio_test" << "mpi_file_read" << "mpi_initialized" << "mpio_wait" << "mpi_file_read_all"
                       << "mpi_int2handle" << "mpi_abort" << "mpi_file_read_all_begin" << "mpi_intercomm_create"
                       << "mpi_address" << "mpi_file_read_all_end" << "mpi_intercomm_merge" << "mpi_allgather"
                       << "mpi_file_read_at" << "mpi_iprobe" << "mpi_allgatherv" << "mpi_file_read_at_all"
                       << "mpi_irecv" << "mpi_allreduce" << "mpi_file_read_at_all_begin" << "mpi_irsend"
                       << "mpi_alltoall" << "mpi_file_read_at_all_end" << "mpi_isend" << "mpi_alltoallv"
                       << "mpi_file_read_ordered" << "mpi_issend" << "mpi_attr_delete" << "mpi_file_read_ordered_begin"
                       << "mpi_keyval_create" << "mpi_attr_get" << "mpi_file_read_ordered_end" << "mpi_keyval_free"
                       << "mpi_attr_put" << "mpi_file_read_shared" << "mpi_null_copy_fn" << "mpi_barrier"
                       << "mpi_file_seek" << "mpi_null_delete_fn" << "mpi_bcast" << "mpi_file_seek_shared"
                       << "mpi_op_create" << "mpi_bsend" << "mpi_file_set_atomicity" << "mpi_op_free" << "mpi_bsend_init"
                       << "mpi_file_set_errhandler" << "mpi_pack" << "mpi_buffer_attach" << "mpi_file_set_info"
                       << "mpi_pack_size" << "mpi_buffer_detach" << "mpi_file_set_size" << "mpi_pcontrol" << "mpi_char"
                       << "mpi_file_set_view" << "mpi_probe" << "mpi_cancel" << "mpi_file_sync" << "mpi_recv"
                       << "mpi_cart_coords" << "mpi_file_write" << "mpi_recv_init" << "mpi_cart_create"
                       << "mpi_file_write_all" << "mpi_reduce" << "mpi_cart_get" << "mpi_file_write_all_begin"
                       << "mpi_reduce_scatter" << "mpi_cart_map" << "mpi_file_write_all_end" << "mpi_request_c2f"
                       << "mpi_cart_rank" << "mpi_file_write_at" << "mpi_request_free" << "mpi_cart_shift"
                       << "mpi_file_write_at_all" << "mpi_rsend" << "mpi_cart_sub" << "mpi_file_write_at_all_begin"
                       << "mpi_rsend_init" << "mpi_cartdim_get" << "mpi_file_write_at_all_end" << "mpi_scan"
                       << "mpi_comm_compare" << "mpi_file_write_ordered" << "mpi_scatter" << "mpi_comm_create"
                       << "mpi_file_write_ordered_begin" << "mpi_scatterv" << "mpi_comm_dup"
                       << "mpi_file_write_ordered_end" << "mpi_send" << "mpi_comm_free" << "mpi_file_write_shared"
                       << "mpi_send_init" << "mpi_comm_get_name" << "mpi_finalize" << "mpi_sendrecv" << "mpi_comm_group"
                       << "mpi_finalized" << "mpi_sendrecv_replace" << "mpi_comm_rank" << "mpi_gather" << "mpi_ssend"
                       << "mpi_comm_remote_group" << "mpi_gatherv" << "mpi_ssend_init" << "mpi_comm_remote_size"
                       << "mpi_get_count" << "mpi_start" << "mpi_comm_set_name" << "mpi_get_elements" << "mpi_startall"
                       << "mpi_comm_size" << "mpi_get_processor_name" << "mpi_status_c2f" << "mpi_comm_split"
                       << "mpi_getVersion" << "mpi_status_set_cancelled" << "mpi_comm_test_inter"
                       << "mpi_graph_create" << "mpi_status_set_elements" << "mpi_dup_fn" << "mpi_graph_get"
                       << "mpi_test" << "mpi_dims_create" << "mpi_graph_map" << "mpi_test_cancelled"
                       << "mpi_errhandler_create" << "mpi_graph_neighbors" << "mpi_testall"
                       << "mpi_errhandler_free" << "mpi_graph_neighbors_count" << "mpi_testany"
                       << "mpi_errhandler_get" << "mpi_graphdims_get" << "mpi_testsome"
                       << "mpi_errhandler_set" << "mpi_group_compare" << "mpi_topo_test" << "mpi_error_class"
                       << "mpi_group_difference" << "mpi_type_commit" << "mpi_error_string"
                       << "mpi_group_excl" << "mpi_type_contiguous" << "mpi_file_c2f" << "mpi_group_free"
                       << "mpi_type_create_darray" << "mpi_file_close" << "mpi_group_incl"
                       << "mpi_type_create_subarray" << "mpi_file_delete" << "mpi_group_intersection"
                       << "mpi_type_extent" << "mpi_file_f2c" << "mpi_group_range_excl" << "mpi_type_free"
                       << "mpi_file_get_amode" << "mpi_group_range_incl" << "mpi_type_get_contents"
                       << "mpi_file_get_atomicity" << "mpi_group_rank" << "mpi_type_get_envelope"
                       << "mpi_file_get_byte_offset" << "mpi_group_size" << "mpi_type_hvector"
                       << "mpi_file_get_errhandler" << "mpi_group_translate_ranks" << "mpi_type_lb"
                       << "mpi_file_get_group" << "mpi_group_union" << "mpi_type_size" << "mpi_file_get_info"
                       << "mpi_ibsend" << "mpi_type_struct" << "mpi_file_get_position" << "mpi_info_c2f"
                       << "mpi_type_ub" << "mpi_file_get_position_shared" << "mpi_info_create"
                       << "mpi_type_vector" << "mpi_file_get_size" << "mpi_info_delete" << "mpi_unpack"
                       << "mpi_file_get_type_extent" << "mpi_info_dup" << "mpi_wait" << "mpi_file_get_view"
                       << "mpi_info_f2c" << "mpi_waitall" << "mpi_file_iread" << "mpi_info_free"
                       << "mpi_waitany" << "mpi_file_iread_at" << "mpi_info_get" << "mpi_waitsome"
                       << "mpi_file_iread_shared" << "mpi_info_get_nkeys" << "mpi_wtick" << "mpi_file_iwrite"
                       << "mpi_info_get_nthkey" << "mpi_wtime" << "mpi_file_iwrite_at"
                       << "mpi_info_get_valuelen" << "mpi_file_iwrite_shared" << "mpi_info_set" );

    return functionList;
}

void HideMPICommand::undo()
{
    while(!m_Nodes.isEmpty()) {
        m_Nodes.takeLast()->setCollapsed(!m_Hide);
    }
}

void HideMPICommand::redo()
{
    // Breadth-first search for all MPI_ functions

    m_Nodes.clear();

    QStringList checked;
    QQueue<DirectedGraphNode *> queue;

    QStringList functions = this->mpiFunctions();

    DirectedGraphNode *root = m_DirectedGraphView->rootNode();
    checked.append(root->getGVName());
    queue.enqueue(root);

    static const QRegExp pmpi = QRegExp("^pmpi", Qt::CaseInsensitive);

    while(!queue.isEmpty()) {
        DirectedGraphNode *node = queue.dequeue();

        // Remove the beginning p for pmpi_ calls
        QString label = node->label().toLower().trimmed();
        if(label.startsWith("pmpi")) {
            label.replace(pmpi, "mpi");
        }

        if(functions.contains(label)) {
            node->setCollapsed(m_Hide);
            m_Nodes.append(node);
        } else {
            foreach(DirectedGraphNode *child, node->childNodes()) {
                if(!checked.contains(child->getGVName())) {
                    checked.append(child->getGVName());
                    queue.enqueue(child);
                }
            }
        }
    }
}




} // namespace DirectedGraphView
} // namespace Plugins
