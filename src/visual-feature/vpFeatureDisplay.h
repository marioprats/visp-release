/****************************************************************************
 *
 * $Id: vpFeatureDisplay.h 3653 2012-03-28 12:43:23Z fspindle $
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
 * Interface with the image for feature display.
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/


#ifndef vpFeatureDisplay_H
#define vpFeatureDisplay_H

/*!
  \file vpFeatureDisplay.h
  \brief interface with the image for feature display
*/

//Color / image / display
#include <visp/vpImage.h>
#include <visp/vpColor.h>
#include <visp/vpRGBa.h>

// Meter/pixel conversion
#include <visp/vpCameraParameters.h>

/*!
  \class vpFeatureDisplay

  \ingroup VsFeatureDisplay
  \brief Interface with the image for feature display.
*/
class VISP_EXPORT vpFeatureDisplay
{

public:
  static void displayPoint(double x,double y,
                           const vpCameraParameters &cam,
                           const vpImage<unsigned char> &I,
                           const vpColor &color = vpColor::green,
                           unsigned int thickness=1) ;
  static void displayLine(double rho,double theta,
                          const vpCameraParameters &cam,
                          const vpImage<unsigned char> &I,
                          const vpColor &color = vpColor::green,
                          unsigned int thickness=1) ;
  static void displayCylinder(double rho1,double theta1,
                              double rho2,double theta2,
                              const vpCameraParameters &cam,
                              const vpImage<unsigned char> &I,
                              const vpColor &color = vpColor::green,
                              unsigned int thickness=1) ;
  static void displayEllipse(double x,double y,
                             double mu20, double mu11, double m02,
                             const vpCameraParameters &cam,
                             const vpImage<unsigned char> &I,
                             const vpColor &color = vpColor::green,
                             unsigned int thickness=1) ;

  static void displayPoint(double x,double y,
                           const vpCameraParameters &cam,
                           const vpImage<vpRGBa> &I,
                           const vpColor &color = vpColor::green,
                           unsigned int thickness=1) ;
  static void displayLine(double rho,double theta,
                          const vpCameraParameters &cam,
                          const vpImage<vpRGBa> &I,
                          const vpColor &color = vpColor::green,
                          unsigned int thickness=1) ;
  static void displayCylinder(double rho1,double theta1,
                              double rho2,double theta2,
                              const vpCameraParameters &cam,
                              const vpImage<vpRGBa> &I,
                              const vpColor &color = vpColor::green,
                              unsigned int thickness=1) ;
  static void displayEllipse(double x,double y,
                             double mu20, double mu11, double m02,
                             const vpCameraParameters &cam,
                             const vpImage<vpRGBa> &I,
                             const vpColor &color = vpColor::green,
                             unsigned int thickness=1) ;


};

#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
