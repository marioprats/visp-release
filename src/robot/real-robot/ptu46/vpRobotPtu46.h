/****************************************************************************
 *
 * $Id: vpRobotPtu46.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Interface for the ptu-46 robot.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/


#include <visp/vpConfig.h>
#ifdef VISP_HAVE_PTU46


#ifndef __vpROBOT_PTU46_H
#define __vpROBOT_PTU46_H


/* ------------------------------------------------------------------------ */
/* --- INCLUDES ----------------------------------------------------------- */
/* ------------------------------------------------------------------------ */

/* --- GENERAL --- */
#include <iostream>
#include <stdio.h>


/* --- ViSP --- */
#include <visp/vpRobot.h>
#include <visp/vpPtu46.h>
#include <visp/vpColVector.h>
#include <visp/vpDebug.h>
#include <visp/vpVelocityTwistMatrix.h>

#include <ptu.h> // Contrib for Ptu-46 robot

/* ------------------------------------------------------------------------ */
/* --- CLASS ------------------------------------------------------------- */
/* ------------------------------------------------------------------------ */


/*!

  \class vpRobotPtu46

  \ingroup Ptu46 RobotDriver

  \brief Interface for the Directed Perception ptu-46 pan, tilt head .

  See http://www.DPerception.com for more details.

  This class provide a position and a speed control interface for the ptu-46
  head.

*/
class VISP_EXPORT vpRobotPtu46
  :
  public vpPtu46,
  public vpRobot
{

private:

  /*! \brief No copy contructor allowed.   */
  vpRobotPtu46 (const vpRobotPtu46 & ass);

  /*! Object to control. This is a contribution. */
  Ptu ptu;

private:
  static bool robotAlreadyCreated;
  double      positioningVelocity;
  int         velocityMesureTempo;
  char	      *device;

public:

  static const double       defaultPositioningVelocity;

  vpRobotPtu46 (const char *device="/dev/ttyS0");
  vpRobotPtu46 (vpRobotPtu46 * pub);
  virtual ~vpRobotPtu46 (void);

  void init (void);


  /* --- STATE ------------------------------------------------------------- */

  vpRobot::vpRobotStateType   setRobotState (vpRobot::vpRobotStateType newState);

  /* --- POSITION --------------------------------------------------- */

  void setPosition(const vpRobot::vpControlFrameType frame,
		   const vpColVector &q) ;
  void setPosition (const vpRobot::vpControlFrameType frame,
		    const double &q1, const double &q2) ;
  void setPosition(const char *filename) ;
  void getPosition (const vpRobot::vpControlFrameType frame,
		    vpColVector &q);


  void   setPositioningVelocity (const double velocity);
  double getPositioningVelocity (void);


  /* --- SPEED ---------------------------------------------------------- */

  void setVelocity (const vpRobot::vpControlFrameType frame,
		    const vpColVector & q_dot);


  void getVelocity (const vpRobot::vpControlFrameType frame,
		    vpColVector & q_dot);

  vpColVector getVelocity (const vpRobot::vpControlFrameType frame);

public:
  void get_cMe(vpHomogeneousMatrix &_cMe) ;
  void get_cVe(vpVelocityTwistMatrix &_cVe) ;
  void get_eJe(vpMatrix &_eJe)  ;
  void get_fJe(vpMatrix &_fJe)  ;

  void stopMotion() ;

  void getCameraDisplacement(vpColVector &d);
  void getArticularDisplacement(vpColVector  &d);
  void getDisplacement(vpRobot::vpControlFrameType  frame, vpColVector &q);

  bool readPositionFile(const char *filename, vpColVector &q);
};



#endif /* #ifndef __vpROBOT_PTU46_H */


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */

#endif
