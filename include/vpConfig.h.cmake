/****************************************************************************
 *
 * $Id: vpConfig.h.cmake 3814 2012-06-26 16:26:16Z fspindle $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2012 by INRIA. All rights reserved.
 * 
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional 
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 * 
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Description:
 * ViSP configuration.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/

#ifndef vpConfig_h
#define vpConfig_h

#if defined _MSC_VER && _MSC_VER >= 1200
  #pragma warning( disable: 4100 4127 4251 4514 4668 4710 4820 )
  #if _MSC_VER >= 1400 // 1400 = MSVC 8 2005
    #pragma warning( disable: 4548 )
  #endif
  #if _MSC_VER > 1500 // 1500 = MSVC 9 2008
    #pragma warning( disable: 4986 )
  #endif

  // 4100 : undocumented ("unreferenced formal parameter")
  // 4127 : conditional expression is constant
  // 4251 : 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2', ie. disable warnings related to inline functions
  // 4514 : 'function' : unreferenced inline function has been removed
  // 4548 : expression before comma has no effect
  // 4668 : 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
  // 4710 : 'function' : function not inlined
  // 4820 : 'bytes' bytes padding added after construct 'member_name'
  // 4986 : undocumented
#endif

// ViSP major version.
#define VISP_VERSION_MAJOR ${VISP_VERSION_MAJOR}

// ViSP minor version.
#define VISP_VERSION_MINOR ${VISP_VERSION_MINOR}

// ViSP patch version.
#define VISP_VERSION_PATCH ${VISP_VERSION_PATCH}

// ViSP version with dots "${VISP_VERSION_MAJOR}.${VISP_VERSION_MINOR}.${VISP_VERSION_PATCH}".
#cmakedefine VISP_VERSION ${VISP_VERSION}

// ViSP version as an integer
#define VP_VERSION_INT(a, b, c) (a<<16 | b<<8 | c)
#define VISP_VERSION_INT VP_VERSION_INT(VISP_VERSION_MAJOR, \
                                        VISP_VERSION_MINOR, \
                                        VISP_VERSION_PATCH)

// ViSP library is either compiled static or shared
// Used to set declspec(import, export) in headers if required under Windows
#cmakedefine VISP_BUILD_SHARED_LIBS

// Defined if deprecated functionalities are requested to build
#cmakedefine VISP_BUILD_DEPRECATED_FUNCTIONS

// Defined if MSVC is the compiler
#cmakedefine VISP_USE_MSVC

// Defined if X11 library available.
#cmakedefine VISP_HAVE_X11

// Defined if XML2 library available.
#cmakedefine VISP_HAVE_XML2

// Defined if pthread library available.
#cmakedefine VISP_HAVE_PTHREAD

// Defined if YARP available.
#cmakedefine VISP_HAVE_YARP

// Defined if OpenCV available.
#cmakedefine VISP_HAVE_OPENCV

// OpenCV version in hexadecimal (for example 2.1.0 gives 0x020100).
#ifdef VISP_HAVE_OPENCV
#  define VISP_HAVE_OPENCV_VERSION ${VISP_HAVE_OPENCV_VERSION}
#endif

// Defined if GTK2 library available
#cmakedefine VISP_HAVE_GTK

// Defined if GDI (Graphics Device Interface) library available
#cmakedefine VISP_HAVE_GDI

// Defined if Direct3D9 library available
#cmakedefine VISP_HAVE_D3D9

// Defined if GSL library available (-lgsl -lgslcblas).
#cmakedefine VISP_HAVE_GSL

// Defined if lapack/blas libraries are available (-lblas -llapack).
#cmakedefine VISP_HAVE_LAPACK

// Defined the path to the basic scenes used by the simulator
#cmakedefine VISP_SCENES_DIR "${VISP_SCENES_DIR}"

// Defined the path to the robot's arms 3D model used by the robot simulators
#cmakedefine VISP_ROBOT_ARMS_DIR "${VISP_ROBOT_ARMS_DIR}"

// Defined if Ogre3d is available.
#cmakedefine VISP_HAVE_OGRE

// Defined if Ogre3d plugins.cfg is available.
#cmakedefine VISP_HAVE_OGRE_PLUGINS_PATH "${VISP_HAVE_OGRE_PLUGINS_PATH}"

// Defined if Ogre3d resources.cfg is available.
#cmakedefine VISP_HAVE_OGRE_RESOURCES_PATH "${VISP_HAVE_OGRE_RESOURCES_PATH}"

// Defined if OIS (Object Oriented Input System) library available.
#cmakedefine VISP_HAVE_OIS

// Defined if Coin3D and one of the GUI (SoXt, SoWin, SoQt + Qt) 
// libraries are available.
#cmakedefine VISP_HAVE_COIN_AND_GUI

// Defined if Coin3D library available.
#cmakedefine VISP_HAVE_COIN

// Defined if OpenGL library available.
#cmakedefine VISP_HAVE_OPENGL

// Defined if Qt library available (either Qt-3 or Qt-4).
#cmakedefine VISP_HAVE_QT

// Defined if SoQt library available.
#cmakedefine VISP_HAVE_SOQT

// Defined if SoWin library available.
#cmakedefine VISP_HAVE_SOWIN

// Defined if SoXt library available.
#cmakedefine VISP_HAVE_SOXT

// Defined if libjpeg library available.
#cmakedefine VISP_HAVE_LIBJPEG

// Defined if libpng library available.
#cmakedefine VISP_HAVE_LIBPNG

// Defined if libfreenect, libusb-1.0 and libpthread libraries available.
#cmakedefine VISP_HAVE_LIBFREENECT_AND_DEPENDENCIES

// Defined if libfreenect library available.
#cmakedefine VISP_HAVE_LIBFREENECT

// Defined if libfreenect library in an old version package for 
// ubuntu 10.04 lucid is available.
// This is a workaround useful to initialise vpKinect depending on the 
// libfreenect version
//#ifdef VISP_HAVE_LIBFREENECT_OLD
//  Freenect::Freenect<vpKinect> freenect;
//  vpKinect & kinect = freenect.createDevice(0);
//#else
//  Freenect::Freenect freenect;
//  vpKinect & kinect = freenect.createDevice<vpKinect>(0);
//#endif
#cmakedefine VISP_HAVE_LIBFREENECT_OLD

// Defined if libusb-1.0 library available.
#cmakedefine VISP_HAVE_LIBUSB_1

// Defined if ffmpeg library available.
#cmakedefine VISP_HAVE_FFMPEG

// Defined if raw1394 and dc1394-1.x libraries available.
#cmakedefine VISP_HAVE_DC1394_1

// Defined if raw1394 and dc1394-2.x libraries available.
#cmakedefine VISP_HAVE_DC1394_2

// Defined if dc1394_camera_enumerate() is available in dc1394-2.x.
// dc1394_camera_enumerate() was introduced after libdc1394-2.0.0-rc7.
#cmakedefine VISP_HAVE_DC1394_2_CAMERA_ENUMERATE

// Defined if dc1394_find_cameras() is available in dc1394-2.x 
// dc1394_find_cameras() is still present until libdc1394-2.0.0-rc7.
// This function was suppress and replace by dc1394_camera_enumerate()
// in more recent releases.
#cmakedefine VISP_HAVE_DC1394_2_FIND_CAMERAS

// Defined if CMU 1394 Digital Camera SDK available.
#cmakedefine VISP_HAVE_CMU1394

// Defined if Video For Linux Two available.
#cmakedefine VISP_HAVE_V4L2

// Defined if DirectShow library is available (only under Windows).
#cmakedefine VISP_HAVE_DIRECTSHOW

// Defined if Irisa's Afma4 robot available.
#cmakedefine VISP_HAVE_AFMA4

// Defined if Irisa's Afma6 robot available.
#cmakedefine VISP_HAVE_AFMA6

// Defined if Biclops pan-tilt head available.
#cmakedefine VISP_HAVE_BICLOPS
#cmakedefine VISP_HAVE_BICLOPS_AND_GET_HOMED_STATE_FUNCTION

// Defined if Irisa's Ptu-46 pan-tilt head available.
#cmakedefine VISP_HAVE_PTU46

// Defined if Irisa's Viper S850 robot available.
#cmakedefine VISP_HAVE_VIPER850

// Defined if Irisa's Cycab car-like mobile robot is found. 
// If found, either VISP_HAVE_CYCABTK_OLD nor VISP_HAVE_CYCABTK
// is defined.  
#cmakedefine VISP_HAVE_CYCAB

// Defined if the old cycabtk library is found. CycabTk is used to
// communicate with Irisa's Cycab car-like robot (obsolete).  
#cmakedefine VISP_HAVE_CYCABTK_OLD

// Defined if the last cycabtk library is found. CycabTk is used to
// communicate with Irisa's Cycab car-like robot (to use).  
#cmakedefine VISP_HAVE_CYCABTK

// Defined if the Aria library and (pthread, rt, dl libraries under Unix) is found. 
// These libraries are used to control Pioneer mobile robots.  
#cmakedefine VISP_HAVE_PIONEER

// Defined if linux/parport.h is available for parallel port usage.
#cmakedefine VISP_HAVE_PARPORT

// Defined if Inria's NAS server hosting /udd/ is available
// Used for the moment in vpAfma6 class to check if config files are
// available in /udd/fspindle/robot/Afma6/current/include/
#cmakedefine VISP_HAVE_ACCESS_TO_NAS

// Defined if Doxygen documentation tool is found
#cmakedefine VISP_HAVE_DOXYGEN

// Defined if dot documentation tool used by Doxygen is found
#cmakedefine VISP_HAVE_DOT

// Defined if we want to compute interaction matrices by combining 
// other interaction matrices
#cmakedefine VISP_MOMENTS_COMBINE_MATRICES

//Defined if we want to use openmp
#cmakedefine VISP_HAVE_OPENMP

//Defined if we want to use c++ 11
#cmakedefine VISP_HAVE_CPP11_COMPATIBILITY

// Under Windows, for shared libraries (DLL) we need to define export on
// compilation or import on use (like a third party project).
// We exploit here the fact that cmake auto set xxx_EXPORTS (with S) on 
// compilation.
#if defined (WIN32) && defined(VISP_BUILD_SHARED_LIBS) 
#  ifdef visp_2_EXPORTS 
#    define VISP_EXPORT __declspec(dllexport)
#  else  
#    define VISP_EXPORT __declspec(dllimport)
#  endif 
#else
#  define VISP_EXPORT
#endif

// Add the material to produce a warning when deprecated functions are used
#ifndef vp_deprecated
#  if defined (UNIX)
#    define vp_deprecated __attribute__((deprecated))
#  else
#    define vp_deprecated __declspec(deprecated)
#  endif
#endif

#endif


