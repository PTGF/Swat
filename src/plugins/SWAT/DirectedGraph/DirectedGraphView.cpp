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

#include <MainWindow/MainWindow.h>

#include <QGraphVizView.h>

#include "DirectedGraphScene.h"
#include "DirectedGraphNode.h"

namespace Plugins {
namespace SWAT {

DirectedGraphView::DirectedGraphView(QWidget *parent) :
    TabWidget(parent),
    m_Scene(NULL),
    m_View(NULL),
    m_UndoStack(new QUndoStack(this)),
    m_Id(QUuid::createUuid()),
    m_EditToolBar(NULL),
    m_ViewToolBar(NULL),
    m_ExpandAll(NULL),
    m_txtFilter(NULL)
{
    m_UndoStack->setActive(false);

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
        if(action->text() == tr("Edit")) {
            QAction *undo = m_UndoStack->createUndoAction(this);
            undo->setIcon(QIcon(":/SWAT/undo.svg"));
            undo->setProperty("swatView_menuitem", m_Id.toString());
            undo->setShortcut(QKeySequence::Undo);

            QAction *redo = m_UndoStack->createRedoAction(this);
            redo->setIcon(QIcon(":/SWAT/redo.svg"));
            redo->setProperty("swatView_menuitem", m_Id.toString());
            redo->setShortcut(QKeySequence::Redo);

            QAction *filter = new QAction(QIcon(":/SWAT/filter.svg"), tr("Filter"), this);
            filter->setProperty("swatView_menuitem", m_Id.toString());
            filter->setShortcut(QKeySequence::Find);
            connect(filter, SIGNAL(triggered()), this, SLOT(filter()));

            m_EditToolBar = new QToolBar("Edit", this);
            m_EditToolBar->setObjectName("EditToolBar");
            m_EditToolBar->setIconSize(QSize(16,16));
            m_EditToolBar->addAction(undo);
            m_EditToolBar->addAction(redo);
            m_EditToolBar->addAction(filter);
            mainWindow.addToolBar(Qt::TopToolBarArea, m_EditToolBar);
            m_EditToolBar->hide();

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
                action->menu()->insertAction(before, filter);
                action->menu()->insertSeparator(before)->setProperty("swatView_menuitem", m_Id.toString());
            } else {
                action->menu()->addAction(undo);
                action->menu()->addAction(redo);
                action->menu()->addAction(filter);
                action->menu()->addSeparator()->setProperty("swatView_menuitem", m_Id.toString());
            }
        }

        if(action->text() == tr("Tools")) {
            m_ExpandAll = new QAction(tr("Expand All"), this);
            m_ExpandAll->setToolTip(tr("Expand all functions in the current stack trace"));
            m_ExpandAll->setIcon(QIcon(":/SWAT/app.gif"));
            m_ExpandAll->setIconVisibleInMenu(true);
            m_ExpandAll->setVisible(false);
            m_ExpandAll->setProperty("swatView_menuitem", m_Id.toString());
            connect(m_ExpandAll, SIGNAL(triggered()), this, SLOT(doExpandAll()));

            m_ViewToolBar = new QToolBar("View", this);
            m_ViewToolBar->setObjectName("ViewToolBar");
            m_ViewToolBar->setIconSize(QSize(16,16));
            m_ViewToolBar->addAction(m_ExpandAll);
            mainWindow.addToolBar(Qt::TopToolBarArea, m_ViewToolBar);
            m_ViewToolBar->hide();

            //! \todo We really need to rely on the ActionManager to do this.
            QAction *before = NULL;
            foreach(QAction *item, action->menu()->actions()) {
                if(item->priority() == QAction::LowPriority) {
                    before = item;
                }
            }

            if(before) {
                action->menu()->insertAction(before, m_ExpandAll);
                action->menu()->insertSeparator(before)->setProperty("swatView_menuitem", m_Id.toString());
            } else {
                action->menu()->addAction(m_ExpandAll);
                action->menu()->addSeparator()->setProperty("swatView_menuitem", m_Id.toString());
            }
        }
    }


    m_txtFilter = new QLineEdit(this);
    connect(m_txtFilter, SIGNAL(textChanged(QString)), this, SLOT(txtFilter_textChanged(QString)));
    m_txtFilter->hide();

 }

DirectedGraphView::~DirectedGraphView()
{
}

void DirectedGraphView::setContent(const QByteArray &content)
{
    if(!scene()) {
        createScene(content);

        setWindowTitle(tr("Directed Graph View"));

        insertTab(0, view(), tr("Stack"));
    }

    undoStack()->setActive();
}


QGraphVizView *DirectedGraphView::view()
{
    if(!m_View) {
        m_View = new QGraphVizView(scene(), this);
        scene()->setParent(this);
    }
    return m_View;
}


DirectedGraphScene *DirectedGraphView::createScene(const QByteArray &content)
{
    if(!m_Scene) {
        m_Scene = new DirectedGraphScene();
        m_Scene->setContent(QString(content));
    }
    return m_Scene;
}

DirectedGraphScene *DirectedGraphView::scene()
{
    return m_Scene;
}


DirectedGraphNode *DirectedGraphView::rootNode()
{
    QList<QGraphVizNode *> nodes = scene()->getNodes();

    if(!nodes.isEmpty()) {
        DirectedGraphNode *root = qgraphicsitem_cast<DirectedGraphNode *>(nodes.at(0));
        return root;
    }

    return NULL;
}


QUndoStack *DirectedGraphView::undoStack()
{
    return m_UndoStack;
}

void DirectedGraphView::filter()
{
    if(m_txtFilter->isVisible()) {
        m_txtFilter->hide();
        txtFilter_textChanged(QString());
    } else {
        m_txtFilter->show();
        m_txtFilter->move(width() - 16 - m_txtFilter->width(), 0);
        m_txtFilter->setFocus();
        txtFilter_textChanged(m_txtFilter->text());
    }
}


void DirectedGraphView::undo()
{
    undoStack()->undo();
}

void DirectedGraphView::redo()
{
    undoStack()->redo();
}


void DirectedGraphView::doExpandAll()
{
    undoStack()->push(new ExpandAllCommand(this));
}


void DirectedGraphView::doCollapse(DirectedGraphNode *node)
{
    undoStack()->push(new CollapseNodeCommand(this, node, true));
}

void DirectedGraphView::doExpand(DirectedGraphNode *node)
{
    undoStack()->push(new CollapseNodeCommand(this, node, false));
}

void DirectedGraphView::doCollapseDepth(int depth)
{
    undoStack()->push(new CollapseNodeDepthCommand(this, depth));
}

void DirectedGraphView::txtFilter_textChanged(const QString &text)
{
    QRegExp rx = QRegExp(text, Qt::CaseInsensitive, QRegExp::RegExp2);
    foreach(QGraphVizNode *gvNode, scene()->getNodes()) {
        if(DirectedGraphNode *node = dynamic_cast<DirectedGraphNode *>(gvNode)) {
            if(!text.isEmpty() && rx.indexIn(node->label()) >= 0) {
                node->setHighlighted(true);
            } else {
                node->setHighlighted(false);
            }
        }
    }
}

QUuid DirectedGraphView::id()
{
    return m_Id;
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

    undoStack()->setActive(true);

    m_ViewToolBar->show();
    m_EditToolBar->show();

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

    undoStack()->setActive(false);

    m_ViewToolBar->hide();
    m_EditToolBar->hide();

    QWidget::hideEvent(event);
}

void DirectedGraphView::resizeEvent(QResizeEvent *event)
{
    TabWidget::resizeEvent(event);
    m_txtFilter->move(width() - 16 - m_txtFilter->width(), 0);
}





ExpandAllCommand::ExpandAllCommand(DirectedGraphView *view) :
    UndoCommand(view)
{
    setText(QObject::tr("Expand all nodes"));
}

void ExpandAllCommand::undo()
{
    UndoCommand::undo();

    if(failed()) { return; }

    foreach(DirectedGraphNode *node, m_Nodes) {
        node->setCollapsed(true);
    }
}

void ExpandAllCommand::redo()
{
    UndoCommand::redo();

    if(failed()) { return; }

    if(m_Nodes.isEmpty()) {
        findNodes();
    }

    foreach(DirectedGraphNode *node, m_Nodes) {
        node->setCollapsed(false);
    }

    setFailed(m_Nodes.isEmpty());
}

void ExpandAllCommand::findNodes()
{
    QList<DirectedGraphNode *> visited;
    QQueue<DirectedGraphNode *> queue;

    visited.append(view()->rootNode());
    queue.enqueue(view()->rootNode());

    while(!queue.isEmpty()) {
        DirectedGraphNode *node = queue.dequeue();

        if(node->isCollapsed()) {
            m_Nodes.append(node);
        }

        foreach(DirectedGraphNode *child, node->childNodes()) {
            if(!visited.contains(child)) {
                visited.append(child);
                queue.enqueue(child);
            }
        }
    }
}

bool ExpandAllCommand::mergeWith(const QUndoCommand *other)
{
    if(other->id() != id()) {
        return false;
    }

    const ExpandAllCommand *command = static_cast<const ExpandAllCommand *>(other);
    if(view() != command->view()) {
        return false;
    }

    return true;
}




CollapseNodeCommand::CollapseNodeCommand(DirectedGraphView *view, DirectedGraphNode *node, bool collapse) :
    UndoCommand(view),
    m_Node(node),
    m_Collapse(collapse)
{
    if(collapse) {
        setText(QObject::tr("Collapse node"));
    } else {
        setText(QObject::tr("Expand node"));
    }
}

void CollapseNodeCommand::undo()
{
    UndoCommand::undo();

    if(failed()) { return; }

    m_Node->setCollapsed(!m_Collapse);
}

void CollapseNodeCommand::redo()
{
    UndoCommand::redo();

    if(failed()) { return; }

    if(m_Node->isCollapsed() == m_Collapse) {
        setFailed(true);
        return;
    }

    m_Node->setCollapsed(m_Collapse);
}

bool CollapseNodeCommand::mergeWith(const QUndoCommand *other)
{
    if(other->id() != id()) {
        return false;
    }

    const CollapseNodeCommand *command = static_cast<const CollapseNodeCommand *>(other);
    if(view() != command->view() || m_Node != command->m_Node || m_Collapse != command->m_Collapse) {
        return false;
    }

    return true;
}




CollapseNodeDepthCommand::CollapseNodeDepthCommand(DirectedGraphView *view, int depth) :
    UndoCommand(view),
    m_Depth(depth)
{
    setText(QObject::tr("Collapse to Node Depth"));
}

void CollapseNodeDepthCommand::undo()
{
    UndoCommand::undo();

    if(failed()) { return; }

    foreach(DirectedGraphNode *node, m_Nodes) {
        node->setCollapsed(false);
    }
}

void CollapseNodeDepthCommand::redo()
{
    UndoCommand::redo();

    if(m_Nodes.isEmpty()) {
        findNodes();
    }

    foreach(DirectedGraphNode *node, m_Nodes) {
        node->setCollapsed(true);
    }

    setFailed(m_Nodes.isEmpty());
}

void CollapseNodeDepthCommand::findNodes()
{
    QList<DirectedGraphNode *> checked;
    QQueue<DirectedGraphNode *> queue;

    DirectedGraphNode *root = view()->rootNode();
    checked.append(root);
    queue.enqueue(root);

    while(!queue.isEmpty()) {
        DirectedGraphNode *node = queue.dequeue();

        if(node->nodeDepth() == m_Depth) {
            if(!node->isCollapsed()) {
                m_Nodes.append(node);
            }
        } else {
            foreach(DirectedGraphNode *child, node->childNodes()) {
                if(!checked.contains(child)) {
                    checked.append(child);
                    queue.enqueue(child);
                }
            }
        }
    }
}

bool CollapseNodeDepthCommand::mergeWith(const QUndoCommand *other)
{
    if(other->id() != id()) {
        return false;
    }

    const CollapseNodeDepthCommand *command = static_cast<const CollapseNodeDepthCommand *>(other);
    if(view() != command->view() || m_Depth != command->m_Depth) {
        return false;
    }

    return true;
}






} // namespace SWAT
} // namespace Plugins
