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

#include "DirectedGraphEdge.h"

#include <QGraphVizNode.h>
#include "DirectedGraphScene.h"

namespace Plugins {
namespace SWAT {

DirectedGraphEdge::DirectedGraphEdge(edge_t *edge, DirectedGraphScene *scene, QGraphicsItem *parent) :
    QGraphVizEdge(edge, scene, parent),
    m_Scene(scene)
{
}

QString DirectedGraphEdge::processList()
{
    QString longLabel = m_Scene->m_EdgeInfos.value(head()->getGVName().toLongLong()).longLabel;
    if(longLabel.isEmpty()) {
        longLabel = labelText();
    }
    return longLabel;
}

} // namespace SWAT
} // namespace Plugins
