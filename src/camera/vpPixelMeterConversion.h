/****************************************************************************
 *
 * $Id: vpPixelMeterConversion.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Pixel to meter conversion.
 *
 * Authors:
 * Eric Marchand
 * Anthony Saunier
 *
 *****************************************************************************/

#ifndef vpPixelMeterConversion_H
#define vpPixelMeterConversion_H


/*!
  \file vpPixelMeterConversion.h
  \brief pixel to meter conversion

*/
#include <visp/vpCameraParameters.h>
#include <visp/vpException.h>
#include <visp/vpMath.h>
#include <visp/vpDebug.h>
#include <visp/vpImagePoint.h>

/*!
  \class vpPixelMeterConversion

  \ingroup CameraModelTransformation CameraModel

  \brief Conversion from pixel coordinates \f$(u,v)\f$ to normalized
  coordinates \f$(x,y)\f$ in meter.

  This class relates to vpCameraParameters.

*/
class VISP_EXPORT vpPixelMeterConversion
{
public:

/*!  
  \brief Point coordinates conversion from pixel coordinates
  \f$(u,v)\f$ to normalized coordinates \f$(x,y)\f$ in meter.

  The used formula depends on the projection model of the camera. To
  know the currently used projection model use
  vpCameraParameter::get_projModel()

  \param cam : camera parameters.
  \param u,v : input coordinates in pixels.
  \param x,y : output coordinates in meter.

  \f$ x = (u-u_0)/p_x \f$ and \f$ y = (v-v_0)/p_y  \f$ in the case of
  perspective projection without distortion.

  \f$ x = (u-u_0)*(1+k_{du}*r^2)/p_x \f$ and
  \f$ y = (v-v_0)*(1+k_{du}*r^2)/p_y  \f$
  with \f$ r^2=((u - u_0)/p_x)^2+((v-v_0)/p_y)^2 \f$ in the case of perspective
  projection with distortion.
*/
inline static void
convertPoint(const vpCameraParameters &cam,
  const double &u, const double &v,
  double &x, double &y)
{
  switch(cam.projModel){
    case vpCameraParameters::perspectiveProjWithoutDistortion :
      convertPointWithoutDistortion(cam,u,v,x,y);
      break;
    case vpCameraParameters::perspectiveProjWithDistortion :
      convertPointWithDistortion(cam,u,v,x,y);
      break;
  }       
}

/*!  
  \brief Point coordinates conversion from pixel coordinates
  Coordinates in pixel to normalized coordinates \f$(x,y)\f$ in meter.

  The used formula depends on the projection model of the camera. To
  know the currently used projection model use
  vpCameraParameter::get_projModel()

  \param cam : camera parameters.
  \param iP : input coordinates in pixels.
  \param x,y : output coordinates in meter.

  Thanks to the pixel coordinates in the frame (u,v), the meter coordinates are given by :

  \f$ x = (u-u_0)/p_x \f$ and \f$ y = (v-v_0)/p_y  \f$ in the case of
  perspective projection without distortion.

  \f$ x = (u-u_0)*(1+k_{du}*r^2)/p_x \f$ and
  \f$ y = (v-v_0)*(1+k_{du}*r^2)/p_y  \f$
  with \f$ r^2=((u - u_0)/p_x)^2+((v-v_0)/p_y)^2 \f$ in the case of perspective
  projection with distortion.
*/
inline static void
convertPoint(const vpCameraParameters &cam,
  const vpImagePoint &iP,
  double &x, double &y)
{
  switch(cam.projModel){
    case vpCameraParameters::perspectiveProjWithoutDistortion :
      convertPointWithoutDistortion(cam,iP,x,y);
      break;
    case vpCameraParameters::perspectiveProjWithDistortion :
      convertPointWithDistortion(cam,iP,x,y);
      break;
  }       
}

/*!
  \brief Point coordinates conversion without distortion from pixel coordinates
  \f$(u,v)\f$ to normalized coordinates \f$(x,y)\f$ in meter.

  \param cam : camera parameters.
  \param u,v : input coordinates in pixels.
  \param x,y : output coordinates in meter.

  \f$ x = (u-u_0)/p_x \f$ and  \f$ y = (v-v_0)/p_y  \f$
*/
inline static void
convertPointWithoutDistortion(
  const vpCameraParameters &cam,
  const double &u, const double &v,
  double &x, double &y)
{
    x = (u - cam.u0)*cam.inv_px ;
    y = (v - cam.v0)*cam.inv_py ;
}

/*!
  \brief Point coordinates conversion without distortion from pixel coordinates
  Coordinates in pixel to normalized coordinates \f$(x,y)\f$ in meter.

  \param cam : camera parameters.
  \param iP : input coordinates in pixels.
  \param x,y : output coordinates in meter.

  Thanks to the pixel coordinates in the frame (u,v), the meter coordinates are given by :

  \f$ x = (u-u_0)/p_x \f$ and  \f$ y = (v-v_0)/p_y  \f$
*/
inline static void
convertPointWithoutDistortion(
  const vpCameraParameters &cam,
  const vpImagePoint &iP,
  double &x, double &y)
{
    x = (iP.get_u() - cam.u0)*cam.inv_px ;
    y = (iP.get_v() - cam.v0)*cam.inv_py ;
}

/*!
  \brief Point coordinates conversion with distortion from pixel coordinates
  \f$(u,v)\f$ to normalized coordinates \f$(x,y)\f$ in meter.

  \param cam : camera parameters.
  \param u,v : input coordinates in pixels.
  \param x,y : output coordinates in meter.

  \f$ x = (u-u_0)*(1+k_{du}*r^2)/p_x \f$ and
  \f$ y = (v-v_0)*(1+k_{du}*r^2)/p_y \f$
  with \f$ r^2=((u - u_0)/p_x)^2 + ((v-v_0)/p_y)^2 \f$
*/
inline static void
convertPointWithDistortion(
  const vpCameraParameters &cam,
  const double &u, const double &v,
  double &x, double &y)
{
  double r2 = 1.+cam.kdu*(vpMath::sqr((u - cam.u0)*cam.inv_px) +
              vpMath::sqr((v-cam.v0)*cam.inv_py));
  x = (u - cam.u0)*r2*cam.inv_px ;
  y = (v - cam.v0)*r2*cam.inv_py ;
}

/*!
  \brief Point coordinates conversion with distortion from pixel coordinates
  Coordinates in pixel to normalized coordinates \f$(x,y)\f$ in meter.

  \param cam : camera parameters.
  \param iP : input coordinates in pixels.
  \param x,y : output coordinates in meter.

  Thanks to the pixel coordinates in the frame (u,v), the meter coordinates are given by :

  \f$ x = (u-u_0)*(1+k_{du}*r^2)/p_x \f$ and
  \f$ y = (v-v_0)*(1+k_{du}*r^2)/p_y \f$
  with \f$ r^2=((u - u_0)/p_x)^2 + ((v-v_0)/p_y)^2 \f$
*/
inline static void
convertPointWithDistortion(
  const vpCameraParameters &cam,
  const vpImagePoint &iP,
  double &x, double &y)
{
  double r2 = 1.+cam.kdu*(vpMath::sqr((iP.get_u() - cam.u0)*cam.inv_px) +
              vpMath::sqr((iP.get_v()-cam.v0)*cam.inv_py));
  x = (iP.get_u() - cam.u0)*r2*cam.inv_px ;
  y = (iP.get_v() - cam.v0)*r2*cam.inv_py ;
}

  //! line coordinates conversion (rho,theta)
  static void convertLine(const vpCameraParameters &cam,
		      const double &rho_p, const double &theta_p,
		      double &rho_m, double &theta_m) ;


  static void convertMoment(const vpCameraParameters &cam,
			    unsigned int order,
			    const vpMatrix &moment_pixel,
			    vpMatrix &moment_meter) ;
} ;

#endif
/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */

