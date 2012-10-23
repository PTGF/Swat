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

#include "STATNode.h"

#include "STATScene.h"
#include "STATEdge.h"

#include <QDebug>

namespace Plugins {
namespace DirectedGraph {

STATNode::STATNode(node_t *node, STATScene *scene, QGraphicsItem *parent) :
    DirectedGraphNode(node, scene, parent),
    m_Scene(scene)
{
}

QString STATNode::functionName()
{
    QString retval = m_Scene->nodeInfo(nodeId(), STATScene::NodeInfoType_FunctionName).toString();
    return retval;
}

quint64 STATNode::programCounter()
{
    quint64 retval = m_Scene->nodeInfo(nodeId(), STATScene::NodeInfoType_ProgramCounter).toULongLong();
    return retval;
}

QString STATNode::sourceFile()
{
    QString retval = m_Scene->nodeInfo(nodeId(), STATScene::NodeInfoType_SourceFile).toString();
    return retval;
}

quint32 STATNode::sourceLine()
{
    quint32 retval = m_Scene->nodeInfo(nodeId(), STATScene::NodeInfoType_SourceLine).toUInt();
    return retval;
}

QString STATNode::iter()
{
    QString retval = m_Scene->nodeInfo(nodeId(), STATScene::NodeInfoType_IterString).toString();
    return retval;
}



QString STATNode::processCount()
{
    QString retval = m_Scene->edgeInfo(nodeId(), STATScene::EdgeInfoType_ProcessCount).toString();
    return retval;
}

QStringList STATNode::processList()
{
    QStringList retval =  m_Scene->edgeInfo(nodeId(), STATScene::EdgeInfoType_ProcessList).toStringList();
    return retval;
}



QList<quint64> STATNode::splitProcessList(const QStringList &lists)
{
    QList<quint64> retval;

    foreach(QString list, lists) {
        retval << splitProcessList(list);
    }

    return retval;
}

QList<quint64> STATNode::splitProcessList(const QString &list)
{
    static const QRegExp rxRange("^([0-9]+)\\-([0-9]+)$");

    bool okay;
    QList<quint64> retval;

    if(rxRange.indexIn(list) >= 0) {
        quint64 start = rxRange.cap(1).toLongLong(&okay);
        if(!okay) {
            qWarning() << "Failed to parse process list properly (start)";
            return retval;
        }

        quint64 finish = rxRange.cap(2).toLongLong(&okay);
        if(!okay) {
            qWarning() << "Failed to parse process list properly (finish)";
            return retval;
        }

        for(quint64 i = start; i <= finish; ++i) {
            retval << i;
        }

    } else {
        quint64 value = list.toLongLong(&okay);
        if(!okay) {
            qWarning() << "Failed to parse process list properly (single)";
            return retval;
        }

        retval << value;
    }

    return retval;

}

QStringList STATNode::mergeProcessList(const QList<quint64> &list)
{
    QStringList retval;

    QList<quint64>sortedList = list;
    qSort(sortedList.begin(), sortedList.end());

    static const quint64 max64 = -1;
    quint64 start = max64;
    quint64 end = max64;
    quint64 last = max64;
    for(int i = 0; i <= sortedList.count(); ++i) {
        quint64 value = max64;
        if(i < sortedList.count()) {
            value = sortedList.at(i);
        }

        if(last != max64 && value == (last + 1)) {
            end = last = value;
        } else {
            if(start != max64) {
                if(start == end) {
                    retval << QString::number(start);
                } else {
                    retval << QString("%1-%2").arg(start).arg(end);
                }
            }
            end = start = last = value;
        }
    }

    return retval;
}


const QStringList &STATNode::leafTasks()
{
    if(!m_LeafTaskCount.isEmpty()) {
        return m_LeafTasks;
    }

    QSet<quint64> processList = splitProcessList(this->processList()).toSet();

    QList<quint64> childProcessLists;
    foreach(QGraphVizNode *node, childNodes()) {
        if(STATNode *statNode = qgraphicsitem_cast<STATNode *>(node)) {
            childProcessLists << splitProcessList(statNode->processList());
        }
    }

    processList.subtract(childProcessLists.toSet());

    m_LeafTaskCount = QString::number(processList.count());

    m_LeafTasks = mergeProcessList(processList.toList());

    return m_LeafTasks;
}


const QString &STATNode::leafTaskCount()
{
    if(m_LeafTasks.isEmpty()) {
        leafTasks();
    }

    return m_LeafTaskCount;
}



void STATNode::showToolTip(const QPoint &pos, QWidget *w, const QRect &rect)
{
    QStringList toolTips;
    toolTips << QApplication::tr("Function:      %1").arg(this->functionName());

    if(this->processList().count() > 8) {
        toolTips << QApplication::tr("<br />Process Count: %1").arg(this->processCount());
    } else {
        toolTips << QApplication::tr("<br />Process Count: %1").arg(this->processCount());
        toolTips << QApplication::tr("Process List:  %1").arg(this->processList().join(", "));
    }

    if(!sourceFile().isEmpty()) {
        toolTips << QApplication::tr("<br />Source File:   %1").arg(this->sourceFile());
        toolTips << QApplication::tr("Source Line:   %1").arg(this->sourceLine());
    }

    QToolTip::showText(pos, QString("<pre>%1</pre>").arg(toolTips.join("<br />")), w, rect);
}


} // namespace DirectedGraph
} // namespace Plugins
