# This file is part of the StackWalker Analysis Tool (SWAT)
# Copyright (C) 2012-2012 Argo Navis Technologies, LLC
# Copyright (C) 2012-2012 University of Wisconsin
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

!isEmpty(PTGF_PRI_INCLUDED): error("PTGF.pri already included")
PTGF_PRI_INCLUDED = 1

##############
# PTGF PATHS #
##############

# Source path for PTGF
isEmpty(PTGF_PATH): PTGF_PATH = $$quote($${PWD})
!exists($${PTGF_PATH}/src/PTGF.pri) {
  warning()
  warning("The PTGF header files could not be found.")
  warning("The Open|SpeedShop plugin requires access to the framework source.")
  error("Pass 'PTGF_PATH=~/src/PTGF' to qmake, where ~/src/PTGF is the base source directory of PTGF.")
}

# Library path for compiled/built PTGF
isEmpty(PTGF_LIBPATH): PTGF_LIBPATH = $$quote($${PWD})

# Possible subdirectories for DLL
PTGF_DIRECTORIES = $${PTGF_LIBPATH}
PTGF_DIRECTORIES += $${PTGF_LIBPATH}/lib
PTGF_DIRECTORIES += $${PTGF_LIBPATH}/debug
PTGF_DIRECTORIES += $${PTGF_LIBPATH}/lib/debug
PTGF_DIRECTORIES += $${PTGF_LIBPATH}/release
PTGF_DIRECTORIES += $${PTGF_LIBPATH}/lib/release

# Find name of library file
CONFIG(debug, debug|release) {
  win32 {
    PTGF_LIBNAME = CoreD0
    PTGF_FILENAME = $${PTGF_LIBNAME}.dll
  } else {
    PTGF_LIBNAME = CoreD
    PTGF_FILENAME = lib$${PTGF_LIBNAME}.so
  }
} else {
  win32 {
    PTGF_LIBNAME = Core0
    PTGF_FILENAME = $${PTGF_LIBNAME}.dll
  } else {
    PTGF_LIBNAME = Core
    PTGF_FILENAME = lib$${PTGF_LIBNAME}.so
  }
}

# Loop through possible directories to find the DLL
for(PTGF_DIRECTORY, PTGF_DIRECTORIES) {
  exists($${PTGF_DIRECTORY}/$${PTGF_FILENAME}) {
    LIBS += -L$$quote($${PTGF_DIRECTORY}) -l$${PTGF_LIBNAME}
    unset(PTGF_DIRECTORIES)
    break()
  }
}

# If we never found an existing library file, notify the user, and error out
!isEmpty(PTGF_DIRECTORIES) {
  for(PTGF_DIRECTORY, PTGF_DIRECTORIES) {
    warning("$${PTGF_DIRECTORY}/$${PTGF_FILENAME} was not found")
  }
  error("Please ensure that you have already built the PTGF Core library.")
}

#################
# INCLUDE PATHS #
#################
INCLUDEPATH += $$quote($${PTGF_PATH}/src/core/lib) $$quote($${PTGF_PATH}/src/plugins)
DEPENDPATH  += $$quote($${PTGF_PATH}/src/core/lib) $$quote($${PTGF_PATH}/src//plugins)
