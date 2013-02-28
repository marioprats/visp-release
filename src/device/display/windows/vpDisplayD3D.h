/****************************************************************************
 *
 * $Id: vpDisplayD3D.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Windows 32 display using D3D
 *
 * Authors:
 * Bruno Renier
 * Fabien Spindler
 *
 *****************************************************************************/

#include <visp/vpConfig.h>
#if ( defined(VISP_HAVE_D3D9) ) 

#ifndef VPDISPLAYD3D_HH
#define VPDISPLAYD3D_HH


#include <visp/vpDisplayWin32.h>

/*!
  \class vpDisplayD3D

  \ingroup ImageGUI

  \brief Display for windows using Direct3D.

  Direct3D is part of the DirectX API available under Windows
  operating systems.

  \warning Requires DirectX9 SDK to compile and DirectX9 DLLs to run.

  The example below shows how to display an image with this video device.
  \code
#include <visp/vpConfig.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplayD3D.h>

int main()
{
#if defined(VISP_HAVE_D3D9)
  vpImage<unsigned char> I; // Grey level image

  // Read an image in PGM P5 format
  vpImageIo::readPGM(I, "C:/temp/ViSP-images/Klimt/Klimt.pgm");

  vpDisplayD3D d;

  // Initialize the display with the image I. Display and image are
  // now link together.
  d.init(I);

  // Specify the window location
  vpDisplay::setWindowPosition(I, 400, 100);

  // Set the display window title
  vpDisplay::setTitle(I, "My Direct 3D display");

  // Set the display background with image I content
  vpDisplay::display(I);

  // Draw a red rectangle in the display overlay (foreground)
  vpImagePoint topLeftCorner;
  topLeftCorner.set_i(10);
  topLeftCorner.set_j(20);
  vpDisplay::displayRectangle(I, topLeftCorner, 100, 20, vpColor::red, true);

  // Flush the foreground and background display
  vpDisplay::flush(I);

  // Wait for a click in the display window
  vpDisplay::getClick(I);
#endif
}
  \endcode
*/
class VISP_EXPORT vpDisplayD3D : public vpDisplayWin32
{
public:
  vpDisplayD3D();
  
  vpDisplayD3D(vpImage<vpRGBa> &I,
		    int winx=-1, int winy=-1,
		    const char *_title=NULL);
  
  vpDisplayD3D(vpImage<unsigned char> &I,
		    int winx=-1, int winy=-1,
		    const char *_title=NULL);
  
  virtual ~vpDisplayD3D();
  
};
#endif
#endif
