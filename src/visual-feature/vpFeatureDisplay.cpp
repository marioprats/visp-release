/****************************************************************************
 *
 * $Id: vpFeatureDisplay.cpp 3653 2012-03-28 12:43:23Z fspindle $
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

#include <visp/vpFeatureDisplay.h>

// Meter/pixel conversion
#include <visp/vpMeterPixelConversion.h>

// display
#include <visp/vpDisplay.h>

// Debug trace
#include <visp/vpDebug.h>

// math
#include <visp/vpMath.h>

#include <visp/vpImagePoint.h>



/*!
  \param x,y : Point coordinates in meters.
  \param cam : Camera intrinsic parameters.
  \param I : Image.

  \param color : Color to use to display the feature
  \param thickness : Thickness of the feature representation.
*/
void vpFeatureDisplay::displayPoint(double x,double y,
                                    const vpCameraParameters &cam,
                                    const vpImage<unsigned char> &I,
                                    const vpColor &color,
                                    unsigned int thickness)
{
  try{
    vpImagePoint ip; // pixel coordinates in float
    vpMeterPixelConversion::convertPoint(cam, x, y, ip) ;

    vpDisplay::displayCross(I, ip, 15, color, thickness) ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }

}
/*!
  \param rho, theta : Line parameters.
  \param cam : Camera intrinsic parameters.
  \param I : Image.
  \param color : Color to use to display the feature
  \param thickness : Thickness of the feature representation.
*/
void vpFeatureDisplay::displayLine(double rho,double theta,
                                   const vpCameraParameters &cam,
                                   const vpImage<unsigned char> &I,
                                   const vpColor &color,
                                   unsigned int thickness )
{


  try{
    //    x cos(theta) + y sin(theta) - rho = 0

    double rhop,thetap ;
    vpMeterPixelConversion::convertLine(cam,rho,theta,rhop,thetap) ;

    //    u cos(thetap) + v sin(thetap) - rhop = 0

    double co = cos(thetap) ;
    double si = sin(thetap) ;
    double c = -rhop ;

    // vpTRACE("rhop %f %f ",rhop, atan2(si,co)) ;
    // double u1,v1,u2,v2 ;

    double a = si ;
    double b = co ;
    vpImagePoint ip1, ip2;

    if (fabs(a) < fabs(b)) {
      ip1.set_ij(0, (-c)/b);
      double h = I.getHeight() - 1;
      ip2.set_ij(h, (-c - a*h)/b);
      vpDisplay::displayLine(I, ip1, ip2, color, thickness);
    }
    else {
      ip1.set_ij((-c)/a, 0);
      double w = I.getWidth()-1;
      ip2.set_ij((-c - b*w)/a, w);
      vpDisplay::displayLine(I, ip1, ip2, color, thickness);
    }
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}
/*!
  \param rho1, theta1 : Cylinder limb1 parameters.
  \param rho2, theta2 : Cylinder limb2 parameters.
  \param cam : Camera intrinsic parameters.
  \param I : Image.
  \param color : Color to use to display the feature
  \param thickness : Thickness of the feature representation.
*/
void vpFeatureDisplay::displayCylinder(double rho1,double theta1,
                                       double rho2,double theta2,
                                       const vpCameraParameters &cam,
                                       const vpImage<unsigned char> &I,
                                       const vpColor &color,
                                       unsigned int thickness)
{
  try
  {
    displayLine(rho1, theta1, cam, I, color, thickness) ;
    displayLine(rho2, theta2, cam, I, color, thickness) ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}
/*!
  \param x, y, mu20, mu11, mu02 : Ellipse parameters.
  \param cam : Camera intrinsic parameters.
  \param I : Image.
  \param color : Color to use to display the feature
  \param thickness : Thickness of the feature representation.
*/
void vpFeatureDisplay::displayEllipse(double x,double y,
                                      double mu20, double mu11, double mu02,
                                      const vpCameraParameters &cam,
                                      const vpImage<unsigned char> &I,
                                      const vpColor &color,
                                      unsigned int thickness)
{


  try{
    {
      unsigned int number_of_points = 45 ;
      const double incr = 2 * M_PI/(double)number_of_points ; // angle increment
      unsigned int i = 0 ;

      //	 std::cout << s.t() ;
      double s = sqrt(vpMath::sqr(mu20-mu02)+4*mu11*mu11) ;
      double e ;

      if (fabs(mu11)<1e-6) e =0 ;
      else e =  (mu02-mu20+s)/(2*mu11) ;
      double a =sqrt( (mu02+mu20+s)/2.0) ;
      double b =sqrt( (mu02+mu20-s)/2.0) ;

      //    vpTRACE("%f %f %f", a,b,e) ;

      double e1  = atan(e) ;

      double k = 0.0 ;

      double ce = cos(e1) ;
      double se = sin(e1) ;

      double x2  = 0;
      double y2 =0;
      vpImagePoint ip1, ip2;

      for( i = 0; i < number_of_points+2 ; i++)
      {
        double    x1 = a *cos(k) ; // equation of an ellipse
        double    y1 = b *sin(k) ; // equation of an ellipse
        double    x11 = x + ce *x1 - se *y1 ;
        double    y11 = y + se *x1 + ce *y1 ;

        vpMeterPixelConversion::convertPoint(cam, x11, y11, ip1);

        if (i > 1) {
          ip2.set_u( x2 );
          ip2.set_v( y2 );

          vpDisplay::displayLine(I, ip1, ip2, color, thickness) ;
        }

        ip2 = ip1;
        y2 = y1 ;
        x2 = x1 ;
        k += incr ;
      } // end for loop
    }
    //    vpDisplay::getClick(I) ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}

/*!
  \param x, y : Point coordinates in meters.
  \param cam : Camera intrinsic parameters.
  \param I : Image.

  \param color : Color to use to display the feature
  \param thickness : Thickness of the feature representation.
*/
void vpFeatureDisplay::displayPoint(double x,double y,
                                    const vpCameraParameters &cam,
                                    const vpImage<vpRGBa> &I,
                                    const vpColor &color,
                                    unsigned int thickness)
{
  try{
    vpImagePoint ip; // pixel coordinates in float
    vpMeterPixelConversion::convertPoint(cam, x, y, ip) ;

    vpDisplay::displayCross(I, ip, 15, color, thickness) ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }

}

/*!
  \param rho, theta : Line parameters.
  \param cam : Camera intrinsic parameters.
  \param I : Image.
  \param color : Color to use to display the feature
  \param thickness : Thickness of the feature representation.
*/
void vpFeatureDisplay::displayLine(double rho,double theta,
                                   const vpCameraParameters &cam,
                                   const vpImage<vpRGBa> &I,
                                   const vpColor &color,
                                   unsigned int thickness )
{


  try{
    //    x cos(theta) + y sin(theta) - rho = 0

    double rhop,thetap ;
    vpMeterPixelConversion::convertLine(cam,rho,theta,rhop,thetap) ;

    //    u cos(thetap) + v sin(thetap) - rhop = 0

    double co = cos(thetap) ;
    double si = sin(thetap) ;
    double c = -rhop ;

    // vpTRACE("rhop %f %f ",rhop, atan2(si,co)) ;
    // double u1,v1,u2,v2 ;

    double a = si ;
    double b = co ;
    vpImagePoint ip1, ip2;

    if (fabs(a) < fabs(b)) {
      ip1.set_ij(0, (-c)/b);
      double h = I.getHeight() - 1;
      ip2.set_ij(h, (-c - a*h)/b);
      vpDisplay::displayLine(I, ip1, ip2, color, thickness);
    }
    else {
      ip1.set_ij((-c)/a, 0);
      double w = I.getWidth()-1;
      ip2.set_ij((-c - b*w)/a, w);
      vpDisplay::displayLine(I, ip1, ip2, color, thickness);
    }
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}
/*!
  \param rho1, theta1 : Cylinder limb1 parameters.
  \param rho2, theta2 : Cylinder limb2 parameters.
  \param cam : Camera intrinsic parameters.
  \param I : Image.
  \param color : Color to use to display the feature
  \param thickness : Thickness of the feature representation.
*/
void vpFeatureDisplay::displayCylinder(double rho1, double theta1,
                                       double rho2, double theta2,
                                       const vpCameraParameters &cam,
                                       const vpImage<vpRGBa> &I,
                                       const vpColor &color,
                                       unsigned int thickness)
{
  try
  {
    displayLine(rho1, theta1, cam, I, color, thickness) ;
    displayLine(rho2, theta2, cam, I, color, thickness) ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}

/*!
  \param x, y, mu20, mu11, mu02 : Ellipse parameters.
  \param cam : Camera intrinsic parameters.
  \param I : Image.
  \param color : Color to use to display the feature
  \param thickness : Thickness of the feature representation.
*/
void vpFeatureDisplay::displayEllipse(double x, double y,
                                      double mu20, double mu11, double mu02,
                                      const vpCameraParameters &cam,
                                      const vpImage<vpRGBa> &I,
                                      const vpColor &color,
                                      unsigned int thickness)
{


  try{
    {
      unsigned int number_of_points = 45 ;
      const double incr = 2 * M_PI/(double)number_of_points ; // angle increment
      unsigned int i = 0 ;

      //	 std::cout << s.t() ;
      double s = sqrt(vpMath::sqr(mu20-mu02)+4*mu11*mu11) ;
      double e ;

      if (fabs(mu11)<1e-6) e =0 ;
      else e =  (mu02-mu20+s)/(2*mu11) ;
      double a =sqrt( (mu02+mu20+s)/2.0) ;
      double b =sqrt( (mu02+mu20-s)/2.0) ;

      //    vpTRACE("%f %f %f", a,b,e) ;

      double e1  = atan(e) ;

      double k = 0.0 ;

      double ce = cos(e1) ;
      double se = sin(e1) ;
      double x2  = 0;
      double y2 =0;
      vpImagePoint ip1, ip2;

      for( i = 0; i < number_of_points+2 ; i++)
      {
        double    x1 = a *cos(k) ; // equation of an ellipse
        double    y1 = b *sin(k) ; // equation of an ellipse
        double    x11 = x + ce *x1 - se *y1 ;
        double    y11 = y + se *x1 + ce *y1 ;

        vpMeterPixelConversion::convertPoint(cam, x11, y11, ip1);

        if (i > 1) {
          ip2.set_u( x2 );
          ip2.set_v( y2 );

          vpDisplay::displayLine(I, ip1, ip2, color, thickness) ;
        }

        ip2 = ip1;
        k += incr ;
      } // end for loop
    }
    //    vpDisplay::getClick(I) ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
