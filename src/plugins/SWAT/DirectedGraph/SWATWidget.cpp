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

#include "GraphLibAdapter.h"
#include <graphlib.h>

#include "SWATScene.h"

namespace Plugins {
namespace DirectedGraph {

SWATWidget::SWATWidget(QWidget *parent) :
    STATWidget(parent),
    m_SWATScene(NULL)
{
}

SWATWidget::~SWATWidget()
{
    try {

        GraphLibAdapter adapter = GraphLibAdapter::instance();
        adapter.deleteGraph(m_GraphId);

#ifdef QT_DEBUG
    } catch(QString err) {
        qWarning() << tr("Error while destroying GraphLib graph") << err;
    } catch(...) {
        qWarning() << tr("Error while destroying GraphLib graph");
#else
    } catch(...) {
        //TODO: Non-debug logging
#endif
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
    if(!m_GraphId.isNull()) {
        throw tr("A GraphLib graph has already been loaded");
    }

    try {

        GraphLibAdapter adapter = GraphLibAdapter::instance();
        m_GraphId = adapter.createGraph(filename);
        adapter.processAttributes(m_GraphId);

        setContent(adapter.exportDotContent(m_GraphId));

#ifdef QT_DEBUG
    } catch(QString err) {
        qWarning() << tr("Error while creating GraphLib graph") << err;
    } catch(...) {
        qWarning() << tr("Error while creating GraphLib graph");
#else
    } catch(...) {
        //TODO: Non-debug logging
#endif
    }
}


} // namespace DirectedGraph
} // namespace Plugins
