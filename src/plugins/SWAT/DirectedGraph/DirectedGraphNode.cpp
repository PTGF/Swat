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
namespace DirectedGraph {

DirectedGraphNode::DirectedGraphNode(node_t *node, DirectedGraphScene *scene, QGraphicsItem *parent) :
    QGraphVizNode(node, scene, parent),
    m_Scene(scene),
    m_NodeId(-1),
    m_Depth(-1),
    m_ParentNode(NULL)
{
}

const qint64 &DirectedGraphNode::nodeId()
{
    if(m_NodeId < 0) {
        m_NodeId = getGVName().toLongLong();
    }
    return m_NodeId;
}

const int &DirectedGraphNode::nodeDepth()
{
    if(m_Depth < 0) {
        m_Depth = 0;
        if(headEdges().count() == 1) {
            if(DirectedGraphNode *parent = dynamic_cast<DirectedGraphNode*>(headEdges().at(0)->tail())) {
                m_Depth += parent->nodeDepth() + 1;
            }
        }
    }
    return m_Depth;
}

QString DirectedGraphNode::label()
{
    return m_Scene->nodeInfo(nodeId(), DirectedGraphScene::NodeInfoType_LongLabel).toString();
}

QString DirectedGraphNode::shortLabel()
{
    return m_Scene->nodeInfo(nodeId(), DirectedGraphScene::NodeInfoType_ShortLabel).toString();
}

QString DirectedGraphNode::edgeLabel()
{
    return m_Scene->edgeInfo(nodeId(), DirectedGraphScene::EdgeInfoType_LongLabel).toString();
}

QString DirectedGraphNode::shortEdgeLabel()
{
    return m_Scene->edgeInfo(nodeId(), DirectedGraphScene::EdgeInfoType_ShortLabel).toString();
}

DirectedGraphNode *DirectedGraphNode::parentNode()
{
    if(!m_ParentNode) {
        QList<QGraphVizEdge *> edges = headEdges();
        if(edges.count() == 1) {
            m_ParentNode = qgraphicsitem_cast<DirectedGraphNode *>(edges.at(0)->tail());
        }
    }

    return m_ParentNode;
}

QList<DirectedGraphNode *> DirectedGraphNode::childNodes()
{
    if(m_ChildNodes.isEmpty()) {
        foreach(QGraphVizEdge *edge, tailEdges()) {
            m_ChildNodes.append(qgraphicsitem_cast<DirectedGraphNode *>(edge->head()));
        }
    }

    return m_ChildNodes;
}


void DirectedGraphNode::showToolTip(const QPoint &pos, QWidget *w, const QRect &rect)
{
    QStringList toolTips;
    toolTips << this->label();
    toolTips << this->edgeLabel();

    QToolTip::showText(pos, toolTips.join("<br />"), w, rect);
}



} // namespace DirectedGraph
} // namespace Plugins
