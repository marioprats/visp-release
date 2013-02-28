#############################################################################
#
# $Id: FindDL.cmake 3719 2012-05-10 05:50:23Z fspindle $
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
# Try to find dl library.
# Once run this will define: 
#
# DL_FOUND
# DL_LIBRARIES
#
# Authors:
# Fabien Spindler
#
#############################################################################


FIND_LIBRARY(DL_LIBRARY
    NAMES dl
    PATHS 
    $ENV{DL_HOME}/lib
    /usr/lib
    /usr/local/lib
    )
  
#MESSAGE("DBG DL_LIBRARY=${DL_LIBRARY}")
  
IF(DL_LIBRARY)
    SET(DL_LIBRARIES ${DL_LIBRARY})
    SET(DL_FOUND TRUE)
ELSE()
    SET(DL_FOUND FALSE)
ENDIF()
  
MARK_AS_ADVANCED(
    DL_LIBRARY
    )

