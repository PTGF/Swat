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

#ifndef PLUGINS_DIRECTEDGRAPH_STATNODE_H
#define PLUGINS_DIRECTEDGRAPH_STATNODE_H

#include <DirectedGraph/DirectedGraphNode.h>
#include "STATScene.h"

namespace Plugins {
namespace DirectedGraph {

class STATNode : public DirectedGraphNode
{
public:
    explicit STATNode(node_t *node, STATScene *scene, QGraphicsItem *parentNode = 0);

    QString functionName();
    quint64 programCounter();
    QString sourceFile();
    quint32 sourceLine();
    QString iter();

    QString processCount();
    QStringList processList();

    QStringList leafTasks();
    QString leafTaskCount();

    virtual void showToolTip(const QPoint &pos, QWidget *w, const QRect &rect);

protected:
    QList<quint64> splitProcessList(QStringList lists);
    QList<quint64> splitProcessList(QString list);
    QStringList mergeProcessList(QList<quint64> list);

private:
    STATScene *m_Scene;

    QStringList m_LeafTasks;
    QString m_LeafTaskCount;

    friend class STATScene;
    friend class STATWidget;
};

} // namespace DirectedGraph
} // namespace Plugins

#endif // PLUGINS_DIRECTEDGRAPH_STATNODE_H
