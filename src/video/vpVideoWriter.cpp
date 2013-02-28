/****************************************************************************
 *
 * $Id: vpImagePoint.h 2359 2009-11-24 15:09:25Z nmelchio $
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
 * Write image sequences.
 *
 * Authors:
 * Nicolas Melchior
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \file vpVideoWriter.cpp
  \brief Write image sequences.
*/

#include <visp/vpDebug.h>
#include <visp/vpVideoWriter.h>

/*!
  Basic constructor.
*/
vpVideoWriter::vpVideoWriter()
{
  initFileName = false;
  firstFrame = 0;
  frameCount = 0;
  
  #ifdef VISP_HAVE_FFMPEG
  ffmpeg = NULL;
  codec = CODEC_ID_MPEG1VIDEO;
  bit_rate = 500000;
  #endif
}


/*!
  Basic destructor.
*/
vpVideoWriter::~vpVideoWriter()
{
  #ifdef VISP_HAVE_FFMPEG
  if (ffmpeg != NULL)
    delete ffmpeg;
  #endif
}


/*!
  It enables to set the path and the name of the files which will be saved.
  
  If you want to write a sequence of images, \f$ filename \f$ corresponds to the path followed by the image name template. For exemple, if you want to write different images named image0001.jpeg, image0002.jpg, ... and located in the folder /local/image, \f$ filename \f$ will be "/local/image/image%04d.jpg". 
  
  \param filename : filename template of an image sequence.
*/
void vpVideoWriter::setFileName(const char *filename)
{
  if (filename == '\0')
  {
    vpERROR_TRACE("filename empty ") ;
    throw (vpImageException(vpImageException::noFileNameError,"filename empty ")) ;
  }
  
  strcpy(this->fileName,filename);
  
  formatType = getFormat(fileName);
  
  initFileName = true;
}


/*!
  Sets all the parameters needed to write the video or the image sequence.
  
  \param I : One image with the right dimensions.
*/
void vpVideoWriter::open(vpImage< vpRGBa > &I)
{
  if (!initFileName)
  {
    vpERROR_TRACE("The generic filename has to be set");
    throw (vpImageException(vpImageException::noFileNameError,"filename empty"));
  }
  
  if (formatType == FORMAT_PGM ||
      formatType == FORMAT_PPM ||
      formatType == FORMAT_JPEG ||
      formatType == FORMAT_PNG)
  {
    width = I.getWidth();
    height = I.getHeight();
  }
  #ifdef VISP_HAVE_FFMPEG
  else if (formatType == FORMAT_AVI ||
           formatType == FORMAT_MPEG ||
           formatType == FORMAT_MOV)
  {
    ffmpeg = new vpFFMPEG;
    ffmpeg->setBitRate(bit_rate);
    if(!ffmpeg->openEncoder(fileName, I.getWidth(), I.getHeight(), codec))
      throw (vpException(vpException::ioError ,"Could not open the video"));
  }
  
  #else
  else if (formatType == FORMAT_AVI ||
           formatType == FORMAT_MPEG ||
           formatType == FORMAT_MOV)
  {
    vpERROR_TRACE("To write video files the FFmpeg library has to be installed");
    throw (vpException(vpException::fatalError ,"the FFmpeg library is required"));
  }
  #endif
  
  frameCount = firstFrame;
  
  isOpen = true;
}


/*!
  Sets all the parameters needed to write the video or the image sequence.
  
  \param I : One image with the right dimensions.
*/
void vpVideoWriter::open(vpImage< unsigned char > &I)
{
  if (!initFileName)
  {
    vpERROR_TRACE("The generic filename has to be set");
    throw (vpImageException(vpImageException::noFileNameError,"filename empty"));
  }
  
  if (formatType == FORMAT_PGM ||
      formatType == FORMAT_PPM ||
      formatType == FORMAT_JPEG ||
      formatType == FORMAT_PNG)
  {
    width = I.getWidth();
    height = I.getHeight();
  }
  #ifdef VISP_HAVE_FFMPEG
  else if (formatType == FORMAT_AVI ||
           formatType == FORMAT_MPEG ||
           formatType == FORMAT_MOV)
  {
    ffmpeg = new vpFFMPEG;
    ffmpeg->setBitRate(bit_rate);
    if(!ffmpeg->openEncoder(fileName, I.getWidth(), I.getHeight(), codec))
      throw (vpException(vpException::ioError ,"Could not open the video"));
  }
  
  #else
  else if (formatType == FORMAT_AVI ||
           formatType == FORMAT_MPEG ||
           formatType == FORMAT_MOV)
  {
    vpERROR_TRACE("To write video files the FFmpeg library has to be installed");
    throw (vpException(vpException::fatalError ,"the FFmpeg library is required"));
  }
  #endif
  
  frameCount = firstFrame;
  
  isOpen = true;
}


/*!
  Saves the image as a frame of the video or as an image belonging to the image sequence.
 
  Each time this method is used, the frame counter is incremented and thus the file name change for the case of an image sequence.
 
  \param I : The image which has to be saved
*/
void vpVideoWriter::saveFrame (vpImage< vpRGBa > &I)
{
  if (!isOpen)
  {
    vpERROR_TRACE("The video has to be open first with the open method");
    throw (vpException(vpException::notInitialized,"file not yet opened"));
  }

  
  if (formatType == FORMAT_PGM ||
      formatType == FORMAT_PPM ||
      formatType == FORMAT_JPEG ||
      formatType == FORMAT_PNG)
  {
    char name[FILENAME_MAX];

    sprintf(name,fileName,frameCount);

    vpImageIo::write(I, name);
  }
  
  #ifdef VISP_HAVE_FFMPEG
  else
  {
    ffmpeg->saveFrame(I);
  }
  #endif

  frameCount++;
}


/*!
  Saves the image as a frame of the video or as an image belonging to the image sequence.
 
  Each time this method is used, the frame counter is incremented and thus the file name change for the case of an image sequence.
 
  \param I : The image which has to be saved
*/
void vpVideoWriter::saveFrame (vpImage< unsigned char > &I)
{
  if (!isOpen)
  {
    vpERROR_TRACE("The video has to be open first with the open method");
    throw (vpException(vpException::notInitialized,"file not yet opened"));
  }

  if (formatType == FORMAT_PGM ||
      formatType == FORMAT_PPM ||
      formatType == FORMAT_JPEG ||
      formatType == FORMAT_PNG)
  {
    char name[FILENAME_MAX];

    sprintf(name,fileName,frameCount);

    vpImageIo::write(I, name);
  }
  
  #ifdef VISP_HAVE_FFMPEG
  else
  {
    ffmpeg->saveFrame(I);
  }
  #endif

  frameCount++;
}


/*!
  Deallocates parameters use to write the video or the image sequence.
*/
void vpVideoWriter::close()
{
  if (!isOpen)
  {
    vpERROR_TRACE("The video has to be open first with the open method");
    throw (vpException(vpException::notInitialized,"file not yet opened"));
  }
  #ifdef VISP_HAVE_FFMPEG
  if (ffmpeg != NULL)
  {
    ffmpeg->endWrite();
  }
  #endif
}


/*!
  Gets the format of the file(s) which has/have to be written.
  
  \return Returns the format.
*/
vpVideoWriter::vpVideoFormatType
vpVideoWriter::getFormat(const char *filename)
{
  std::string sfilename(filename);

  std::string ext = vpVideoWriter::getExtension(sfilename);

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
  else if (ext.compare(".AVI") == 0)
    return FORMAT_AVI;
  else if (ext.compare(".avi") == 0)
    return FORMAT_AVI;
  else if (ext.compare(".MPEG") == 0)
    return FORMAT_MPEG;
  else if (ext.compare(".mpeg") == 0)
    return FORMAT_MPEG;
  else if (ext.compare(".MPG") == 0)
    return FORMAT_MPEG;
  else if (ext.compare(".mpg") == 0)
    return FORMAT_MPEG;
  else if (ext.compare(".MOV") == 0)
    return FORMAT_MOV;
  else if (ext.compare(".mov") == 0)
    return FORMAT_MOV;
  else
    return FORMAT_UNKNOWN;
}

// return the extension of the file including the dot
std::string vpVideoWriter::getExtension(const std::string &filename)
{
  // extract the extension
  size_t dot = filename.find_last_of(".");
  std::string ext = filename.substr(dot, filename.size()-1);
  return ext;
}
