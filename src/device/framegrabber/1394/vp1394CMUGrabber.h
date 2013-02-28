/****************************************************************************
 *
 * $Id: vp1394CMUGrabber.h 3731 2012-05-14 17:35:14Z fspindle $
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
 * Firewire cameras video capture based on CMU 1394 Digital Camera SDK.
 *
 * Authors:
 * Lucas Lopes Lemos FEMTO-ST, AS2M departement, Besancon
 * Guillaume Laurent FEMTO-ST, AS2M departement, Besancon
 * Fabien Spindler
 *
 *****************************************************************************/

#ifndef vp1394CMUGrabber_h
#define vp1394CMUGrabber_h

#include <visp/vpConfig.h>

#ifdef VISP_HAVE_CMU1394

#include <windows.h>
#include <1394Camera.h> // CMU library

#include <visp/vpImage.h>
#include <visp/vpFrameGrabber.h>
#include <visp/vpFrameGrabberException.h>
#include <visp/vpRGBa.h>


/*!
  \file vp1394CMUGrabber.h
  \brief Firewire cameras video capture based on CMU 1394 Digital Camera SDK.
*/

/*!
  \class vp1394CMUGrabber

  \ingroup Framegrabber CameraDriver

  \brief Firewire cameras video capture based on CMU 1394 Digital Camera SDK.

   This block is based on the CMU 1394 Digital Camera SDK. The CMU 1394 Digital
   Camera Driver must be installed (go to http://www.cs.cmu.edu/~iwan/1394/index.html
   to download it).
   - Parameters:
    - Camera index (0, 1, 2, ... or 10). First camera has index 0.
    - Image format
    - Frame rate. Real frame rate depends on device capacities.
    - Control : shutter speed and gain can be internally set, but it is possible to set manually them.
          * Exposure time register value. Real exposure time depends on device capacities.
          * Gain register value. Real gain depends on device capacities.

  The examples below show how to use this class.

  Example 1:
  \code
#include <iostream>

#include <visp/vpConfig.h>
#include <visp/vpImage.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vp1394CMUGrabber.h>

int main()
{
#if defined(VISP_HAVE_CMU1394)
  std::cout << "ViSP Image acquisition example" <<std::endl;

  vpImage<unsigned char> I;
  vp1394CMUGrabber g;

  g.open(I);
  g.acquire(I);

  vpDisplayOpenCV d(I);
    vpDisplay::display(I);

  for(;;)
  {
    g.acquire(I);
    vpDisplay::display(I);
    vpDisplay::flush(I);
    if (vpDisplay::getClick(I, false)) // a click to exit
        break;
  }

  g.close();
#endif
  std::cout << "ViSP exiting..." <<std::endl;
  return 0;
}
  \endcode

  Example 2:
  \code
#include <iostream>

#include <visp/vpConfig.h>
#include <visp/vpImage.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vp1394CMUGrabber.h>

int main()
{
#if defined(VISP_HAVE_CMU1394)
  std::cout << "ViSP Image acquisition example" << std::endl;

  vpImage<unsigned char> I;
  vp1394CMUGrabber g;

  if( g.getNumberOfConnectedCameras() > 1 )
    std::cout << "There are " << g.getNumberOfConnectedCameras() << " connected cameras." << std::endl;
  if( g.getNumberOfConnectedCameras() == 1 )
    std::cout << "There is " << g.getNumberOfConnectedCameras() << " connected camera." << std::endl;
  else
    std::cout << "There is no connected camera." << std::endl;

  // Setting camera parameters manually
  g.selectCamera(0);
  g.setControl(0, 2000);
  g.setFramerate(3);    // 15 FPS
  g.setVideoMode(0, 5); // 640x480 - MONO

  g.open(I);
  g.acquire(I);

  // Display camera description
  g.displayCameraDescription(0);
  g.displayCameraModel();
  std::cout << "Height: " << g.getHeight() << " Width: " << g.getWidth() << std::endl;

  vpDisplayOpenCV d(I);
  vpDisplay::display(I);

  for(;;)
  {
    g.acquire(I);
    vpDisplay::display(I);
    vpDisplay::flush(I);
    if (vpDisplay::getClick(I, false)) // a click to exit
        break;
  }

  g.close();
#endif
  std::cout << "ViSP exiting..." <<std::endl;
  return 0;
}
  \endcode
*/

class VISP_EXPORT vp1394CMUGrabber : public vpFrameGrabber
{
public:
  /*!
    Enumeration of color codings.
  */
  typedef enum {
    YUV444,
    YUV422,
    YUV411,
    RGB8,
    MONO8,
    MONO16,
    UNKNOWN
  } vpColorCodingType;

private :
  //! Current camera handle
  C1394Camera *camera;
  //! Camera index on the bus
  int index;
  //! Current video format
  unsigned long _format;
  //! Current video mode
  unsigned long _mode ;
  //! Current video frame rate
  unsigned long _fps ;
  //! Current auto mode
  bool _modeauto;
  //! Current gain
  unsigned short _gain;
  //! Current shutter
  unsigned short _shutter;
  //! Color coding
  vpColorCodingType _color;

public:

  // Constructor.
  vp1394CMUGrabber();
  // Destructor.
  virtual ~vp1394CMUGrabber();

  // Initialization of the grabber using a greyscale image.
  void open(vpImage<unsigned char> &I);

  // Initialization of the grabber using a color image.
  void open(vpImage<vpRGBa> &I);

  // Acquire one frame in a greyscale image.
  void acquire(vpImage<unsigned char> &I);

  // Acquire one frame in a color image.
  void acquire(vpImage<vpRGBa> &I);

  // Stop the acquisition.
  void close();

  // Display information about the camera on the standard output.
  void displayCameraDescription(int cam_id);

  // Display camera model on the standard output. Call it after open the grabber.
  void displayCameraModel();

  // Get the number of connected cameras.
  int getNumberOfConnectedCameras() const ;

  //! Get the video color coding format.
  vpColorCodingType getVideoColorCoding() const
  {
    vpColorCodingType color = vp1394CMUGrabber::UNKNOWN;
    if (_format == 0)
    {
      switch(_mode)
      {
        case 0: color = vp1394CMUGrabber::YUV444; break;
        case 1: color = vp1394CMUGrabber::YUV422; break;
        case 2: color = vp1394CMUGrabber::YUV411; break;
        case 3: color = vp1394CMUGrabber::YUV422; break;
        case 4: color = vp1394CMUGrabber::RGB8; break;
        case 5: color = vp1394CMUGrabber::MONO8; break;
        case 6: color = vp1394CMUGrabber::MONO16; break;
      }
    }
    else if (_format == 1)
    {
      switch(_mode)
      {
        case 0: color = vp1394CMUGrabber::YUV422; break;
        case 1: color = vp1394CMUGrabber::RGB8; break;
        case 2: color = vp1394CMUGrabber::MONO8; break;
        case 3: color = vp1394CMUGrabber::YUV422; break;
        case 4: color = vp1394CMUGrabber::RGB8; break;
        case 5: color = vp1394CMUGrabber::MONO8; break;
        case 6: color = vp1394CMUGrabber::MONO16; break;
        case 7: color = vp1394CMUGrabber::MONO16; break;
      }
    }
    else if (_format == 2)
    {
      switch(_mode)
      {
        case 0: color = vp1394CMUGrabber::YUV422; break;
        case 1: color = vp1394CMUGrabber::RGB8; break;
        case 2: color = vp1394CMUGrabber::MONO8; break;
        case 3: color = vp1394CMUGrabber::YUV422; break;
        case 4: color = vp1394CMUGrabber::RGB8; break;
        case 5: color = vp1394CMUGrabber::MONO8; break;
        case 6: color = vp1394CMUGrabber::MONO16; break;
        case 7: color = vp1394CMUGrabber::MONO16; break;
      }
    }

    return color;

  }

  // Select the camera on the bus. Call it before open the grabber.
  void selectCamera(int cam_id);

  // Set the gain and the shutter values. Call it before open the grabber
  void setControl(unsigned short gain , unsigned short shutter) ;

  // Set the frame rate. Call it before open the grabber.
  void setFramerate(unsigned long fps);

  // Set the video format and mode. Call it before open the grabber.
  void setVideoMode(unsigned long format , unsigned long mode );

private :

  void initCamera();

};

#endif
#endif
