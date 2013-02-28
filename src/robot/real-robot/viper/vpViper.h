/****************************************************************************
 *
 * $Id: vpViper.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Interface for a  generic ADEPT Viper (either 650 or 850) robot.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/

#ifndef vpViper_h
#define vpViper_h

/*!

  \file vpViper.h

  Modelisation of the ADEPT Viper 650 or 850 robot.

*/

#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpImage.h>
#include <visp/vpRGBa.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpVelocityTwistMatrix.h>
#include <visp/vpRobotException.h>

/*!

  \class vpViper

  \ingroup Viper

  \brief Modelisation of the ADEPT Viper robot 

  This robot has six degrees of freedom.

  The non modified Denavit-Hartenberg representation of the robot is
  given in the table below, where \f$q_1^*, \ldots, q_6^*\f$
  are the variable joint positions.

  \f[
  \begin{tabular}{|c|c|c|c|c|}
  \hline
  Joint & $a_i$ & $d_i$ & $\alpha_i$ & $\theta_i$ \\
  \hline
  1 & $a_1$ & $d_1$ & $-\pi/2$ & $q_1^*$ \\
  2 & $a_2$ & 0     & 0        & $q_2^*$ \\
  3 & $a_3$ & 0     & $-\pi/2$ & $q_3^* - \pi$ \\
  4 & 0     & $d_4$ & $\pi/2$  & $q_4^*$ \\
  5 & 0     & 0     & $-\pi/2$ & $q_5^*$ \\
  6 & 0     & 0     & 0        & $q_6^*-\pi$ \\
  7 & 0     & $d_6$ & 0        & 0 \\
  \hline
  \end{tabular}
  \f]
  
  In this modelisation, different frames have to be considered.
  - \f$ {\cal F}_f \f$: the reference frame, also called world frame,

  - \f$ {\cal F}_w \f$: the wrist frame located at the intersection of
    the last three rotations, with \f$ ^f{\bf M}_w = ^0{\bf M}_6 \f$,

  - \f$ {\cal F}_e \f$: the end-effector frame, with \f$^f{\bf M}_e =
    0{\bf M}_7 \f$,

  - \f$ {\cal F}_c \f$: the camera frame, with \f$^f{\bf M}_c = ^f{\bf
    M}_e \; ^e{\bf M}_c \f$ where \f$ ^e{\bf M}_c \f$ is the result of
    a calibration stage.
  
  The forward kinematics of the robot is implemented in get_fMw(),
  get_fMe() and get_fMc().

  The robot forward jacobian used to compute the cartesian velocities
  from joint ones is given and implemented in get_fJw(), get_fJe() and
  get_eJe().

*/
class VISP_EXPORT vpViper
{
 public:
  vpViper();
  virtual ~vpViper() {};

  vpHomogeneousMatrix getForwardKinematics(const vpColVector & q);
  unsigned int getInverseKinematicsWrist(const vpHomogeneousMatrix & fMw, vpColVector & q);
  unsigned int getInverseKinematics(const vpHomogeneousMatrix & fMc, vpColVector & q);
  vpHomogeneousMatrix get_fMc (const vpColVector & q);
  void get_fMw(const vpColVector & q, vpHomogeneousMatrix & fMw);
  void get_wMe(vpHomogeneousMatrix & wMe);
  void get_eMc(vpHomogeneousMatrix & eMc);
  void get_fMe(const vpColVector & q, vpHomogeneousMatrix & fMe);
  void get_fMc(const vpColVector & q, vpHomogeneousMatrix & fMc);

  void get_cMe(vpHomogeneousMatrix &cMe) ;
  void get_cVe(vpVelocityTwistMatrix &cVe) ;
  void get_fJw(const vpColVector &q, vpMatrix &fJw)  ;
  void get_fJe(const vpColVector &q, vpMatrix &fJe)  ;
  void get_eJe(const vpColVector &q, vpMatrix &eJe)  ;

  friend VISP_EXPORT std::ostream & operator << (std::ostream & os,
						 const vpViper & viper);

  vpColVector getJointMin();
  vpColVector getJointMax();
  double getCoupl56();

 private:
  bool convertJointPositionInLimits(unsigned int joint, const double &q, double &q_mod);

 public:
  static const unsigned int njoint; ///< Number of joint.

 protected:
  vpHomogeneousMatrix eMc; //!< End effector to camera transformation
  // Minimal representation of eMc
  vpTranslationVector etc; // meters
  vpRxyzVector        erc; // radian

  // Denavit-Hartenberg parameters
  double a1, d1; //!< for joint 1
  double a2;     //!< for joint 2
  double a3;     //!< for joint 3
  double d4;     //!< for joint 4
  double d6;     //!< for joint 6
  double c56;    //!< Mechanical coupling between joint 5 and joint 6
  
  // Software joint limits in radians
  vpColVector joint_max; // Maximal value of the joints
  vpColVector joint_min; // Minimal value of the joints


};

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */

#endif

