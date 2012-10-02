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

#include "DirectedGraphScene.h"

#include "DirectedGraphNode.h"
#include "DirectedGraphEdge.h"

namespace Plugins {
namespace SWAT {

DirectedGraphScene::DirectedGraphScene(QObject *parent) :
    QGraphVizScene(parent)
{
}

void DirectedGraphScene::setContent(QString content)
{
    QGraphVizScene::setContent(preprocessContent(content));
}

QGraphVizNode *DirectedGraphScene::createNode(node_t *node)
{
    return new DirectedGraphNode(node, this);
}

QGraphVizEdge *DirectedGraphScene::createEdge(edge_t *edge)
{
    return new DirectedGraphEdge(edge, this);
}

QString DirectedGraphScene::preprocessContent(const QString &content)
{
    if(content.isEmpty()) {
        return QString();
    }

    QString retval;

    QRegExp rxLabel = QRegExp("label=\"(.*)\"", Qt::CaseInsensitive);
    rxLabel.setMinimal(true);  // non-greedy

    QRegExp rxNode = QRegExp("^\\s+([0-9\\-]+)\\s\\[.*fillcolor.*\\]", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxEdge = QRegExp("^\\s+([0-9\\-]+)\\s->\\s([0-9\\-]+)\\s\\[.*\\]", Qt::CaseInsensitive, QRegExp::RegExp2);

    foreach(QString line, content.split('\n')) {
        if(rxNode.indexIn(line) >= 0 && rxNode.captureCount() == 1) {           // Is node
            if(rxLabel.indexIn(line) >= 0 && rxLabel.captureCount() == 1) {     // With label
                QString label = rxLabel.cap(1);
                qint64 id = rxNode.cap(1).toLongLong();

                processNodeLabel(id, label);

                line = line.replace(label, nodeInfo(id, NodeInfoType_ShortLabel).toString());
            }
        } else if(rxEdge.indexIn(line) >= 0 && rxEdge.captureCount() == 2) {    // Is edge
            if(rxLabel.indexIn(line) >= 0 && rxLabel.captureCount() == 1) {     // With label
                QString label = rxLabel.cap(1);
                qint64 id = rxEdge.cap(2).toLongLong();

                processEdgeLabel(id, label);

                line = line.replace(label, edgeInfo(id, EdgeInfoType_ShortLabel).toString());
            }
        }

        retval += line;
    }

    return retval;
}

void DirectedGraphScene::processNodeLabel(quint64 id, QString label)
{

    static const quint8 maxNodeLabelSize = 64;

    QString shortLabel = label;
    if(shortLabel.count() > maxNodeLabelSize) {
        shortLabel = "..." + shortLabel.right(maxNodeLabelSize - 3);
    }

    setNodeInfo(id, NodeInfoType_LongLabel, label);
    setNodeInfo(id, NodeInfoType_ShortLabel, shortLabel);

}

void DirectedGraphScene::processEdgeLabel(quint64 id, QString label)
{
    static const quint8 maxEdgeLabelSize = 24;

    QString shortLabel = label;
    if(shortLabel.count() > maxEdgeLabelSize) {
        shortLabel = shortLabel.left(maxEdgeLabelSize - 3) + "...";
    }

    setEdgeInfo(id, EdgeInfoType_LongLabel, label);
    setEdgeInfo(id, EdgeInfoType_ShortLabel, shortLabel);

}


void DirectedGraphScene::setNodeInfo(const qint64 &id, const int &type, const QVariant &value)
{
    m_NodeInfos[id][type] = value;
}

QVariant DirectedGraphScene::nodeInfo(const qint64 &id, const int &type, const QVariant &defaultValue) const
{
    if(m_NodeInfos.contains(id)) {
        info nodeInfo = m_NodeInfos.value(id);
        if(nodeInfo.contains(type)) {
            return nodeInfo.value(type);
        }
    }

    return defaultValue;
}

void DirectedGraphScene::setEdgeInfo(const qint64 &id, const int &type, const QVariant &value)
{
    m_EdgeInfos[id][type] = value;
}

QVariant DirectedGraphScene::edgeInfo(const qint64 &id, const int &type, const QVariant &defaultValue) const
{
    if(m_EdgeInfos.contains(id)) {
        info edgeInfo = m_EdgeInfos.value(id);
        if(edgeInfo.contains(type)) {
            return edgeInfo.value(type);
        }
    }

    return defaultValue;
}

} // namespace SWAT
} // namespace Plugins
