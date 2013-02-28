/****************************************************************************
 *
 * $Id: vpKinect.h 3746 2012-05-29 16:24:29Z cteulier $
 *
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
 * API for using a Microsoft Kinect device
 * Requires libfreenect as a third party library
 *
 * Authors:
 * Celine Teuliere
 *
 *****************************************************************************/


#ifndef __VP_KINECT__
#define __VP_KINECT__

#include <visp/vpConfig.h>
// Note that libfreenect needs libusb-1.0 and libpthread 
#if defined(VISP_HAVE_LIBFREENECT_AND_DEPENDENCIES)

#include <iostream>
#include <libfreenect.hpp>

#include <visp/vpMutex.h> // need pthread
#include <visp/vpImage.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpPixelMeterConversion.h>
#include <visp/vpMeterPixelConversion.h>

/*!

  \class vpKinect

  \ingroup KinectDriver

  \brief Driver for the Kinect device.

  The following example shows how to use this class to acquire data
  (depth map and color image) from a Kinect.

  \code
#include <visp/vpConfig.h>
#include <visp/vpImage.h>
#include <visp/vpKinect.h>

int main() {
#ifdef VISP_HAVE_LIBFREENECT
  // Init Kinect device
#ifdef VISP_HAVE_LIBFREENECT_OLD
  // This is the way to initialize Freenect with an old version of libfreenect
  // package under ubuntu lucid 10.04
  Freenect::Freenect<vpKinect> freenect;
  vpKinect * kinect = &freenect.createDevice(0);
#else
  Freenect::Freenect freenect;
  vpKinect * kinect = &freenect.createDevice<vpKinect>(0);
#endif
  kinect->start(); // Start acquisition thread

  // Set tilt angle
  float angle = -5;
  kinect->setTiltDegrees(angle);

  vpImage<unsigned char> I(480,640);
  vpImage<vpRGBa> Irgb(480,640);
  vpImage<float> dmap(480,640);

  // Acquisition loop
  for (int i=0; i<100; i++)
  {
    kinect->getDepthMap(dmap,I);
    kinect->getRGB(Irgb);
  }
  kinect->stop(); // Stop acquisition thread
#endif
  return 0;
}
  \endcode
*/
class VISP_EXPORT vpKinect : public Freenect::FreenectDevice
{
 public:
  /*! 
    Depth map resolution.
  */
  typedef enum {
    DMAP_LOW_RES,   /*!< Depth map has a resolution of 320 by 240. */
    DMAP_MEDIUM_RES /*!< Depth map has a resolution of 640 by 480. */
  } vpDMResolution;

  vpKinect(freenect_context *ctx, int index);
  virtual ~vpKinect();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  vpKinect(const vpKinect &); // Not implemented!
  void operator=(const vpKinect &){
    throw vpException(vpException::functionNotImplementedError,"Not implemented!");
  }
#endif

  void start(vpKinect::vpDMResolution res = DMAP_LOW_RES);
  void stop();

  bool getDepthMap(vpImage<float>& map);
  bool getDepthMap(vpImage<float>& map, vpImage<unsigned char>& Imap);
  bool getRGB(vpImage<vpRGBa>& IRGB);


  inline void getIRCamParameters(vpCameraParameters &cam) const {
    cam = IRcam;
  }
  inline void getRGBCamParameters(vpCameraParameters &cam) const {
    cam = RGBcam;
  }
  inline void setIRCamParameters(const vpCameraParameters &cam) {
    IRcam = cam;
  }
  inline void setRGBCamParameters(const vpCameraParameters &cam) {
    RGBcam = cam;
  }

  void warpRGBFrame(const vpImage<vpRGBa> & Irgb, const vpImage<float> & Idepth, vpImage<vpRGBa> & IrgbWarped);//warp the RGB image into the Depth camera frame

 private:
  //!Instantiation of Freenect virtual functions
  // Do not call directly even in child
  void VideoCallback(void* rgb, uint32_t timestamp);

  // Do not call directly even in child
  void DepthCallback(void* depth, uint32_t timestamp);

 private:
  vpMutex m_rgb_mutex;
  vpMutex m_depth_mutex;

  vpCameraParameters RGBcam, IRcam;//intrinsic parameters of the two cameras
  vpHomogeneousMatrix rgbMir;//Transformation from IRcam coordinate frame to RGBcam coordinate frame.
  vpHomogeneousMatrix irMrgb;//Transformation from RGBcam coordinate frame to IRcam coordinate frame .
  vpDMResolution DMres;
  unsigned int hd;//height of the depth map
  unsigned int wd;//width of the depth map

  //Access protected by a mutex:
  vpImage<float> dmap;
  vpImage<vpRGBa> IRGB;
  bool m_new_rgb_frame;
  bool m_new_depth_map;
  bool m_new_depth_image;
  unsigned int height;//height of the rgb image
  unsigned int width;//width of the rgb image

};

#endif

#endif
