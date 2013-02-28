/****************************************************************************
 *
 * $Id: vpDisplayOpenCV.h 3797 2012-06-21 07:44:05Z fspindle $
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
 * Christophe Collewet
 * Eric Marchand
 *
 *****************************************************************************/

#ifndef vpDisplayOpenCV_h
#define vpDisplayOpenCV_h

#include <visp/vpConfig.h>
#if ( defined(VISP_HAVE_OPENCV) )

#include <visp/vpImage.h>
#include <visp/vpImageConvert.h>
#include <visp/vpDisplay.h>

#if VISP_HAVE_OPENCV_VERSION >= 0x020101
#  include <opencv2/core/core.hpp>
#  include <opencv2/highgui/highgui.hpp>
#else
#  include <cv.h>
#  include <highgui.h>
#  include <cxcore.h>
#endif

/*!
  \file vpDisplayOpenCV.h
  \brief Define the OpenCV console to display images.
*/


/*!

  \class vpDisplayOpenCV

  \ingroup ImageGUI

  \brief The vpDisplayOpenCV allows to display image using the opencv library.

  The example below shows how to display an image with this video device.
  \code
#include <visp/vpConfig.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vpImagePoint.h>

int main() 
{
#if defined(VISP_HAVE_OPENCV)
  vpImage<unsigned char> I; // Grey level image

  // Read an image in PGM P5 format
  vpImageIo::readPGM(I, "/local/soft/ViSP/ViSP-images/Klimt/Klimt.pgm");

  vpDisplayOpenCV d; 

  // Initialize the display with the image I. Display and image are
  // now link together.
  d.init(I);

  // Specify the window location
  vpDisplay::setWindowPosition(I, 400, 100);

  // Set the display window title
  vpDisplay::setTitle(I, "My OpenCV display");

  // Set the display background with image I content
  vpDisplay::display(I);

  // Draw a red rectangle in the display overlay (foreground)
  vpDisplay::displayRectangle(I, 10, 10, 100, 20, vpColor::red, true);

  // Draw a red rectangle in the display overlay (foreground)
  vpImagePoint topLeftCorner;
  topLeftCorner.set_i(10);
  topLeftCorner.set_j(50);
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

class VISP_EXPORT vpDisplayOpenCV: public vpDisplay
{
private:
  //! true if OpenCV display is ready to use
  IplImage* background;
  int window;
  int windowXPosition ; int  windowYPosition ;
  static int count; 
  CvScalar *col ;
  CvScalar cvcolor;
  CvFont *font;
  int fontHeight;  
  int ncol, nrow ;
  int x_move;
  int y_move;
  bool move;
  int x_lbuttondown ;
  int y_lbuttondown ;
  bool lbuttondown;
  int x_mbuttondown ;
  int y_mbuttondown ;
  bool mbuttondown;
  int x_rbuttondown ;
  int y_rbuttondown ;
  bool rbuttondown;
  int x_lbuttonup ;
  int y_lbuttonup ;
  bool lbuttonup;
  int x_mbuttonup ;
  int y_mbuttonup ;
  bool mbuttonup;
  int x_rbuttonup ;
  int y_rbuttonup ;
  bool rbuttonup;
  
public:
  vpDisplayOpenCV() ;
  vpDisplayOpenCV(int winx, int winy, const char *title=NULL) ;
  vpDisplayOpenCV(vpImage<unsigned char> &I, int winx=-1, int winy=-1,
	       const char *title=NULL) ;
  vpDisplayOpenCV(vpImage<vpRGBa> &I, int winx=-1, int winy=-1,
	       const char *title=NULL) ;

  virtual ~vpDisplayOpenCV() ;

  void init(vpImage<unsigned char> &I,
	    int winx=-1, int winy=-1,
	    const char *title=NULL)  ;
  void init(vpImage<vpRGBa> &I,
	    int winx=-1, int winy=-1,
	    const char *title=NULL)  ;

  void init(unsigned int width, unsigned int height,
	    int winx=-1, int winy=-1 ,
	    const char *title=NULL) ;
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
  bool getClick(vpImagePoint &ip, bool blocking=true) ;
  bool getClick(vpImagePoint &ip,
		 vpMouseButton::vpMouseButtonType& button,
		bool blocking=true) ;
  bool getClickUp(vpImagePoint &ip,
		  vpMouseButton::vpMouseButtonType& button,
		  bool blocking=true) ;

  inline  unsigned int getWidth() const  { return width ; }
  inline  unsigned int getHeight() const { return height ; }

  bool getKeyboardEvent(bool blocking=true);
  bool getKeyboardEvent(char *string, bool blocking=true);
  bool getPointerMotionEvent (vpImagePoint &ip);
  bool getPointerPosition (vpImagePoint &ip);

  static void on_mouse( int event, int x, int y, int flags, void* param );
} ;

#endif
#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
