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
namespace DirectedGraphView {

DirectedGraphScene::DirectedGraphScene(QString content, QObject *parent) :
    QGraphVizScene(parent)
{
    setContent(preprocessContent(content));
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
    QString retval;

    QRegExp rxLabel = QRegExp("label=\"(.*)\"", Qt::CaseInsensitive);
    rxLabel.setMinimal(true);  // non-greedy

    QRegExp rxNode = QRegExp("^\\s+([0-9\\-]+)\\s\\[.*fillcolor.*\\]", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rxEdge = QRegExp("^\\s+([0-9\\-]+)\\s->\\s([0-9\\-]+)\\s\\[.*\\]", Qt::CaseInsensitive, QRegExp::RegExp2);

    foreach(QString line, content.split('\n')) {
        if(rxNode.indexIn(line) >= 0 && rxNode.captureCount() == 1) {           // Is node
            if(rxLabel.indexIn(line) >= 0 && rxLabel.captureCount() == 1) {     // With label
                QString label = rxLabel.cap(1);
                NodeInfo nodeInfo = getNodeInfo(label);
                m_NodeInfos.insert(rxNode.cap(1).toLongLong(), nodeInfo);
                line = line.replace(label, nodeInfo.shortLabel);
            }
        } else if(rxEdge.indexIn(line) >= 0 && rxEdge.captureCount() == 2) {    // Is edge
            if(rxLabel.indexIn(line) >= 0 && rxLabel.captureCount() == 1) {     // With label
                QString label = rxLabel.cap(1);
                EdgeInfo edgeInfo = getEdgeInfo(label);
                m_EdgeInfos.insert(rxEdge.cap(2).toLongLong(), edgeInfo);
                line = line.replace(label, edgeInfo.shortLabel);
            }
        }

        retval += line;
    }

    return retval;
}

DirectedGraphScene::NodeInfo DirectedGraphScene::getNodeInfo(QString label)
{
    static const quint8 maxNodeLabelSize = 64;
    static QRegExp rxFullText = QRegExp("^([\\d\\w\\_\\-\\.]+)(?:@([\\d\\w\\_\\-\\.\\/\\\\]+)(?:\\:(\\d+))?)?(?:\\$(.+))?");

    NodeInfo nodeInfo;

    nodeInfo.longLabel = label.trimmed();
    if(rxFullText.indexIn(nodeInfo.longLabel) >= 0) {
        nodeInfo.functionName = rxFullText.cap(1);
        nodeInfo.longLabel = nodeInfo.functionName;

        if(!rxFullText.cap(2).isEmpty()) {
            if(!rxFullText.cap(3).isEmpty()) {
                nodeInfo.sourceFile = rxFullText.cap(2);
                nodeInfo.sourceLine = rxFullText.cap(3).toULong();

                nodeInfo.longLabel += QString("@%1:%2").arg(QFileInfo(nodeInfo.sourceFile).fileName()).arg(nodeInfo.sourceLine);

            } else {
                nodeInfo.programCounter = rxFullText.cap(2).toULongLong(0, 16);
                nodeInfo.longLabel += QString("@%1").arg(nodeInfo.programCounter);
            }
        }

        if(!rxFullText.cap(4).isEmpty()) {
            nodeInfo.iter_string = rxFullText.cap(4);
            nodeInfo.longLabel += QString("$%1").arg(nodeInfo.iter_string);
        }
    }

    nodeInfo.shortLabel = nodeInfo.longLabel;
    if(nodeInfo.shortLabel.count() > maxNodeLabelSize) {
        nodeInfo.shortLabel = "..." + nodeInfo.shortLabel.right(maxNodeLabelSize - 3);
    }

    return nodeInfo;
}

DirectedGraphScene::EdgeInfo DirectedGraphScene::getEdgeInfo(QString label)
{
    static const quint8 maxEdgeLabelSize = 24;
    static QRegExp rxLabel = QRegExp("(?:(\\d+):)*\\[(.*)\\]");
    static QRegExp rxRange = QRegExp("(\\d+)\\-(\\d+)");

    EdgeInfo edgeInfo;

    if(rxLabel.indexIn(label) >= 0) {
        if(rxLabel.cap(2).isEmpty()) {
            edgeInfo.processList = rxLabel.cap(1).split(',');
        } else {
            edgeInfo.processCount = rxLabel.cap(1);
            edgeInfo.processList = rxLabel.cap(2).split(',');
        }
    }

    if(edgeInfo.processCount.isEmpty()) {
        if(edgeInfo.processList.contains("...")) {
            edgeInfo.processCount = "?";    // Can't count a truncated proc list
        } else {
            bool okay;
            quint64 processCount = 0;

            // Iterate through the list and count the procs
            foreach(QString processes, edgeInfo.processList) {
                if(rxRange.indexIn(processes) >= 0 && rxRange.captureCount() == 2) {
                    quint64 first = rxRange.cap(1).toULongLong(&okay);
                    if(!okay) { continue; }

                    quint64 last = rxRange.cap(2).toULongLong(&okay);
                    if(!okay) { continue; }

                    processCount += (last - first) + 1;
                } else {
                    ++processCount;
                }
            }

            edgeInfo.processCount = QString::number(processCount);
        }
    }

    edgeInfo.longLabel = QString("%1:[%2]").arg(edgeInfo.processCount).arg(edgeInfo.processList.join(","));

    // Reduce the length of the printed label and add an ellipsis
    QStringList processList = edgeInfo.processList;
    if((3 + edgeInfo.processCount.count() + edgeInfo.processList.join(",").count()) > maxEdgeLabelSize) {
        processList.removeOne("...");    // In case there's already an ellipsis

        while((6 + edgeInfo.processCount.count() + processList.join(",").count()) > maxEdgeLabelSize) {
            processList.pop_back();
        }

        processList.append("...");
    }
    edgeInfo.shortLabel = QString("%1:[%2]").arg(edgeInfo.processCount).arg(processList.join(","));

    return edgeInfo;
}


} // namespace DirectedGraphView
} // namespace Plugins
