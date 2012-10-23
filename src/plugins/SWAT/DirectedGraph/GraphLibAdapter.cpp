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

#include "GraphLibAdapter.h"

#include <iostream>
#include <fstream>
#include <string>

#include <graphlib.h>
#include <graphlib_internals.h>
#include <boost/graph/graph_traits.hpp>
#include <graphlib_graph_traits.h>
#include <GraphRenderOrder.h>

namespace Plugins {
namespace DirectedGraph {

GraphLibAdapter &GraphLibAdapter::instance()
{
    static GraphLibAdapter m_Instance;
    return m_Instance;
}

GraphLibAdapter::GraphLibAdapter()
{
}

GraphLibAdapter::~GraphLibAdapter()
{
    // Clean up all edge and node attribute caches
    QList<QUuid> graphIds;
    graphIds.append(m_Nodes.keys());
    graphIds.append(m_Edges.keys());
    foreach(QUuid graphId, graphIds) {
        if(m_Nodes.contains(graphId)) {
            QList<GraphLibNode *> *nodes = m_Nodes.take(graphId);
            qDeleteAll(*nodes);
            delete nodes;
        }
        if(m_Edges.contains(graphId)) {
            QList<GraphLibEdge *> *edges = m_Edges.take(graphId);
            qDeleteAll(*edges);
            delete edges;
        }
    }
}

graphlib_graph_p GraphLibAdapter::graph(const QUuid &graphId) const
{
    if(!m_Graphs.contains(graphId)) {
        throw QString("Graph with ID %1 does not exist").arg(graphId.toString());
    }
    return m_Graphs.value(graphId);
}

QUuid GraphLibAdapter::createGraph(const QString &filename)
{
    graphlib_graph_p graph;
    if(GRL_IS_FATALERROR(graphlib_loadGraph(filename.toLocal8Bit().data(), &graph))) {
        throw QString("Failed to open GraphLib native format from file: %1").arg(filename);
    }

    QUuid graphId = QUuid::createUuid();
    m_Graphs.insert(graphId, graph);

    processAttributes(graphId);

    return graphId;
}

void GraphLibAdapter::deleteGraph(const QUuid &graphId)
{
    graphlib_graph_p graph = this->graph(graphId);
    graphlib_delGraph(graph);
}


QByteArray GraphLibAdapter::exportDotContent(const QUuid &graphId)
{
    graphlib_graph_p graph = this->graph(graphId);
    QByteArray content;

    {
        QString tempFilename = QString("/tmp/%1.tmp").arg(graphId.toString());
        if(GRL_IS_FATALERROR(graphlib_exportGraph(tempFilename.toLocal8Bit().data(), GRF_DOT, graph))) {
            throw QString("Could not export GraphLib as GraphViz DOT file: %1 => %2").arg(graphId.toString(), tempFilename);
        }

        std::ifstream file(tempFilename.toLocal8Bit().data());
        if(!file.is_open()) {
            throw QString("Could not export GraphLib as GraphViz DOT file: %1 => %2").arg(graphId.toString(), tempFilename);
        }
        std::string line;
        while(file.good()) {
            getline(file, line);
            content.append(line.c_str());
            content.append('\n');
        }
        file.close();

        unlink(tempFilename.toLocal8Bit().data());
    }

    return content;
}


void GraphLibAdapter::processAttributes(const QUuid &graphId)
{

    graphlib_graph_p graph = this->graph(graphId);

    {
        QList<GraphLibNode *> *nodes = new QList<GraphLibNode *>();

        graphlib_nodefragment_d *nodefrag = graph->nodes;
        while(nodefrag!=NULL) {
            for(int i=0; i < nodefrag->count; ++i) {
                if(boost::full(nodefrag, i)) {
                    graphlib_nodedata_d *node = get(nodefrag, i);

                    GraphLibNode *nodeAttr = new GraphLibNode();
                    nodeAttr->id = node->id;
                    nodeAttr->name = QString(node->attr.name);
                    nodes->append(nodeAttr);

                }
            }
            nodefrag = nodefrag->next;
        }
        m_Nodes.insert(graphId, nodes);
    }


    {
        QList<GraphLibEdge *> *edges = new QList<GraphLibEdge *>();

        graphlib_edgefragment_d *edgefrag = graph->edges;
        while(edgefrag!=NULL) {
            for(int i=0; i < edgefrag->count; ++i) {
                if(boost::full(edgefrag, i)) {
                    graphlib_edgedata_d *edge = get(edgefrag, i);

                    GraphLibEdge *edgeAttr = new GraphLibEdge();
                    edgeAttr->fromId = edge->node_from;
                    edgeAttr->toId = edge->node_to;
                    edgeAttr->name = QString(edge->attr.name);
                    edges->append(edgeAttr);
                }
            }
            edgefrag = edgefrag->next;
        }
        m_Edges.insert(graphId, edges);
    }


}


const QList<GraphLibNode *> &GraphLibAdapter::nodes(QUuid graphId) const
{
    if(!m_Graphs.contains(graphId)) {
        throw QString("Graph with ID %1 does not exist").arg(graphId.toString());
    }

    if(!m_Nodes.contains(graphId)) {
        throw QString("Graph with ID %1 does not have any processed nodes; has proccessAtributes() been called?").arg(graphId.toString());
    }

    return *m_Nodes.value(graphId);
}

const QList<GraphLibEdge *> &GraphLibAdapter::edges(QUuid graphId) const
{
    if(!m_Graphs.contains(graphId)) {
        throw QString("Graph with ID %1 does not exist").arg(graphId.toString());
    }

    if(!m_Edges.contains(graphId)) {
        throw QString("Graph with ID %1 does not have any processed edges; has proccessAtributes() been called?").arg(graphId.toString());
    }

    return *m_Edges.value(graphId);
}





} // namespace DirectedGraph
} // namespace Plugins
