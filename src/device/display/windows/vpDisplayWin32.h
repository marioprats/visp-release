/****************************************************************************
 *
 * $Id: vpDisplayWin32.h 3587 2012-03-02 16:26:32Z fspindle $
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
 * Windows 32 display base class
 *
 * Authors:
 * Bruno Renier
 *
 *****************************************************************************/

#include <visp/vpConfig.h>

#if ( defined(WIN32) )

#ifndef vpDisplayWin32_hh
#define vpDisplayWin32_hh

#include <visp/vpImage.h>
#include <visp/vpDisplay.h>
#include <windows.h>
#include <visp/vpWin32Window.h>
#include <visp/vpWin32Renderer.h>
#include <visp/vpRect.h>
#include <visp/vpImagePoint.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/*!
  Used to pass parameters to the window's thread.
*/
struct threadParam
{
  //! Pointer to the display associated with the window.
  vpDisplayWin32 * vpDisp;

  //! X position of the window.
  int x;

  //! Y position of the window.
  int y;

  //! Width of the window's client area.
  unsigned int w;

  //! Height of the window's client area.
  unsigned int h;

  //! Title of the window.
  char * title;
};
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*!
  \class vpDisplayWin32

  \brief Base abstract class for Windows 32 displays.
  Implements the window creation in a separate thread
  and the associated event handling functions for
  Windows 32 displays.
  Uses calls to a renderer to do some display.
  (i.e. all display methods are implemented in the renderer)

  \author Bruno Renier
*/
class VISP_EXPORT vpDisplayWin32 : public vpDisplay
{
 protected:
  //! Maximum delay for window initialization
  static const int MAX_INIT_DELAY;

  //! Handle of the window's thread.
  HANDLE hThread;

  //! Id of the window's thread.
  DWORD threadId;

  //! Initialization status.
  bool iStatus;

  //! The window.
  vpWin32Window window;

  //!
  RECT roi;

  //! Function used to launch the window in a thread.
  friend void vpCreateWindow(threadParam * param);

 public:

  vpDisplayWin32(vpWin32Renderer * rend = NULL);

  vpDisplayWin32(vpImage<vpRGBa> &I,
		 int winx=-1, int winy=-1,
		 const char *title=NULL);

  vpDisplayWin32(vpImage<unsigned char> &I,
		 int winx=-1, int winy=-1,
		 const char *title=NULL);

  virtual ~vpDisplayWin32();

  void setTitle(const char *windowtitle);
  void setFont( const char *fontname );
  void setWindowPosition(int winx, int winy);
  void clearDisplay(const vpColor &color=vpColor::white);
  void closeDisplay();
  void displayImage(const vpImage<vpRGBa> &I);
  void displayImage(const vpImage<unsigned char> &I);
  
  void displayImageROI(const vpImage<unsigned char> &I,const vpImagePoint &iP, const unsigned int width, const unsigned int height);
  void displayImageROI(const vpImage<vpRGBa> &I,const vpImagePoint &iP, const unsigned int width, const unsigned int height);

  void flushDisplay();
  void flushDisplayROI(const vpImagePoint &iP, const unsigned int width, const unsigned int height);

  void init(vpImage<unsigned char> &I,
	    int winx=-1, int winy=-1,
	    const char *title=NULL)  ;

  void init(vpImage<vpRGBa> &I,
	    int winx=-1, int winy=-1,
	    const char *title=NULL)  ;

  void init(unsigned int width, unsigned int height,
	    int winx=-1, int winy=-1 ,
	    const char *title=NULL) ;

  void getImage(vpImage<vpRGBa> &I);

 protected:

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

  void displayLine(const vpImagePoint &ip1, 
		   const vpImagePoint &ip2,
		   const vpColor &color, unsigned int thickness=1);


  void displayPoint(const vpImagePoint &ip, const vpColor &color);

  void displayRectangle(const vpImagePoint &topLeft,
			unsigned int width, unsigned int height,
			const vpColor &color, bool fill = false,
			unsigned int thickness=1);
  void displayRectangle(const vpImagePoint &topLeft,
			const vpImagePoint &bottomRight,
			const vpColor &color, bool fill = false,
			unsigned int thickness=1) ;
  void displayRectangle(const vpRect &rectangle,
			const vpColor &color, bool fill = false,
			unsigned int thickness=1) ;

  bool getClick( bool blocking=true);

  bool getClick(vpImagePoint &ip, bool blocking=true);

  bool getClick(vpImagePoint &ip,
		vpMouseButton::vpMouseButtonType& button, bool blocking=true);

  bool getClickUp(vpImagePoint &ip,
		  vpMouseButton::vpMouseButtonType& button,
		  bool blocking=true);
  bool getKeyboardEvent( bool blocking=true);
  bool getKeyboardEvent(char *string, bool blocking);
  bool getPointerMotionEvent (vpImagePoint &ip);
  bool getPointerPosition (vpImagePoint &ip);

  void waitForInit();

};
#endif
#endif

