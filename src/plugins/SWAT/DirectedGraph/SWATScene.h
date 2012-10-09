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

#ifndef PLUGINS_DIRECTEDGRAPH_SWATSCENE_H
#define PLUGINS_DIRECTEDGRAPH_SWATSCENE_H

#include "STATScene.h"

namespace Plugins {
namespace DirectedGraph {

class SWATScene : public STATScene
{
    Q_OBJECT
public:
    explicit SWATScene(QObject *parent = 0);

protected:
    enum SwatNodeInfoTypes {
        NodeInfoType_SwatInfo = 7
    };

    enum SwatEdgeInfoTypes {
        EdgeInfoType_SwatInfo = 4
    };

    virtual QGraphVizNode *createNode(node_t *node);
    virtual QGraphVizEdge *createEdge(edge_t *edge);

    virtual void processNodeLabel(quint64 id, QString label);
    virtual void processEdgeLabel(quint64 id, QString label);

};

} // namespace DirectedGraph
} // namespace Plugins

#endif // PLUGINS_DIRECTEDGRAPH_SWATSCENE_H
