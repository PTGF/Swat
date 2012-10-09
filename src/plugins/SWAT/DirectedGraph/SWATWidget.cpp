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

#include "DirectedGraph/SWATWidget.h"

#include <graphlib.h>

//#include <graphlib_graph_traits.h>
//#include <GraphRenderOrder.h>

#include "SWATScene.h"

namespace Plugins {
namespace DirectedGraph {

SWATWidget::SWATWidget(QWidget *parent) :
    STATWidget(parent),
    m_Graph(NULL),
    m_SWATScene(NULL)
{
}

SWATWidget::~SWATWidget()
{
    if(m_Graph) {
        graphlib_delGraph(m_Graph);
    }
}



DirectedGraphScene *SWATWidget::scene()
{
    return m_SWATScene;
}

DirectedGraphScene *SWATWidget::createScene(const QByteArray &content)
{
    if(!m_SWATScene) {
        m_SWATScene = new SWATScene();
        m_SWATScene->setContent(QString(content));
    }
    return m_SWATScene;
}



void SWATWidget::setContent(const QByteArray &content)
{
    STATWidget::setContent(content);

    // Connect any SWAT QActions

    // Do default SWAT actions on the view/scene

}



void SWATWidget::loadGraphLib(const QString filename)
{
    if(m_Graph) {
        throw tr("A GraphLib graph has already been loaded");
    }

    if(GRL_IS_FATALERROR(graphlib_loadGraph(filename.toLocal8Bit().data(), &m_Graph))) {
        throw tr("Failed to open GraphLib native format from file: %1").arg(filename);
    }

    QTemporaryFile temp;
    if(temp.open()) {
        temp.close();

        if(GRL_IS_FATALERROR(graphlib_exportGraph(temp.fileName().toLocal8Bit().data(), GRF_DOT, m_Graph))) {
            throw tr("Could not export GraphLib as GraphViz DOT file: %1 => %2").arg(filename, temp.fileName());
        }

        if(temp.open()) {
            QByteArray content = temp.readAll();
            temp.close();
            setContent(content);
        }
    }

}


} // namespace DirectedGraph
} // namespace Plugins
