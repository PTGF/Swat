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

#ifndef DIRECTEDGRAPHNODE_H
#define DIRECTEDGRAPHNODE_H

#include <QtCore>
#include <QtGui>

#include <QGraphVizNode.h>

#include "DirectedGraphScene.h"

namespace Plugins {
namespace DirectedGraphView {

class DirectedGraphScene;

class DirectedGraphNode : public QGraphVizNode
{
public:
    explicit DirectedGraphNode(node_t *node, DirectedGraphScene *scene, QGraphicsItem *parent = 0);

protected:
    DirectedGraphScene::NodeInfo getNodeInfo();
    DirectedGraphScene::EdgeInfo getEdgeInfo();

private:
    DirectedGraphScene *m_Scene;

    friend class DirectedGraphScene;
    friend class DirectedGraphView;
};

} // namespace DirectedGraphView
} // namespace Plugins

#endif // DIRECTEDGRAPHNODE_H
