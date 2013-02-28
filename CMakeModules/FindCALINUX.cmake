#############################################################################
#
# $Id: FindCALINUX.cmake 3530 2012-01-03 10:52:12Z fspindle $
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
# Try to find Cerebellum CA linux library material 
#
# CALINUX_FOUND
# CALINUX_INCLUDE_DIRS
# CALINUX_LIBRARIES
#
# Authors:
# Fabien Spindler
#
#############################################################################

IF(NOT UNIX)
  # MESSAGE("FindCALINUX.cmake: only available for Unix.")
  SET(CALINUX_FOUND FALSE)
ELSE(NOT UNIX)
  
  FIND_PATH(CALINUX_INCLUDE_DIR calinux.h
    $ENV{CALINUX_HOME}/include
    /local/soft/Cerebellum/CALinux/current/include
    )
  #MESSAGE("DBG CALINUX_INCLUDE_DIR=${CALINUX_INCLUDE_DIR}")  
  
  FIND_LIBRARY(CALINUX_LIBRARY
    NAMES calinux
    PATHS 
    $ENV{CALINUX_HOME}/lib
    /local/soft/Cerebellum/CALinux/current/lib
    )

  #MESSAGE("DBG CALINUX_LIBRARY=${CALINUX_LIBRARY}")
  
  ## --------------------------------
  
  IF(CALINUX_LIBRARY)
    SET(CALINUX_LIBRARIES ${CALINUX_LIBRARY})
  ELSE(CALINUX_LIBRARY)
#     MESSAGE(SEND_ERROR "Calinux library not found.")
  ENDIF(CALINUX_LIBRARY)
  
  IF(NOT CALINUX_INCLUDE_DIR)
#     MESSAGE(SEND_ERROR "Calinux include dir not found.")
  ENDIF(NOT CALINUX_INCLUDE_DIR)
  
  IF(CALINUX_LIBRARIES AND CALINUX_INCLUDE_DIR)
    SET(CALINUX_INCLUDE_DIRS ${CALINUX_INCLUDE_DIR})
    SET(CALINUX_FOUND TRUE)
  ELSE(CALINUX_LIBRARIES AND CALINUX_INCLUDE_DIR)
    SET(CALINUX_FOUND FALSE)
  ENDIF(CALINUX_LIBRARIES AND CALINUX_INCLUDE_DIR)
  
  MARK_AS_ADVANCED(
    CALINUX_INCLUDE_DIR
    CALINUX_LIBRARIES
    CALINUX_LIBRARY
    )
ENDIF(NOT UNIX)
