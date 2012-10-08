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

#ifndef PLUGINS_SWAT_SWATVIEW_H
#define PLUGINS_SWAT_SWATVIEW_H

#include <QWidget>
#include <DirectedGraph/STATView.h>

struct graphlib_graph_d;
typedef graphlib_graph_d *graphlib_graph_p;

namespace Plugins {
namespace SWAT {

class SWATView : public STATView
{
    Q_OBJECT

public:
    explicit SWATView(QWidget * parent = 0);
    ~SWATView();

    virtual void setContent(const QByteArray &content);
    void loadGraphLib(const QString filename);

private:
    graphlib_graph_p m_Graph;

};


} // namespace SWAT
} // namespace Plugins
#endif // PLUGINS_SWAT_SWATVIEW_H
