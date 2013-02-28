#############################################################################
#
# $Id: CTestConfig.cmake,v 1.9 2008-12-11 13:19:44 fspindle Exp $
#
# This file is part of the ViSP software.
# Copyright (C) 2005 - 2012 by INRIA. All rights reserved.
# 
# This software is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# ("GPL") version 2 as published by the Free Software Foundation.
# See the file LICENSE.txt at the root directory of this source
# distribution for additional information about the GNU GPL.
#
# For using ViSP with software that can not be combined with the GNU
# GPL, please contact INRIA about acquiring a ViSP Professional 
# Edition License.
#
# See http://www.irisa.fr/lagadic/visp/visp.html for more information.
# 
# This software was developed at:
# INRIA Rennes - Bretagne Atlantique
# Campus Universitaire de Beaulieu
# 35042 Rennes Cedex
# France
# http://www.irisa.fr/lagadic
#
# If you have questions regarding the use of this file, please contact
# INRIA at visp@inria.fr
# 
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# Description:
# CDash configuration.
#
# Authors:
# Fabien Spindler
#
#############################################################################

set(CTEST_PROJECT_NAME "ViSP")
set(CTEST_NIGHTLY_START_TIME "00:00:00 GMT")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "cdash.inria.fr")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=ViSP")
set(CTEST_DROP_SITE_CDASH TRUE)

#--------------------------------------------------------------------
# BUILNAME variable construction
# This variable will be used to set the build name which will appear 
# on the ViSP dashboard http://cdash.irisa.fr/CDash/
#--------------------------------------------------------------------
# Start with the short system name, e.g. "Linux", "FreeBSD" or "Windows"
IF(BUILDNAME)
  SET(BUILDNAME "${BUILDNAME}-${CMAKE_SYSTEM_NAME}")
ELSE(BUILDNAME)
  # To suppress the first space if BUILDNAME is not set
  SET(BUILDNAME "${CMAKE_SYSTEM_NAME}")
ENDIF(BUILDNAME)

# Add the compiler name, e.g. "g++, msvc7..."
IF(MSVC70)
  SET(BUILDNAME "${BUILDNAME}-msvc70")
ELSEIF(MSVC71)
  SET(BUILDNAME "${BUILDNAME}-msvc71")
ELSEIF(MSVC80)
  SET(BUILDNAME "${BUILDNAME}-msvc80")
ELSEIF(MSVC90)
  SET(BUILDNAME "${BUILDNAME}-msvc90")
ELSEIF(MSVC10)
  SET(BUILDNAME "${BUILDNAME}-msvc10")
ELSEIF(BORLAND)
  SET(BUILDNAME "${BUILDNAME}-borland")
ELSEIF(MINGW)
  SET(BUILDNAME "${BUILDNAME}-mingw")
ELSE(MSVC70)
  # g++
  SET(BUILDNAME "${BUILDNAME}-${CMAKE_BASE_NAME}")
ENDIF(MSVC70)

# Find out the version of gcc being used.
IF(CMAKE_COMPILER_IS_GNUCC)
  EXEC_PROGRAM(${CMAKE_CXX_COMPILER}
    ARGS -dumpversion
    OUTPUT_VARIABLE COMPILER_VERSION
  )
  #MESSAGE("COMPILER_VERSION 1: ${COMPILER_VERSION}")
  STRING(REGEX REPLACE ".* ([0-9])\\.([0-9])\\.[0-9].*" "\\1\\2" 
    COMPILER_VERSION ${COMPILER_VERSION})
  #MESSAGE("COMPILER_VERSION 2: ${COMPILER_VERSION}")

  SET(BUILDNAME "${BUILDNAME}${COMPILER_VERSION}")
  
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Add the type of library generation, e.g. "Dynamic or Static"
IF(BUILD_SHARED_LIBS)
  SET(BUILDNAME "${BUILDNAME}-Dynamic")
ELSE(BUILD_SHARED_LIBS)
  SET(BUILDNAME "${BUILDNAME}-Static")
ENDIF(BUILD_SHARED_LIBS)

# Add the build type, e.g. "Debug, Release..."
IF(CMAKE_BUILD_TYPE)
  SET(BUILDNAME "${BUILDNAME}-${CMAKE_BUILD_TYPE}")
ENDIF(CMAKE_BUILD_TYPE)

#---- Robots ----
# Add specific Afma4 robots
IF(VISP_HAVE_AFMA4)
  SET(BUILDNAME "${BUILDNAME}-Afma4")
ENDIF(VISP_HAVE_AFMA4)
 
# Add specific Afma6 robots
IF(VISP_HAVE_AFMA6)
  SET(BUILDNAME "${BUILDNAME}-Afma6")
ENDIF(VISP_HAVE_AFMA6)

# Add specific Ptu46 robots
IF(VISP_HAVE_PTU46)
  SET(BUILDNAME "${BUILDNAME}-Ptu46")
ENDIF(VISP_HAVE_PTU46)

# Add specific Biclops robots
IF(VISP_HAVE_BICLOPS)
  SET(BUILDNAME "${BUILDNAME}-Biclops")
ENDIF(VISP_HAVE_BICLOPS)

#---- Framegrabers ----
# Firewire dc1394-2.x 
IF(VISP_HAVE_DC1394_2)
  SET(BUILDNAME "${BUILDNAME}-dc1394.2")
ENDIF(VISP_HAVE_DC1394_2)
# Firewire dc1394-1.x 
IF(VISP_HAVE_DC1394_1)
  SET(BUILDNAME "${BUILDNAME}-dc1394.1")
ENDIF(VISP_HAVE_DC1394_1)
# Video 4 linux 2 (V4L2)
IF(VISP_HAVE_V4L2)
  SET(BUILDNAME "${BUILDNAME}-v4l2")
ENDIF(VISP_HAVE_V4L2)
# Directshow
IF(VISP_HAVE_DIRECTSHOW)
  SET(BUILDNAME "${BUILDNAME}-Directshow")
ENDIF(VISP_HAVE_DIRECTSHOW)
IF(VISP_HAVE_CMU1394)
  SET(BUILDNAME "${BUILDNAME}-CMU1394")
ENDIF(VISP_HAVE_CMU1394)
IF(VISP_HAVE_LIBFREENECT)
  SET(BUILDNAME "${BUILDNAME}-freenect")
ENDIF()
IF(VISP_HAVE_LIBUSB_1)
  SET(BUILDNAME "${BUILDNAME}-usb1")
ENDIF()


#---- Video-devices ----
# X11
IF(VISP_HAVE_X11)
  SET(BUILDNAME "${BUILDNAME}-X11")
ENDIF(VISP_HAVE_X11)
# GTK
IF(VISP_HAVE_GTK)
  SET(BUILDNAME "${BUILDNAME}-gtk")
ENDIF(VISP_HAVE_GTK)
# GDI (Windows Graphics Device Interface)
IF(VISP_HAVE_GDI)
  SET(BUILDNAME "${BUILDNAME}-gdi")
ENDIF(VISP_HAVE_GDI)
# D3D (Direct3D9)
IF(VISP_HAVE_D3D9)
  SET(BUILDNAME "${BUILDNAME}-Direct3D")
ENDIF(VISP_HAVE_D3D9)
# OpenCV
IF(VISP_HAVE_OPENCV)
  SET(BUILDNAME "${BUILDNAME}-OpenCV")
ENDIF(VISP_HAVE_OPENCV)

#---- Mathematics ----
# Lapack (Linear Algebra PACKage)
IF(VISP_HAVE_LAPACK)
  SET(BUILDNAME "${BUILDNAME}-lapack")
ENDIF()
# GSL (Gnu Scientific Library)
IF(VISP_HAVE_GSL)
  SET(BUILDNAME "${BUILDNAME}-gsl")
ENDIF(VISP_HAVE_GSL)

#---- Simulator ----
# Ogre
IF(VISP_HAVE_OGRE)
  SET(BUILDNAME "${BUILDNAME}-Ogre")
ENDIF()
# Coin
IF(VISP_HAVE_COIN)
  SET(BUILDNAME "${BUILDNAME}-Coin")
ENDIF(VISP_HAVE_COIN)
# SoQt
IF(VISP_HAVE_SOQT)
  SET(BUILDNAME "${BUILDNAME}-SoQt")
ENDIF(VISP_HAVE_SOQT)
# Qt
IF(VISP_HAVE_QT)
  SET(BUILDNAME "${BUILDNAME}-Qt${DESIRED_QT_VERSION}")
ENDIF(VISP_HAVE_QT)
# SoWin
IF(VISP_HAVE_SOWIN)
  SET(BUILDNAME "${BUILDNAME}-SoWin")
ENDIF(VISP_HAVE_SOWIN)
# SoXt
IF(VISP_HAVE_SOXT)
  SET(BUILDNAME "${BUILDNAME}-SoXt")
ENDIF(VISP_HAVE_SOXT)

#---- Images ----
IF(VISP_HAVE_FFMPEG)
  SET(BUILDNAME "${BUILDNAME}-ffmpeg")
ENDIF(VISP_HAVE_FFMPEG)
IF(VISP_HAVE_LIBJPEG)
  SET(BUILDNAME "${BUILDNAME}-libjpeg")
ENDIF(VISP_HAVE_LIBJPEG)
IF(VISP_HAVE_LIBPNG)
  SET(BUILDNAME "${BUILDNAME}-libpng")
ENDIF(VISP_HAVE_LIBPNG)
IF(VISP_HAVE_ZLIB)
  SET(BUILDNAME "${BUILDNAME}-zlib")
ENDIF()

#---- Misc ----
# XML
IF(VISP_HAVE_XML2)
  SET(BUILDNAME "${BUILDNAME}-xml")
ENDIF(VISP_HAVE_XML2)
# PThread
IF(VISP_HAVE_PTHREAD)
  SET(BUILDNAME "${BUILDNAME}-pthread")
ENDIF(VISP_HAVE_PTHREAD)
# OpenMP
IF(VISP_HAVE_OPENMP)
  SET(BUILDNAME "${BUILDNAME}-OpenMP")
ENDIF()

#MESSAGE("BUILDNAME=${BUILDNAME}")
