/****************************************************************************
 *
 * $Id: vpMomentGravityCenterNormalized.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
 * 2D normalized gravity center moment descriptor (usually described by the pair Xn,Yn)
 *
 * Authors:
 * Filip Novotny
 *
 *****************************************************************************/

#include <visp/vpMomentGravityCenterNormalized.h>
#include <visp/vpMomentObject.h>
#include <visp/vpMomentGravityCenter.h>
#include <visp/vpMomentAreaNormalized.h>

/*!
  Computes normalized gravity center moment.
  Depends on vpMomentAreaNormalized and on vpMomentGravityCenter.
*/
void vpMomentGravityCenterNormalized::compute(){
    bool found_moment_gravity;    
    bool found_moment_surface_normalized;    
    
    vpMomentAreaNormalized& momentSurfaceNormalized = static_cast<vpMomentAreaNormalized&>(getMoments().get("vpMomentAreaNormalized",found_moment_surface_normalized));
    vpMomentGravityCenter& momentGravity = static_cast<vpMomentGravityCenter&>(getMoments().get("vpMomentGravityCenter",found_moment_gravity));

    if(!found_moment_surface_normalized) throw vpException(vpException::notInitialized,"vpMomentAreaNormalized not found");
    if(!found_moment_gravity) throw vpException(vpException::notInitialized,"vpMomentGravityCenter not found");

    double Xn = momentGravity.get()[0]*momentSurfaceNormalized.get()[0];
    double Yn = momentGravity.get()[1]*momentSurfaceNormalized.get()[0];

    values[0] = Xn;
    values[1] = Yn;

}

/*!
  Default constructor.
*/
vpMomentGravityCenterNormalized::vpMomentGravityCenterNormalized() : vpMomentGravityCenter(){}

/*!
  Outputs the moment's values to a stream.
*/
std::ostream & operator<<(std::ostream & os, const vpMomentGravityCenterNormalized& m){
    os << "Xn:" << m.values[0] << ",Yn=" << m.values[1];
    
    return os;    
}
