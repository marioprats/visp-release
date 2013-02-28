/****************************************************************************
 *
 * $Id: vpRobot.h 3695 2012-05-03 07:20:19Z fspindle $
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
 * Generic virtual robot.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/

#ifndef vpRobot_H
#define vpRobot_H

/*!
  \file vpRobot.h
  \brief class that defines a generic virtual robot
*/

#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpMatrix.h>
#include <visp/vpColVector.h>
#include <visp/vpPoseVector.h>


/*!
  \class vpRobot
  \brief class that defines a generic virtual robot
*/
class VISP_EXPORT vpRobot
{
public:
  /*!
    Robot control states.
  */
  typedef enum 
    {
      STATE_STOP,  /*!< Stops robot motion especially in velocity and
		     acceleration control. */
      STATE_VELOCITY_CONTROL, //!< Initialize the velocity controller.
      STATE_POSITION_CONTROL, //!< Initialize the position controller.
      STATE_ACCELERATION_CONTROL //!< Initialize the acceleration controller.
    } vpRobotStateType ;

    /** \brief valeur utilisee par default pour l'etat du robot a
     * la construction. */
  static const vpRobot::vpRobotStateType
  defaultEtatRobot = vpRobot::STATE_STOP;

  /*!
    Robot control frames. 
  */
  typedef enum 
    {
      
      REFERENCE_FRAME, /*!< Corresponds to a fixed reference frame
	attached to the robot structure. */
      ARTICULAR_FRAME, /*!< Corresponds to the joint space. */
      CAMERA_FRAME,    /*!< Corresponds to a frame attached to the
	camera mounted on the robot end-effector. */
      MIXT_FRAME /*!< Corresponds to a "virtual" frame where
	translations are expressed in the reference frame, and
	rotations in the camera frame.*/      
    } vpControlFrameType ;

    static const vpRobot::vpControlFrameType
    defaultFrameRobot = vpRobot::CAMERA_FRAME ;

private:  /* Membres privees */
    vpRobot::vpRobotStateType   stateRobot;
    vpRobot::vpControlFrameType   frameRobot; 


protected:
  double maxTranslationVelocity;
  static const double maxTranslationVelocityDefault;// = 0.2;
  double maxRotationVelocity;
  static const double maxRotationVelocityDefault;// = 0.7;

  //! number of degrees of freedom
  int nDof ;
  //! robot Jacobian expressed in the end-effector frame
  vpMatrix eJe ;
  //! is the robot Jacobian expressed in the end-effector frame available
  int eJeAvailable ;
  //! robot Jacobian expressed in the robot reference frame available
  vpMatrix fJe ;
  //! is the robot Jacobian expressed in the robot reference frame available
  int fJeAvailable ;

public:
  vpRobot (void);
  virtual ~vpRobot() { ; }

  //---------- Jacobian -----------------------------
  //! Get the robot Jacobian expressed in the end-effector frame
  virtual void get_eJe(vpMatrix &_eJe)  = 0 ;
  //! Get the robot Jacobian expressed in the robot reference (or world) frame.
  virtual void get_fJe(vpMatrix &_fJe)  = 0 ;

  //! Get a displacement expressed in the joint space between two successive position control.
  virtual void getArticularDisplacement(vpColVector  &qdot) = 0 ;
  //! Get a displacement expressed in the camera frame between two successive position control.
  virtual void getCameraDisplacement(vpColVector &v) = 0 ;
  //! Get a displacement (frame as to ve specified) between two successive position control.
  virtual void getDisplacement(const vpRobot::vpControlFrameType frame,
                               vpColVector &q) = 0 ;

  double getMaxTranslationVelocity (void) const ;
  double getMaxRotationVelocity (void) const;
  //! Get the robot position (frame has to be specified).
  virtual void getPosition(const vpRobot::vpControlFrameType frame,
                           vpColVector &q)   = 0 ;

  //! Return the robot position (frame has to be specified).
  vpColVector getPosition (const vpRobot::vpControlFrameType frame);
  vpControlFrameType   getRobotFrame (void) { return frameRobot ; }
  virtual vpRobotStateType     getRobotState (void) { return stateRobot ; }

  virtual void init() = 0 ;

  static vpColVector saturateVelocities(const vpColVector &v_in, const vpColVector &v_max, bool verbose=false);

  void setMaxRotationVelocity (const double maxVr);
  void setMaxTranslationVelocity (const double maxVt);
  //! Set a displacement (frame has to be specified) in position control.
  virtual void setPosition(const vpRobot::vpControlFrameType frame,
                           const vpColVector &q)   = 0 ;
  vpControlFrameType   setRobotFrame (vpRobot::vpControlFrameType newFrame);
  virtual vpRobotStateType setRobotState (const vpRobot::vpRobotStateType newState);

  //! Set the velocity (frame has to be specified) that will be applied to the velocity controller.
  virtual void setVelocity(const vpRobot::vpControlFrameType frame,
                           const vpColVector &vel) = 0 ;

  /*
    Joint limits stuff
  */
private:
  int areJointLimitsAvailable ;
  double *qmin;
  double *qmax ;
public:
  //  virtual void getJointLimits() ;
} ;

#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
