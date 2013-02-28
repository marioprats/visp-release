/****************************************************************************
 *
 * $Id: vpMeterPixelConversion.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Meter to pixel conversion.
 *
 * Authors:
 * Eric Marchand
 * Anthony Saunier
 *
 *****************************************************************************/


#ifndef vpMeterPixelConversion_H
#define vpMeterPixelConversion_H


/*!
  \file vpMeterPixelConversion.h
  \brief meter to pixel  conversion

*/

#include <visp/vpCameraParameters.h>
#include<visp/vpException.h>
#include<visp/vpMath.h>
#include<visp/vpImagePoint.h>

/*!
  \class vpMeterPixelConversion

  \ingroup CameraModelTransformation CameraModel

  \brief Conversion from normalized coordinates \f$(x,y)\f$ in meter
  to pixel coordinates \f$(u,v)\f$.

  This class relates to vpCameraParameters.

*/
class VISP_EXPORT vpMeterPixelConversion
{
public:

/*!

  \brief Point coordinates conversion from normalized coordinates
  \f$(x,y)\f$ in meter to pixel coordinates \f$(u,v)\f$.

  The used formula depends on the projection model of the camera. To
  know the currently used projection model use
  vpCameraParameter::get_projModel()

  \param cam : camera parameters.
  \param x,y : input coordinates in meter.
  \param u,v : output coordinates in pixels.
  
  \f$ u = x*p_x + u_0 \f$ and  \f$ v = y*p_y + v_0 \f$ in the case of
  perspective projection without distortion.

  \f$ u = x*p_x*(1+k_{ud}*r^2)+u_0 \f$ and  \f$ v = y*p_y*(1+k_{ud}*r^2)+v_0 \f$
  with \f$ r^2 = x^2+y^2 \f$ in the  case of perspective projection with
  distortion.
*/

  inline static void
  convertPoint( const vpCameraParameters &cam,
                const double &x, const double &y,
                double &u, double &v)

  {
    switch(cam.projModel){
      case vpCameraParameters::perspectiveProjWithoutDistortion :   
        convertPointWithoutDistortion(cam,x,y,u,v);
        break;
      case vpCameraParameters::perspectiveProjWithDistortion :
        convertPointWithDistortion(cam,x,y,u,v);
      break;
    }       
  }

/*!

  \brief Point coordinates conversion from normalized coordinates
  \f$(x,y)\f$ in meter to pixel coordinates.

  The used formula depends on the projection model of the camera. To
  know the currently used projection model use
  vpCameraParameter::get_projModel()

  \param cam : camera parameters.
  \param x,y : input coordinates in meter.
  \param iP : output coordinates in pixels.

  In the frame (u,v) the result is given by:

  \f$ u = x*p_x + u_0 \f$ and  \f$ v = y*p_y + v_0 \f$ in the case of
  perspective projection without distortion.

  \f$ u = x*p_x*(1+k_{ud}*r^2)+u_0 \f$ and  \f$ v = y*p_y*(1+k_{ud}*r^2)+v_0 \f$
  with \f$ r^2 = x^2+y^2 \f$ in the  case of perspective projection with
  distortion.
*/

  inline static void
  convertPoint( const vpCameraParameters &cam,
                const double &x, const double &y,
                vpImagePoint &iP)

  {
    switch(cam.projModel){
      case vpCameraParameters::perspectiveProjWithoutDistortion :   
        convertPointWithoutDistortion(cam,x,y,iP);
        break;
      case vpCameraParameters::perspectiveProjWithDistortion :
        convertPointWithDistortion(cam,x,y,iP);
      break;
    }       
  }

  /*!

    \brief Point coordinates conversion without distortion from
    normalized coordinates \f$(x,y)\f$ in meter to pixel coordinates
    \f$(u,v)\f$.

    \f$ u = x*p_x+u_0 \f$ and  \f$ v = y*p_y+v_0  \f$
  */

  inline static void
  convertPointWithoutDistortion(const vpCameraParameters &cam,
				const double &x, const double &y,
				double &u, double &v)

  {
      u = x * cam.px + cam.u0 ;
      v = y * cam.py + cam.v0 ;
  }

  /*!

    \brief Point coordinates conversion without distortion from
    normalized coordinates \f$(x,y)\f$ in meter to pixel coordinates.

    In the frame (u,v) the result is given by:

    \f$ u = x*p_x+u_0 \f$ and  \f$ v = y*p_y+v_0  \f$
  */

  inline static void
  convertPointWithoutDistortion(const vpCameraParameters &cam,
				const double &x, const double &y,
				vpImagePoint &iP)

  {
      iP.set_u( x * cam.px + cam.u0 );
      iP.set_v( y * cam.py + cam.v0 );
  }
  
  /*!

    \brief Point coordinates conversion with distortion from
    normalized coordinates \f$(x,y)\f$ in meter to pixel coordinates
    \f$(u,v)\f$.

    \param cam : camera parameters.
    \param x,y : input coordinates in meter.
    \param u,v : output coordinates in pixels.

    \f$ u = x*p_x*(1+k_{ud}*r^2)+u_0 \f$ and
    \f$ v = y*p_y*(1+k_{ud}*r^2)+v_0 \f$
    with \f$ r^2 = x^2+y^2 \f$
  */
  inline static void
  convertPointWithDistortion(const vpCameraParameters &cam,
			     const double &x, const double &y,
			     double &u, double &v)

  {
    double r2 = 1.+cam.kud*(x*x+y*y);
    u = cam.u0 + cam.px*x*r2;
    v = cam.v0 + cam.py*y*r2;
  }

  /*!

    \brief Point coordinates conversion with distortion from
    normalized coordinates \f$(x,y)\f$ in meter to pixel coordinates.

    \param cam : camera parameters.
    \param x,y : input coordinates in meter.
    \param iP : output coordinates in pixels.

    In the frame (u,v) the result is given by:

    \f$ u = x*p_x*(1+k_{ud}*r^2)+u_0 \f$ and
    \f$ v = y*p_y*(1+k_{ud}*r^2)+v_0 \f$
    with \f$ r^2 = x^2+y^2 \f$
  */
  inline static void
  convertPointWithDistortion(const vpCameraParameters &cam,
              const double &x, const double &y,
              vpImagePoint &iP)

  {
    double r2 = 1.+cam.kud*(x*x+y*y);
    iP.set_u( cam.u0 + cam.px*x*r2 );
    iP.set_v( cam.v0 + cam.py*y*r2 );
  }

  //! line coordinates conversion (rho,theta)
  static void convertLine(const vpCameraParameters &cam,
			  const double &rho_m, const double &theta_m,
			  double &rho_p, double &theta_p) ;
} ;

#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */

