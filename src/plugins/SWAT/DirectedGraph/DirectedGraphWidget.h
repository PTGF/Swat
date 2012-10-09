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

#ifndef PLUGINS_DIRECTEDGRAPH_DIRECTEDGRAPHWIDGET_H
#define PLUGINS_DIRECTEDGRAPH_DIRECTEDGRAPHWIDGET_H

#include <QtCore>
#include <QtGui>

#include <PrettyWidgets/TabWidget.h>

#ifdef QT_DEBUG
#include <QtDebug>
#endif

class QGraphVizView;

namespace Plugins {
namespace DirectedGraph {

class DirectedGraphWidget;
class DirectedGraphScene;
class DirectedGraphNode;
class DirectedGraphEdge;

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(DirectedGraphWidget *view) : m_View(view), m_Failed(false) { }
    bool failed() { return m_Failed; }

protected:
    DirectedGraphWidget *view() const { return m_View; }
    void setFailed(bool failed) { m_Failed = failed; }

private:
    DirectedGraphWidget *m_View;
    bool m_Failed;
};

class ExpandAllCommand : public UndoCommand
{
public:
    ExpandAllCommand(DirectedGraphWidget *view);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 5; }

protected:
    void findNodes();

private:
    QList<DirectedGraphNode *> m_Nodes;
};

class CollapseNodeCommand : public UndoCommand
{
public:
    CollapseNodeCommand(DirectedGraphWidget *view, DirectedGraphNode *node, bool collapse = true);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 1; }

private:
    DirectedGraphNode *m_Node;
    bool m_Collapse;
};

class CollapseNodeDepthCommand : public UndoCommand
{
public:
    CollapseNodeDepthCommand(DirectedGraphWidget *view, int depth);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 2; }

protected:
    void findNodes();

private:
    int m_Depth;
    QList<DirectedGraphNode *> m_Nodes;
};

class DirectedGraphWidget : public TabWidget
{
    Q_OBJECT

public:
    explicit DirectedGraphWidget(QWidget * parent = 0);
    ~DirectedGraphWidget();

    virtual void setContent(const QByteArray &content);

    virtual QGraphVizView *view();
    virtual DirectedGraphScene *scene();
    DirectedGraphNode *rootNode();

public slots:
    void undo();
    void redo();
    void filter();

    void doExpandAll();
    void doExpand(DirectedGraphNode *node);
    void doCollapse(DirectedGraphNode *node);
    void doCollapseDepth(int depth);

    void doZoomIn();
    void doZoomOut();
    void doZoomFit();
    void doRefresh();

protected:
    QUuid id();
    QUndoStack *undoStack();
    virtual DirectedGraphScene *createScene(const QByteArray &content);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

    QToolBar *editToolBar() { return m_EditToolBar; }
    QToolBar *viewToolBar() { return m_ViewToolBar; }

protected slots:
    void txtFilter_textChanged(const QString &text);

private:
    DirectedGraphScene *m_Scene;
    QGraphVizView *m_View;
    QUndoStack *m_UndoStack;
    QUuid m_Id;

    QList<DirectedGraphNode *> m_Nodes;
    QList<DirectedGraphEdge *> m_Edges;


    QToolBar *m_EditToolBar;
    QToolBar *m_ViewToolBar;
    QAction *m_ExpandAll;
    QLineEdit *m_txtFilter;

    friend class ExpandAllCommand;
    friend class CollapseNodeCommand;
    friend class CollapseNodeDepthCommand;
};

} // namespace DirectedGraph
} // namespace Plugins

#endif // PLUGINS_DIRECTEDGRAPH_DIRECTEDGRAPHWIDGET_H
