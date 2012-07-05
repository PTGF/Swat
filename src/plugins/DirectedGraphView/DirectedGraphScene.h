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

#ifndef DIRECTEDGRAPHSCENE_H
#define DIRECTEDGRAPHSCENE_H

#include <QtCore>
#include <QtGui>

#include <QGraphVizScene.h>

namespace Plugins {
namespace DirectedGraphView {

class DirectedGraphScene : public QGraphVizScene
{
    Q_OBJECT
public:
    explicit DirectedGraphScene(QString content, QObject *parent = 0);

protected:
    struct NodeInfo {
        QString longLabel;
        QString shortLabel;

        QString functionName;
        quint64 programCounter;
        QString sourceFile;
        quint32 sourceLine;
        QString iter_string;
    };

    struct EdgeInfo {
        QString longLabel;
        QString shortLabel;

        QString processCount;
        QStringList processList;
    };

    QGraphVizNode *createNode(node_t *node);
    QGraphVizEdge *createEdge(edge_t *edge);

    QString preprocessContent(const QString &content);
    NodeInfo getNodeInfo(QString label);
    EdgeInfo getEdgeInfo(QString label);

private:
    QHash<int, NodeInfo> m_NodeInfos;
    QHash<int, EdgeInfo> m_EdgeInfos;

    friend class DirectedGraphNode;
    friend class DirectedGraphEdge;
    friend class DirectedGraphView;
};

} // namespace DirectedGraphView
} // namespace Plugins

#endif // DIRECTEDGRAPHSCENE_H
