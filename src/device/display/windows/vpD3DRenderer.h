/****************************************************************************
 *
 * $Id: vpD3DRenderer.h 3587 2012-03-02 16:26:32Z fspindle $
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
 * D3D renderer for windows 32 display
 *
 * Authors:
 * Bruno Renier
 *
 *****************************************************************************/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <visp/vpConfig.h>

#if ( defined(VISP_HAVE_D3D9) )

#ifndef VPD3DRENDERER_HH
#define VPD3DRENDERER_HH

#include <windows.h>
#include <d3dx9.h>
#include <visp/vpWin32Renderer.h>
#include <visp/vpDisplayException.h>


#include <iostream>

/*!
  \class vpD3DRenderer.h

  \brief Display under windows using Direct3D9.
  Is used by vpD3DDisplay to do the drawing.

*/
class VISP_EXPORT vpD3DRenderer : public vpWin32Renderer
{

  IDirect3D9 * pD3D;

  //The d3d device we will be working with.
  IDirect3DDevice9 * pd3dDevice;

  //Sprite used to render the texture.
  ID3DXSprite * pSprite;

  //The system memory texture :
  //The one we will be drawing on.
  IDirect3DTexture9 * pd3dText;

  //The video memory texture :
  //The one we will use for display.
  IDirect3DTexture9 * pd3dVideoText;

  //The texture's width.
  unsigned int textWidth;

  //The window's handle.
  HWND hWnd;

  //Colors  for overlay drawn with d3d directly.
  unsigned long colors[vpColor::id_unknown];

  //Colors for overlay drawn with GDI.
  COLORREF colorsGDI[vpColor::id_unknown];

  //Font used for text drawing.
  HFONT hFont;

 public:

  bool init(HWND hwnd, unsigned int width, unsigned int height);
  bool render();

  vpD3DRenderer();
  virtual ~vpD3DRenderer();

  void setImg(const vpImage<vpRGBa>& im);
  void setImg(const vpImage<unsigned char>& im);
  void setImgROI(const vpImage<vpRGBa>& im, const vpImagePoint &iP, const unsigned int width, const unsigned int height );
  void setImgROI(const vpImage<unsigned char>& im, const vpImagePoint &iP, const unsigned int width, const unsigned int height );

  void setPixel(const vpImagePoint &iP, const vpColor &color);

  void drawLine(const vpImagePoint &ip1, 
		const vpImagePoint &ip2,
		const vpColor &color, unsigned int thickness, int style=PS_SOLID);

  void drawRect(const vpImagePoint &topLeft,
		unsigned int width, unsigned int height,
		const vpColor &color, bool fill=false,
		unsigned int thickness=1);

  void clear(const vpColor &color);

  void drawCircle(const vpImagePoint &center, unsigned int radius,
		  const vpColor &color, bool fill=false, unsigned int thickness=1);

  void drawText(const vpImagePoint &ip, const char * text,
		const vpColor &color);

  void drawCross(const vpImagePoint &ip, unsigned int size,
		 const vpColor &color, unsigned int thickness=1);

  void drawArrow(const vpImagePoint &ip1, 
		 const vpImagePoint &ip2,
		 const vpColor &color, unsigned int w,unsigned int h, unsigned int thickness=1);

  void getImage(vpImage<vpRGBa> &I);



 private:

  void initView(float, float);

  /*!
    Sub function for circle drawing.
    Circle drawing is based on Bresenham 's circle algorithm.
  */
  void subDrawCircle(int i, int j,
		     int x, int y,
		     vpColor col, unsigned char* buf,
		     unsigned int pitch, unsigned int maxX, unsigned int maxY);


  /*!
    Safe useful inline function to set a pixel in a texture buffer.
    \param buf The texture's buffer.
    \param pitch The image pitch.
    \param x The x-coordinate of the pixel (in the locked rectangle base)
    \param y The y-coordinate of the pixel (in the locked rectangle base)
    \param color The color of the pixel.
    \param maxX The maximum x value (equals to the width of the locked rectangle).
    \param maxY The maximum y value (equals to the height of the locked rectangle).

  */
  inline void setBufferPixel(unsigned char* buf, unsigned int pitch,
			     int x, int y,
			     const vpColor &color,
			     unsigned int maxX, unsigned int maxY)
    {
      unsigned long c;
      if (color.id < vpColor::id_unknown)
		c = colors[color.id];
      else {
		c = D3DCOLOR_ARGB(0xFF, color.R, color.G, color.B);
	  }

      if(x>=0 && y>=0 && x<= (int)maxX && y<= (int)maxY)
	  *(unsigned long*)(buf + (y*pitch) + (x<<2)) = c; //colors[color];
    }
  /*!
    Unsafe useful inline function to set a pixel in a texture buffer.
    \param buf The texture's buffer.
    \param pitch The image pitch.
    \param x The x-coordinate of the pixel (in the locked rectangle base)
    \param y The y-coordinate of the pixel (in the locked rectangle base)
    \param color The color of the pixel.

  */
  inline void setBufferPixel(unsigned char* buf, unsigned int pitch,
                             int x, int y,
                             const vpColor &color)
  {
      unsigned long c;
      if (color.id < vpColor::id_unknown)
		c = colors[color.id];
      else {
		c = D3DCOLOR_ARGB(0xFF, color.R, color.G, color.B);
	  }

	  *(unsigned long*)(buf + (y*pitch) + (x<<2)) = c; //colors[color];
  }

  unsigned int supPowerOf2(unsigned int n);

};
#endif
#endif
#endif
