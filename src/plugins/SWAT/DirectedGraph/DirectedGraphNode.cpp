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

#include "DirectedGraphNode.h"

#include <QGraphVizEdge.h>


namespace Plugins {
namespace SWAT {

DirectedGraphNode::DirectedGraphNode(node_t *node, DirectedGraphScene *scene, QGraphicsItem *parent) :
    QGraphVizNode(node, scene, parent),
    m_Scene(scene),
    m_Depth(-1)
{
    m_NodeId = getGVName().toLongLong();
    m_NodeInfo = m_Scene->m_NodeInfos.value(nodeId(), DirectedGraphScene::NodeInfo());
    m_EdgeInfo = m_Scene->m_EdgeInfos.value(nodeId(), DirectedGraphScene::EdgeInfo());
}

qint64 DirectedGraphNode::nodeId()
{
    return m_NodeId;
}

int DirectedGraphNode::nodeDepth()
{
    if(m_Depth >= 0) {
        return m_Depth;
    }

    m_Depth = 0;
    if(headEdges().count() == 1) {
        if(DirectedGraphNode *parent = dynamic_cast<DirectedGraphNode*>(headEdges().at(0)->tail())) {
            m_Depth += parent->nodeDepth() + 1;
        }
    }
    return m_Depth;
}


QString DirectedGraphNode::label()
{
    return nodeInfo().longLabel;
}
QString DirectedGraphNode::shortLabel()
{
    return nodeInfo().shortLabel;
}
QString DirectedGraphNode::functionName()
{
    return nodeInfo().functionName;
}
quint64 DirectedGraphNode::programCounter()
{
    return nodeInfo().programCounter;
}
QString DirectedGraphNode::sourceFile()
{
    return nodeInfo().sourceFile;
}
quint32 DirectedGraphNode::sourceLine()
{
    return nodeInfo().sourceLine;
}
QString DirectedGraphNode::iter()
{
    return nodeInfo().iter_string;
}

QString DirectedGraphNode::edgeLabel()
{
    return edgeInfo().longLabel;
}
QString DirectedGraphNode::shortEdgeLabel()
{
    return edgeInfo().shortLabel;
}
QString DirectedGraphNode::processCount()
{
    return edgeInfo().processCount;
}
QStringList DirectedGraphNode::processList()
{
    return edgeInfo().processList;
}


DirectedGraphScene::NodeInfo DirectedGraphNode::nodeInfo()
{
    return m_NodeInfo;
}

DirectedGraphScene::EdgeInfo DirectedGraphNode::edgeInfo()
{
    return m_EdgeInfo;
}


DirectedGraphNode *DirectedGraphNode::parentNode()
{
    QList<QGraphVizEdge *> edges = headEdges();
    if(edges.count() == 1) {
        return qgraphicsitem_cast<DirectedGraphNode *>(edges.at(0)->tail());
    }
    return NULL;
}

QList<DirectedGraphNode *> DirectedGraphNode::childNodes()
{
    QList<DirectedGraphNode *> tailNodes;
    foreach(QGraphVizEdge *edge, tailEdges()) {
        tailNodes.append(qgraphicsitem_cast<DirectedGraphNode *>(edge->head()));
    }
    return tailNodes;
}


} // namespace SWAT
} // namespace Plugins
