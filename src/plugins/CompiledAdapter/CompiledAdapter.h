/*!
   \file CompiledAdapter.h
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

#ifndef COMPILEDADAPTER_H
#define COMPILEDADAPTER_H

#include <QThread>
#include <QObject>
#include <SWAT/ConnectionManager/IAdapter.h>

namespace Plugins {
namespace CompiledAdapter {

//// Only way to get a calling thread to sleep using Qt4.
//class Thread : public QThread
//{
//public:
//    static void sleep(unsigned long msecs) { QThread::msleep(msecs); }
//};

class CompiledAdapter : public Plugins::SWAT::IAdapter
{
    Q_OBJECT
    Q_INTERFACES(Plugins::SWAT::IAdapter)

public:
    explicit CompiledAdapter(QObject *parent = 0);

    void attach();
    void reAttach();
    void detach();
    void pause();
    void resume();
    void sample();
    void sampleMultiple();

};

} // namespace CompiledAdapter
} // namespace Plugins

#endif // COMPILEDADAPTER_H
