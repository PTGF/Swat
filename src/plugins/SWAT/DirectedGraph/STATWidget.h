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

#ifndef PLUGINS_DIRECTEDGRAPH_STATWIDGET_H
#define PLUGINS_DIRECTEDGRAPH_STATWIDGET_H

#include <DirectedGraph/DirectedGraphWidget.h>

namespace Plugins { namespace SourceView { class SourceView; } }

namespace Plugins {
namespace DirectedGraph {

class STATNode;
class STATWidget;
class STATScene;


class STATUndoCommand : public UndoCommand
{
public:
    STATUndoCommand(STATWidget *view);
protected:
    STATWidget *statWidget() const;
};

class HideMPICommand : public STATUndoCommand
{
public:
    HideMPICommand(STATWidget *view);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 4; }

protected:
    void findNodes();
    QStringList mpiFunctions();

private:
    QList<STATNode *> m_Nodes;
};

class FocusNodeCommand : public STATUndoCommand
{
public:
    FocusNodeCommand(STATWidget *view, STATNode *node);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 3; }

protected:
    void findNodes();

private:
    STATNode *m_Node;
    QList<STATNode *> m_Nodes;
};

class HideNonBranchingCommand : public STATUndoCommand
{
public:
    HideNonBranchingCommand(STATWidget *view);
    bool mergeWith(const QUndoCommand *other);
    void undo();
    void redo();

    int id() const { return 5; }

protected:
    bool findNodes(STATNode *parent = 0);

private:
    QList<STATNode *> m_Nodes;
};

class STATWidget : public DirectedGraphWidget
{
    Q_OBJECT

public:
    explicit STATWidget(QWidget * parent = 0);
    ~STATWidget();

    virtual void setContent(const QByteArray &content);
    virtual DirectedGraphScene *scene();

public slots:
    void doHideMPI();
    void doHideNonBranching();
    void doFocus(STATNode *node);

protected:
    virtual DirectedGraphScene *createScene(const QByteArray &content);

    void openSourceFile(const QString &filename, const int &lineNumber = 0);
    void loadSourceFromFile(const QString &filename, const int &lineNumber = 0);
    void loadSourceFromContent(const QByteArray &content, const QString &title);
    SourceView::SourceView *getSourceView(const QString &content);

protected slots:
    void selectionChanged();

private:
    STATScene *m_STATScene;
    QAction *m_HideMPI;
    QAction *m_HideNonBranching;
    QToolBar *m_EditToolBar;

    friend class HideMPICommand;
    friend class HideNonBranchingCommand;
    friend class FocusNodeCommand;
    friend class STATNodeDialog;

};


} // namespace DirectedGraph
} // namespace Plugins
#endif // PLUGINS_DIRECTEDGRAPH_STATWidget_H
