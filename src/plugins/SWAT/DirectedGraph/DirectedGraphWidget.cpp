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

#include "DirectedGraphWidget.h"

#include <MainWindow/MainWindow.h>

#include <QGraphVizView.h>

#include "DirectedGraphScene.h"
#include "DirectedGraphNode.h"

namespace Plugins {
namespace DirectedGraph {

DirectedGraphWidget::DirectedGraphWidget(QWidget *parent) :
    TabWidget(parent),
    m_Scene(NULL),
    m_View(NULL),
    m_UndoStack(new QUndoStack(this)),
    m_Id(QUuid::createUuid()),
    m_EditToolBar(NULL),
    m_ViewToolBar(NULL),
    m_ExpandAll(NULL),
    m_txtFilter(NULL),
    m_FilterTimer(new QTimer(this))
{
    m_UndoStack->setActive(false);


    // Set up the timer for filtering on a delay so that we're not searching too frequently
    m_FilterTimer->setInterval(350);     // Value found by trial and error; might be a better way to do this
    m_FilterTimer->setSingleShot(true);
    connect(m_FilterTimer, SIGNAL(timeout()), this, SLOT(doFilter()));


    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.menuBar()->actions()) {
        if(action->text() == tr("Edit")) {
            QAction *undo = m_UndoStack->createUndoAction(this);
            undo->setIcon(QIcon(":/SWAT/undo.svg"));
            undo->setProperty("swatWidget_menuitem", m_Id.toString());
            undo->setShortcut(QKeySequence::Undo);

            QAction *redo = m_UndoStack->createRedoAction(this);
            redo->setIcon(QIcon(":/SWAT/redo.svg"));
            redo->setProperty("swatWidget_menuitem", m_Id.toString());
            redo->setShortcut(QKeySequence::Redo);

            QAction *filter = new QAction(QIcon(":/SWAT/filter.svg"), tr("Filter"), this);
            filter->setProperty("swatWidget_menuitem", m_Id.toString());
            filter->setShortcut(QKeySequence::Find);
            connect(filter, SIGNAL(triggered()), this, SLOT(filter()));

            m_EditToolBar = new QToolBar(tr("Edit"), this);
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
                action->menu()->insertSeparator(before)->setProperty("swatWidget_menuitem", m_Id.toString());
            } else {
                action->menu()->addAction(undo);
                action->menu()->addAction(redo);
                action->menu()->addAction(filter);
                action->menu()->addSeparator()->setProperty("swatWidget_menuitem", m_Id.toString());
            }
        } else if(action->text() == tr("Tools")) {
            QAction *zoomIn = new QAction(QIcon(":/SWAT/zoom-in.svg"), tr("Zoom In"), this);
            zoomIn->setProperty("swatWidget_menuitem", m_Id.toString());
            zoomIn->setShortcut(QKeySequence::ZoomIn);
            connect(zoomIn, SIGNAL(triggered()), this, SLOT(doZoomIn()));

            QAction *zoomOut = new QAction(QIcon(":/SWAT/zoom-out.svg"), tr("Zoom Out"), this);
            zoomOut->setProperty("swatWidget_menuitem", m_Id.toString());
            zoomOut->setShortcut(QKeySequence::ZoomOut);
            connect(zoomOut, SIGNAL(triggered()), this, SLOT(doZoomOut()));

            QAction *zoomFit = new QAction(QIcon(":/SWAT/zoom-fit.svg"), tr("Zoom Fit"), this);
            zoomFit->setProperty("swatWidget_menuitem", m_Id.toString());
            zoomFit->setShortcut(QKeySequence("ctrl+0"));
            connect(zoomFit, SIGNAL(triggered()), this, SLOT(doZoomFit()));

            QAction *refresh = new QAction(QIcon(":/SWAT/refresh.svg"), tr("Refresh"), this);
            refresh->setProperty("swatWidget_menuitem", m_Id.toString());
            refresh->setShortcut(QKeySequence::Refresh);
            connect(refresh, SIGNAL(triggered()), this, SLOT(doRefresh()));


            m_ExpandAll = new QAction(tr("Expand All"), this);
            m_ExpandAll->setToolTip(tr("Expand all functions in the current stack trace"));
            m_ExpandAll->setIcon(QIcon(":/SWAT/app.gif"));
            m_ExpandAll->setIconVisibleInMenu(true);
            m_ExpandAll->setVisible(false);
            m_ExpandAll->setProperty("swatWidget_menuitem", m_Id.toString());
            connect(m_ExpandAll, SIGNAL(triggered()), this, SLOT(doExpandAll()));

            m_ViewToolBar = new QToolBar("View", this);
            m_ViewToolBar->setObjectName("ViewToolBar");
            m_ViewToolBar->setIconSize(QSize(16,16));
            m_ViewToolBar->addAction(zoomIn);
            m_ViewToolBar->addAction(zoomOut);
            m_ViewToolBar->addAction(zoomFit);
            m_ViewToolBar->addAction(refresh);
            m_ViewToolBar->addSeparator();
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
                action->menu()->insertAction(before, zoomIn);
                action->menu()->insertAction(before, zoomOut);
                action->menu()->insertAction(before, zoomFit);
                action->menu()->insertAction(before, refresh);
                action->menu()->insertSeparator(before)->setProperty("swatWidget_menuitem", m_Id.toString());
                action->menu()->insertAction(before, m_ExpandAll);
            } else {
                action->menu()->addSeparator()->setProperty("swatWidget_menuitem", m_Id.toString());
                action->menu()->addAction(zoomIn);
                action->menu()->addAction(zoomOut);
                action->menu()->addAction(zoomFit);
                action->menu()->addAction(refresh);
                action->menu()->addSeparator()->setProperty("swatWidget_menuitem", m_Id.toString());
                action->menu()->addAction(m_ExpandAll);
            }
        }
    }


    m_txtFilter = new QLineEdit(this);
    connect(m_txtFilter, SIGNAL(textChanged(QString)), this, SLOT(txtFilter_textChanged(QString)));
    m_txtFilter->hide();

 }

DirectedGraphWidget::~DirectedGraphWidget()
{
}

void DirectedGraphWidget::setContent(const QByteArray &content)
{
    if(!scene()) {
        createScene(content);

        setWindowTitle(tr("Directed Graph View"));

        insertTab(0, view(), tr("Stack"));
    }

    undoStack()->setActive();
}


QGraphVizView *DirectedGraphWidget::view()
{
    if(!m_View) {
        QGraphicsScene *scene = this->scene();
        m_View = new QGraphVizView(scene, this);
        m_View->setHandlesKeyboardEvents(false);
        scene->setParent(this);
    }
    return m_View;
}


DirectedGraphScene *DirectedGraphWidget::createScene(const QByteArray &content)
{
    if(!m_Scene) {
        m_Scene = new DirectedGraphScene();
        m_Scene->setContent(QString(content));
    }
    return m_Scene;
}

DirectedGraphScene *DirectedGraphWidget::scene() const
{
    return m_Scene;
}


DirectedGraphNode *DirectedGraphWidget::rootNode() const
{
    QList<QGraphVizNode *> nodes = scene()->getNodes();

    if(!nodes.isEmpty()) {
        DirectedGraphNode *root = qgraphicsitem_cast<DirectedGraphNode *>(nodes.at(0));
        return root;
    }

    return NULL;
}


QUndoStack *DirectedGraphWidget::undoStack() const
{
    return m_UndoStack;
}

void DirectedGraphWidget::filter()
{
    if(m_txtFilter->isVisible()) {
        m_txtFilter->hide();

        // Immediately clear the selection
        m_FilterText = QString();
        doFilter();
    } else {
        m_txtFilter->show();
        m_txtFilter->move(width() - 16 - m_txtFilter->width(), 0);
        m_txtFilter->setFocus();

        // Immediately reselect whatever is already in the textbox
        m_FilterText = m_txtFilter->text();
        doFilter();
    }
}


void DirectedGraphWidget::undo()
{
    undoStack()->undo();
}

void DirectedGraphWidget::redo()
{
    undoStack()->redo();
}


void DirectedGraphWidget::doExpandAll()
{
    undoStack()->push(new ExpandAllCommand(this));
}


void DirectedGraphWidget::doCollapse(DirectedGraphNode *node)
{
    undoStack()->push(new CollapseNodeCommand(this, node, true));
}

void DirectedGraphWidget::doExpand(DirectedGraphNode *node)
{
    undoStack()->push(new CollapseNodeCommand(this, node, false));
}

void DirectedGraphWidget::doCollapseDepth(const int &depth)
{
    undoStack()->push(new CollapseNodeDepthCommand(this, depth));
}



void DirectedGraphWidget::doZoomIn()
{
    if(m_View) {
        m_View->zoomIn();
    }
}

void DirectedGraphWidget::doZoomOut()
{
    if(m_View) {
        m_View->zoomOut();
    }
}

void DirectedGraphWidget::doZoomFit()
{
    if(m_View) {
        m_View->zoomFit();
    }
}

void DirectedGraphWidget::doRefresh()
{
    if(m_View) {
        m_View->update();
    }
}



void DirectedGraphWidget::txtFilter_textChanged(const QString &text)
{
    m_FilterText = text;

    // Restart the timer for each change
    m_FilterTimer->stop();
    m_FilterTimer->start();
}

void DirectedGraphWidget::doFilter()
{
    m_FilterTimer->stop();

    foreach(QGraphVizNode *gvNode, scene()->getNodes()) {
        if(DirectedGraphNode *node = dynamic_cast<DirectedGraphNode *>(gvNode)) {
            node->setHighlighted(false);
        }
    }

    if(m_FilterText.isEmpty()) {
        return;
    }

    QRegExp rx = QRegExp(m_FilterText, Qt::CaseInsensitive, QRegExp::RegExp2);
    foreach(QGraphVizNode *gvNode, scene()->getNodes()) {
        if(DirectedGraphNode *node = dynamic_cast<DirectedGraphNode *>(gvNode)) {
            if(rx.indexIn(node->label()) >= 0) {
                node->setHighlighted(true);
            }
        }
    }
}

const QUuid &DirectedGraphWidget::id() const
{
    return m_Id;
}

void DirectedGraphWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(m_Id.toString() == action->property("swatWidget_menuitem").toString()) {
            action->setVisible(true);
        }
    }

    undoStack()->setActive(true);

    m_ViewToolBar->show();
    m_EditToolBar->show();

    QWidget::showEvent(event);
}

void DirectedGraphWidget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    using namespace Core::MainWindow;
    MainWindow &mainWindow = MainWindow::instance();
    foreach(QAction *action, mainWindow.allActions()) {
        if(action->property("swatWidget_menuitem").isValid() && (m_Id == action->property("swatWidget_menuitem").toString())) {
            action->setVisible(false);
        }
    }

    undoStack()->setActive(false);

    m_ViewToolBar->hide();
    m_EditToolBar->hide();

    QWidget::hideEvent(event);
}

void DirectedGraphWidget::resizeEvent(QResizeEvent *event)
{
    TabWidget::resizeEvent(event);
    m_txtFilter->move(width() - 16 - m_txtFilter->width(), 0);
}





ExpandAllCommand::ExpandAllCommand(DirectedGraphWidget *view) :
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




CollapseNodeCommand::CollapseNodeCommand(DirectedGraphWidget *view, DirectedGraphNode *node, bool collapse) :
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




CollapseNodeDepthCommand::CollapseNodeDepthCommand(DirectedGraphWidget *view, const int &depth) :
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






} // namespace DirectedGraph
} // namespace Plugins
