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

#ifndef PLUGINS_SWAT_STATSCENE_H
#define PLUGINS_SWAT_STATSCENE_H

#include <QtCore>
#include <QtGui>

#include <DirectedGraph/DirectedGraphScene.h>

namespace Plugins {
namespace SWAT {

class STATScene : public DirectedGraphScene
{
    Q_OBJECT
public:
    explicit STATScene(QString content, QObject *parent = 0);

protected:
    enum StatNodeInfoTypes {
        NodeInfoType_FunctionName = 2,
        NodeInfoType_ProgramCounter = 3,
        NodeInfoType_SourceFile = 4,
        NodeInfoType_SourceLine = 5,
        NodeInfoType_IterString = 6
    };

    enum StatEdgeInfoTypes {
        EdgeInfoType_ProcessCount = 2,
        EdgeInfoType_ProcessList = 3
    };

    virtual void processNodeLabel(quint64 id, QString label);
    virtual void processEdgeLabel(quint64 id, QString label);

    virtual QGraphVizNode *createNode(node_t *node);
    virtual QGraphVizEdge *createEdge(edge_t *edge);

    virtual DirectedGraphScene *createScene(const QByteArray &content);


private:

    friend class STATNode;
    friend class STATEdge;
    friend class STATView;

};

} // namespace SWAT
} // namespace Plugins

#endif // PLUGINS_SWAT_STATSCENE_H
