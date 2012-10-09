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

#include "STATWidget.h"

#include <MainWindow/MainWindow.h>
#include <SettingManager/SettingManager.h>
#include <PluginManager/PluginManager.h>
#include <SourceView/ISourceViewFactory.h>
#include <SourceView/SourceView.h>

#include "STATScene.h"
#include "STATNode.h"
#include "STATNodeDialog.h"


namespace Plugins {
namespace DirectedGraph {

STATWidget::STATWidget(QWidget *parent) :
    DirectedGraphWidget(parent),
    m_STATScene(NULL),
    m_HideMPI(NULL),
    m_HideNonBranching(NULL),
    m_EditToolBar(NULL)
{
    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
        if(action->text() == tr("Tools")) {
            m_HideMPI = new QAction(tr("Hide MPI Functions"), this);
            m_HideMPI->setToolTip(tr("Collapse all MPI functions in the current stack trace"));
            m_HideMPI->setIcon(QIcon(":/SWAT/app.gif"));
            m_HideMPI->setIconVisibleInMenu(true);
            m_HideMPI->setVisible(false);
            m_HideMPI->setProperty("swatWidget_menuitem", id().toString());
            connect(m_HideMPI, SIGNAL(triggered()), this, SLOT(doHideMPI()));

            m_HideNonBranching = new QAction(tr("Hide Non-Branching Functions"), this);
            m_HideNonBranching->setToolTip(tr("Collapse all Non-Branching functions in the current stack trace"));
            m_HideNonBranching->setIcon(QIcon(":/SWAT/app.gif"));
            m_HideNonBranching->setIconVisibleInMenu(true);
            m_HideNonBranching->setVisible(false);
            m_HideNonBranching->setProperty("swatWidget_menuitem", id().toString());
            connect(m_HideNonBranching, SIGNAL(triggered()), this, SLOT(doHideNonBranching()));

            viewToolBar()->addAction(m_HideMPI);
            viewToolBar()->addAction(m_HideNonBranching);

            //! \todo We really need to rely on the ActionManager to do this.
            QAction *before = NULL;
            foreach(QAction *item, action->menu()->actions()) {
                if(item->priority() == QAction::LowPriority) {
                    before = item;
                }
            }

            if(before) {
                action->menu()->insertAction(before, m_HideNonBranching);
                action->menu()->insertAction(before, m_HideMPI);
            } else {
                action->menu()->addAction(m_HideNonBranching);
                action->menu()->addAction(m_HideMPI);
            }
        }
    }

}

STATWidget::~STATWidget()
{
}

void STATWidget::setContent(const QByteArray &content)
{
    DirectedGraphWidget::setContent(content);

    connect(scene(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    // Get settings from SettingManager and perform default functions on loaded scene
    Core::SettingManager::SettingManager &settingManager = Core::SettingManager::SettingManager::instance();
    settingManager.beginGroup("Plugins/SWAT");
    if(settingManager.value("viewDefaults/hideMPI", true).toBool()) {
        doHideMPI();
    }
    if(settingManager.value("viewDefaults/hideNonBranching", true).toBool()) {
        doHideNonBranching();
    }
    settingManager.endGroup();

    undoStack()->clear();  // Clear the undo stack so that the user is forced to use "expand all" to get to default
}


void STATWidget::doHideMPI()
{
    undoStack()->push(new HideMPICommand(this));
}

void STATWidget::doHideNonBranching()
{
    undoStack()->push(new HideNonBranchingCommand(this));
}

void STATWidget::doFocus(STATNode *node)
{
    undoStack()->push(new FocusNodeCommand(this, node));
}

void STATWidget::selectionChanged()
{
    if(scene()->selectedItems().count() == 1) {
        if(STATNode *node = qgraphicsitem_cast<STATNode *>(scene()->selectedItems().at(0))) {
            STATNodeDialog *dlg = new STATNodeDialog(this);
            dlg->setNode(node);
            dlg->exec();
        }
    }
}

DirectedGraphScene *STATWidget::createScene(const QByteArray &content)
{
    if(!m_STATScene) {
        m_STATScene = new STATScene();
        m_STATScene->setContent(QString(content));
    }
    return m_STATScene;
}

DirectedGraphScene *STATWidget::scene()
{
    return m_STATScene;
}

void STATWidget::openSourceFile(const QString &filename, const int &lineNumber)
{
    loadSourceFromFile(filename, lineNumber);
}

void STATWidget::loadSourceFromFile(const QString &filename, const int &lineNumber)
{
    QFileInfo fileInfo(filename);

    //Ensure that the file isn't already open
    for(int i=0; i < count(); ++i) {
        if(widget(i)->windowFilePath() == fileInfo.absoluteFilePath() || widget(i)->windowFilePath() == fileInfo.fileName()) {
            if(Plugins::SourceView::SourceView *view = qobject_cast<Plugins::SourceView::SourceView *>(widget(i))) {
                view->setCurrentLineNumber(lineNumber);
            }

            setCurrentIndex(i);
            return;
        }
    }

    if(!fileInfo.exists()) {
        //TODO: Allow user to try to find it
        throw tr("File does not exist: '%1'").arg(fileInfo.absoluteFilePath());
    }

    QFile file(fileInfo.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly)) {
        throw tr("Failed to open file: '%1'").arg(fileInfo.absoluteFilePath());
    }

    QByteArray fileContent = file.readAll();

    file.close();

    if(SourceView::SourceView *view = getSourceView(fileContent)) {
        view->setWindowFilePath(fileInfo.absoluteFilePath());
        view->setWindowTitle(fileInfo.fileName());
        view->setCurrentLineNumber(lineNumber);
        setCurrentIndex(addTab(view, view->windowTitle()));
    }
}

void STATWidget::loadSourceFromContent(const QByteArray &content, const QString &title)
{
    if(Plugins::SourceView::SourceView *view = getSourceView(content)) {
        view->setWindowFilePath(title);

        if(!title.isEmpty()) {
            view->setWindowTitle(title);
        }

        setCurrentIndex(addTab(view, view->windowTitle()));
    }
}

SourceView::SourceView *STATWidget::getSourceView(const QString &content)
{
    try {

        using namespace Core::PluginManager;
        using namespace Plugins::SourceView;

        PluginManager &pluginManager = PluginManager::instance();

        foreach(QObject *object, pluginManager.allObjects()) {
            ISourceViewFactory *viewFactory = qobject_cast<ISourceViewFactory *>(object);
            if(viewFactory) {
                Plugins::SourceView::SourceView *view = viewFactory->sourceViewWidget(content);
                view->setParent(this);
                return view;
            }
        }

    } catch(...) { }

    return NULL;
}




STATUndoCommand::STATUndoCommand(STATWidget *view) :
    UndoCommand(view)
{
}

STATWidget *STATUndoCommand::statWidget() const
{
    return qobject_cast<STATWidget *>(view());
}




FocusNodeCommand::FocusNodeCommand(STATWidget *view, STATNode *node) :
    STATUndoCommand(view),
    m_Node(node)
{
    setText(QObject::tr("Focus on Node"));
}

void FocusNodeCommand::undo()
{
    UndoCommand::undo();

    if(failed()) { return; }

    foreach(STATNode *node, m_Nodes) {
        node->setCollapsed(false);
    }
}

void FocusNodeCommand::redo()
{
    UndoCommand::redo();

    if(m_Nodes.isEmpty()) {
        findNodes();
    }

    foreach(STATNode *node, m_Nodes) {
        node->setCollapsed(true);
    }

    setFailed(m_Nodes.isEmpty());
}

void FocusNodeCommand::findNodes()
{
    // Breadth-first search; probably not optimal

    STATNode *root = qgraphicsitem_cast<STATNode *>(statWidget()->rootNode());

    QList<STATNode *> related;
    STATNode *parent = m_Node;
    while(parent) {
        related.append(parent);
        parent = qgraphicsitem_cast<STATNode *>(parent->parentNode());
    }

    QList<STATNode *> checked;
    QQueue<STATNode *> queue;
    checked.append(root);
    queue.enqueue(root);

    while(!queue.isEmpty()) {
        STATNode *node = queue.dequeue();

        if(!related.contains(node)) {
            if(!node->isCollapsed()) {
                m_Nodes.append(node);
            }
        } else {
            foreach(DirectedGraphNode *child, node->childNodes()) {
                STATNode *statChild = qgraphicsitem_cast<STATNode *>(child);
                if(!checked.contains(statChild) && statChild != m_Node) {
                    checked.append(statChild);
                    queue.enqueue(statChild);
                }
            }
        }
    }
}

bool FocusNodeCommand::mergeWith(const QUndoCommand *other)
{
    if(other->id() != id()) {
        return false;
    }

    const FocusNodeCommand *command = static_cast<const FocusNodeCommand *>(other);
    if(view() != command->view() || m_Node != command->m_Node) {
        return false;
    }

    return true;
}




HideMPICommand::HideMPICommand(STATWidget *view) :
    STATUndoCommand(view)
{
    setText(QObject::tr("Hide MPI Functions"));
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
    UndoCommand::undo();

    if(failed()) { return; }

    foreach(STATNode *node, m_Nodes) {
        node->setCollapsed(false);
    }
}

void HideMPICommand::redo()
{
    UndoCommand::redo();

    if(m_Nodes.isEmpty()) {
        findNodes();
    }

    foreach(STATNode *node, m_Nodes) {
        node->setCollapsed(true);
    }

    setFailed(m_Nodes.isEmpty());
}

void HideMPICommand::findNodes(STATNode *parent)
{
    if(!parent) {
        if(!(parent = qgraphicsitem_cast<STATNode *>(view()->rootNode()))) {
            return;
        }
    }

    if(parent->isCollapsed()) {
        return;
    }

    static const QRegExp pmpi = QRegExp("^pmpi", Qt::CaseInsensitive);
    QString label = parent->label().toLower().trimmed();
    if(label.startsWith("pmpi")) {
        label.replace(pmpi, "mpi");
    }

    if(mpiFunctions().contains(label)) {
        m_Nodes.append(parent);
        return;
    } else {
        foreach(DirectedGraphNode *child, parent->childNodes()) {
            if(STATNode *statChild = qgraphicsitem_cast<STATNode *>(child)) {
                findNodes(statChild);
            }
        }
    }
}

bool HideMPICommand::mergeWith(const QUndoCommand *other)
{
    if(other->id() != id()) {
        return false;
    }

    const HideMPICommand *command = static_cast<const HideMPICommand *>(other);
    if(view() != command->view()) {
        return false;
    }

    return true;
}





HideNonBranchingCommand::HideNonBranchingCommand(STATWidget *view) :
    STATUndoCommand(view)
{
    setText(QObject::tr("Hide Non-Branching Functions"));
}

void HideNonBranchingCommand::undo()
{
    UndoCommand::undo();

    if(failed()) { return; }

    foreach(STATNode *node, m_Nodes) {
        node->setCollapsed(false);
    }
}

void HideNonBranchingCommand::redo()
{
    UndoCommand::redo();

    if(m_Nodes.isEmpty()) {
        findNodes();
    }

    foreach(STATNode *node, m_Nodes) {
        node->setCollapsed(true);
    }

    setFailed(m_Nodes.isEmpty());
}

bool HideNonBranchingCommand::findNodes(STATNode *parent)
{
    if(!parent) {
        parent = qgraphicsitem_cast<STATNode *>(view()->rootNode());
    }

    bool parentBranches = (parent->childNodes().count() > 1);

    foreach(DirectedGraphNode *child, parent->childNodes()) {
        if(STATNode *statChild = qgraphicsitem_cast<STATNode *>(child)) {

            bool childBranches = findNodes(statChild);

            if(!childBranches && parentBranches) {
                if(!statChild->isCollapsed()) {
                    m_Nodes.append(statChild);
                }
            }

            parentBranches = (childBranches || parentBranches);
        }
    }

    return parentBranches;
}

bool HideNonBranchingCommand::mergeWith(const QUndoCommand *other)
{
    if(other->id() != id()) {
        return false;
    }

    const HideNonBranchingCommand *command = static_cast<const HideNonBranchingCommand *>(other);
    if(view() != command->view()) {
        return false;
    }

    return true;
}


} // namespace DirectedGraph
} // namespace Plugins
