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

#ifndef JOBCONTROLDIALOG_H
#define JOBCONTROLDIALOG_H

#include <QDialog>

namespace Ui {
class JobControlDialog;
}

class JobControlDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit JobControlDialog(QWidget *parent = 0);
    ~JobControlDialog();
    
    enum Types { Type_Attach, Type_Launch };
    void setType(Types);

private:
    Ui::JobControlDialog *ui;
};

#endif // JOBCONTROLDIALOG_H
