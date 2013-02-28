/****************************************************************************
 *
 * $Id: vpDisplayX.h 3590 2012-03-05 09:48:31Z ayol $
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
 * Image display.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/

#ifndef vpDisplayX_h
#define vpDisplayX_h

#include <visp/vpConfig.h>
#ifdef VISP_HAVE_X11

//namespace X11name
//{
#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <X11/Xatom.h>
//#include <X11/cursorfont.h>
//} ;

//using namespace X11name ;

#include <visp/vpImage.h>
#include <visp/vpDisplay.h>
#include <visp/vpRect.h>



/*!
  \file vpDisplayX.h
  \brief Define the X11 console to display images.
*/


/*!
  \class vpDisplayX

  \ingroup ImageGUI

  \brief Define the X11 console to display images.

  \note Ready to use with a Unix System (tested for Linux and SunOS)

  This class define the X11 console to display  images
  It also define method to display some geometric feature (point, line, circle)
  in the image.

  The example below shows how to display an image with this video device.
  \code
#include <visp/vpConfig.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplayX.h>
#include <visp/vpImagePoint.h>

int main() 
{
#if defined(VISP_HAVE_X11)
  vpImage<unsigned char> I; // Grey level image

  // Read an image in PGM P5 format
  vpImageIo::readPGM(I, "/local/soft/ViSP/ViSP-images/Klimt/Klimt.pgm");

  vpDisplayX d; 

  // Initialize the display with the image I. Display and image are
  // now link together.
  d.init(I);

  // Specify the window location
  vpDisplay::setWindowPosition(I, 400, 100);

  // Set the display window title
  vpDisplay::setTitle(I, "My X11 display");

  // Set the display background with image I content
  vpDisplay::display(I);

  // Draw a red rectangle in the display overlay (foreground)
  vpDisplay::displayRectangle(I, 10, 10, 100, 20, vpColor::red, true);

  // Draw a red rectangle in the display overlay (foreground)
  vpImagePoint topLeftCorner;
  topLeftCorner.set_i(50);
  topLeftCorner.set_j(10);
  vpDisplay::displayRectangle(I, topLeftCorner, 100, 20, vpColor::green, true);

  // Flush the foreground and background display
  vpDisplay::flush(I);

  // Get non blocking keyboard events
  std::cout << "Check keyboard events..." << std::endl; 
  char key[10];
  bool ret;
  for (int i=0; i< 200; i++) {
    bool ret = vpDisplay::getKeyboardEvent(I, key, false);
    if (ret) 
      std::cout << "keyboard event: key: " << "\"" << key << "\"" << std::endl;
    vpTime::wait(40);
  }

  // Get a blocking keyboard event
  std::cout << "Wait for a keyboard event..." << std::endl; 
  ret = vpDisplay::getKeyboardEvent(I, key, true);
  std::cout << "keyboard event: " << ret << std::endl;
  if (ret) 
    std::cout << "key: " << "\"" << key << "\"" << std::endl;

  // Wait for a click in the display window
  std::cout << "Wait for a button click..." << std::endl;
  vpDisplay::getClick(I);
#endif
}
  \endcode

*/

class VISP_EXPORT vpDisplayX: public vpDisplay
{
private:
  int num_Xdisplay ;
  Display 	*display ;
  Window   	window ;
  XImage   	*Ximage ;
  Colormap	lut ;
  GC		context ;
  int      	screen ;
  int		planes;
  XEvent	event;
  Pixmap	pixmap;
  unsigned long	*x_color; // Array of predefined colors
  unsigned int screen_depth  ;
  unsigned short  colortable[256];
  XColor        xcolor;
  XGCValues     values;
  int size ;
  bool ximage_data_init;
  
public:
  vpDisplayX() ;
  vpDisplayX(int winx, int winy, const char *title=NULL) ;
  vpDisplayX(vpImage<unsigned char> &I, int winx=-1, int winy=-1,
	     const char *title=NULL) ;
  vpDisplayX(vpImage<vpRGBa> &I, int winx=-1, int winy=-1,
	     const char *title=NULL) ;


  virtual ~vpDisplayX() ;

  void init(vpImage<unsigned char> &I,
	    int winx=-1, int winy=-1,
	    const char *title=NULL)  ;
  void init(vpImage<vpRGBa> &I,
	   int winx=-1, int winy=-1,
	   const char *title=NULL)  ;

  void init(unsigned int width, unsigned int height,
	    int winx=-1, int winy=-1 ,
	    const char *title=NULL) ;
  
  unsigned int getScreenDepth();
  void getScreenSize(unsigned int &width, unsigned int &height);
  void getImage(vpImage<vpRGBa> &I) ;

protected:

  void setFont( const char *font );
  void setTitle(const char *title) ;
  void setWindowPosition(int winx, int winy);

  void clearDisplay(const vpColor &color=vpColor::white) ;

  void closeDisplay() ;

  void displayArrow(const vpImagePoint &ip1, 
		    const vpImagePoint &ip2,
		    const vpColor &color=vpColor::white,
		    unsigned int w=4,unsigned int h=2,
		    unsigned int thickness=1) ;

  void displayCharString(const vpImagePoint &ip, const char *text,
			 const vpColor &color=vpColor::green) ;

  void displayCircle(const vpImagePoint &center, unsigned int radius,
		     const vpColor &color,
		     bool fill = false,
		     unsigned int thickness=1);
  void displayCross(const vpImagePoint &ip, unsigned int size,
		    const vpColor &color, unsigned int thickness=1) ;
  void displayDotLine(const vpImagePoint &ip1, 
		      const vpImagePoint &ip2,
		      const vpColor &color, unsigned int thickness=1) ;

  void displayImage(const vpImage<vpRGBa> &I) ;
  void displayImage(const vpImage<unsigned char> &I) ;
  void displayImage(const unsigned char *I) ;
  
  void displayImageROI(const vpImage<unsigned char> &I,const vpImagePoint &iP, const unsigned int width, const unsigned int height);
  void displayImageROI(const vpImage<vpRGBa> &I,const vpImagePoint &iP, const unsigned int width, const unsigned int height);

  void displayLine(const vpImagePoint &ip1, 
		   const vpImagePoint &ip2,
		   const vpColor &color, unsigned int thickness=1) ;
  void displayPoint(const vpImagePoint &ip, const vpColor &color) ;

  void displayRectangle(const vpImagePoint &topLeft,
			unsigned int width, unsigned int height,
			const vpColor &color, bool fill = false,
			unsigned int thickness=1) ;
  void displayRectangle(const vpImagePoint &topLeft,
			const vpImagePoint &bottomRight,
			const vpColor &color, bool fill = false,
			unsigned int thickness=1) ;
  void displayRectangle(const vpRect &rectangle,
			const vpColor &color, bool fill = false,
			unsigned int thickness=1) ;

  void flushDisplay() ;
  void flushDisplayROI(const vpImagePoint &iP, const unsigned int width, const unsigned int height);

  bool getClick(bool blocking=true) ;
  bool getClick(vpImagePoint &ip, bool blocking=true);
  bool getClick(vpImagePoint &ip,
		vpMouseButton::vpMouseButtonType& button,
		bool blocking=true) ;
  bool getClickUp(vpImagePoint &ip,
		  vpMouseButton::vpMouseButtonType& button,
		  bool blocking=true);

  bool getKeyboardEvent(bool blocking=true);
  bool getKeyboardEvent(char *string, bool blocking=true);
  bool getPointerMotionEvent (vpImagePoint &ip);
  bool getPointerPosition (vpImagePoint &ip);

  inline  unsigned int getWidth() const  { return width ; }
  inline  unsigned int getHeight() const { return height ; }
} ; 


#endif
#endif
