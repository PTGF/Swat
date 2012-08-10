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

#ifndef DIRECTEDGRAPHVIEW_H
#define DIRECTEDGRAPHVIEW_H

#include <QtCore>
#include <QtGui>

#ifdef QT_DEBUG
#include <QtDebug>
#endif

class QGraphVizView;

namespace Plugins {
namespace DirectedGraphView {

class DirectedGraphScene;

namespace Ui {
class DirectedGraphView;
}

class DirectedGraphView : public QWidget
{
    Q_OBJECT

public:
    explicit DirectedGraphView(const QByteArray &content, QWidget * parent = 0);
    ~DirectedGraphView();

    QGraphVizView *view();

protected:
    void openSourceFile(QString filename, int lineNumber = 0);
    void loadSourceFromFile(QString filename);
    void loadSourceFromContent(QByteArray content, QString title);
    QPlainTextEdit *getSourceView(const QString &content);

protected slots:
    void on_txtFilter_textChanged(const QString &);
    void selectionChanged();

private:
    Ui::DirectedGraphView *ui;
    DirectedGraphScene *m_Scene;
    QGraphVizView *m_View;

    friend class DirectedGraphNodeDialog;
};

} // namespace DirectedGraphView
} // namespace Plugins

#endif // DIRECTEDGRAPHVIEW_H
