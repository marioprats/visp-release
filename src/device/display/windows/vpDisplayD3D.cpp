/****************************************************************************
 *
 * $Id: vpDisplayD3D.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
 *
 * Description:
 * windows 32 display using D3D
 *
 * Authors:
 * Bruno Renier
 *
 *****************************************************************************/

/*! 
\file vpDisplayD3D.cpp
\brief windows 32 display using D3D
*/ 

#include <visp/vpConfig.h>
#if ( defined(WIN32) & defined(VISP_HAVE_D3D9) ) 

#include <visp/vpDisplayD3D.h>
#include <visp/vpD3DRenderer.h>

/*!
  \brief Basic constructor.
*/
vpDisplayD3D::vpDisplayD3D(): vpDisplayWin32(new vpD3DRenderer()){}

/*!

\brief Constructor : initialize a display to visualize a RGBa image
(32 bits).

\param I : Image to be displayed (note that image has to be initialized).
\param winx, winy : The window is set at position x,y (column index, row index).
\param _title : Window's title.

*/
vpDisplayD3D::vpDisplayD3D(vpImage<vpRGBa> &I,
			   int winx, int winy,
			   const char *_title)
  : vpDisplayWin32(new vpD3DRenderer())
{
  init(I,winx,winy,_title);
}

/*!

\brief Constructor : initialize a display to visualize a grayscale image
(8 bits).

\param I  Image to be displayed (note that image has to be initialized).
\param winx, winy The window is set at position x,y (column index, row index).
\param _title  Window's title.

*/
vpDisplayD3D::vpDisplayD3D(vpImage<unsigned char> &I,
			   int winx, int winy,
			   const char *_title)
  : vpDisplayWin32(new vpD3DRenderer())
{
  init(I,winx,winy,_title);
}

/*!
  \brief Basic destructor.
*/
vpDisplayD3D::~vpDisplayD3D(){}

#endif

