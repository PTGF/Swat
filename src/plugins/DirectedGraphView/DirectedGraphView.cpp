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

#include <QGraphVizView.h>

#include <PluginManager/PluginManager.h>
#include <SourceView/ISourceViewFactory.h>

#include "DirectedGraphScene.h"
#include "DirectedGraphNode.h"
#include "DirectedGraphNodeDialog.h"

namespace Plugins {
namespace DirectedGraphView {

DirectedGraphView::DirectedGraphView(const QByteArray &content, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DirectedGraphView),
    m_Scene(NULL),
    m_View(NULL)
{
    ui->setupUi(this);

    m_Scene = new DirectedGraphScene(QString(content));
    m_View = new QGraphVizView(m_Scene, this);
    m_Scene->setParent(m_View);

    setWindowTitle(tr("Directed Graph View"));

    ui->layStack->insertWidget(0, m_View);

    connect(m_Scene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
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
    QRegExp rx = QRegExp(filter, Qt::CaseInsensitive, QRegExp::RegExp2);
    foreach(QGraphVizNode *gvNode, m_Scene->getNodes()) {
        if(DirectedGraphNode *node = dynamic_cast<DirectedGraphNode *>(gvNode)) {
            if(rx.indexIn(node->label()) >= 0) {
                node->setSelected(true);
            } else {
                node->setSelected(false);
            }
        }
    }
}

void DirectedGraphView::selectionChanged()
{
    if(m_Scene->selectedItems().count() == 1) {
        if(DirectedGraphNode *node = dynamic_cast<DirectedGraphNode *>(m_Scene->selectedItems().at(0))) {
            DirectedGraphNodeDialog *dlg = new DirectedGraphNodeDialog(this);
            dlg->setNode(node);
            dlg->exec();
        }
    }
}

void DirectedGraphView::openSourceFile(QString filename, int lineNumber)
{
    loadSourceFromFile(filename);
    //TODO: Set view line number
}

void DirectedGraphView::loadSourceFromFile(QString filename)
{
    QFileInfo fileInfo(filename);

    if(!fileInfo.exists()) {
        throw tr("File does not exist: '%1'").arg(fileInfo.absoluteFilePath());
    }

    QFile file(fileInfo.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly)) {
        throw tr("Failed to open file: '%1'").arg(fileInfo.absoluteFilePath());
    }

    QByteArray fileContent = file.readAll();

    file.close();

    loadSourceFromContent(fileContent, fileInfo.fileName());
}

void DirectedGraphView::loadSourceFromContent(QByteArray content, QString title)
{
    if(QPlainTextEdit *view = getSourceView(content)) {
        if(!title.isEmpty()) {
            view->setWindowTitle(title);
        }

        int index = ui->tabWidget->addTab(view, view->windowTitle());
        ui->tabWidget->setCurrentIndex(index);
    }
}

QPlainTextEdit *DirectedGraphView::getSourceView(const QString &content)
{
    try {

        using namespace Core::PluginManager;
        using namespace Plugins::SourceView;

        PluginManager &pluginManager = PluginManager::instance();

        foreach(QObject *object, pluginManager.allObjects()) {
            ISourceViewFactory *viewFactory = qobject_cast<ISourceViewFactory *>(object);
            if(viewFactory) {
                QPlainTextEdit *view = viewFactory->sourceViewWidget(content);
                view->setParent(this);
                return view;
            }
        }

    } catch(...) { }

    return NULL;
}


} // namespace DirectedGraphView
} // namespace Plugins
