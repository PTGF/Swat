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

#ifndef PLUGINS_DIRECTEDGRAPH_GRAPHLIBADAPTER_H
#define PLUGINS_DIRECTEDGRAPH_GRAPHLIBADAPTER_H

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QUuid>
#include <QtCore/QMap>
#include <QtCore/QByteArray>

struct graphlib_graph_d;
typedef graphlib_graph_d *graphlib_graph_p;

namespace Plugins {
namespace DirectedGraph {

struct GraphLibNode {
    qint64 id;
    QString name;
};

struct GraphLibEdge {
    qint64 fromId;
    qint64 toId;
    QString name;
};


class GraphLibAdapter
{
public:
    static GraphLibAdapter &instance();
    ~GraphLibAdapter();

    QUuid createGraph(const QString &filename);
    void deleteGraph(const QUuid &graphId);
    QByteArray exportDotContent(const QUuid &graphId);

    void processAttributes(const QUuid &graphId);
    const QList<GraphLibNode *> &nodes(QUuid graphId) const;
    const QList<GraphLibEdge *> &edges(QUuid graphId) const;

protected:
    GraphLibAdapter();
    graphlib_graph_p graph(const QUuid &graphId) const;

private:
    QMap<QUuid, graphlib_graph_p> m_Graphs;

    QMap<QUuid, QList<GraphLibNode *> *> m_Nodes;
    QMap<QUuid, QList<GraphLibEdge *> *> m_Edges;

    graphlib_graph_p m_CurrentGraph;

};

} // namespace DirectedGraph
} // namespace Plugins

#endif // PLUGINS_DIRECTEDGRAPH_GRAPHLIBADAPTER_H
