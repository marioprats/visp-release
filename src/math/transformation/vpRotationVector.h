/****************************************************************************
 *
 * $Id: vpRotationVector.h 3648 2012-03-26 20:38:55Z fspindle $
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
 * Generic rotation vector (cannot be used as is !).
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/

#ifndef vpRotationVECTOR_H
#define vpRotationVECTOR_H

/*!
  \file vpRotationVector.h
  \brief class that consider the case of a generic rotation vector
  (cannot be used as is !)
*/




#include <visp/vpMath.h>
#include <visp/vpRowVector.h>
#include <stdio.h>
#include <iostream>


#include <math.h>

/*!
  \class vpRotationVector

  \ingroup RotTransformation

  \brief Class that consider the case of a generic rotation vector
  (cannot be used as is !) consisting in three angles.

  The code below shows how this class can be used to manipulate a vpRxyzVector.

  \code
#include <iostream>
#include <visp/vpRxyzVector.h>
#include <visp/vpMath.h>

int main() 
{
  vpRxyzVector r;         // By default initialized to zero
  r[0] = vpMath::rad(45); // Rotation arround x set to 45 degres converted in radians
  r[1] = M_PI;            // Rotation arround y set to PI radians
  r[2] = 0;               // Rotation arround z set to 0 radians
  
  std::cout << "Rxyz rotation vector: " << r << std::endl;

  double rx = r[1];       // Get the value of the angle arround x axis
  double ry = r[2];       // Get the value of the angle arround y axis
  double rz = r[3];       // Get the value of the angle arround z axis
}
  
  \endcode

*/

class VISP_EXPORT vpRotationVector
{  
  friend class vpColVector;
protected:
  double *r ;
  unsigned int _size;
  void init(const unsigned int size);
public:
  //! Constructor that constructs a vector of size 3 initialize three vector values to zero.
  vpRotationVector() { 
	init(3);
  }

  
  //! Constructor that constructs a vector of size n initialize three vector values to zero.
  vpRotationVector(const unsigned int n) { 
	init(n);
  }
  
  ~vpRotationVector();

  /*!
    Operator that allows to set the value of an element of the rotation 
    vector: r[i] = value
  */
  inline double &operator [](unsigned int n) {  return *(r + n);  }
  /*!
    Operator that allows to get the value of an element of the rotation 
    vector: value = r[i]
  */
  inline const double &operator [](unsigned int n) const { return *(r+n);  }

    
  /*! Returns the size of the rotation vector
   */
  unsigned int size() const;

  // Transpose of the rotation vector.
  vpRowVector t() const;

  friend VISP_EXPORT std::ostream &operator << (std::ostream &s,
						const vpRotationVector &m);

} ;

#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
