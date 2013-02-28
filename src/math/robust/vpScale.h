/****************************************************************************
 *
 * $Id: vpScale.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Median Absolute Deviation (MAD), MPDE, Mean shift kernel density estimation.
 *
 * Authors:
 * Andrew Comport
 *
 *****************************************************************************/

/*!
 \file vpScale.h
*/

// =========================================================
/*!
 * \brief Contains various estimators for scale.
 * \n Methods : Median Absolute Deviation (MAD),
 * 							MPDE, Mean shift kernel density estimation.
 * \author Andrew Comport
 * \date 24/10/03
 */
// ==========================================================


#ifndef VPSCALE_HH
#define VPSCALE_HH

#define EPANECHNIKOV 0

#include <visp/vpColVector.h>
#include <math.h>


/*!
  \class vpScale
  \ingroup Robust

  \brief Contains different methods for estimating the robust scale of
  an error distribution.

  Methods of Median Absolute Deviation and Density Gradient estimation
  using the Mean Shift method.

  \author Andrew Comport
  \date 24/10/03
 */
class VISP_EXPORT vpScale
{

private:
  double bandwidth;
  unsigned int dimension;
  int kernel_type;

public:

  //! Constructor
  vpScale();
  vpScale(double, int, int);
  //! Destructor
  virtual ~vpScale(void);

  double MeanShift(vpColVector &error);
  double KernelDensity(vpColVector &error, unsigned int position);
  double KernelDensityGradient(vpColVector &error, unsigned int position);

  double KernelDensity_EPANECHNIKOV(vpColVector &X);
  double KernelDensityGradient_EPANECHNIKOV(double X, unsigned int n);

};

#endif
