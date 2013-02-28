/****************************************************************************
 *
 * $Id: vpImageIo.cpp 3793 2012-06-14 10:13:30Z fspindle $
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
  \file vpImageIo.cpp
  \brief Read/write images
*/

#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpImageConvert.h> //image  conversion

const int vpImageIo::vpMAX_LEN = 100;

/*!

  Open a file with read access.

  \param filename : Name of the file to open.

  \return File descriptor.
*/
FILE *
vpImageIo::openFileRead(const char *filename)
{

  FILE *fd ;

  // Lecture du nom du fichier image.
  if (filename == '\0')   {
    vpERROR_TRACE("filename empty ") ;
    throw (vpImageException(vpImageException::noFileNameError,
			    "filename empty ")) ;
  }

  // Ouverture de l'image.
  if ((fd = fopen(filename, "r")) == NULL)
  {
    vpERROR_TRACE("cannot open file") ;
    throw (vpImageException(vpImageException::ioError,
			    "cannot open file")) ;
  }
  return fd ;
}

/*!

  Open a file with write access.

  \param filename : Name of the file to open.

  \param mode : Access mode. By default set to "w" for write
  access. Could be changed to set for example the access mode to "wa"
  to append data in the file.

  \return File descriptor.
*/
FILE *
vpImageIo::openFileWrite(const char *filename, const char *mode)
{
  FILE *fd ;

 // Lecture du nom du fichier image.
  if (filename == '\0')
  {
    vpERROR_TRACE("filename empty ") ;
    throw (vpImageException(vpImageException::noFileNameError,
			    "filename empty ")) ;
  }

  // Ouverture de l'image.
  if ((fd = fopen(filename, mode)) == NULL)
  {
    vpERROR_TRACE("cannot open file") ;
    throw (vpImageException(vpImageException::ioError,
			    "cannot open file")) ;
  }
  return fd ;
}

/*!

  Open a file with read access.

  \param filename : Name of the file to open.

  \return File descriptor.
*/
FILE *
vpImageIo::openFileRead(const std::string filename)
{

  FILE *fd ;

  // Lecture du nom du fichier image.
  if (filename.empty()) {
    vpERROR_TRACE("filename empty ") ;
    throw (vpImageException(vpImageException::noFileNameError,
			    "filename empty ")) ;
  }
  
  // Ouverture de l'image.
  if ((fd = fopen(filename.c_str(), "r")) == NULL)
  {
    vpERROR_TRACE("cannot open file") ;
    throw (vpImageException(vpImageException::ioError,
			    "cannot open file")) ;
  }
  return fd ;
}

/*!

  Open a file with write access.

  \param filename : Name of the file to open.

  \param mode : Access mode. By default set to "w" for write
  access. Could be changed to set for example the access mode to "wa"
  to append data in the file.

  \return File descriptor.
*/
FILE *
vpImageIo::openFileWrite(const std::string filename,
			 const std::string mode)
{
  FILE *fd ;

 // Lecture du nom du fichier image.
  if (filename.empty())
  {
    vpERROR_TRACE("filename empty ") ;
    throw (vpImageException(vpImageException::noFileNameError,
			    "filename empty ")) ;
  }

  // Ouverture de l'image.
  if ((fd = fopen(filename.c_str(), mode.c_str())) == NULL)
  {
    vpERROR_TRACE("cannot open file") ;
    throw (vpImageException(vpImageException::ioError,
			    "cannot open file")) ;
  }
  return fd ;
}

vpImageIo::vpImageFormatType
vpImageIo::getFormat(const char *filename)
{
  std::string sfilename(filename);

  std::string ext = vpImageIo::getExtension(sfilename);

  if (ext.compare(".PGM") == 0)
    return FORMAT_PGM;
  else if (ext.compare(".pgm") == 0)
    return FORMAT_PGM;
  else if (ext.compare(".PPM") == 0)
    return FORMAT_PPM;
  else if (ext.compare(".ppm") == 0)
    return FORMAT_PPM;
  else if (ext.compare(".JPG") == 0)
    return FORMAT_JPEG;
  else if (ext.compare(".jpg") == 0)
    return FORMAT_JPEG;
  else if (ext.compare(".JPEG") == 0)
    return FORMAT_JPEG;
  else if (ext.compare(".jpeg") == 0)
    return FORMAT_JPEG;
  else if (ext.compare(".PNG") == 0)
    return FORMAT_PNG;
  else if (ext.compare(".png") == 0)
    return FORMAT_PNG;
  else
    return FORMAT_UNKNOWN;
}

// return the extension of the file including the dot
std::string vpImageIo::getExtension(const std::string &filename)
{
  // extract the extension
  size_t dot = filename.find_last_of(".");
  std::string ext = filename.substr(dot, filename.size()-1);
  return ext;
}


/*!
  Read the contents of the file,
  allocate memory for the corresponding greyscale image.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.
 */
void
vpImageIo::read(vpImage<unsigned char> &I, const char *filename)
{
  switch(getFormat(filename)){
    case FORMAT_PGM :
      readPGM(I,filename); break;
    case FORMAT_PPM :
      readPPM(I,filename); break;
  case FORMAT_JPEG :
#if (defined(VISP_HAVE_LIBJPEG) || defined(VISP_HAVE_OPENCV))
      readJPEG(I,filename); break;
#else
      vpCERROR << "You need the libjpeg library to open JPEG files "
         << std::endl;
      break;
#endif
  case FORMAT_PNG :
#if (defined(VISP_HAVE_LIBPNG) || defined(VISP_HAVE_OPENCV))
      readPNG(I,filename); break;
#else
      vpCERROR << "You need the libpng library to open PNG files "
         << std::endl;
      break;
#endif
    case FORMAT_UNKNOWN :
      vpCERROR << "Error: Only PNM (PGM P5 and PPM P6), JPEG and PNG " << std::endl
          << " image format are implemented..." << std::endl;
      throw (vpImageException(vpImageException::ioError,
             "cannot read file")) ;
      break;
  }
}
/*!
  Read the contents of the file,
  allocate memory for the corresponding greyscale image.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.
 */
void
vpImageIo::read(vpImage<unsigned char> &I, const std::string filename)
{
  read(I,filename.c_str());
}
/*!
  Read the contents of the file,
  allocate memory for the corresponding vpRGBa image.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.
 */
void
vpImageIo::read(vpImage<vpRGBa> &I, const char *filename)
{
  switch(getFormat(filename)){
    case FORMAT_PGM :
      readPGM(I,filename); break;
    case FORMAT_PPM :
      readPPM(I,filename); break;
  case FORMAT_JPEG :
#if (defined(VISP_HAVE_LIBJPEG) || defined(VISP_HAVE_OPENCV))
      readJPEG(I,filename); break;
#else
      vpCERROR << "You need the libjpeg library to open JPEG files "
         << std::endl;
      break;
#endif
  case FORMAT_PNG :
#if (defined(VISP_HAVE_LIBPNG) || defined(VISP_HAVE_OPENCV))
      readPNG(I,filename); break;
#else
      vpCERROR << "You need the libpng library to open PNG files "
         << std::endl;
      break;
#endif
    case FORMAT_UNKNOWN :
      vpCERROR << "Error: Only PNM (PGM P5 and PPM P6), JPEG and PNG " << std::endl
          << " image format are implemented..." << std::endl;
      throw (vpImageException(vpImageException::ioError,
             "cannot read file")) ;
      break;
  }
}
/*!
  Read the contents of the file,
  allocate memory for the corresponding vpRGBa image.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.
 */
void
vpImageIo::read(vpImage<vpRGBa> &I, const std::string filename)
{
  read(I,filename.c_str());
}

/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a PNM (PGM P5 or PPM P6) or a JPEG file depending on the
  filename extension.

  \param I : Image to save as a PNM or a JPEG file.
  \param filename : Name of the file containing the image.
 */
void
vpImageIo::write(const vpImage<unsigned char> &I, const char *filename)
{
  switch(getFormat(filename)){
  case FORMAT_PGM :
    writePGM(I,filename); break;
  case FORMAT_PPM :
    writePPM(I,filename); break;
  case FORMAT_JPEG :
#if (defined(VISP_HAVE_LIBJPEG) || defined(VISP_HAVE_OPENCV))
    writeJPEG(I,filename); break;
#else
    vpCERROR << "You need the libjpeg library to write JPEG files "
         << std::endl;
    break;
#endif
  case FORMAT_PNG :
#if (defined(VISP_HAVE_LIBPNG) || defined(VISP_HAVE_OPENCV))
    writePNG(I,filename); break;
#else
    vpCERROR << "You need the libpng library to write PNG files "
         << std::endl;
    break;
#endif
  case FORMAT_UNKNOWN :
    vpCERROR << "Error: Only PNM (PGM P5 and PPM P6) JPEG and PNG " << std::endl
       << " image format are implemented..." << std::endl;
    throw (vpImageException(vpImageException::ioError,
          "cannot write file")) ;
    break;
  }
}
/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a PNM (PGM P5 or PPM P6) or a JPEG file depending on the
  filename extension.

  \param I : Image to save as a PNM or a JPEG file.
  \param filename : Name of the file containing the image.
 */
void
vpImageIo::write(const vpImage<unsigned char> &I, const std::string filename)
{
  write(I,filename.c_str());
}
/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a PNM (PGM P5 or PPM P6) or a JPEG file depending on the
  filename extension.

  \param I : Image to save as a PNM or a JPEG file.
  \param filename : Name of the file containing the image.
 */
void
vpImageIo::write(const vpImage<vpRGBa> &I, const char *filename)
{
  switch(getFormat(filename)){
  case FORMAT_PGM :
    writePGM(I,filename); break;
  case FORMAT_PPM :
    writePPM(I,filename); break;
  case FORMAT_JPEG :
#if (defined(VISP_HAVE_LIBJPEG) || defined(VISP_HAVE_OPENCV))
    writeJPEG(I,filename); break;
#else
      vpCERROR << "You need the libjpeg library to write JPEG files "
         << std::endl;
      break;
#endif
  case FORMAT_PNG :
#if (defined(VISP_HAVE_LIBPNG) || defined(VISP_HAVE_OPENCV))
    writePNG(I,filename); break;
#else
      vpCERROR << "You need the libpng library to write PNG files "
         << std::endl;
      break;
#endif
  case FORMAT_UNKNOWN :
    vpCERROR << "Error: Only PNM (PGM P5 and PPM P6), JPEG and PNG " << std::endl
       << " image format are implemented..." << std::endl;
    throw (vpImageException(vpImageException::ioError,
          "cannot write file")) ;
    break;
  }
}
/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a PNM (PGM P5 or PPM P6) or a JPEG file depending on the
  filename extension.

  \param I : Image to save as a PNM or a JPEG file.
  \param filename : Name of the file containing the image.
 */
void
vpImageIo::write(const vpImage<vpRGBa> &I, const std::string filename)
{
  write(I,filename.c_str());
}
//--------------------------------------------------------------------------
// PFM
//--------------------------------------------------------------------------

/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function is built like portable gray pixmap (eg PGM P5) file.
  but considers float image data.

  \param I : Image to save as a (PFM P8) file.
  \param filename : Name of the file containing the image.
*/

void
vpImageIo::writePFM(const vpImage<float> &I,
        const char *filename)
{

  FILE* fd;

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  fd = fopen(filename, "wb");

  if (fd == NULL) {
     vpERROR_TRACE("couldn't write to file \"%s\"\n",  filename);
    throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  // Write the head
  fprintf(fd, "P8\n");					// Magic number
  fprintf(fd, "%d %d\n", I.getWidth(), I.getHeight());	// Image size
  fprintf(fd, "255\n");					// Max level

  // Write the bitmap
  size_t ierr;
  size_t nbyte = I.getWidth()*I.getHeight();

  ierr = fwrite(I.bitmap, sizeof(float), nbyte, fd) ;
  if (ierr != nbyte) {
    fclose(fd);
    vpERROR_TRACE("couldn't write %d bytes to file \"%s\"\n",
      nbyte, filename) ;
    throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  fflush(fd);
  fclose(fd);

}
//--------------------------------------------------------------------------
// PGM
//--------------------------------------------------------------------------

/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PGM P5) file.

  \param I : Image to save as a (PGM P5) file.
  \param filename : Name of the file containing the image.
*/

void
vpImageIo::writePGM(const vpImage<unsigned char> &I,
        const char *filename)
{

  FILE* fd;

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  fd = fopen(filename, "wb");

  if (fd == NULL) {
     vpERROR_TRACE("couldn't write to file \"%s\"\n",  filename);
    throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  // Write the head
  fprintf(fd, "P5\n");					// Magic number
  fprintf(fd, "%d %d\n", I.getWidth(), I.getHeight());	// Image size
  fprintf(fd, "255\n");					// Max level

  // Write the bitmap
  size_t ierr;
  size_t nbyte = I.getWidth()*I.getHeight();

  ierr = fwrite(I.bitmap, sizeof(unsigned char), nbyte, fd) ;
  if (ierr != nbyte) {
    fclose(fd);
    vpERROR_TRACE("couldn't write %d bytes to file \"%s\"\n",
      nbyte, filename) ;
    throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  fflush(fd);
  fclose(fd);

}
/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PGM P5) file.

  \param I : Image to save as a (PGM P5) file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePGM(const vpImage<short> &I, const char *filename)
{
  vpImage<unsigned char> Iuc ;
  unsigned int nrows = I.getHeight();
  unsigned int ncols = I.getWidth();

  Iuc.resize(nrows, ncols);

  for (unsigned int i=0 ; i < nrows * ncols ; i++)
    Iuc.bitmap[i] =  (unsigned char)I.bitmap[i] ;

  vpImageIo::writePGM(Iuc, filename) ;


}
/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PGM P5) file.
  Color image is converted into a grayscale image.

  \param I : Image to save as a (PGM P5) file.
  \param filename : Name of the file containing the image.
*/

void
vpImageIo::writePGM(const vpImage<vpRGBa> &I, const char *filename)
{

  FILE* fd;

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  fd = fopen(filename, "wb");

  if (fd == NULL) {
     vpERROR_TRACE("couldn't write to file \"%s\"\n",  filename);
    throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  // Write the head
  fprintf(fd, "P5\n");					// Magic number
  fprintf(fd, "%d %d\n", I.getWidth(), I.getHeight());	// Image size
  fprintf(fd, "255\n");					// Max level

  // Write the bitmap
  size_t ierr;
  size_t nbyte = I.getWidth()*I.getHeight();


  vpImage<unsigned char> Itmp ;
  vpImageConvert::convert(I,Itmp) ;

  ierr = fwrite(Itmp.bitmap, sizeof(unsigned char), nbyte, fd) ;
  if (ierr != nbyte) {
    fclose(fd);
    vpERROR_TRACE("couldn't write %d bytes to file \"%s\"\n",
      nbyte, filename) ;
    throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  fflush(fd);
  fclose(fd);

}

/*!
  Read a PGM P5 file and initialize a scalar image.

  Read the contents of the portable gray pixmap (PGM P5) filename, allocate
  memory for the corresponding image, and set the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/

void
vpImageIo::readPFM(vpImage<float> &I, const char *filename)
{
  FILE* fd = NULL; // File descriptor
  int   ierr;
  int   line;
  int   is255;
  char* err ;
  char  str[vpMAX_LEN];
  unsigned int   w, h;

  // Test the filename
  if (filename == '\0')
  {
    vpERROR_TRACE("no filename") ;
    throw (vpImageException(vpImageException::ioError,
          " no filename")) ;

  }

  // Open the filename
  fd = fopen(filename, "rb");
  if (fd == NULL)
  {
    vpERROR_TRACE("couldn't read file \"%s\"", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  // Read the first line with magic number P5
  line = 0;

  err = fgets(str, vpMAX_LEN - 1, fd);
  line++;
  if (err == NULL)
  {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",  line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if (strlen(str) < 3)
  {
    fclose (fd);
    vpERROR_TRACE("\"%s\" is not a PGM file\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "this is not a pfm file")) ;
  }

  str[2] = '\0';
  if (strcmp(str, "P8") != 0)
  {
    fclose (fd);
    vpERROR_TRACE("\"%s\" is not a PFM file\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "this is not a pgm file")) ;
  }

  // Jump the possible comment, or empty line and read the following line
  do {
    err = fgets(str, vpMAX_LEN - 1, fd);
    line++;
    if (err == NULL) {
      fprintf(stderr, "couldn't read line %d of file \"%s\"\n", line, filename);
      fclose (fd);
    }
  } while ((str[0] == '#') || (str[0] == '\n'));

  // Extract image size
  ierr = sscanf(str, "%d %d", &w, &h);
  if(ierr == 1){// the norm allows to have the two values on two separated lines.
    do {
      err = fgets(str, vpMAX_LEN - 1, fd);
      line++;
      if (err == NULL) {
        fprintf(stderr, "couldn't read line %d of file \"%s\"\n", line, filename);
        fclose (fd);
      }
    } while ((str[0] == '#') || (str[0] == '\n'));
    ierr = sscanf(str, "%d", &h);
  }
  if (ierr == EOF)
  {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if ((h != I.getHeight())||( w != I.getWidth()))
  {

    try
    {
      I.resize(h,w) ;
    }
    catch(...)
    {
      vpERROR_TRACE(" ") ;
      throw ;
    }
  }

  // Read 255
  err = fgets(str, vpMAX_LEN - 1, fd);
  line++;
  if (err == NULL) {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  ierr = sscanf(str, "%d", &is255);
  if (ierr == EOF) {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n", line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if (is255 != 255)
  {
    fclose (fd);
    vpERROR_TRACE("MAX_VAL is not 255 in file \"%s\"\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "error reading pfm file")) ;
  }

  unsigned int nbyte = I.getHeight()*I.getWidth();
  if (fread (I.bitmap, sizeof(float), nbyte, fd ) != nbyte)
  {
    fclose (fd);
    vpERROR_TRACE("couldn't read %d bytes in file \"%s\"\n", nbyte, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "error reading pfm file")) ;
  }

  fclose (fd);


}



/*!
  Read a PGM P5 file and initialize a scalar image.

  Read the contents of the portable gray pixmap (PGM P5) filename, allocate
  memory for the corresponding image, and set the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/

void
vpImageIo::readPGM(vpImage<unsigned char> &I, const char *filename)
{
  FILE* fd = NULL; // File descriptor
  int   ierr;
  int   line;
  int   is255;
  char* err ;
  char  str[vpMAX_LEN];
  unsigned int   w, h;

  // Test the filename
  if (filename == '\0')
  {
    vpERROR_TRACE("no filename") ;
    throw (vpImageException(vpImageException::ioError,
          " no filename")) ;

  }

  // Open the filename
  fd = fopen(filename, "rb");
  if (fd == NULL)
  {
    vpERROR_TRACE("couldn't read file \"%s\"", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  // Read the first line with magic number P5
  line = 0;

  err = fgets(str, vpMAX_LEN - 1, fd);
  line++;
  if (err == NULL)
  {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",  line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if (strlen(str) < 3)
  {
    fclose (fd);
    vpERROR_TRACE("\"%s\" is not a PGM file\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "this is not a pgm file")) ;
  }

  str[2] = '\0';
  if (strcmp(str, "P5") != 0)
  {
    fclose (fd);
    vpERROR_TRACE("\"%s\" is not a PGM file\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "this is not a pgm file")) ;
  }

  // Jump the possible comment, or empty line and read the following line
  do {
    err = fgets(str, vpMAX_LEN - 1, fd);
    line++;
    if (err == NULL) {
      fprintf(stderr, "couldn't read line %d of file \"%s\"\n", line, filename);
      fclose (fd);
    }
  } while ((str[0] == '#') || (str[0] == '\n'));

  // Extract image size
  ierr = sscanf(str, "%d %d", &w, &h);
  if(ierr == 1){// the norm allows to have the two values on two separated lines.
    do {
      err = fgets(str, vpMAX_LEN - 1, fd);
      line++;
      if (err == NULL) {
        fprintf(stderr, "couldn't read line %d of file \"%s\"\n", line, filename);
        fclose (fd);
      }
    } while ((str[0] == '#') || (str[0] == '\n'));
    ierr = sscanf(str, "%d", &h);
  }
  if (ierr == EOF)
  {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if ((h != I.getHeight())||( w != I.getWidth()))
  {

    try
    {
      I.resize(h,w) ;
    }
    catch(...)
    {
      vpERROR_TRACE(" ") ;
      throw ;
    }
  }

  // Read 255
  err = fgets(str, vpMAX_LEN - 1, fd);
  line++;
  if (err == NULL) {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  ierr = sscanf(str, "%d", &is255);
  if (ierr == EOF) {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n", line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if (is255 != 255)
  {
    fclose (fd);
    vpERROR_TRACE("MAX_VAL is not 255 in file \"%s\"\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "error reading pgm file")) ;
  }

  unsigned int nbyte = I.getHeight()*I.getWidth();
  if (fread (I.bitmap, sizeof(unsigned char), nbyte, fd ) != nbyte)
  {
    fclose (fd);
    vpERROR_TRACE("couldn't read %d bytes in file \"%s\"\n", nbyte, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "error reading pgm file")) ;
  }

  fclose (fd);


}


/*!
  Read a PGM P5 file and initialize a scalar image.

  Read the contents of the portable gray pixmap (PGM P5) filename, allocate
  memory for the corresponding image, and set the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  The gray level image contained in the \e filename is converted in a
  color image in \e I.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/

void
vpImageIo::readPGM(vpImage<vpRGBa> &I, const char *filename)
{

  try
  {
    vpImage<unsigned char> Itmp ;

    vpImageIo::readPGM(Itmp, filename) ;


    vpImageConvert::convert(Itmp, I) ;

  }
  catch(...)
  {
    vpERROR_TRACE(" ") ;
    throw ;
  }
}


//--------------------------------------------------------------------------
// PPM
//--------------------------------------------------------------------------

/*!
  Read the contents of the portable pixmap (PPM P6) filename, allocate memory
  for the corresponding gray level image, convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. The quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readPPM(vpImage<unsigned char> &I, const char *filename)
{

  try
  {
    vpImage<vpRGBa> Itmp ;

    vpImageIo::readPPM(Itmp, filename) ;

    vpImageConvert::convert(Itmp, I) ;
  }
  catch(...)
  {
    vpERROR_TRACE(" ") ;
    throw ;
  }
}


/*!
  Read the contents of the portable pixmap (PPM P6) filename,
  allocate memory for the corresponding vpRGBa image.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readPPM(vpImage<vpRGBa> &I, const char *filename)
{

  FILE* fd = NULL; // File descriptor
  int   ierr;
  int   line;
  int   is255;
  char* err ;
  char  str[vpMAX_LEN];
  unsigned int   w, h;

  // Test the filename
  if (filename == '\0')
  {
    vpERROR_TRACE("no filename") ;
    throw (vpImageException(vpImageException::ioError,
          " no filename")) ;

  }

  // Open the filename
  fd = fopen(filename, "rb");
  if (fd == NULL)
  {
    vpERROR_TRACE("couldn't read file \"%s\"", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  // Read the first line with magic number P5
  line = 0;

  err = fgets(str, vpMAX_LEN - 1, fd);
  line++;
  if (err == NULL)
  {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",  line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if (strlen(str) < 3)
  {
    fclose (fd);
    vpERROR_TRACE("\"%s\" is not a PPM file\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "this is not a ppm file")) ;
  }

  str[2] = '\0';
  if (strcmp(str, "P6") != 0)
  {
    fclose (fd);
    vpERROR_TRACE("\"%s\" is not a PPM file\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "this is not a ppm file")) ;
  }

  // Jump the possible comment, or empty line and read the following line
  do {
    err = fgets(str, vpMAX_LEN - 1, fd);
    line++;
    if (err == NULL) {
      fprintf(stderr, "couldn't read line %d of file \"%s\"\n", line, filename);
      fclose (fd);
    }
  } while ((str[0] == '#') || (str[0] == '\n'));

  // Extract image size
  ierr = sscanf(str, "%d %d", &w, &h);
  if(ierr == 1){// the norm allows to have the two values on two separated lines.
    do {
      err = fgets(str, vpMAX_LEN - 1, fd);
      line++;
      if (err == NULL) {
        fprintf(stderr, "couldn't read line %d of file \"%s\"\n", line, filename);
        fclose (fd);
      }
    } while ((str[0] == '#') || (str[0] == '\n'));
    ierr = sscanf(str, "%d", &h);
  }
  if (ierr == EOF)
  {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if ((h != I.getHeight())||( w != I.getWidth()))
  {

    try
    {
      I.resize(h,w) ;
    }
    catch(...)
    {
      vpERROR_TRACE(" ") ;
      throw ;
    }
  }

  // Read 255
  err = fgets(str, vpMAX_LEN - 1, fd);
  line++;
  if (err == NULL) {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n",line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  ierr = sscanf(str, "%d", &is255);
  if (ierr == EOF) {
    fclose (fd);
    vpERROR_TRACE("couldn't read line %d of file \"%s\"\n", line, filename) ;
    throw (vpImageException(vpImageException::ioError,
          "couldn't read file")) ;
  }

  if (is255 != 255)
  {
    fclose (fd);
    vpERROR_TRACE("MAX_VAL is not 255 in file \"%s\"\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "error reading ppm file")) ;
  }

  for(unsigned int i=0;i<I.getHeight();i++)
  {
    for(unsigned int j=0;j<I.getWidth();j++)
    {
      vpRGBa v ;
      size_t res = fread(&v.R,sizeof(v.R),1,fd) ;
      res |= fread(&v.G,sizeof(v.G),1,fd) ;
      res |= fread(&v.B,sizeof(v.B),1,fd) ;
      if (res==0)
      {
   fclose (fd);
   vpERROR_TRACE("couldn't read  bytes in file \"%s\"\n", filename) ;
   throw (vpImageException(vpImageException::ioError,
         "error reading ppm file")) ;
      }
      I[i][j] = v ;
    }
  }
  fclose(fd) ;

}

/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PPM P6) file.
  grayscale image is converted into a color image vpRGBa.

  \param I : Image to save as a (PPM P6) file.
  \param filename : Name of the file containing the image.

*/

void
vpImageIo::writePPM(const vpImage<unsigned char> &I, const char *filename)
{

  try
  {
    vpImage<vpRGBa> Itmp ;

    vpImageConvert::convert(I, Itmp) ;

    vpImageIo::writePPM(Itmp, filename) ;
  }
  catch(...)
  {
    vpERROR_TRACE(" ") ;
    throw ;
  }
}


/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PPM P6) file.

  \param I : Image to save as a (PPM P6) file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePPM(const vpImage<vpRGBa> &I, const char *filename)
{

  FILE* f;


  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  f = fopen(filename, "wb");

  if (f == NULL) {
     vpERROR_TRACE("couldn't write to file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }



  fprintf(f,"P6\n");			         // Magic number
  fprintf(f,"%d %d\n", I.getWidth(), I.getHeight());	// Image size
  fprintf(f,"%d\n",255);	        	// Max level

  for(unsigned int i=0;i<I.getHeight();i++)
  {
    for(unsigned int j=0;j<I.getWidth();j++)
    {
      vpRGBa P ;
      size_t res ;
      P = I[i][j] ;
      unsigned char   tmp ;
      tmp = P.R ;
      res = fwrite(&tmp,sizeof(tmp),1,f) ;
      if (res==0)
      {
  fclose(f);
  vpERROR_TRACE("couldn't write file") ;
  throw (vpImageException(vpImageException::ioError,
        "cannot write file")) ;
      }
      tmp = P.G;
      res = fwrite(&tmp,sizeof(tmp),1,f) ;
      if (res==0)
      {
  fclose(f);
  vpERROR_TRACE("couldn't write file") ;
  throw (vpImageException(vpImageException::ioError,
        "cannot write file")) ;
      }
      tmp = P.B ;
      res = fwrite(&tmp,sizeof(tmp),1,f) ;
      if (res==0)
      {
  fclose(f);
  vpERROR_TRACE("couldn't write file") ;
  throw (vpImageException(vpImageException::ioError,
        "cannot write file")) ;
      }
    }
  }

  fflush(f);
  fclose(f);
}


/*!
  Read a PGM P5 file and initialize a scalar image.

  Read the contents of the portable gray pixmap (PGM P5) filename, allocate
  memory for the corresponding image, and set the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/

void
vpImageIo::readPGM(vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::readPGM(I, filename.c_str());
}

/*!
  Read a PGM P5 file and initialize a scalar image.

  Read the contents of the portable gray pixmap (PGM P5) filename, allocate
  memory for the corresponding image, and set the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/

void
vpImageIo::readPGM(vpImage<vpRGBa> &I, const  std::string filename)
{
  vpImageIo::readPGM(I, filename.c_str());
}

/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PGM P5) file.

  \param I : Image to save as a (PGM P5) file.
  \param filename : Name of the file containing the image.

*/

void
vpImageIo::writePGM(const vpImage<unsigned char> &I,
        const std::string filename)
{
  vpImageIo::writePGM(I, filename.c_str());
}

/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PGM P5) file.

  \param I : Image to save as a (PGM P5) file.
  \param filename : Name of the file containing the image.
*/

void
vpImageIo::writePGM(const vpImage<short> &I, const std::string filename)
{

  vpImageIo::writePGM(I, filename.c_str());
}

/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PGM P5) file.
  Color image is converted into a grayscale image.

  \param I : Image to save as a (PGM P5) file.
  \param filename : Name of the file containing the image.

*/

void
vpImageIo::writePGM(const vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::writePGM(I, filename.c_str());
}

//--------------------------------------------------------------------------
// PPM
//--------------------------------------------------------------------------

/*!
  Read the contents of the portable pixmap (PPM P6) filename, allocate memory
  for the corresponding gray level image, convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. The quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readPPM(vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::readPPM(I, filename.c_str());
}

/*!
  Read the contents of the portable pixmap (PPM P6) filename,
  allocate memory for the corresponding vpRGBa image.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readPPM(vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::readPPM(I, filename.c_str());
}

/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PPM P6) file.
  grayscale image is converted into a color image vpRGBa.

  \param I : Image to save as a (PPM P6) file.
  \param filename : Name of the file containing the image.

*/

void
vpImageIo::writePPM(const vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::writePPM(I, filename.c_str());
}

/*!
  Write the content of the bitmap in the file which name is given by \e
  filename. This function writes a portable gray pixmap (PPM P6) file.

  \param I : Image to save as a (PPM P6) file.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::writePPM(const vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::writePPM(I, filename.c_str());
}


//--------------------------------------------------------------------------
// JPEG
//--------------------------------------------------------------------------

#if defined(VISP_HAVE_LIBJPEG)

/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a JPEG file.

  \param I : Image to save as a JPEG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writeJPEG(const vpImage<unsigned char> &I, const char *filename)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  file = fopen(filename, "wb");

  if (file == NULL) {
     vpERROR_TRACE("couldn't write file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  unsigned int width = I.getWidth();
  unsigned int height = I.getHeight();

  jpeg_stdio_dest(&cinfo, file);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 1;
  cinfo.in_color_space = JCS_GRAYSCALE;
  jpeg_set_defaults(&cinfo);

  jpeg_start_compress(&cinfo,TRUE);

  unsigned char *line;
  line = new unsigned char[width];
  unsigned char* input = (unsigned char*)I.bitmap;
  while (cinfo.next_scanline < cinfo.image_height)
  {
    for (unsigned int i = 0; i < width; i++)
    {
      line[i] = *(input);
    input++;
    }
  jpeg_write_scanlines(&cinfo, &line, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  delete [] line;
  fclose(file);
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a JPEG file.

  \param I : Image to save as a JPEG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writeJPEG(const vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::writeJPEG(I, filename.c_str());
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a JPEG file.

  \param I : Image to save as a JPEG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writeJPEG(const vpImage<vpRGBa> &I, const char *filename)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  file = fopen(filename, "wb");

  if (file == NULL) {
     vpERROR_TRACE("couldn't write file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  unsigned int width = I.getWidth();
  unsigned int height = I.getHeight();

  jpeg_stdio_dest(&cinfo, file);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);

  jpeg_start_compress(&cinfo,TRUE);

  unsigned char *line;
  line = new unsigned char[3*width];
  unsigned char* input = (unsigned char*)I.bitmap;
  while (cinfo.next_scanline < cinfo.image_height)
  {
    for (unsigned int i = 0; i < width; i++)
    {
      line[i*3] = *(input); input++;
    line[i*3+1] = *(input); input++;
    line[i*3+2] = *(input); input++;
    input++;
    }
  jpeg_write_scanlines(&cinfo, &line, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  delete [] line;
  fclose(file);
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a JPEG file.

  \param I : Image to save as a JPEG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writeJPEG(const vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::writeJPEG(I, filename.c_str());
}


/*!
  Read the contents of the JPEG file, allocate memory
  for the corresponding gray level image, if necessary convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. If necessary, the quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readJPEG(vpImage<unsigned char> &I, const char *filename)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  file = fopen(filename, "rb");

  if (file == NULL) {
     vpERROR_TRACE("couldn't read file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot read file")) ;
  }

  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

  unsigned int width = cinfo.image_width;
  unsigned int height = cinfo.image_height;

  if ( (width != I.getWidth()) || (height != I.getHeight()) )
    I.resize(height,width);

  jpeg_start_decompress(&cinfo);

  unsigned int rowbytes = cinfo.output_width * (unsigned int)(cinfo.output_components);
  JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
                      ((j_common_ptr) &cinfo, JPOOL_IMAGE, rowbytes, 1);

  if (cinfo.out_color_space == JCS_RGB) {
    vpImage<vpRGBa> Ic(height,width);
    unsigned char* output = (unsigned char*)Ic.bitmap;
    while (cinfo.output_scanline<cinfo.output_height)	{
      jpeg_read_scanlines(&cinfo,buffer,1);
      for (unsigned int i = 0; i < width; i++) {
        *(output++) = buffer[0][i*3];
        *(output++) = buffer[0][i*3+1];
        *(output++) = buffer[0][i*3+2];
	*(output++) = 0;
      }
    }
    vpImageConvert::convert(Ic,I) ;
  }

  else if (cinfo.out_color_space == JCS_GRAYSCALE)
  {
    unsigned int row;
    while (cinfo.output_scanline<cinfo.output_height)
    {
      row = cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,buffer,1);
      memcpy(I[row], buffer[0], rowbytes);
    }
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(file);
}


/*!
  Read the contents of the JPEG file, allocate memory
  for the corresponding gray level image, if necessary convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. If necessary, the quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readJPEG(vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::readJPEG(I, filename.c_str());
}


/*!
  Read a JPEG file and initialize a scalar image.

  Read the contents of the JPEG file, allocate
  memory for the corresponding image, and set
  the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  If the file corresponds to a grayscaled image, a conversion is done to deal
  with \e I which is a color image.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readJPEG(vpImage<vpRGBa> &I, const char *filename)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  file = fopen(filename, "rb");

  if (file == NULL) {
     vpERROR_TRACE("couldn't read file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot read file")) ;
  }

  jpeg_stdio_src(&cinfo, file);

  jpeg_read_header(&cinfo, TRUE);

  unsigned int width = cinfo.image_width;
  unsigned int height = cinfo.image_height;

  if ( (width != I.getWidth()) || (height != I.getHeight()) )
    I.resize(height,width);

  jpeg_start_decompress(&cinfo);

  unsigned int rowbytes = cinfo.output_width * (unsigned int)(cinfo.output_components);
  JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
                      ((j_common_ptr) &cinfo, JPOOL_IMAGE, rowbytes, 1);

  if (cinfo.out_color_space == JCS_RGB)
  {
    unsigned char* output = (unsigned char*)I.bitmap;
    while (cinfo.output_scanline<cinfo.output_height)
    {
      jpeg_read_scanlines(&cinfo,buffer,1);
      for (unsigned int i = 0; i < width; i++) {
        *(output++) = buffer[0][i*3];
        *(output++) = buffer[0][i*3+1];
        *(output++) = buffer[0][i*3+2];
	*(output++) = 0;
      }
    }
  }

  else if (cinfo.out_color_space == JCS_GRAYSCALE)
  {
    vpImage<unsigned char> Ig(height,width);

    unsigned int row;
    while (cinfo.output_scanline<cinfo.output_height)
    {
      row = cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,buffer,1);
      memcpy(Ig[row], buffer[0], rowbytes);
    }

    vpImageConvert::convert(Ig,I) ;
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(file);
}


/*!
  Read a JPEG file and initialize a scalar image.

  Read the contents of the JPEG file, allocate
  memory for the corresponding image, and set
  the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  If the file corresponds to a grayscaled image, a conversion is done to deal
  with \e I which is a color image.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readJPEG(vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::readJPEG(I, filename.c_str());
}

#elif defined(VISP_HAVE_OPENCV)

/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a JPEG file.

  \param I : Image to save as a JPEG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writeJPEG(const vpImage<unsigned char> &I, const char *filename)
{
  IplImage* Ip = NULL;
  vpImageConvert::convert(I, Ip);

  cvSaveImage(filename, Ip);

  cvReleaseImage(&Ip);
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a JPEG file.

  \param I : Image to save as a JPEG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writeJPEG(const vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::writeJPEG(I, filename.c_str());
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a JPEG file.

  \param I : Image to save as a JPEG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writeJPEG(const vpImage<vpRGBa> &I, const char *filename)
{
  IplImage* Ip = NULL;
  vpImageConvert::convert(I, Ip);

  cvSaveImage(filename, Ip);

  cvReleaseImage(&Ip);
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a JPEG file.

  \param I : Image to save as a JPEG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writeJPEG(const vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::writeJPEG(I, filename.c_str());
}


/*!
  Read the contents of the JPEG file, allocate memory
  for the corresponding gray level image, if necessary convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. If necessary, the quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readJPEG(vpImage<unsigned char> &I, const char *filename)
{
  IplImage* Ip = NULL;
  Ip = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
  if (Ip != NULL)
    vpImageConvert::convert(Ip, I);
  else
    throw (vpImageException(vpImageException::ioError,
           "Can't read the image")) ;
  cvReleaseImage(&Ip);
}


/*!
  Read the contents of the JPEG file, allocate memory
  for the corresponding gray level image, if necessary convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. If necessary, the quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readJPEG(vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::readJPEG(I, filename.c_str());
}


/*!
  Read a JPEG file and initialize a scalar image.

  Read the contents of the JPEG file, allocate
  memory for the corresponding image, and set
  the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  If the file corresponds to a grayscaled image, a conversion is done to deal
  with \e I which is a color image.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readJPEG(vpImage<vpRGBa> &I, const char *filename)
{
  IplImage* Ip = NULL;
  Ip = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
  if (Ip != NULL)
    vpImageConvert::convert(Ip, I);
  else
    throw (vpImageException(vpImageException::ioError,
           "Can't read the image")) ;
  cvReleaseImage(&Ip);
}


/*!
  Read a JPEG file and initialize a scalar image.

  Read the contents of the JPEG file, allocate
  memory for the corresponding image, and set
  the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  If the file corresponds to a grayscaled image, a conversion is done to deal
  with \e I which is a color image.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readJPEG(vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::readJPEG(I, filename.c_str());
}

#endif






//--------------------------------------------------------------------------
// PNG
//--------------------------------------------------------------------------

#if defined(VISP_HAVE_LIBPNG)

/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a PNG file.

  \param I : Image to save as a PNG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePNG(const vpImage<unsigned char> &I, const char *filename)
{
  FILE *file;

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  file = fopen(filename, "wb");

  if (file == NULL) {
     vpERROR_TRACE("couldn't write file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  /* create a png info struct */
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL, NULL, NULL);
  if (!png_ptr)
  {
    fclose (file);
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
  }

  /* initialize the setjmp for returning properly after a libpng error occured */
  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    vpERROR_TRACE("Error during init_io\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG write error")) ;
  }

  /* setup libpng for using standard C fwrite() function with our FILE pointer */
  png_init_io (png_ptr, file);

  unsigned int width = I.getWidth();
  unsigned int height = I.getHeight();
  int bit_depth = 8;
  int color_type = PNG_COLOR_TYPE_GRAY;
  /* set some useful information from header */

  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    vpERROR_TRACE("Error during write header\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG write error")) ;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height,
         bit_depth, color_type, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  png_bytep* row_ptrs = new png_bytep[height];
  for (unsigned int i = 0; i < height; i++)
    row_ptrs[i] = new png_byte[width];

  unsigned char* input = (unsigned char*)I.bitmap;

  for (unsigned int i = 0; i < height; i++)
  {
    png_byte* row = row_ptrs[i];
    for(unsigned int j = 0; j < width; j++)
    {
      row[j] = *(input);
      input++;
    }
  }

  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    vpERROR_TRACE("Error during write image\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG write error")) ;
  }

  png_write_image(png_ptr, row_ptrs);

  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    vpERROR_TRACE("Error during write end\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG write error")) ;
  }

  png_write_end(png_ptr, NULL);

  for(unsigned int j = 0; j < height; j++)
    delete[] /*(png_byte)*/row_ptrs[j];

  delete[] (png_bytep)row_ptrs;

  png_destroy_write_struct (&png_ptr, &info_ptr);

  fclose(file);
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a PNG file.

  \param I : Image to save as a PNG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePNG(const vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::writePNG(I, filename.c_str());
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a PNG file.

  \param I : Image to save as a PNG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePNG(const vpImage<vpRGBa> &I, const char *filename)
{
  FILE *file;

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  file = fopen(filename, "wb");

  if (file == NULL) {
     vpERROR_TRACE("couldn't write file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot write file")) ;
  }

  /* create a png info struct */
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL, NULL, NULL);
  if (!png_ptr)
  {
    fclose (file);
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
  }

  /* initialize the setjmp for returning properly after a libpng error occured */
  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    vpERROR_TRACE("Error during init_io\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG write error")) ;
  }

  /* setup libpng for using standard C fwrite() function with our FILE pointer */
  png_init_io (png_ptr, file);

  unsigned int width = I.getWidth();
  unsigned int height = I.getHeight();
  int bit_depth = 8;
  int color_type = PNG_COLOR_TYPE_RGB;
  /* set some useful information from header */

  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    vpERROR_TRACE("Error during write header\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG write error")) ;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height,
         bit_depth, color_type, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  png_bytep* row_ptrs = new png_bytep[height];
  for (unsigned int i = 0; i < height; i++)
    row_ptrs[i] = new png_byte[3*width];

  unsigned char* input = (unsigned char*)I.bitmap;;

  for (unsigned int i = 0; i < height; i++)
  {
    png_byte* row = row_ptrs[i];
    for(unsigned int j = 0; j < width; j++)
    {
      row[3*j] = *(input);input++;
      row[3*j+1] = *(input);input++;
      row[3*j+2] = *(input);input++;
      input++;
    }
  }

  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    vpERROR_TRACE("Error during write image\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG write error")) ;
  }

  png_write_image(png_ptr, row_ptrs);

  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_write_struct (&png_ptr, &info_ptr);
    vpERROR_TRACE("Error during write end\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG write error")) ;
  }

  png_write_end(png_ptr, NULL);

  for(unsigned int j = 0; j < height; j++)
    delete[] /*(png_byte)*/row_ptrs[j];

  delete[] (png_bytep)row_ptrs;

  png_destroy_write_struct (&png_ptr, &info_ptr);

  fclose(file);
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a PNG file.

  \param I : Image to save as a PNG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePNG(const vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::writePNG(I, filename.c_str());
}

/*!
  Read the contents of the PNG file, allocate memory
  for the corresponding gray level image, if necessary convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. If necessary, the quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readPNG(vpImage<unsigned char> &I, const char *filename)
{
  FILE *file;
  png_byte magic[8];

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  file = fopen(filename, "rb");

  if (file == NULL) {
     vpERROR_TRACE("couldn't read file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot read file")) ;
  }

  /* read magic number */
  if (fread (magic, 1, sizeof (magic), file) != sizeof (magic))
  {
    fclose (file);
    vpERROR_TRACE("couldn't read magic number in file \"%s\"\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "error reading png file")) ;
  }

  /* check for valid magic number */
  if (png_sig_cmp (magic,0, sizeof (magic)))
  {
    fprintf (stderr, "error: \"%s\" is not a valid PNG image!\n",filename);
    fclose (file);
    throw (vpImageException(vpImageException::ioError,
          "error reading png file")) ;
  }

  /* create a png read struct */
  //printf("version %s\n", PNG_LIBPNG_VER_STRING);
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL)
  {
    fprintf (stderr, "error: can't create a png read structure!\n");
    fclose (file);
    throw (vpImageException(vpImageException::ioError,
          "error reading png file")) ;
  }

  /* create a png info struct */
  png_infop info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL)
  {
    fprintf (stderr, "error: can't create a png info structure!\n");
    fclose (file);
    png_destroy_read_struct (&png_ptr, NULL, NULL);
    throw (vpImageException(vpImageException::ioError,
          "error reading png file")) ;
  }

  /* initialize the setjmp for returning properly after a libpng error occured */
  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    vpERROR_TRACE("Error during init io\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG read error")) ;
  }

  /* setup libpng for using standard C fread() function with our FILE pointer */
  png_init_io (png_ptr, file);

  /* tell libpng that we have already read the magic number */
  png_set_sig_bytes (png_ptr, sizeof (magic));

  /* read png info */
  png_read_info (png_ptr, info_ptr);

  unsigned int width = png_get_image_width(png_ptr, info_ptr);
  unsigned int height = png_get_image_height(png_ptr, info_ptr);

  unsigned int bit_depth, channels, color_type;
  /* get some useful information from header */
  bit_depth = png_get_bit_depth (png_ptr, info_ptr);
  channels = png_get_channels(png_ptr, info_ptr);
  color_type = png_get_color_type (png_ptr, info_ptr);

  /* convert index color images to RGB images */
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb (png_ptr);

  /* convert 1-2-4 bits grayscale images to 8 bits grayscale. */
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand (png_ptr);

//  if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
//    png_set_tRNS_to_alpha (png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_strip_alpha(png_ptr);

  if (bit_depth == 16)
    png_set_strip_16 (png_ptr);
  else if (bit_depth < 8)
    png_set_packing (png_ptr);

  /* update info structure to apply transformations */
  png_read_update_info (png_ptr, info_ptr);

  channels = png_get_channels(png_ptr, info_ptr);

  if ( (width != I.getWidth()) || (height != I.getHeight()) )
    I.resize(height,width);

  png_bytep* rowPtrs = new png_bytep[height];

  unsigned int stride = width * bit_depth * channels / 8;
  unsigned char* data = new  unsigned char[stride * height];

  for (unsigned int  i =0; i < height; i++)
    rowPtrs[i] = (png_bytep)data + (i * stride);

  png_read_image(png_ptr, rowPtrs);

  vpImage<vpRGBa> Ic(height,width);
  unsigned char* output;

  switch (channels)
  {
  case 1:
    output = (unsigned char*)I.bitmap;
    for (unsigned int i = 0; i < width*height; i++)
      {
        *(output++) = data[i];
      }
    break;
  case 2:
    output = (unsigned char*)I.bitmap;
    for (unsigned int i = 0; i < width*height; i++)
      {
        *(output++) = data[i*2];
      }
    break;
  case 3:

    output = (unsigned char*)Ic.bitmap;
      for (unsigned int i = 0; i < width*height; i++)
      {
        *(output++) = data[i*3];
        *(output++) = data[i*3+1];
        *(output++) = data[i*3+2];
        *(output++) = 0;
      }
    vpImageConvert::convert(Ic,I) ;
    break;
  case 4:
    output = (unsigned char*)Ic.bitmap;
      for (unsigned int i = 0; i < width*height; i++)
      {
        *(output++) = data[i*4];
        *(output++) = data[i*4+1];
        *(output++) = data[i*4+2];
        *(output++) = data[i*4+3];
      }
    vpImageConvert::convert(Ic,I) ;
    break;
  }

  delete [] (png_bytep)rowPtrs;
  delete [] data;
  png_read_end (png_ptr, NULL);
  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
  fclose(file);
}


/*!
  Read the contents of the PNG file, allocate memory
  for the corresponding gray level image, if necessary convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. If necessary, the quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readPNG(vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::readPNG(I, filename.c_str());
}


/*!
  Read a PNG file and initialize a scalar image.

  Read the contents of the PNG file, allocate
  memory for the corresponding image, and set
  the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  If the file corresponds to a grayscaled image, a conversion is done to deal
  with \e I which is a color image.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readPNG(vpImage<vpRGBa> &I, const char *filename)
{
  FILE *file;
  png_byte magic[8];

  // Test the filename
  if (filename == '\0')   {
     vpERROR_TRACE("no filename\n");
    throw (vpImageException(vpImageException::ioError,
           "no filename")) ;
  }

  file = fopen(filename, "rb");

  if (file == NULL) {
     vpERROR_TRACE("couldn't read file \"%s\"\n",  filename);
     throw (vpImageException(vpImageException::ioError,
           "cannot read file")) ;
  }

  /* read magic number */
  if (fread (magic, 1, sizeof (magic), file) != sizeof (magic))
  {
    fclose (file);
    vpERROR_TRACE("couldn't read magic number in file \"%s\"\n", filename) ;
    throw (vpImageException(vpImageException::ioError,
          "error reading pgm file")) ;
  }

  /* check for valid magic number */
  if (png_sig_cmp (magic,0, sizeof (magic)))
  {
    fprintf (stderr, "error: \"%s\" is not a valid PNG image!\n",filename);
    fclose (file);
  }

  /* create a png read struct */
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    fclose (file);
  }

  /* create a png info struct */
  png_infop info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
  {
    fclose (file);
    png_destroy_read_struct (&png_ptr, NULL, NULL);
  }

  /* initialize the setjmp for returning properly after a libpng error occured */
  if (setjmp (png_jmpbuf (png_ptr)))
  {
    fclose (file);
    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    vpERROR_TRACE("Error during init io\n");
    throw (vpImageException(vpImageException::ioError,
           "PNG read error")) ;
  }

  /* setup libpng for using standard C fread() function with our FILE pointer */
  png_init_io (png_ptr, file);

  /* tell libpng that we have already read the magic number */
  png_set_sig_bytes (png_ptr, sizeof (magic));

  /* read png info */
  png_read_info (png_ptr, info_ptr);

  unsigned int width = png_get_image_width(png_ptr, info_ptr);
  unsigned int height = png_get_image_height(png_ptr, info_ptr);

  unsigned int bit_depth, channels, color_type;
  /* get some useful information from header */
  bit_depth = png_get_bit_depth (png_ptr, info_ptr);
  channels = png_get_channels(png_ptr, info_ptr);
  color_type = png_get_color_type (png_ptr, info_ptr);

  /* convert index color images to RGB images */
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb (png_ptr);

  /* convert 1-2-4 bits grayscale images to 8 bits grayscale. */
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand (png_ptr);

//  if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
//    png_set_tRNS_to_alpha (png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_strip_alpha(png_ptr);

  if (bit_depth == 16)
    png_set_strip_16 (png_ptr);
  else if (bit_depth < 8)
    png_set_packing (png_ptr);

  /* update info structure to apply transformations */
  png_read_update_info (png_ptr, info_ptr);

  channels = png_get_channels(png_ptr, info_ptr);

  if ( (width != I.getWidth()) || (height != I.getHeight()) )
    I.resize(height,width);

  png_bytep* rowPtrs = new png_bytep[height];

  unsigned int stride = width * bit_depth * channels / 8;
  unsigned char* data = new  unsigned char[stride * height];


  for (unsigned int  i =0; i < height; i++)
    rowPtrs[i] = (png_bytep)data + (i * stride);

  png_read_image(png_ptr, rowPtrs);

  vpImage<unsigned char> Ig(height,width);
  unsigned char* output;

  switch (channels)
  {
  case 1:
    output = (unsigned char*)Ig.bitmap;
    for (unsigned int i = 0; i < width*height; i++)
      {
        *(output++) = data[i];
      }
    vpImageConvert::convert(Ig,I) ;
    break;
  case 2:
    output = (unsigned char*)Ig.bitmap;
    for (unsigned int i = 0; i < width*height; i++)
      {
        *(output++) = data[i*2];
      }
    vpImageConvert::convert(Ig,I) ;
    break;
  case 3:

    output = (unsigned char*)I.bitmap;
      for (unsigned int i = 0; i < width*height; i++)
      {
        *(output++) = data[i*3];
        *(output++) = data[i*3+1];
        *(output++) = data[i*3+2];
        *(output++) = 0;
      }
    break;
  case 4:
    output = (unsigned char*)I.bitmap;
      for (unsigned int i = 0; i < width*height; i++)
      {
        *(output++) = data[i*4];
        *(output++) = data[i*4+1];
        *(output++) = data[i*4+2];
        *(output++) = data[i*4+3];
      }
    break;
  }

  delete [] (png_bytep)rowPtrs;
  delete [] data;
  png_read_end (png_ptr, NULL);
  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
  fclose(file);
}


/*!
  Read a PNG file and initialize a scalar image.

  Read the contents of the PNG file, allocate
  memory for the corresponding image, and set
  the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  If the file corresponds to a grayscaled image, a conversion is done to deal
  with \e I which is a color image.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readPNG(vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::readPNG(I, filename.c_str());
}

#elif defined(VISP_HAVE_OPENCV)

/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a PNG file.

  \param I : Image to save as a PNG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePNG(const vpImage<unsigned char> &I, const char *filename)
{
  IplImage* Ip = NULL;
  vpImageConvert::convert(I, Ip);

  cvSaveImage(filename, Ip);

  cvReleaseImage(&Ip);
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a PNG file.

  \param I : Image to save as a PNG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePNG(const vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::writeJPEG(I, filename.c_str());
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a PNG file.

  \param I : Image to save as a PNG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePNG(const vpImage<vpRGBa> &I, const char *filename)
{
  IplImage* Ip = NULL;
  vpImageConvert::convert(I, Ip);

  cvSaveImage(filename, Ip);

  cvReleaseImage(&Ip);
}


/*!
  Write the content of the image bitmap in the file which name is given by \e
  filename. This function writes a PNG file.

  \param I : Image to save as a PNG file.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::writePNG(const vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::writePNG(I, filename.c_str());
}


/*!
  Read the contents of the PNG file, allocate memory
  for the corresponding gray level image, if necessary convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. If necessary, the quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readPNG(vpImage<unsigned char> &I, const char *filename)
{
  IplImage* Ip = NULL;
  Ip = cvLoadImage(filename, CV_LOAD_IMAGE_GRAYSCALE);
  if (Ip != NULL)
    vpImageConvert::convert(Ip, I);
  else
    throw (vpImageException(vpImageException::ioError,
           "Can't read the image")) ;
  cvReleaseImage(&Ip);
}


/*!
  Read the contents of the PNG file, allocate memory
  for the corresponding gray level image, if necessary convert the data in gray level, and
  set the bitmap whith the gray level data. That means that the image \e I is a
  "black and white" rendering of the original image in \e filename, as in a
  black and white photograph. If necessary, the quantization formula used is \f$0,299 r +
  0,587 g + 0,114 b\f$.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  \param I : Image to set with the \e filename content.
  \param filename : Name of the file containing the image.

*/
void
vpImageIo::readPNG(vpImage<unsigned char> &I, const std::string filename)
{
  vpImageIo::readPNG(I, filename.c_str());
}


/*!
  Read a PNG file and initialize a scalar image.

  Read the contents of the PNG file, allocate
  memory for the corresponding image, and set
  the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  If the file corresponds to a grayscaled image, a conversion is done to deal
  with \e I which is a color image.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readPNG(vpImage<vpRGBa> &I, const char *filename)
{
  IplImage* Ip = NULL;
  Ip = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
  if (Ip != NULL)
    vpImageConvert::convert(Ip, I);
  else
    throw (vpImageException(vpImageException::ioError,
           "Can't read the image")) ;
  cvReleaseImage(&Ip);
}


/*!
  Read a PNG file and initialize a scalar image.

  Read the contents of the PNG file, allocate
  memory for the corresponding image, and set
  the bitmap whith the content of
  the file.

  If the image has been already initialized, memory allocation is done
  only if the new image size is different, else we re-use the same
  memory space.

  If the file corresponds to a grayscaled image, a conversion is done to deal
  with \e I which is a color image.

  \param I : Color image to set with the \e filename content.
  \param filename : Name of the file containing the image.
*/
void
vpImageIo::readPNG(vpImage<vpRGBa> &I, const std::string filename)
{
  vpImageIo::readPNG(I, filename.c_str());
}

#endif


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */



/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
