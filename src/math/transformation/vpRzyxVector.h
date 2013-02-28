/****************************************************************************
 *
 * $Id: vpRzyxVector.h 3699 2012-05-03 15:22:11Z fspindle $
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
 * Rzyx angle parameterization for the rotation.
 * Rzyx(phi,theta,psi) = Rot(z,phi)Rot(y,theta)Rot(x,psi)
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/

#ifndef vpRzyxVector_h
#define vpRzyxVector_h

/*!
  \file vpRzyxVector.h

  \brief class that consider the case of the Rzyx angle
  parameterization for the rotation.

  Rzyx(phi,theta,psi) = Rot(z,phi)Rot(y,theta)Rot(x,psi)
*/

#include <visp/vpRotationMatrix.h>
#include <visp/vpRotationVector.h>

class vpRotationMatrix;
class vpThetaUVector;

/*!
  \class vpRzyxVector

  \ingroup RotTransformation

  \brief Class that consider the case of the Euler
  \f$(\varphi,\theta,\psi)\f$ angle using the z-y-x convention, where \f$(\varphi,\theta,\psi)\f$ are respectively the
  rotation angles around the \f$z\f$, \f$y\f$ and \f$x\f$ axis.

  \f[R_{zyx}(\varphi,\theta,\psi) = R_z(\varphi) \; R_y(\theta) \; R_x(\psi)\f]

  with

  \f[
  R_{z}(\varphi) = \left(
  \begin{array}{ccc}
  \cos \varphi & -\sin\varphi & 0\\
  \sin\varphi &\cos \varphi& 0 \\
  0 & 0 & 1
  \end{array}
  \right) \;
  R_{y}(\theta) = \left(
  \begin{array}{ccc}
  \cos \theta & 0 & \sin\theta\\
  0 & 1 & 0 \\
  -\sin\theta & 0 &\cos \theta
  \end{array}
  \right) \;
  R_{x}(\psi) = \left(
  \begin{array}{ccc}
  1 & 0 & 0 \\
  0 &\cos \psi & -\sin\psi \\
  0 &\sin \psi & \cos\psi \\
  \end{array}
  \right) 
  \f]

  The rotation matrix corresponding to the z-y-x convention is given by:

  \f[
  R_{zyx}(\varphi,\theta,\psi) = \left(
  \begin{array}{ccc}
  \cos\varphi \cos\theta & -\sin\varphi \cos\psi + \cos\varphi\sin\theta\sin\psi & \sin\varphi \sin\psi +\cos\varphi\sin\theta\cos\psi \\
  \sin\varphi \cos\theta & \cos\varphi\cos\psi + \sin\varphi\sin\theta \sin\psi & -\cos\varphi \sin\psi +\sin\varphi\sin\theta\cos\psi \\
  -\sin\theta & \cos\theta \sin\psi & \cos\theta \cos\psi
  \end{array}
  \right)
  \f]

  The code below shows first how to initialize this representation of
  Euler angles, than how to contruct a rotation matrix from a
  vpRzyxVector and finaly how to extract the vpRzyxVector Euler angles
  from the build rotation matrix.

  \code
#include <visp/vpMath.h>
#include <visp/vpRotationMatrix.h>
#include <visp/vpRzyxVector.h>

int main()
{
  vpRzyxVector rzyx;

  // Initialise the Euler angles
  rzyx[0] = vpMath::rad( 45.f); // phi   angle in rad/s around z axis 
  rzyx[1] = vpMath::rad(-30.f); // theta angle in rad/s around y axis
  rzyx[2] = vpMath::rad( 90.f); // psi   angle in rad/s around x axis

  // Construct a rotation matrix from the Euler angles
  vpRotationMatrix R(rzyx);

  // Extract the Euler angles around z,y,x axis from a rotation matrix
  rzyx.buildFrom(R);

  // Print the extracted Euler angles. Values are the same than the
  // one used for initialization
  std::cout << rzyx; 

  // Since the rotation vector is 3 values column vector, the
  // transpose operation produce a row vector.
  vpRowVector rzyx_t = rzyx.t();
  
  // Print the transpose row vector
  std::cout << rzyx_t << std::endl;
}
  \endcode

*/

class VISP_EXPORT vpRzyxVector : public vpRotationVector
{
  friend class vpRotationMatrix;
  friend class vpThetaUVector;

public:
  //! Default constructor. Initialize the angles to zero.
  vpRzyxVector() { ; }

  // Copy constructor.
  vpRzyxVector(const vpRzyxVector &m);

  /*!
    Constructor from 3 angles (in radian).
    \param phi : \f$\varphi\f$ angle around the \f$z\f$ axis.
    \param theta : \f$\theta\f$ angle around the \f$y\f$ axis.
    \param psi : \f$\psi\f$ angle around the \f$x\f$ axis.
  */
  vpRzyxVector(const double phi, const double theta, const double psi) :
	vpRotationVector (3) { r[0]=phi;r[1]=theta;r[2]=psi; }

  // initialize a Rzyx vector from a rotation matrix
  vpRzyxVector(const vpRotationMatrix& R) ;

  // initialize a Rzyx vector from a ThetaU vector
  vpRzyxVector(const vpThetaUVector&  tu) ;

  // Affectation of two vectors.
  vpRzyxVector &operator=(const vpRzyxVector &m);

  /*!
    Construction from 3 angles (in radian).
    \param phi : \f$\varphi\f$ angle around the \f$z\f$ axis.
    \param theta : \f$\theta\f$ angle around the \f$y\f$ axis.
    \param psi : \f$\psi\f$ angle around the \f$x\f$ axis.
  */
  void buildFrom(const double phi, const double theta, const double psi)
  {
    r[0] = phi ;
    r[1] = theta ;
    r[2] = psi ;
  }

  // convert a rotation matrix into Rzyx vector
  vpRzyxVector buildFrom(const vpRotationMatrix& R) ;

  // convert a ThetaU vector into a Rzyx vector
  vpRzyxVector buildFrom(const vpThetaUVector& R) ;

} ;

#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
