/****************************************************************************
 *
 * $Id: vpGenericFeature.h 3653 2012-03-28 12:43:23Z fspindle $
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
 * Generic feature (used to create new feature not implemented in ViSP).
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/



#ifndef vpGenericFeature_hh
#define vpGenericFeature_hh

/*!
  \file vpGenericFeature.h
  \brief class that defines what is a generic feature (used to create new
     feature not implemented in ViSP2
 */

#include <visp/vpMatrix.h>
#include <visp/vpBasicFeature.h>
#include <visp/vpRGBa.h>

#include <math.h>

/*!
  \class vpGenericFeature
  \ingroup VsFeatureGeneric

  \brief Class that enables to define a feature or a set of features which are not implemented in ViSP as a specific class. It is indeed possible to create its own features, to use the corresponding interaction matrix, and to compute an error between the current and the desired feature. Moreover the created features can be mixed with features already implemented.

  The following example shows how to use the vpGenericFeature class to create and use the feature \f$ log(Z) \f$ where Z corresponds to the depth of a point whose 2D coordinates in the camera frame are \f$ x \f$ and \f$ y \f$. The interaction matrix corresponding to this feature is \f[ L = \left[\begin{array}{cccccc} 0 & 0 & -1/Z & -y & x & 0 \end{array}\right]\f].
  \code
#include <visp/vpGenericFeature.h>
#include <visp/vpServo.h>

int main()
{
  vpServo task; // Visual servoing task

  //First we have to define the desired feature log(Z*) corresponding to the desired point.
  double xd = 0; //The x coordinate of the desired point.
  double yd = 0; //The y coordinate of the desired point.
  double Zd = 1; //The depth of the desired point.
  vpGenericFeature logZd(1); //The dimension of the feature is 1.
  logZd.set_s( log(Zd) );

  //Then we have to define the current feature log(Z) corresponding to the current point.
  double x = 1; //The x coordinate of the current point.
  double y = 1; //The y coordinate of the current point.
  double Z = 2; //The depth of the current point.
  vpGenericFeature logZ(1); //The dimension of the feature is 1.
  logZ.set_s( log(Z) );

  // Set eye-in-hand control law. 
  // The computed velocities will be expressed in the camera frame
  task.setServo(vpServo::EYEINHAND_CAMERA);
  // Interaction matrix is computed with the current visual features sd
  task.setInteractionMatrixType(vpServo::CURRENT);

  // Add the point feature to the task
  task.addFeature(logZ, logZd);

  // Control loop
  for ( ; ; ) {
    // The new parameters x, y and Z must be computed here.
    
    // Update the current point visual feature
    logZ.set_s( log(Z) ) ;

    // We have to compute the interaction matrix corresponding to the feature.
    vpMatrix LlogZ(1,6) ;
    LlogZ[0][0] = LlogZ[0][1] = LlogZ[0][5] = 0 ;
    LlogZ[0][2] = -1/Z;
    LlogZ[0][3] = -y;
    LlogZ[0][4] =  x;
    logZ.setInteractionMatrix(LlogZ) ;

    
    // compute the control law
    vpColVector v = task.computeControlLaw(); // camera velocity
  }
  return 0;
}
  \endcode

The second example shows how to create and use a feature whose specificity is to have a desired feature fixed to zero. It is the case for the feature \f$ log( \frac{Z}{Z^*}) \f$.

  \code
#include <visp/vpGenericFeature.h>
#include <visp/vpServo.h>

int main()
{
  vpServo task; // Visual servoing task

  //First we have to define the desired feature log(Z*) corresponding to the desired point.
  double xd = 0; //The x coordinate of the desired point.
  double yd = 0; //The y coordinate of the desired point.
  double Zd = 1; //The depth of the desired point.

  //Then we have to define the current feature log(Z) corresponding to the current point.
  double x = 1; //The x coordinate of the current point.
  double y = 1; //The y coordinate of the current point.
  double Z = 2; //The depth of the current point.
  vpGenericFeature logZ(1); //The dimension of the feature is 1.
  logZ.set_s( log(Z/Zd) );

  // Set eye-in-hand control law. 
  // The computed velocities will be expressed in the camera frame
  task.setServo(vpServo::EYEINHAND_CAMERA);
  // Interaction matrix is computed with the current visual features sd
  task.setInteractionMatrixType(vpServo::CURRENT);

  // Add the point feature to the task
  task.addFeature(logZ);

  // Control loop
  for ( ; ; ) {
    // The new parameters x, y and Z must be computed here.
    
    // Update the current point visual feature
    logZ.set_s( log(Z/Zd) ) ;

    // We have to compute the interaction matrix corresponding to the feature.
    vpMatrix LlogZ(1,6) ;
    LlogZ[0][0] = LlogZ[0][1] = LlogZ[0][5] = 0 ;
    LlogZ[0][2] = -1/Z;
    LlogZ[0][3] = -y;
    LlogZ[0][4] =  x;
    logZ.setInteractionMatrix(LlogZ) ;

    
    // compute the control law
    vpColVector v = task.computeControlLaw(); // camera velocity
  }
  return 0;
}
  \endcode

If the feature needs to be use with other features, the example servoSimuPoint2DhalfCamVelocity2.cpp shows how to do it.
 */
class VISP_EXPORT vpGenericFeature : public vpBasicFeature
{
private:
  vpGenericFeature() ;
public:
  void init() ;
  vpGenericFeature(unsigned int dim) ;
  virtual ~vpGenericFeature() ;
public:

  vpMatrix  interaction(const unsigned int select = FEATURE_ALL);

  vpColVector error(const vpBasicFeature &s_star,
		    const unsigned int select = FEATURE_ALL)  ;

  vpColVector error(const unsigned int select = FEATURE_ALL)  ;

  void print(const unsigned int select = FEATURE_ALL ) const ;

  vpGenericFeature *duplicate() const ;

private:
  typedef enum
    {
      errorNotInitalized,
      errorInitialized,
      errorHasToBeUpdated
    } vpGenericFeatureErrorType;

  vpMatrix L ;
  vpColVector err ;
  vpGenericFeatureErrorType errorStatus ;

public:
  void setInteractionMatrix(const vpMatrix &L) ;
  vpMatrix getInteractionMatrix() const { return L ; }
  void setError(vpColVector &error)  ;
  void set_s(const vpColVector &s) ;
  void set_s(const double s0) ;
  void set_s(const double s0, const double s1) ;
  void set_s(const double s0, const double s1, const double s2) ;
  
  void get_s(vpColVector &s) const;
  void get_s(double &s0) const;
  void get_s(double &s0, double &s1) const;
  void get_s(double &s0, double &s1, double &s2) const;

public:
  void display(const vpCameraParameters &cam,
               const vpImage<unsigned char> &I,
               const vpColor &color=vpColor::green,
               unsigned int thickness=1) const ;
  void display(const vpCameraParameters &cam,
               const vpImage<vpRGBa> &I,
               const vpColor &color=vpColor::green,
               unsigned int thickness=1) const ;


} ;

#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
