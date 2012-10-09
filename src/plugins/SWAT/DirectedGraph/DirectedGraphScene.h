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

#ifndef PLUGINS_DIRECTEDGRAPH_DIRECTEDGRAPHSCENE_H
#define PLUGINS_DIRECTEDGRAPH_DIRECTEDGRAPHSCENE_H

#include <QtCore>
#include <QtGui>

#include <QGraphVizScene.h>

namespace Plugins {
namespace DirectedGraph {

class DirectedGraphScene : public QGraphVizScene
{
    Q_OBJECT
public:
    explicit DirectedGraphScene(QObject *parent = 0);

    void setContent(QString content);

    QVariant nodeInfo(const qint64 &id, const int &type, const QVariant &defaultValue = QVariant()) const;
    QVariant edgeInfo(const qint64 &id, const int &type, const QVariant &defaultValue = QVariant()) const;

protected:
    enum NodeInfoTypes {
        NodeInfoType_LongLabel = 0,
        NodeInfoType_ShortLabel = 1
    };

    enum EdgeInfoTypes {
        EdgeInfoType_LongLabel = 0,
        EdgeInfoType_ShortLabel = 1
    };

    virtual QString preprocessContent(const QString &content);
    virtual void processNodeLabel(quint64 id, QString label);
    virtual void processEdgeLabel(quint64 id, QString label);

    QGraphVizNode *createNode(node_t *node);
    QGraphVizEdge *createEdge(edge_t *edge);

    void setNodeInfo(const qint64 &id, const int &type, const QVariant &value);
    void setEdgeInfo(const qint64 &id, const int &type, const QVariant &value);

private:
    typedef QHash<int, QVariant> info;
    QHash<qint64, info> m_NodeInfos;
    QHash<qint64, info> m_EdgeInfos;

    friend class DirectedGraphNode;
    friend class DirectedGraphEdge;
    friend class DirectedGraphWidget;
};

} // namespace DirectedGraph
} // namespace Plugins

#endif // PLUGINS_DIRECTEDGRAPH_DIRECTEDGRAPHSCENE_H
