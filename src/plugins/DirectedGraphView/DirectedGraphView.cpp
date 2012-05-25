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

#include "DirectedGraphView.h"
#include "ui_DirectedGraphView.h"

namespace Plugins {
namespace DirectedGraphView {

DirectedGraphView::DirectedGraphView(const QByteArray &content, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirectedGraphView),
    m_Scene(NULL),
    m_View(NULL)
{
    ui->setupUi(this);

    m_Scene = new QGraphVizScene(QString(content));
    m_View = new QGraphVizView(m_Scene, this);
    m_Scene->setParent(m_View);
    m_View->setWindowTitle(tr("File"));

    ui->verticalLayout->insertWidget(0, m_View);

}

DirectedGraphView::~DirectedGraphView()
{
    delete ui;
}

QGraphVizView *DirectedGraphView::view()
{
    return m_View;
}

void DirectedGraphView::on_txtFilter_textChanged(const QString &filter)
{
//TODO:    m_View->setFilter(filter);
}


} // namespace DirectedGraphView
} // namespace Plugins
