/*!
   \file 
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

#ifndef SWATSETTINGPAGE_H
#define SWATSETTINGPAGE_H

#include <QWidget>
#include <SettingManager/ISettingPage.h>
#include <SettingManager/SettingManager.h>

namespace Plugins {
namespace SWAT {

namespace Ui {
    class SettingPage;
}

class SettingPage : public Core::SettingManager::ISettingPage
{
    Q_OBJECT
    Q_INTERFACES(Core::SettingManager::ISettingPage)

public:
    SettingPage(QWidget *parent = 0);
    ~SettingPage();

public slots:
    void apply();
    void reset();

protected:
    void initialize();

private:
    Ui::SettingPage *ui;
};


}}
#endif // SWATSETTINGPAGE_H