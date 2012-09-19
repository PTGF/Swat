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

#ifndef DIRECTEDGRAPHVIEW_H
#define DIRECTEDGRAPHVIEW_H

#include <QtCore>
#include <QtGui>

#ifdef QT_DEBUG
#include <QtDebug>
#endif

class QGraphVizView;

namespace Plugins {
namespace SWAT {

class DirectedGraphView;
class DirectedGraphScene;
class DirectedGraphNode;
class DirectedGraphEdge;

namespace Ui {
class DirectedGraphView;
}

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(DirectedGraphView *view) : m_View(view), m_Failed(false) { }
    bool failed() { return m_Failed; }

protected:
    DirectedGraphView *view() const { return m_View; }
    void setFailed(bool failed) { m_Failed = failed; }

private:
    DirectedGraphView *m_View;
    bool m_Failed;
};

class ExpandAllCommand : public UndoCommand
{
public:
    ExpandAllCommand(DirectedGraphView *view);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 5; }

protected:
    void findNodes();

private:
    QList<DirectedGraphNode *> m_Nodes;
};

class HideMPICommand : public UndoCommand
{
public:
    HideMPICommand(DirectedGraphView *view);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 4; }

protected:
    void findNodes();
    QStringList mpiFunctions();

private:
    QList<DirectedGraphNode *> m_Nodes;
};

class CollapseNodeCommand : public UndoCommand
{
public:
    CollapseNodeCommand(DirectedGraphView *view, DirectedGraphNode *node, bool collapse = true);
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
    CollapseNodeDepthCommand(DirectedGraphView *view, int depth);
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

class FocusNodeCommand : public UndoCommand
{
public:
    FocusNodeCommand(DirectedGraphView *view, DirectedGraphNode *node);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 3; }

protected:
    void findNodes();

private:
    DirectedGraphNode *m_Node;
    QList<DirectedGraphNode *> m_Nodes;
};

class DirectedGraphView : public QWidget
{
    Q_OBJECT

public:
    explicit DirectedGraphView(const QByteArray &content, QWidget * parent = 0);
    ~DirectedGraphView();

    QGraphVizView *view();

public slots:
    void undo();
    void redo();

    void doExpandAll();
    void doHideMPI();
    void doExpand(DirectedGraphNode *node);
    void doCollapse(DirectedGraphNode *node);
    void doCollapseDepth(int depth);
    void doFocus(DirectedGraphNode *node);

protected:
    void openSourceFile(QString filename, int lineNumber = 0);
    void loadSourceFromFile(QString filename);
    void loadSourceFromContent(QByteArray content, QString title);
    QPlainTextEdit *getSourceView(const QString &content);

    DirectedGraphNode *rootNode();

    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

protected slots:
    void on_txtFilter_textChanged(const QString &);
    void selectionChanged();

private:
    Ui::DirectedGraphView *ui;
    DirectedGraphScene *m_Scene;
    QGraphVizView *m_View;
    QUndoStack *m_UndoStack;

    QList<DirectedGraphNode *> m_Nodes;
    QList<DirectedGraphEdge *> m_Edges;

    QUuid m_Id;

    QAction *m_ExpandAll;
    QAction *m_HideMPI;

    friend class DirectedGraphNodeDialog;

    friend class ExpandAllCommand;
    friend class HideMPICommand;
    friend class CollapseNodeCommand;
    friend class CollapseNodeDepthCommand;
    friend class FocusNodeCommand;

};

} // namespace SWAT
} // namespace Plugins

#endif // DIRECTEDGRAPHVIEW_H
