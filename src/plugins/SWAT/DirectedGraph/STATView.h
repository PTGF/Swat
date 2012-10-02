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

#ifndef PLUGINS_SWAT_STATVIEW_H
#define PLUGINS_SWAT_STATVIEW_H

#include <DirectedGraph/DirectedGraphView.h>

namespace Plugins { namespace SourceView { class SourceView; } }

namespace Plugins {
namespace SWAT {

class STATNode;
class STATView;
class STATScene;


class STATUndoCommand : public UndoCommand
{
public:
    STATUndoCommand(STATView *view);
protected:
    STATView *statView() const;
};

class HideMPICommand : public STATUndoCommand
{
public:
    HideMPICommand(STATView *view);
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
    FocusNodeCommand(STATView *view, STATNode *node);
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


class STATView : public DirectedGraphView
{
    Q_OBJECT

public:
    explicit STATView(QWidget * parent = 0);
    ~STATView();

    virtual void setContent(const QByteArray &content);
    virtual DirectedGraphScene *scene();

public slots:
    void doHideMPI();
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

    friend class HideMPICommand;
    friend class FocusNodeCommand;
    friend class STATNodeDialog;

};


} // namespace SWAT
} // namespace Plugins
#endif // PLUGINS_SWAT_STATVIEW_H
