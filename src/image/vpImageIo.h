/****************************************************************************
 *
 * $Id: vpImageIo.h 3794 2012-06-14 11:59:30Z fspindle $
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
 * Read/write images.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


/*!
  \file vpImageIo.h
  \brief Read/write images
*/

#ifndef vpIMAGEIO_H
#define vpIMAGEIO_H

#include <visp/vpImage.h>
#include <visp/vpDebug.h>
#include <visp/vpRGBa.h>
#include <visp/vpImageConvert.h>

#include <stdio.h>
#include <iostream>

#ifdef WIN32
#  include <windows.h>
#endif

#if defined(VISP_HAVE_LIBJPEG)
#include <jpeglib.h>
#include <jerror.h>
#endif

#if defined(VISP_HAVE_LIBPNG)
#include <png.h>
#endif


/*!
  \class vpImageIo
  
  \ingroup ImageRW

  \brief Read/write images with various image format.

  The code below shows how to convert an PPM P6 image file format into
  a PGM P5 image file format. The extension of the filename is here
  used in read() and write() functions to set the image file format
  (".pgm" for PGM P5 and ".ppm" for PPM P6).

  \code
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>

int main()
{
  vpImage<unsigned char> I;
#ifdef UNIX
  std::string filename("/local/soft/ViSP/ViSP-images/Klimt/Klimt.ppm");
#elif WIN32
  std::string filename("C:/temp/ViSP-images/Klimt/Klimt.ppm");
#endif

  vpImageIo::read(I, filename); // Convert the color image in a gray level image
  vpImageIo::write(I, "Klimt.pgm"); // Write the image in a PGM P5 image file format 
}
  \endcode
*/

class VISP_EXPORT vpImageIo
{

private:
  
  typedef enum
  {
    FORMAT_PGM,
    FORMAT_PPM,
    FORMAT_JPEG,
    FORMAT_PNG,
    FORMAT_UNKNOWN
  } vpImageFormatType;
  
  static const int vpMAX_LEN;

  static FILE * openFileRead(const char *filename) ;
  static FILE * openFileWrite(const char *filename, const char *mode="w") ;

  static FILE * openFileRead(const std::string filename) ;
  static FILE * openFileWrite(const std::string filename, 
			      const std::string mode="w") ;

  static vpImageFormatType getFormat(const char *filename) ;
  static std::string getExtension(const std::string &filename);

public:

  static
  void read(vpImage<unsigned char> &I, const char *filename) ;
  static
  void read(vpImage<unsigned char> &I, const std::string filename) ;
  static
  void read(vpImage<vpRGBa> &I, const char *filename) ;
  static
  void read(vpImage<vpRGBa> &I, const std::string filename) ;
  
  static
  void write(const vpImage<unsigned char> &I, const char *filename) ;
  static
  void write(const vpImage<unsigned char> &I, const std::string filename) ;
  static
  void write(const vpImage<vpRGBa> &I, const char *filename) ;
  static
  void write(const vpImage<vpRGBa> &I, const std::string filename) ;

 static
  void readPFM(vpImage<float> &I, const char *filename) ;


  static
  void readPGM(vpImage<unsigned char> &I, const char *filename) ;
  static
  void readPGM(vpImage<unsigned char> &I, const std::string filename) ;
  static
  void readPGM(vpImage<vpRGBa> &I, const char *filename) ;
  static
  void readPGM(vpImage<vpRGBa> &I, const std::string filename) ;

  static
  void readPPM(vpImage<unsigned char> &I, const char *filename) ;
  static
  void readPPM(vpImage<unsigned char> &I, const std::string filename) ;
  static
  void readPPM(vpImage<vpRGBa> &I, const char *filename) ;
  static
  void readPPM(vpImage<vpRGBa> &I, const std::string filename) ;

#if (defined(VISP_HAVE_LIBJPEG) || defined(VISP_HAVE_OPENCV))
  static
  void readJPEG(vpImage<unsigned char> &I, const char *filename) ;
  static
  void readJPEG(vpImage<unsigned char> &I, const std::string filename) ;
  static
  void readJPEG(vpImage<vpRGBa> &I, const char *filename) ;
  static
  void readJPEG(vpImage<vpRGBa> &I, const std::string filename) ;
#endif

#if (defined(VISP_HAVE_LIBPNG) || defined(VISP_HAVE_OPENCV))
  static
  void readPNG(vpImage<unsigned char> &I, const char *filename) ;
  static
  void readPNG(vpImage<unsigned char> &I, const std::string filename) ;
  static
  void readPNG(vpImage<vpRGBa> &I, const char *filename) ;
  static
  void readPNG(vpImage<vpRGBa> &I, const std::string filename) ;
#endif

  static
  void writePFM(const vpImage<float> &I, const char *filename) ;
 


  static
  void writePGM(const vpImage<unsigned char> &I, const char *filename) ;
  static
  void writePGM(const vpImage<unsigned char> &I, const std::string filename) ;
  static
  void writePGM(const vpImage<short> &I, const char *filename) ;
  static
  void writePGM(const vpImage<short> &I, const std::string filename) ;
  static
  void writePGM(const vpImage<vpRGBa> &I, const char *filename) ;
  static
  void writePGM(const vpImage<vpRGBa> &I, const std::string filename) ;

  static
  void writePPM(const vpImage<unsigned char> &I, const char *filename) ;
  static
  void writePPM(const vpImage<unsigned char> &I, const std::string filename) ;
  static
  void writePPM(const vpImage<vpRGBa> &I, const char *filename) ;
  static
  void writePPM(const vpImage<vpRGBa> &I, const std::string filename) ;

#if (defined(VISP_HAVE_LIBJPEG) || defined(VISP_HAVE_OPENCV))
  static
  void writeJPEG(const vpImage<unsigned char> &I, const char *filename) ;
  static
  void writeJPEG(const vpImage<unsigned char> &I, const std::string filename) ;
  static
  void writeJPEG(const vpImage<vpRGBa> &I, const char *filename) ;
  static
  void writeJPEG(const vpImage<vpRGBa> &I, const std::string filename) ;
#endif

#if (defined(VISP_HAVE_LIBPNG) || defined(VISP_HAVE_OPENCV))
  static
  void writePNG(const vpImage<unsigned char> &I, const char *filename) ;
  static
  void writePNG(const vpImage<unsigned char> &I, const std::string filename) ;
  static
  void writePNG(const vpImage<vpRGBa> &I, const char *filename) ;
  static
  void writePNG(const vpImage<vpRGBa> &I, const std::string filename) ;
#endif

  } ;
#endif

