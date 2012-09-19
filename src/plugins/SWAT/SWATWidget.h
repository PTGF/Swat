/*!
   \file SWATWidget.h
   \author Dane Gardner <dane.gardner@gmail.com>
   \version

   \section LICENSE
   This file is part of the StackWalker Analysis Tool (SWAT)
   Copyright (C) 2012-2012 Argo Navis Technologies, LLC
   Copyright (C) 2012-2012 University of Wisconsin

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

#ifndef SWATWIDGET_H
#define SWATWIDGET_H

#include <QtCore>
#include <QtGui>

#include <PrettyWidgets/TabWidget.h>

#include "SWATLibrary.h"

namespace Plugins {
namespace SWAT {

class IAdapter;

namespace Ui {
    class SWATWidget;
}

class SWAT_EXPORT SWATWidget : public TabWidget
{
    Q_OBJECT

public:
    explicit SWATWidget(QWidget *parent = 0);
    ~SWATWidget();

public slots:
    void attachJob();
    void launchJob();

protected:
    void tabInserted(int index);
    void tabRemoved(int index);
    void checkAdapterProgress(IAdapter *adapter);

    QAction *m_AttachJob;
    QAction *m_LaunchJob;
    QAction *m_LoadFile;
    QAction *m_CloseJob;

    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

    DirectedGraphView *getTraceView(QByteArray content);

protected slots:
    void tabTitleChanged();

    void CurrentAdapterChanged(IAdapter*,IAdapter*);

    void attaching(QUuid);
    void attached(QUuid);
    void launching(QUuid);
    void launched(QUuid);
    void progress(int, QUuid);
    void progressMessage(QString, QUuid);
    void cancelAttach();

    void sampled(QString content, QUuid id);

    void loadTraceFile();
    void loadTraceFromFile(QString filename);
    void loadTraceFromContent(QByteArray content, QString title = QString());

    void closeJob(int index = -1);

private:
    Ui::SWATWidget *ui;
    QString m_StyleSheet;
    QList<QProgressDialog*> m_ProgressDialogs;

};

} // namespace SWAT
} // namespace Plugins

#endif // SWATWIDGET_H
