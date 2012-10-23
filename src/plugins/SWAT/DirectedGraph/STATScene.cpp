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

#include "STATScene.h"

#include "STATEdge.h"
#include "STATNode.h"

namespace Plugins {
namespace DirectedGraph {

STATScene::STATScene(QObject *parent) :
    DirectedGraphScene(parent)
{
}

QGraphVizNode *STATScene::createNode(node_t *node)
{
    return new STATNode(node, this);
}

QGraphVizEdge *STATScene::createEdge(edge_t *edge)
{
    return new STATEdge(edge, this);
}


void STATScene::processNodeLabel(const qint64 &id, const QString &label)
{
    static const quint8 maxNodeLabelSize = 64;
    static QRegExp rxFullText = QRegExp("^([\\d\\w\\_\\-\\.]+)(?:@([\\d\\w\\_\\-\\.\\/\\\\]+)(?:\\:(\\d+))?)?(?:\\$(.+))?");

    QString longLabel = label.trimmed();
    if(rxFullText.indexIn(longLabel) >= 0) {
        setNodeInfo(id, NodeInfoType_FunctionName, rxFullText.cap(1));
        longLabel = rxFullText.cap(1);

        if(!rxFullText.cap(2).isEmpty()) {
            if(!rxFullText.cap(3).isEmpty()) {
                QString sourceFile = rxFullText.cap(2);
                quint32 sourceLine = rxFullText.cap(3).toULong();
                setNodeInfo(id, NodeInfoType_SourceFile, sourceFile);
                setNodeInfo(id, NodeInfoType_SourceLine, sourceLine);
                longLabel += QString("@%1:%2").arg(QFileInfo(sourceFile).fileName()).arg(sourceLine);
            } else {
                quint64 programCounter = rxFullText.cap(2).toULongLong(0, 16);
                setNodeInfo(id, NodeInfoType_ProgramCounter, programCounter);
                longLabel += QString("@%1").arg(programCounter);
            }
        }

        if(!rxFullText.cap(4).isEmpty()) {
            QString iterString = rxFullText.cap(4);
            setNodeInfo(id, NodeInfoType_IterString, iterString);
            longLabel += QString("$%1").arg(iterString);
        }
    }


    QString shortLabel = longLabel;
    if(shortLabel.count() > maxNodeLabelSize) {
        shortLabel = "..." + shortLabel.right(maxNodeLabelSize - 3);
    }

    setNodeInfo(id, NodeInfoType_LongLabel, longLabel);
    setNodeInfo(id, NodeInfoType_ShortLabel, shortLabel);
}

void STATScene::processEdgeLabel(const qint64 &id, const QString &label)
{
    static const quint8 maxEdgeLabelSize = 24;
    static QRegExp rxLabel = QRegExp("(?:(\\d+):)*\\[(.*)\\]");
    static QRegExp rxRange = QRegExp("(\\d+)\\-(\\d+)");

    QStringList processList;
    QString processCount;
    if(rxLabel.indexIn(label) >= 0) {
        if(rxLabel.cap(2).isEmpty()) {
            processList = rxLabel.cap(1).split(',');
        } else {
            processCount = rxLabel.cap(1);
            processList = rxLabel.cap(2).split(',');
        }
    }

    if(processCount.isEmpty()) {
        if(processList.contains("...")) {
            processCount = "?";    // Can't count a truncated proc list
        } else {
            bool okay;
            quint64 numProcessCount = 0;

            // Iterate through the list and count the procs
            foreach(QString processes, processList) {
                if(rxRange.indexIn(processes) >= 0 && rxRange.captureCount() == 2) {
                    quint64 first = rxRange.cap(1).toULongLong(&okay);
                    if(!okay) { continue; }

                    quint64 last = rxRange.cap(2).toULongLong(&okay);
                    if(!okay) { continue; }

                    numProcessCount += (last - first) + 1;
                } else {
                    ++numProcessCount;
                }
            }

            processCount = QString::number(numProcessCount);
        }
    }

    setEdgeInfo(id, EdgeInfoType_ProcessList, processList);
    setEdgeInfo(id, EdgeInfoType_ProcessCount, processCount);


    QString longLabel = QString("%1:[%2]").arg(processCount).arg(processList.join(","));

    // Reduce the length of the printed label and add an ellipsis
    if((3 + processCount.count() + processList.join(",").count()) > maxEdgeLabelSize) {
        processList.removeOne("...");    // In case there's already an ellipsis

        while((6 + processCount.count() + processList.join(",").count()) > maxEdgeLabelSize) {
            processList.pop_back();
        }

        processList.append("...");
    }
    QString shortLabel = QString("%1:[%2]").arg(processCount).arg(processList.join(","));

    setEdgeInfo(id, EdgeInfoType_LongLabel, longLabel);
    setEdgeInfo(id, EdgeInfoType_ShortLabel, shortLabel);
}



} // namespace DirectedGraph
} // namespace Plugins
