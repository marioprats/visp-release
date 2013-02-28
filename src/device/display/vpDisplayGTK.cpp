/****************************************************************************
 *
 * $Id: vpDisplayGTK.cpp 3590 2012-03-05 09:48:31Z ayol $
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
 * Fabien Spindler
 *
 *****************************************************************************/


/*!
  \file vpDisplayGTK.cpp
  \brief Define the GTK console to display images.
*/

#include <visp/vpConfig.h>

#if ( defined(VISP_HAVE_GTK) )

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>    // std::fabs
#include <limits>   // numeric_limits

// Display stuff
#include <visp/vpDisplay.h>
#include <visp/vpDisplayGTK.h>

//debug / exception
#include <visp/vpDebug.h>
#include <visp/vpDisplayException.h>
#include <visp/vpMath.h>
#include <visp/vpImageTools.h>

/*!

  Constructor : initialize a display to visualize a gray level image
  (8 bits).

  \param I : Image to be displayed (not that image has to be initialized).
  \param x, y : The window is set at position x,y (column index, row index).
  \param title : Window title.

*/
vpDisplayGTK::vpDisplayGTK(vpImage<unsigned char> &I,
                           int x,
                           int y,
                           const char *title) : vpDisplay()
{
  col = NULL;
  title = NULL ;
  widget = NULL ;
  init(I, x, y, title) ;
}


/*!
  Constructor : initialize a display to visualize a RGBa level image
  (32 bits).

  \param I : Image to be displayed (not that image has to be initialized)
  \param x, y : The window is set at position x,y (column index, row index).
  \param title : Window title.
*/
vpDisplayGTK::vpDisplayGTK(vpImage<vpRGBa> &I,
                           int x,
                           int y,
                           const char *title)
{
  col = NULL;
  title = NULL ;
  widget = NULL ;
  init(I, x, y, title) ;
}



/*!

  Constructor that just initialize the display position in the screen
  and the display title.

  \param x, y : The window is set at position x,y (column index, row index).
  \param title : Window title.

  To initialize the display size, you need to call init().

  \code
#include <visp/vpDisplayGTK.h>
#include <visp/vpImage.h>

int main()
{
  vpDisplayGTK d(100, 200, "My display");
  vpImage<unsigned char> I(240,384);
  d.init(I);
}
  \endcode
*/
vpDisplayGTK::vpDisplayGTK(int x, int y, const char *title)
{
  windowXPosition = x ;
  windowYPosition = y ;

  col = NULL;
  title = NULL ;
  widget = NULL ;

  if (title != NULL)
  {
    this->title = new char[strlen(title) + 1] ; // Modif Fabien le 19/04/02
    strcpy(this->title, title) ;
  }

  displayHasBeenInitialized = false ;
}

/*!
  Basic constructor.

  To initialize the window position, title and size you may call
  init(vpImage<unsigned char> &, int, int, const char *) or
  init(vpImage<vpRGBa> &, int, int, const char *).

  \code
#include <visp/vpDisplayGTK.h>
#include <visp/vpImage.h>

int main()
{
  vpDisplayGTK d;
  vpImage<unsigned char> I(240,384);
  d.init(I, 100, 200, "My display");
}
  \endcode
*/
vpDisplayGTK::vpDisplayGTK()
{
  windowXPosition = windowYPosition = -1 ;

  col = NULL;
  title = NULL ;
  widget = NULL ;
  if (title != NULL)
  {
    delete [] title ;
    title = NULL ;
  }
  title = new char[1] ;
  strcpy(title,"") ;

  displayHasBeenInitialized = false ;
}

/*!
  Destructor.
*/
vpDisplayGTK::~vpDisplayGTK()
{
  closeDisplay() ;
}

/*!
  Initialize the display (size, position and title) of a gray level image.

  \param I : Image to be displayed (not that image has to be initialized)
  \param x, y : The window is set at position x,y (column index, row index).
  \param title : Window title.

*/
void
vpDisplayGTK::init(vpImage<unsigned char> &I,
                   int x,
                   int y,
                   const char *title)
{

  if ((I.getHeight() == 0) || (I.getWidth()==0))
  {
    vpERROR_TRACE("Image not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "Image not initialized")) ;
  }
  init (I.getWidth(), I.getHeight(), x, y, title) ;
  I.display = this ;
  displayHasBeenInitialized = true ;
}

/*!  
  Initialize the display (size, position and title) of a color
  image in RGBa format.

  \param I : Image to be displayed (not that image has to be initialized)
  \param x, y : The window is set at position x,y (column index, row index).
  \param title : Window title.

*/
void
vpDisplayGTK::init(vpImage<vpRGBa> &I,
                   int x,
                   int y,
                   const char *title)
{
  if ((I.getHeight() == 0) || (I.getWidth()==0))
  {
    vpERROR_TRACE("Image not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "Image not initialized")) ;
  }

  init (I.getWidth(), I.getHeight(), x, y, title) ;
  I.display = this ;
  displayHasBeenInitialized = true ;
}
/*!
  Initialize the display size, position and title.

  \param width, height : Width and height of the window.
  \param x, y : The window is set at position x,y (column index, row index).
  \param title : Window title.

*/
void
vpDisplayGTK::init(unsigned int width, unsigned int height,
                   int x, int y,
                   const char *title)
{
  /* Initialisation of thegdk et gdk_rgb library */
  int *argc=NULL ;
  char **argv ;
  gtk_init(argc,&argv);

  this->width  = width;
  this->height = height;

  /* Create the window*/
  widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_widget_add_events(widget, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK 
			| GDK_POINTER_MOTION_MASK);

  gtk_window_set_default_size(GTK_WINDOW(widget), (gint)width, (gint)height);

  gtk_window_move(GTK_WINDOW(widget), x, y); 

  gtk_widget_show(widget);

  gdk_rgb_init();

  /* Create background pixmap */
  background = gdk_pixmap_new(widget->window, (gint)width, (gint)height, -1);

  /* Create graphic context */
  gc = gdk_gc_new(widget->window);

  /* get the colormap  */
  colormap = gdk_window_get_colormap(widget->window);

  col = new GdkColor *[vpColor::id_unknown] ; // id_unknown = number of predefined colors

  /* Create color */
  gdk_color_parse("light blue",&lightBlue);
  gdk_colormap_alloc_color(colormap,&lightBlue,FALSE,TRUE);
  col[vpColor::id_lightBlue] = &lightBlue ;
  
  gdk_color_parse("blue",&blue);
  gdk_colormap_alloc_color(colormap,&blue,FALSE,TRUE);
  col[vpColor::id_blue] = &blue ;
  
  gdk_color_parse("dark blue",&darkBlue);
  gdk_colormap_alloc_color(colormap,&darkBlue,FALSE,TRUE);
  col[vpColor::id_darkBlue] = &darkBlue ;
  
  gdk_color_parse("#FF8C8C",&lightRed);
  gdk_colormap_alloc_color(colormap,&lightRed,FALSE,TRUE);
  col[vpColor::id_lightRed] = &lightRed ;

  gdk_color_parse("red",&red);
  gdk_colormap_alloc_color(colormap,&red,FALSE,TRUE);
  col[vpColor::id_red] = &red ;
  
  gdk_color_parse("dark red",&darkRed);
  gdk_colormap_alloc_color(colormap,&darkRed,FALSE,TRUE);
  col[vpColor::id_darkRed] = &darkRed ;
  
  gdk_color_parse("light green",&lightGreen);
  gdk_colormap_alloc_color(colormap,&lightGreen,FALSE,TRUE);
  col[vpColor::id_lightGreen] = &lightGreen ;

  gdk_color_parse("green",&green);
  gdk_colormap_alloc_color(colormap,&green,FALSE,TRUE);
  col[vpColor::id_green] = &green ;
  
  gdk_color_parse("dark green",&darkGreen);
  gdk_colormap_alloc_color(colormap,&darkGreen,FALSE,TRUE);
  col[vpColor::id_darkGreen] = &darkGreen ;

  gdk_color_parse("yellow",&yellow);
  gdk_colormap_alloc_color(colormap,&yellow,FALSE,TRUE);
  col[vpColor::id_yellow] = &yellow ;

  gdk_color_parse("cyan",&cyan);
  gdk_colormap_alloc_color(colormap,&cyan,FALSE,TRUE);
  col[vpColor::id_cyan] = &cyan ;

  gdk_color_parse("orange",&orange);
  gdk_colormap_alloc_color(colormap,&orange,FALSE,TRUE);
  col[vpColor::id_orange] = &orange ;
  
  gdk_color_parse("purple",&purple);
  gdk_colormap_alloc_color(colormap,&purple,FALSE,TRUE);
  col[vpColor::id_purple] = &purple ;

  gdk_color_parse("white",&white);
  gdk_colormap_alloc_color(colormap,&white,FALSE,TRUE);
  col[vpColor::id_white] = &white ;

  gdk_color_parse("black",&black);
  gdk_colormap_alloc_color(colormap,&black,FALSE,TRUE);
  col[vpColor::id_black] = &black ;
  
  gdk_color_parse("#C0C0C0",&lightGray);
  gdk_colormap_alloc_color(colormap,&lightGray,FALSE,TRUE);
  col[vpColor::id_lightGray] = &lightGray ;
  
  gdk_color_parse("#808080",&gray);
  gdk_colormap_alloc_color(colormap,&gray,FALSE,TRUE);
  col[vpColor::id_gray] = &gray ;
  
  gdk_color_parse("#404040",&darkGray);
  gdk_colormap_alloc_color(colormap,&darkGray,FALSE,TRUE);
  col[vpColor::id_darkGray] = &darkGray ;

  /* Chargement des polices */
  Police1 = gdk_font_load("-*-times-medium-r-normal-*-16-*-*-*-*-*-*-*");
  Police2 = gdk_font_load("-*-courier-bold-r-normal-*-*-140-*-*-*-*-*-*");

  if (title != NULL)
  {
    if (this->title != NULL)
    {
      delete [] this->title ;
      this->title = NULL ;
    }
    this->title = new char[strlen(title) + 1] ;
    strcpy(this->title, title) ;
  }
  displayHasBeenInitialized = true ;
  setTitle(title) ;
}


/*!

  \warning Not implemented yet.

  Set the font used to display a text in overlay. The display is
  performed using displayCharString().

  \param font : The expected font name. 

  \note Under UNIX, to know all the available fonts, use the
  "xlsfonts" binary in a terminal. You can also use the "xfontsel" binary.

  \sa displayCharString()
*/
void
vpDisplayGTK::setFont(const char * /* font */)
{
  vpERROR_TRACE("Not yet implemented" ) ;
}

/*!
  Set the window title.
  \param title : Window title.
*/
void
vpDisplayGTK::setTitle(const char *title)
{
  if (displayHasBeenInitialized)
  {
    if (title != NULL)
      gdk_window_set_title(widget->window,(char *)title);
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}

/*!
  Set the window position in the screen.

  \param winx, winy : Position of the upper-left window's border in the screen.

  \exception vpDisplayException::notInitializedError : If the video
  device is not initialized.
*/
void vpDisplayGTK::setWindowPosition(int winx, int winy)
{

  if (displayHasBeenInitialized)  {
    gtk_window_move(GTK_WINDOW(widget), winx, winy); 
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}


/*!
  Display the gray level image \e I (8bits).

  \warning Display has to be initialized.

  \warning Suppress the overlay drawing.

  \param I : Image to display.

  \sa init(), closeDisplay()
*/
void vpDisplayGTK::displayImage(const vpImage<unsigned char> &I)
{

  if (displayHasBeenInitialized)
  {
    /* Copie de l'image dans le pixmap fond */
    gdk_draw_gray_image(background,
                        gc, 0, 0, (gint)width, (gint)height,
                        GDK_RGB_DITHER_NONE,
                        I.bitmap,
                        (gint)width);

    /* Le pixmap background devient le fond de la zone de dessin */
    gdk_window_set_back_pixmap(widget->window, background, FALSE);

    /* Affichage */
    //gdk_window_clear(GTK_WINDOW(widget));
    //gdk_flush();
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}


/*!
  Display a selection of the gray level image \e I (8bits).

  \warning Display has to be initialized.

  \warning Suppress the overlay drawing in the region of interest.

  \param I : Image to display.
  
  \param iP : Top left corner of the region of interest
  
  \param width : Width of the region of interest
  
  \param height : Height of the region of interest

  \sa init(), closeDisplay()
*/
void vpDisplayGTK::displayImageROI ( const vpImage<unsigned char> &I,const vpImagePoint &iP, const unsigned int width, const unsigned int height )
{
  if (displayHasBeenInitialized)
  {
    vpImage<unsigned char> Itemp;
    vpImageTools::createSubImage(I,(unsigned int)iP.get_i(),(unsigned int)iP.get_j(),height,width,Itemp);
    /* Copie de l'image dans le pixmap fond */
    gdk_draw_gray_image(background,
                        gc, (gint)iP.get_u(), (gint)iP.get_v(), (gint)width, (gint)height,
                        GDK_RGB_DITHER_NONE,
                        I.bitmap,
                        (gint)width);

    /* Le pixmap background devient le fond de la zone de dessin */
    gdk_window_set_back_pixmap(widget->window, background, FALSE);

    /* Affichage */
    //gdk_window_clear(GTK_WINDOW(widget));
    //gdk_flush();
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}


/*!
  Display the color image \e I in RGBa format (32bits).

  \warning Display has to be initialized.

  \warning Suppress the overlay drawing.

  \param I : Image to display.

  \sa init(), closeDisplay()
*/
void vpDisplayGTK::displayImage(const vpImage<vpRGBa> &I)
{

  if (displayHasBeenInitialized)
  {

    /* Copie de l'image dans le pixmap fond */
    gdk_draw_rgb_32_image(background,
                          gc, 0, 0,  (gint)width, (gint)height,
                          GDK_RGB_DITHER_NONE,
                          (unsigned char *)I.bitmap,
                          (gint)(4*width));

    /* Permet de fermer la fen�tre si besoin (cas des s�quences d'images) */
    //while (g_main_iteration(FALSE));

    /* Le pixmap background devient le fond de la zone de dessin */
    gdk_window_set_back_pixmap(widget->window, background, FALSE);

    /* Affichage */
    //gdk_window_clear(GTK_WINDOW(widget));
    //flushDisplay() ;

  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}

/*!
  Display a selection of the color image \e I in RGBa format (32bits).

  \warning Display has to be initialized.

  \warning Suppress the overlay drawing in the region of interest.

  \param I : Image to display.
  
  \param iP : Top left corner of the region of interest
  
  \param width : Width of the region of interest
  
  \param height : Height of the region of interest

  \sa init(), closeDisplay()
*/
void vpDisplayGTK::displayImageROI ( const vpImage<vpRGBa> &I,const vpImagePoint &iP, const unsigned int width, const unsigned int height )
{
  if (displayHasBeenInitialized)
  {
    vpImage<vpRGBa> Itemp;
    vpImageTools::createSubImage(I,(unsigned int)iP.get_i(),(unsigned int)iP.get_j(),height,width,Itemp);
    /* Copie de l'image dans le pixmap fond */
    gdk_draw_rgb_32_image(background,
                          gc, (gint)iP.get_u(), (gint)iP.get_v(), (gint)width, (gint)height,
                          GDK_RGB_DITHER_NONE,
                          (unsigned char *)Itemp.bitmap,
                          (gint)(4*width));

    /* Permet de fermer la fen�tre si besoin (cas des s�quences d'images) */
    //while (g_main_iteration(FALSE));

    /* Le pixmap background devient le fond de la zone de dessin */
    gdk_window_set_back_pixmap(widget->window, background, FALSE);

    /* Affichage */
    //gdk_window_clear(GTK_WINDOW(widget));
    //flushDisplay() ;
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}

/*!
  \warning Not implemented yet.

  \sa init(), closeDisplay()
*/
void vpDisplayGTK::displayImage(const unsigned char * /* I */)
{
  vpTRACE(" not implemented ") ;
}

/*!
  Close the window.

  \sa init()
*/
void vpDisplayGTK::closeDisplay()
{
  if (col != NULL)
  {
    delete [] col ; col = NULL ;
  }
  if (title != NULL)
  {
    delete [] title ;
    title = NULL ;
  }

  if (widget != NULL)
  {
    gdk_window_hide (widget->window);
    gdk_window_destroy(widget->window);
    widget = NULL;
  }
  displayHasBeenInitialized = false;
}


/*!
  Flushes the display buffer.
  It's necessary to use this function to see the results of any drawing.
*/
void vpDisplayGTK::flushDisplay()
{
  if (displayHasBeenInitialized)
  {
    gdk_window_clear(widget->window);
    gdk_flush();
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}


/*!
  Flushes the display buffer.
  It's necessary to use this function to see the results of any drawing.
*/
void vpDisplayGTK::flushDisplayROI(const vpImagePoint &/*iP*/, const unsigned int /*width*/, const unsigned int /*height*/)
{
  if (displayHasBeenInitialized)
  {
    gdk_window_clear(widget->window);
    gdk_flush();
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}


/*!
  \warning Not implemented yet.
*/
void vpDisplayGTK::clearDisplay(const vpColor & /* color */)
{
  vpTRACE("Not implemented") ;
}

/*!
  Display an arrow from image point \e ip1 to image point \e ip2.
  \param ip1,ip2 : Initial and final image point.
  \param color : Arrow color.
  \param w,h : Width and height of the arrow.
  \param thickness : Thickness of the lines used to display the arrow.
*/
void vpDisplayGTK::displayArrow ( const vpImagePoint &ip1, 
				  const vpImagePoint &ip2,
				  const vpColor &color,
				  unsigned int w, unsigned int h,
				  unsigned int thickness)
{
  if (displayHasBeenInitialized)
  {
    try{
      double a = ip2.get_i() - ip1.get_i() ;
      double b = ip2.get_j() - ip1.get_j() ;
      double lg = sqrt(vpMath::sqr(a)+vpMath::sqr(b)) ;

      //if ((a==0)&&(b==0))
      if ((std::fabs(a) <= std::numeric_limits<double>::epsilon() )&&(std::fabs(b) <= std::numeric_limits<double>::epsilon()) )
      {
        // DisplayCrossLarge(i1,j1,3,col) ;
      }
      else
      {
        a /= lg ;
        b /= lg ;

	vpImagePoint ip3;
        ip3.set_i(ip2.get_i() - w*a);
        ip3.set_j(ip2.get_j() - w*b);

	vpImagePoint ip4;
	ip4.set_i( ip3.get_i() - b*h );
	ip4.set_j( ip3.get_j() + a*h );

	displayLine ( ip2, ip4, color, thickness ) ;
        
	ip4.set_i( ip3.get_i() + b*h );
	ip4.set_j( ip3.get_j() - a*h );

	displayLine ( ip2, ip4, color, thickness ) ;
	displayLine ( ip1, ip2, color, thickness ) ;
      }
    }
    catch (...)
    {
      vpERROR_TRACE("Error caught") ;
      throw ;
    }
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}


/*!
  Display a string at the image point \e ip location.

  To select the font used to display the string, use setFont().

  \param ip : Upper left image point location of the string in the display.
  \param text : String to display in overlay.
  \param color : String color.

  \sa setFont()
*/
void vpDisplayGTK::displayCharString ( const vpImagePoint &ip,
				       const char *text, 
				       const vpColor &color )
{
  if (displayHasBeenInitialized)
  {
    if (color.id < vpColor::id_unknown)
      gdk_gc_set_foreground(gc, col[color.id]);
    else {
      gdkcolor.red   = 256 * color.R;
      gdkcolor.green = 256 * color.G;
      gdkcolor.blue  = 256 * color.B;
      gdk_colormap_alloc_color(colormap,&gdkcolor,FALSE,TRUE);
      gdk_gc_set_foreground(gc, &gdkcolor);     
    }

    gdk_draw_string(background, Police2, gc,
		    vpMath::round( ip.get_u() ), 
		    vpMath::round( ip.get_v() ),
		    (const gchar *)text);

  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}
/*!
  Display a circle.
  \param center : Circle center position.
  \param radius : Circle radius.
  \param color : Circle color.
  \param fill : When set to true fill the circle.
  \param thickness : Thickness of the circle. This parameter is only useful 
  when \e fill is set to false.
*/
void vpDisplayGTK::displayCircle ( const vpImagePoint &center,
				   unsigned int radius,
				   const vpColor &color,
				   bool fill,
				   unsigned int thickness )
{
  if (displayHasBeenInitialized)
  {
    if ( thickness == 1 ) thickness = 0;

    if (color.id < vpColor::id_unknown)
      gdk_gc_set_foreground(gc, col[color.id]);
    else {
      gdkcolor.red   = 256 * color.R;
      gdkcolor.green = 256 * color.G;
      gdkcolor.blue  = 256 * color.B;
      gdk_colormap_alloc_color(colormap,&gdkcolor,FALSE,TRUE);
      gdk_gc_set_foreground(gc, &gdkcolor);     
    }

    gdk_gc_set_line_attributes(gc, (gint)thickness,
			       GDK_LINE_SOLID, GDK_CAP_BUTT,
			       GDK_JOIN_BEVEL) ;

    if (fill == false)
      gdk_draw_arc(background, gc, FALSE,
		   vpMath::round( center.get_u()-radius ), 
		   vpMath::round( center.get_v()-radius ),
		   (gint)(2*radius), (gint)(2*radius), 23040, 23040) ; /* 23040 = 360*64 */
    else
      gdk_draw_arc(background, gc, TRUE,
		   vpMath::round( center.get_u()-radius ), 
		   vpMath::round( center.get_v()-radius ),
		   (gint)(2*radius), (gint)(2*radius), 23040, 23040) ; /* 23040 = 360*64 */
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}
/*!
  Display a cross at the image point \e ip location.
  \param ip : Cross location.
  \param size : Size (width and height) of the cross.
  \param color : Cross color.
  \param thickness : Thickness of the lines used to display the cross.
*/
void vpDisplayGTK::displayCross ( const vpImagePoint &ip, 
				  unsigned int size, 
				  const vpColor &color,
				  unsigned int thickness)
{
  if (displayHasBeenInitialized)
  {
    try{
      double i = ip.get_i();
      double j = ip.get_j();
      vpImagePoint ip1, ip2;

      ip1.set_i( i-size/2 ); 
      ip1.set_j( j );
      ip2.set_i( i+size/2 ); 
      ip2.set_j( j );
      displayLine ( ip1, ip2, color, thickness ) ;

      ip1.set_i( i ); 
      ip1.set_j( j-size/2 );
      ip2.set_i( i ); 
      ip2.set_j( j+size/2 );

      displayLine ( ip1, ip2, color, thickness ) ;
    }
    catch (...)
    {
      vpERROR_TRACE("Error caught") ;
      throw ;
    }
  }

  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}
/*!
  Display a dashed line from image point \e ip1 to image point \e ip2.
  \param ip1,ip2 : Initial and final image points.
  \param color : Line color.
  \param thickness : Line thickness.
*/
void vpDisplayGTK::displayDotLine ( const vpImagePoint &ip1, 
				    const vpImagePoint &ip2,
				    const vpColor &color, 
				    unsigned int thickness )
{

  if (displayHasBeenInitialized)
  {
    if ( thickness == 1 ) thickness = 0;

    if (color.id < vpColor::id_unknown)
      gdk_gc_set_foreground(gc, col[color.id]);
    else {
      gdkcolor.red   = 256 * color.R;
      gdkcolor.green = 256 * color.G;
      gdkcolor.blue  = 256 * color.B;
      gdk_colormap_alloc_color(colormap,&gdkcolor,FALSE,TRUE);
      gdk_gc_set_foreground(gc, &gdkcolor);     
    }

    gdk_gc_set_line_attributes(gc, (gint)thickness, 
			       GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT,
                               GDK_JOIN_BEVEL) ;
    gdk_draw_line(background, gc,
		  vpMath::round( ip1.get_u() ),
		  vpMath::round( ip1.get_v() ),
		  vpMath::round( ip2.get_u() ),
		  vpMath::round( ip2.get_v() ) );
    gdk_gc_set_line_attributes(gc, 0,
                               GDK_LINE_SOLID, GDK_CAP_BUTT,
                               GDK_JOIN_BEVEL) ;
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}

/*!
  Display a line from image point \e ip1 to image point \e ip2.
  \param ip1,ip2 : Initial and final image points.
  \param color : Line color.
  \param thickness : Line thickness.
*/
void vpDisplayGTK::displayLine ( const vpImagePoint &ip1, 
				 const vpImagePoint &ip2,
				 const vpColor &color, 
				 unsigned int thickness )
{
  if (displayHasBeenInitialized)
  {
    if ( thickness == 1 ) thickness = 0;

    if (color.id < vpColor::id_unknown)
      gdk_gc_set_foreground(gc, col[color.id]);
    else {
      gdkcolor.red   = 256 * color.R;
      gdkcolor.green = 256 * color.G;
      gdkcolor.blue  = 256 * color.B;
      gdk_colormap_alloc_color(colormap,&gdkcolor,FALSE,TRUE);
      gdk_gc_set_foreground(gc, &gdkcolor);     
    }

    gdk_gc_set_line_attributes(gc, (gint)thickness,
			       GDK_LINE_SOLID, GDK_CAP_BUTT,
			       GDK_JOIN_BEVEL) ;
    gdk_draw_line(background, gc,
		  vpMath::round( ip1.get_u() ),
		  vpMath::round( ip1.get_v() ),
		  vpMath::round( ip2.get_u() ),
		  vpMath::round( ip2.get_v() ) );
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}

/*!
  Display a point at the image point \e ip location.
  \param ip : Point location.
  \param color : Point color.
*/
void vpDisplayGTK::displayPoint ( const vpImagePoint &ip,
				  const vpColor &color )
{
  if (displayHasBeenInitialized)
  {
    if (color.id < vpColor::id_unknown)
      gdk_gc_set_foreground(gc, col[color.id]);
    else {
      gdkcolor.red   = 256 * color.R;
      gdkcolor.green = 256 * color.G;
      gdkcolor.blue  = 256 * color.B;
      gdk_colormap_alloc_color(colormap,&gdkcolor,FALSE,TRUE);
      gdk_gc_set_foreground(gc, &gdkcolor);     
    }

    gdk_draw_point(background,gc, 
		   vpMath::round( ip.get_u() ), 
		   vpMath::round( ip.get_v() ) );
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}


/*!  
  Display a rectangle with \e topLeft as the top-left corner and \e
  width and \e height the rectangle size.

  \param topLeft : Top-left corner of the rectangle.
  \param width,height : Rectangle size.
  \param color : Rectangle color.
  \param fill : When set to true fill the rectangle.

  \param thickness : Thickness of the four lines used to display the
  rectangle. This parameter is only useful when \e fill is set to
  false.
*/
void
vpDisplayGTK::displayRectangle ( const vpImagePoint &topLeft,
				 unsigned int width, unsigned int height,
				 const vpColor &color, bool fill,
				 unsigned int thickness )
{
  if (displayHasBeenInitialized)
  {
    if ( thickness == 1 ) thickness = 0;

    if (color.id < vpColor::id_unknown)
      gdk_gc_set_foreground(gc, col[color.id]);
    else {
      gdkcolor.red   = 256 * color.R;
      gdkcolor.green = 256 * color.G;
      gdkcolor.blue  = 256 * color.B;
      gdk_colormap_alloc_color(colormap,&gdkcolor,FALSE,TRUE);
      gdk_gc_set_foreground(gc, &gdkcolor);     
    }
    gdk_gc_set_line_attributes(gc, (gint)thickness,
			       GDK_LINE_SOLID, GDK_CAP_BUTT,
			       GDK_JOIN_BEVEL) ;

    if (fill == false)
      gdk_draw_rectangle(background, gc, FALSE,
			 vpMath::round( topLeft.get_u() ),
			 vpMath::round( topLeft.get_v() ),
			 (gint)width-1, (gint)height-1);
    else
      gdk_draw_rectangle(background, gc, TRUE,
			 vpMath::round( topLeft.get_u() ),
			 vpMath::round( topLeft.get_v() ),
			 (gint)width, (gint)height);

    if (thickness > 1)
      gdk_gc_set_line_attributes(gc, 0, GDK_LINE_SOLID, GDK_CAP_BUTT,
                                 GDK_JOIN_BEVEL) ;
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}

/*!  
  Display a rectangle.

  \param topLeft : Top-left corner of the rectangle.
  \param bottomRight : Bottom-right corner of the rectangle.
  \param color : Rectangle color.
  \param fill : When set to true fill the rectangle.

  \param thickness : Thickness of the four lines used to display the
  rectangle. This parameter is only useful when \e fill is set to
  false.
*/
void
vpDisplayGTK::displayRectangle ( const vpImagePoint &topLeft,
				 const vpImagePoint &bottomRight,
				 const vpColor &color, bool fill,
				 unsigned int thickness )
{
  if (displayHasBeenInitialized)
  {
    if ( thickness == 1 ) thickness = 0;

    if (color.id < vpColor::id_unknown)
      gdk_gc_set_foreground(gc, col[color.id]);
    else {
      gdkcolor.red   = 256 * color.R;
      gdkcolor.green = 256 * color.G;
      gdkcolor.blue  = 256 * color.B;
      gdk_colormap_alloc_color(colormap,&gdkcolor,FALSE,TRUE);
      gdk_gc_set_foreground(gc, &gdkcolor);     
    }

    gdk_gc_set_line_attributes(gc, (gint)thickness,
			       GDK_LINE_SOLID, GDK_CAP_BUTT,
			       GDK_JOIN_BEVEL) ;

    int width  = vpMath::round( bottomRight.get_u() - topLeft.get_u() );
    int height = vpMath::round( bottomRight.get_v() - topLeft.get_v() );

    if (fill == false)
      gdk_draw_rectangle(background, gc, FALSE,
			 vpMath::round( topLeft.get_u() ),
			 vpMath::round( topLeft.get_v() ),
			 width-1,height-1);
    else
      gdk_draw_rectangle(background, gc, TRUE,
			 vpMath::round( topLeft.get_u() ),
			 vpMath::round( topLeft.get_v() ),
			 width, height);

    if (thickness > 1)
      gdk_gc_set_line_attributes(gc, 0, GDK_LINE_SOLID, GDK_CAP_BUTT,
                                 GDK_JOIN_BEVEL) ;
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}

/*!
  Display a rectangle.

  \param rectangle : Rectangle characteristics.
  \param color : Rectangle color.
  \param fill : When set to true fill the rectangle.

  \param thickness : Thickness of the four lines used to display the
  rectangle. This parameter is only useful when \e fill is set to
  false.

*/
void
vpDisplayGTK::displayRectangle ( const vpRect &rectangle,
				 const vpColor &color, bool fill,
				 unsigned int thickness )
{
  if (displayHasBeenInitialized)
  {
    if (color.id < vpColor::id_unknown)
      gdk_gc_set_foreground(gc, col[color.id]);
    else {
      gdkcolor.red   = 256 * color.R;
      gdkcolor.green = 256 * color.G;
      gdkcolor.blue  = 256 * color.B;
      gdk_colormap_alloc_color(colormap,&gdkcolor,FALSE,TRUE);
      gdk_gc_set_foreground(gc, &gdkcolor);     
    }

    if ( thickness == 1 ) thickness = 0;

    gdk_gc_set_line_attributes(gc, (gint)thickness, GDK_LINE_SOLID, GDK_CAP_BUTT,
			       GDK_JOIN_BEVEL) ;

    if (fill == false)
      gdk_draw_rectangle(background, gc, FALSE,
                       vpMath::round( rectangle.getLeft() ), 
		       vpMath::round( rectangle.getTop() ),
		       vpMath::round( rectangle.getWidth()-1 ), 
		       vpMath::round( rectangle.getHeight()-1 ) );

    else
      gdk_draw_rectangle(background, gc, TRUE,
                       vpMath::round( rectangle.getLeft() ), 
		       vpMath::round( rectangle.getTop() ),
		       vpMath::round( rectangle.getWidth()-1 ), 
		       vpMath::round( rectangle.getHeight()-1 ) );

    if (thickness > 1)
      gdk_gc_set_line_attributes(gc, 0, GDK_LINE_SOLID, GDK_CAP_BUTT,
                                 GDK_JOIN_BEVEL) ;
  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
}


/*!
  Wait for a click from one of the mouse button.

  \param blocking [in] : Blocking behavior.
  - When set to true, this method waits until a mouse button is
    pressed and then returns always true.
  - When set to false, returns true only if a mouse button is
    pressed, otherwise returns false.

  \return 
  - true if a button was clicked. This is always the case if blocking is set 
    to \e true.
  - false if no button was clicked. This can occur if blocking is set
    to \e false.
*/
bool
vpDisplayGTK::getClick(bool blocking)
{
  bool ret = false;

  int cpt =0;
  if (displayHasBeenInitialized) {

//    flushDisplay() ;
    GdkEvent *ev = NULL;
    do {
      while ((ev = gdk_event_get())!=NULL){
	cpt++;
	//	printf("event %d type %d on window %p My window %p\n", 
	//cpt, ev->type, ev->any.window, widget->window);
	
        if (ev->any.window == widget->window && ev->type == GDK_BUTTON_PRESS){
          ret = true ;
	  //printf("Click detection\n");
        }
        gdk_event_free(ev) ;
      }
      if (blocking){
        flushDisplay();
        vpTime::wait(100);
      }
    } while ( ret == false && blocking == true);
  }
  else {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
  return ret;
}

/*!
  Wait for a click from one of the mouse button and get the position
  of the clicked image point.

  \param ip [out] : The coordinates of the clicked image point.

  \param blocking [in] : true for a blocking behaviour waiting a mouse
  button click, false for a non blocking behaviour.

  \return 
  - true if a button was clicked. This is always the case if blocking is set 
    to \e true.
  - false if no button was clicked. This can occur if blocking is set
    to \e false.

*/
bool
vpDisplayGTK::getClick(vpImagePoint &ip, bool blocking)
{
  bool ret = false;

  if (displayHasBeenInitialized) {

    GdkEvent *ev = NULL;
    double u, v ;
    do {
      while ((ev = gdk_event_get())!=NULL){
        if (ev->any.window == widget->window && ev->type == GDK_BUTTON_PRESS) {
          u = ((GdkEventButton *)ev)->x ;
          v = ((GdkEventButton *)ev)->y ;
	  ip.set_u( u );
	  ip.set_v( v );
          ret = true ;
        }
        gdk_event_free(ev) ;
      }
      if (blocking){
        flushDisplay();
        vpTime::wait(100);
      }
    } while ( ret == false && blocking == true);
  }
  else {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
  return ret ;
}


/*!

  Wait for a mouse button click and get the position of the clicked
  pixel. The button used to click is also set.
  
  \param ip [out] : The coordinates of the clicked image point.

  \param button [out] : The button used to click.

  \param blocking [in] : 
  - When set to true, this method waits until a mouse button is
    pressed and then returns always true.
  - When set to false, returns true only if a mouse button is
    pressed, otherwise returns false.

  \return true if a mouse button is pressed, false otherwise. If a
  button is pressed, the location of the mouse pointer is updated in
  \e ip.
*/
bool
vpDisplayGTK::getClick(vpImagePoint &ip,
                       vpMouseButton::vpMouseButtonType& button,
                       bool blocking)
{
  bool ret = false;

  if (displayHasBeenInitialized) {
    GdkEvent *ev = NULL;
    double u, v ;
    do {
      while ((ev = gdk_event_get())){
        if (ev->any.window == widget->window && ev->type == GDK_BUTTON_PRESS){
          u = ((GdkEventButton *)ev)->x ;
          v = ((GdkEventButton *)ev)->y ;
	  ip.set_u( u );
	  ip.set_v( v );

          switch ((int)((GdkEventButton *)ev)->button) {
            case 1:
              button = vpMouseButton::button1; break;
            case 2:
              button = vpMouseButton::button2; break;
            case 3:
              button = vpMouseButton::button3; break;
          }
          ret = true ;
        }
        gdk_event_free(ev) ;
      }
      if (blocking){
        flushDisplay();
        vpTime::wait(100);
      }

    } while ( ret == false && blocking == true);
  }
  else {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
  return ret;
}

/*!

  Wait for a mouse button click release and get the position of the
  image point were the click release occurs.  The button used to click is
  also set. Same method as getClick(unsigned int&, unsigned int&,
  vpMouseButton::vpMouseButtonType &, bool).

  \param ip [out] : Position of the clicked image point.

  \param button [in] : Button used to click.

  \param blocking [in] : true for a blocking behaviour waiting a mouse
  button click, false for a non blocking behaviour.

  \return 
  - true if a button was clicked. This is always the case if blocking is set 
    to \e true.
  - false if no button was clicked. This can occur if blocking is set
    to \e false.

  \sa getClick(vpImagePoint &, vpMouseButton::vpMouseButtonType &, bool)

*/
bool
vpDisplayGTK::getClickUp(vpImagePoint &ip,
                         vpMouseButton::vpMouseButtonType& button,
                         bool blocking)
{
  bool ret = false;

  if ( displayHasBeenInitialized ) {

    //flushDisplay() ;
    GdkEvent *ev = NULL;
    double u, v ;
    do {
      while ((ev = gdk_event_get())!=NULL){
        if ( ev->any.window == widget->window  
	     && ev->type == GDK_BUTTON_RELEASE) {
          u = ((GdkEventButton *)ev)->x ;
          v = ((GdkEventButton *)ev)->y ;
	  ip.set_u( u );
	  ip.set_v( v );

          switch ( ( int ) ( ( GdkEventButton * ) ev )->button ) {
            case 1:
              button = vpMouseButton::button1; break;
            case 2:
              button = vpMouseButton::button2; break;
            case 3:
              button = vpMouseButton::button3; break;
          }
          ret = true ;
        }
        gdk_event_free(ev) ;
      }
      if (blocking){
        flushDisplay();
        vpTime::wait(100);
      }

    } while ( ret == false && blocking == true);
  }
  else {
    vpERROR_TRACE ( "GTK not initialized " ) ;
    throw ( vpDisplayException ( vpDisplayException::notInitializedError,
                                 "GTK not initialized" ) ) ;
  }
  return ret;
}

/*
  \brief gets the displayed image (including the overlay plane)
  and returns an RGBa image
*/
void vpDisplayGTK::getImage(vpImage<vpRGBa> &I)
{


  // shoudl certainly be optimized.
  // doesn't work
  if (displayHasBeenInitialized)
  {

    GdkImage *ImageGtk;
    /*
     */

    ImageGtk = gdk_image_get(background, 0, 0, (gint)width, (gint)height);


    I.resize(height,width) ;
    guchar *pos;
    guint32 pixel;
    gint x,y;
    guchar OctetRouge,OctetVert,OctetBleu,mask;
    mask = 0x000000FF;

    pos = (unsigned char *)I.bitmap;
    for (y=0;y<(gint)height;y++)
    {
      for (x=0;x<(gint)width;x++)
      {
        pixel = gdk_image_get_pixel(ImageGtk,x,y);
        OctetBleu  = (guchar)pixel & mask;
        OctetVert  = (guchar)(pixel>>8) & mask;
        OctetRouge = (guchar)(pixel>>16) & mask;
        *pos++ = OctetRouge;
        *pos++ = OctetVert;
        *pos++ = OctetBleu;
        *pos++ = 0;
      }
    }


  }
  else
  {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }

}

/*!
  \brief get the window depth (8,16,24,32)

  usualy it 24 bits now...
*/
unsigned int vpDisplayGTK::getScreenDepth()
{

  unsigned int depth;

  depth = (unsigned int)gdk_window_get_visual(widget->window)->depth ;

  return (depth);
}

/*!
  \brief get the window size

  \warning Not implemented
*/
void vpDisplayGTK::getScreenSize(unsigned int &width, unsigned int &height)
{
  vpTRACE("Not implemented") ;
  width = 0;
  height = 0;
}

/*!
  Get a keyboard event.

  \param blocking [in] : Blocking behavior.
  - When set to true, this method waits until a key is
    pressed and then returns always true.
  - When set to false, returns true only if a key is
    pressed, otherwise returns false.

  \return 
  - true if a key was pressed. This is always the case if blocking is set 
    to \e true.
  - false if no key was pressed. This can occur if blocking is set
    to \e false.
*/
bool
vpDisplayGTK::getKeyboardEvent(bool blocking)
{
  bool ret = false;

  int cpt =0;
  if (displayHasBeenInitialized) {

    GdkEvent *ev = NULL;
    do {
      while ((ev = gdk_event_get())!=NULL){
	cpt++;
	//	printf("event %d type %d on window %p My window %p\n", 
	//cpt, ev->type, ev->any.window, widget->window);
	
        if (ev->any.window == widget->window && ev->type == GDK_KEY_PRESS){
          ret = true ;
	  //printf("Key press detection\n");
        }
        gdk_event_free(ev) ;
      }
      if (blocking){
        flushDisplay();
        vpTime::wait(100);
      }
    } while ( ret == false && blocking == true);
  }
  else {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
  return ret;
}

/*!

  Get a keyboard event.

  \param blocking [in] : Blocking behavior.
  - When set to true, this method waits until a key is
    pressed and then returns always true.
  - When set to false, returns true only if a key is
    pressed, otherwise returns false.

  \param string [out]: If possible, an ISO Latin-1 character
  corresponding to the keyboard key.

  \return 
  - true if a key was pressed. This is always the case if blocking is set 
    to \e true.
  - false if no key was pressed. This can occur if blocking is set
    to \e false.
*/
bool
vpDisplayGTK::getKeyboardEvent(char *string, bool blocking)
{
  bool ret = false;

  int cpt =0;
  if (displayHasBeenInitialized) {

    GdkEvent *ev = NULL;
    do {
      while ((ev = gdk_event_get())!=NULL){
	cpt++;
	//	printf("event %d type %d on window %p My window %p\n", 
	//cpt, ev->type, ev->any.window, widget->window);
	
        if (ev->any.window == widget->window && ev->type == GDK_KEY_PRESS){
	  //std::cout << "Key val: \"" << gdk_keyval_name (ev->key.keyval) /*ev->key.string*/ << "\"" << std::endl;
	  sprintf(string, "%s", gdk_keyval_name (ev->key.keyval));
          ret = true ;
	  //printf("Key press detection\n");
        }
        gdk_event_free(ev) ;
      }
      if (blocking){
        flushDisplay();
        vpTime::wait(100);
      }
    } while ( ret == false && blocking == true);
  }
  else {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
  return ret;
}

/*!

  Get the coordinates of the mouse pointer.

  \param ip [out] : The coordinates of the mouse pointer.

  \return true if a pointer motion event was received, false otherwise.

  \exception vpDisplayException::notInitializedError : If the display
  was not initialized.

*/
bool
vpDisplayGTK::getPointerMotionEvent ( vpImagePoint &ip)
{
  bool ret = false;

  if (displayHasBeenInitialized) {
    GdkEvent *ev = NULL;
    double u, v ;
    if ((ev = gdk_event_get())){
      if (ev->any.window == widget->window && ev->type == GDK_MOTION_NOTIFY){
	u = ((GdkEventMotion *)ev)->x ;
	v = ((GdkEventMotion *)ev)->y ;
	ip.set_u( u );
	ip.set_v( v );
	
	ret = true ;
      }
      gdk_event_free(ev) ;
    }
  }
  else {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
  return ret;
}

/*!
  Get the coordinates of the mouse pointer.

  \param ip [out] : The coordinates of the mouse pointer.

  \return true.

  \exception vpDisplayException::notInitializedError : If the display
  was not initialized.
*/
bool
vpDisplayGTK::getPointerPosition ( vpImagePoint &ip)
{
  if (displayHasBeenInitialized)
  {
    int u,v;
    gdk_window_get_pointer(widget->window, &u, &v, NULL);
    ip.set_u( u );
    ip.set_v( v );
  }
  else {
    vpERROR_TRACE("GTK not initialized " ) ;
    throw(vpDisplayException(vpDisplayException::notInitializedError,
                             "GTK not initialized")) ;
  }
  
  return true;
}

#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
