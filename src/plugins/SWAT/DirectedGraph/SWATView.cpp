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

#include "SWATView.h"

#include <graphlib.h>

namespace Plugins {
namespace SWAT {

SWATView::SWATView(QWidget *parent) :
    STATView(parent),
    m_Graph(NULL)
{
}

SWATView::~SWATView()
{
    if(m_Graph) {
        graphlib_delGraph(m_Graph);
    }
}

void SWATView::setContent(const QByteArray &content)
{

    STATView::setContent(content);
}

void SWATView::loadGraphLib(const QString filename)
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
            qDebug() << temp.fileName() << temp.readAll();

            QByteArray content = temp.readAll();
            temp.close();
            setContent(content);
        }
    }
}


} // namespace SWAT
} // namespace Plugins
