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

#ifndef IADAPTER_H
#define IADAPTER_H

#include <QObject>
#include <QMetaType>

namespace Plugins {
namespace SWAT {

class IAdapter : public QObject
{
    Q_OBJECT

public:
    explicit IAdapter(QObject *parent = 0);

    virtual void attach() = 0;
    virtual void reAttach() = 0;
    virtual void detach() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void sample() = 0;
    virtual void sampleMultiple() = 0;

};

} // namespace SWAT
} // namespace Plugins

//! We do this so that we can use the pointer in a QVariant
Q_DECLARE_METATYPE(Plugins::SWAT::IAdapter *);

Q_DECLARE_INTERFACE(Plugins::SWAT::IAdapter, "org.krellinst.swat.IAdapter/0.1");

#endif // IADAPTER_H
