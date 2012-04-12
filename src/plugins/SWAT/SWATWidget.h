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

#include <QTabWidget>
#include <QAction>
#include <MainWindow/MainWindow.h>
#include "SWATLibrary.h"

namespace Plugins {
namespace SWAT {

namespace Ui {
    class SWATWidget;
}

class SWAT_EXPORT SWATWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit SWATWidget(QWidget *parent = 0);
    ~SWATWidget();

public slots:

protected:
    void tabInserted(int index);
    void tabRemoved(int index);

protected slots:
    void tabTitleChanged();

private:
    Ui::SWATWidget *ui;
    QString m_StyleSheet;

};

} // namespace SWAT
} // namespace Plugins

#endif // SWATWIDGET_H
