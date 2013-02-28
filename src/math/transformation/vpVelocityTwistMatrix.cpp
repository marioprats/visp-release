/****************************************************************************
 *
 * $Id: vpVelocityTwistMatrix.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
 * Velocity twist transformation matrix.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/

#include <visp/vpVelocityTwistMatrix.h>

// Exception
#include <visp/vpException.h>
#include <visp/vpMatrixException.h>

// Debug trace
#include <visp/vpDebug.h>


/*!
  \file vpVelocityTwistMatrix.cpp

  \brief Definition of the vpVelocityTwistMatrix. Class that consider the
  particular case of twist transformation matrix that allows to
  transform a velocity skew from one frame to an other.
*/


/*!
  Copy operator.

  \param V : Velocity twist matrix to copy.
*/
vpVelocityTwistMatrix &
vpVelocityTwistMatrix::operator=(const vpVelocityTwistMatrix &V)
{
  init() ;

  for (int i=0; i<6; i++)
    {
      for (int j=0; j<6; j++)
	{
	  rowPtrs[i][j] = V.rowPtrs[i][j];
	}
    }

  return *this;
}


/*!
  Initialize a 6x6 velocity twist matrix as identity. 
*/

void
vpVelocityTwistMatrix::init()
{
  unsigned int i,j ;

  try {
    resize(6,6) ;
  }
  catch(vpException me)
    {
      vpERROR_TRACE("Error caught") ;
      throw ;
    }

  for (i=0 ; i < 6 ; i++)
    for (j=0 ; j < 6; j++)
      if (i==j)
	(*this)[i][j] = 1.0 ;
      else
	(*this)[i][j] = 0.0;
}

/*!
  Initialize a velocity twist transformation matrix as identity.
*/
vpVelocityTwistMatrix::vpVelocityTwistMatrix() : vpMatrix()
{
  init() ;
}

/*!
  Initialize a velocity twist transformation matrix from another velocity twist matrix.

  \param V : Velocity twist matrix used as initializer.
*/
vpVelocityTwistMatrix::vpVelocityTwistMatrix(const vpVelocityTwistMatrix &V) : vpMatrix()
{
  init() ;
  *this = V;
}

/*!

  Initialize a velocity twist transformation matrix from an homogeneous matrix
  \f$M\f$ with \f[ {\bf M} = \left[\begin{array}{cc} {\bf R} & {\bf t}
  \\ {\bf 0}_{1\times 3} & 1 \end{array} \right] \f]

  \param M : Homogeneous matrix \f$M\f$ used to initialize the velocity twist
  transformation matrix.

*/
vpVelocityTwistMatrix::vpVelocityTwistMatrix(const vpHomogeneousMatrix &M) : vpMatrix()
{
  init() ;
  buildFrom(M);
}

/*!

  Initialize a velocity twist transformation matrix from a translation vector
  \e t and a rotation vector with \f$\theta u \f$ parametrization.

  \param t : Translation vector.
  
  \param thetau : \f$\theta u\f$ rotation vector.

*/
vpVelocityTwistMatrix::vpVelocityTwistMatrix(const vpTranslationVector &t,
					     const vpThetaUVector &thetau) : vpMatrix()
{
  init() ;
  buildFrom(t, thetau) ;
}

/*!

  Initialize a velocity twist transformation matrix from a translation vector
  \e t and a rotation matrix M.

  \param t : Translation vector.
  
  \param R : Rotation matrix.

*/
vpVelocityTwistMatrix::vpVelocityTwistMatrix(const vpTranslationVector &t,
					     const vpRotationMatrix &R)
{
  init() ;
  buildFrom(t,R) ;
}

/*!

  Initialize a velocity twist transformation matrix from a translation vector
  \e t and a rotation vector with \f$\theta u \f$ parametrization.

  \param tx,ty,tz : Translation vector in meters.

  \param tux,tuy,tuz : \f$\theta u\f$ rotation vector expressed in radians.
*/
vpVelocityTwistMatrix::vpVelocityTwistMatrix(const double tx,
					     const double ty,
					     const double tz,
					     const double tux,
					     const double tuy,
					     const double tuz) : vpMatrix()
{
  init() ;
  vpTranslationVector T(tx,ty,tz) ;
  vpThetaUVector tu(tux,tuy,tuz) ;
  buildFrom(T,tu) ;  
}

/*!

  Set the velocity twist transformation matrix to identity.

*/
void
vpVelocityTwistMatrix::setIdentity()
{
  init() ;
}


/*!

  Operator that allows to multiply a velocity twist transformation matrix by an
  other velocity twist transformation matrix.

*/
vpVelocityTwistMatrix
vpVelocityTwistMatrix::operator*(const vpVelocityTwistMatrix &V) const
{
  vpVelocityTwistMatrix p ;

  for (unsigned int i=0;i<6;i++)
    for (unsigned int j=0;j<6;j++)
      {
	double s =0 ;
	for (int k=0;k<6;k++)
	  s +=rowPtrs[i][k] * V.rowPtrs[k][j];
	p[i][j] = s ;
      }
  return p;
}

/*!
  Operator that allows to multiply a velocity twist transformation matrix by a matrix.

  As shown in the example below, this operator can be used to compute the corresponding
  camera velocity skew from the joint velocities knowing the robot jacobian.

  \code
  #include <visp/vpConfig.h>
  #include <visp/vpRobotAfma6.h>
  #include <visp/vpColVector.h>
  #include <visp/vpVelocityTwistMatrix.h>

  #ifdef VISP_HAVE_AFMA6

  int main()
  {
  vpRobotAfma6 robot;

  vpColVector q_vel(6); // Joint velocity on the 6 joints
  // ... q_vel need here to be initialized
  
  vpColVector c_v(6); // Velocity in the camera frame: vx,vy,vz,wx,wy,wz 

  vpVelocityTwistMatrix cVe;  // Velocity skew transformation from camera frame to end-effector
  robot.get_cVe(cVe);

  vpMatrix eJe;       // Robot jacobian
  robot.get_eJe(eJe);

  // Compute the velocity in the camera frame
  c_v = cVe * eJe * q_vel;

  return 0;
  }
  #endif  
  \endcode

  \exception vpMatrixException::incorrectMatrixSizeError If M is not a 6 rows
  dimension matrix.
*/
vpMatrix
vpVelocityTwistMatrix::operator*(const vpMatrix &M) const
{

  if (6 != M.getRows())
    {
      vpERROR_TRACE("vpVelocityTwistMatrix mismatch in vpVelocityTwistMatrix/vpMatrix multiply") ;
      throw(vpMatrixException::incorrectMatrixSizeError) ;
    }

  vpMatrix p(6, M.getCols()) ;
  for (unsigned int i=0;i<6;i++)
    for (unsigned int j=0;j<M.getCols();j++)
      {
	double s =0 ;
	for (unsigned int k=0;k<6;k++)
	  s += rowPtrs[i][k] * M[k][j];
	p[i][j] = s ;
      }
  return p;
}

/*!

  Operator that allows to multiply a twist transformation matrix by a
  column vector.

  Operation c = V * v (V, the velocity skew transformation matrix is unchanged,
  c and v are 6 dimension vectors).

  \param v : Velocity skew vector.

  \exception vpMatrixException::incorrectMatrixSizeError If v is not a 6
  dimension vector.

*/
vpColVector
vpVelocityTwistMatrix::operator*(const vpColVector &v) const
{
  vpColVector c(6);

  if (6 != v.getRows())
    {
      vpERROR_TRACE("vpVelocityTwistMatrix mismatch in vpVelocityTwistMatrix/vector multiply") ;
      throw(vpMatrixException::incorrectMatrixSizeError) ;
    }

  c = 0.0;

  for (unsigned int i=0;i<6;i++) {
    for (unsigned int j=0;j<6;j++) {
      {
 	c[i]+=rowPtrs[i][j] * v[j];
      }
    }
  }

  return c ;
}


/*!

  Build a velocity twist transformation matrix from a translation vector
  \e t and a rotation matrix M.

  \param t : Translation vector.
  
  \param R : Rotation matrix.

*/
vpVelocityTwistMatrix
vpVelocityTwistMatrix::buildFrom(const vpTranslationVector &t,
				 const vpRotationMatrix &R)
{
  unsigned int i, j;
  vpMatrix skewaR = t.skew(t)*R ;

  for (i=0 ; i < 3 ; i++)
    for (j=0 ; j < 3 ; j++)
      {
	(*this)[i][j] = R[i][j] ;
	(*this)[i+3][j+3] = R[i][j] ;
	(*this)[i][j+3] = skewaR[i][j] ;

      }
  return (*this) ;
}

/*!

  Initialize a velocity twist transformation matrix from a translation vector
  \e t and a rotation vector with \f$\theta u \f$ parametrization.

  \param t : Translation vector.
  
  \param thetau : \f$\theta u\f$ rotation vector.

*/
vpVelocityTwistMatrix
vpVelocityTwistMatrix::buildFrom(const vpTranslationVector &t,
				 const vpThetaUVector &thetau)
{
  vpRotationMatrix R ;
  R.buildFrom(thetau) ;
  buildFrom(t,R) ;
  return (*this) ;

}


/*!

  Initialize a velocity twist transformation matrix from an homogeneous matrix
  \f$M\f$ with \f[ {\bf M} = \left[\begin{array}{cc} {\bf R} & {\bf t}
  \\ {\bf 0}_{1\times 3} & 1 \end{array} \right] \f]

  \param M : Homogeneous matrix \f$M\f$ used to initialize the twist
  transformation matrix.

*/
vpVelocityTwistMatrix
vpVelocityTwistMatrix::buildFrom(const vpHomogeneousMatrix &M)
{
  vpTranslationVector t ;
  vpRotationMatrix R ;
  M.extract(R) ;
  M.extract(t) ;

  buildFrom(t, R) ;
  return (*this) ;
}


//! invert the twist matrix
vpVelocityTwistMatrix
vpVelocityTwistMatrix::inverse() const
{
  vpVelocityTwistMatrix Wi;
  vpRotationMatrix R;extract(R);
  vpTranslationVector T;extract(T);
  vpTranslationVector RtT ; RtT = -(R.t()*T) ;

  Wi.buildFrom(RtT,R.t());

  return Wi ;
}


//! invert the twist matrix
void
vpVelocityTwistMatrix::inverse(vpVelocityTwistMatrix &Wi) const
{
	Wi = inverse();
}

//! extract the rotational matrix from the twist matrix
void
vpVelocityTwistMatrix::extract( vpRotationMatrix &R) const
{
	for (unsigned int i=0 ; i < 3 ; i++)
	    for (unsigned int j=0 ; j < 3; j++)
	      R[i][j] = (*this)[i][j];
}

//! extract the translation vector from the twist matrix
void
vpVelocityTwistMatrix::extract(vpTranslationVector &t) const
{
	vpRotationMatrix R;extract(R);
	vpMatrix skTR(3,3);
	for (unsigned int i=0 ; i < 3 ; i++)
	  for (unsigned int j=0 ; j < 3; j++)
		skTR[i][j] = (*this)[i][j+3];

	vpMatrix skT = skTR*R.t();
	t[0] = skT[2][1];
	t[1] = skT[0][2];
	t[2] = skT[1][0];
}

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
