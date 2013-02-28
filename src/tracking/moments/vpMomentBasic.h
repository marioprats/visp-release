/****************************************************************************
 *
 * $Id: vpMomentBasic.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Basic moment descriptor
 *
 * Authors:
 * Filip Novotny
 *
 *****************************************************************************/

/*!
  \file vpMomentBasic.h
  \brief Basic moment descriptor (also called m).
*/

#ifndef __MOMENTBASIC_H__
#define __MOMENTBASIC_H__

#include <visp/vpMoment.h>

/*!
  \class vpMomentBasic

  \ingroup TrackingMoments

  \brief This class defines the 2D basic moment \f$m_{ij}\f$.
    This class is a wrapper for vpMomentObject wich allows to use it as a moment.
    This enables all common operations such as adding to a vpMomentDatabase.

  In the case of a dense object \e O, 2D basic moments are defined by:
  \f[m_{ij} = \int \int_{O} x^j y^j dx dy\f]

  In the case of a discrete set of \e n points, the basic moments are defined by
  \f[m_{ij} = \sum_{k=1}^{n} x_k^j y_k^j \f]

  The selection of a dense or discrete object is done in vpMomentObject.
  The maximum order of vpMomentBasic is the same as that of vpMomentObject (ie: \f$i+j \leq \f$ vpMomentObject::getOrder).

  Note that in the continuous case, the basic moment \f$m_{00}\f$ corresponds to the surface \f$a\f$ of the object.
  In the discrete case, it is the number of discrete points \f$n\f$.
*/

class VISP_EXPORT vpMomentBasic : public vpMoment {
 public:	
        vpMomentBasic();	

        void compute();
        std::vector<double>& get();
        double get(unsigned int i,unsigned int j);
        /*!
          Moment name.
          */
        const char* name(){ return "vpMomentBasic";}

};
std::ostream & operator<<(std::ostream & os, vpMomentBasic& v);
#endif
