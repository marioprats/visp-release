
/****************************************************************************
 *
 * $Id: servoSimuPoint2DhalfCamVelocity3.cpp 2457 2010-01-07 10:41:18Z nmelchio $
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
 * Simulation of a 2 1/2 D visual servoing using theta U visual features.
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \file servoSimuPoint2DhalfCamVelocity3.cpp
  \brief Simulation of a 2 1/2 D visual servoing (x,y, t,theta u_z)
  - (x,y, t,theta u_z) features
  - eye-in-hand control law,
  - velocity computed in the camera frame,
  - no display.
*/

/*!
  \example servoSimuPoint2DhalfCamVelocity3.cpp
  Simulation of a 2 1/2 D visual servoing (x,y, t,theta u_z)
  - (x,y, t,theta u_z) features
  - eye-in-hand control law,
  - velocity computed in the camera frame,
  - no display.
*/



#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpPoint.h>
#include <visp/vpFeaturePoint.h>
#include <visp/vpFeatureThetaU.h>
#include <visp/vpGenericFeature.h>
#include <visp/vpServo.h>
#include <visp/vpRobotCamera.h>
#include <visp/vpDebug.h>
#include <visp/vpFeatureBuilder.h>
#include <visp/vpParseArgv.h>
#include <stdlib.h>
#include <stdio.h>
// List of allowed command line options
#define GETOPTARGS	"h"

/*!

Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.

*/
void usage(const char *name, const char *badparam)
{
  fprintf(stdout, "\n\
Simulation of a 2 1/2 D visual servoing (x,y,logZ, theta U):\n\
- eye-in-hand control law,\n\
- velocity computed in the camera frame,\n\
- without display.\n\
          \n\
SYNOPSIS\n\
  %s [-h]\n", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
                  \n\
  -h\n\
     Print the help.\n");

  if (badparam)
    fprintf(stdout, "\nERROR: Bad parameter [%s]\n", badparam);
}

/*!

Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.
  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, const char **argv)
{
  const char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'h': usage(argv[0], NULL); return false; break;

    default:
      usage(argv[0], optarg);
      return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], NULL);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg << std::endl << std::endl;
    return false;
  }

  return true;
}

int
main(int argc, const char ** argv)
{
  // Read the command line options
  if (getOptions(argc, argv) == false) {
    exit (-1);
  }
  
  std::cout << std::endl ;
  std::cout << "-------------------------------------------------------" << std::endl ;
  std::cout << " simulation of a 2 1/2 D visual servoing " << std::endl ;
  std::cout << "-------------------------------------------------------" << std::endl ;
  std::cout << std::endl ;

  // In this example we will simulate a visual servoing task.
  // In simulation, we have to define the scene frane Ro and the
  // camera frame Rc.
  // The camera location is given by an homogenous matrix cMo that
  // describes the position of the camera in the scene frame.


  vpServo task ;
  

  vpTRACE("sets the initial camera location " ) ;
  // we give the camera location as a size 6 vector (3 translations in meter
  // and 3 rotation (theta U representation) )
  vpPoseVector c_r_o(0.1,0.2,2,
                     vpMath::rad(20), vpMath::rad(10),  vpMath::rad(50)
                     ) ;

  // this pose vector is then transformed in a 4x4 homogeneous matrix
  vpHomogeneousMatrix cMo(c_r_o) ;
  
  // We define a robot
  // The vpRobotCamera implements a simple moving that is juste defined
  // by its location cMo
  vpRobotCamera robot ;

  // the robot position is set to the defined cMo position
  robot.setPosition(cMo) ;

  // Now that the current camera position has been defined,
  // let us defined the defined camera location.
  // It is defined by cdMo
  vpTRACE("sets the desired camera location " ) ;
  vpPoseVector cd_r_o(0,0,1,
                      vpMath::rad(0),vpMath::rad(0),vpMath::rad(0)) ;
  vpHomogeneousMatrix cdMo(cd_r_o) ;



  //----------------------------------------------------------------------
  // A 2 1/2 D visual servoing can be defined by
  // - the position of a point x,y
  // - the difference between this point depth and a desire depth
  //   modeled by log Z/Zd to be regulated to 0
  // - the rotation that the camera has to realized cdMc

  // Let us now defined the current value of these features


  // since we simulate we have to define a 3D point that will
  // forward-projected to define the current position x,y of the
  // reference point

  //------------------------------------------------------------------
  // First feature (x,y)
  //------------------------------------------------------------------
  vpTRACE("1st feature (x,y)");
  // Let oP be this ... point,
  // a vpPoint class has three main member
  // .oP : 3D coordinates in scene frame
  // .cP : 3D coordinates in camera frame
  // .p : 2D

  //------------------------------------------------------------------
  vpTRACE("\tsets the point coordinates in the world frame "  ) ;
  vpPoint P ;
  // defined point coordinates in the scene frame : oP
  P.setWorldCoordinates(0,0,0) ;
  // computes  the P coordinates in the camera frame and its
  // 2D coordinates cP and then p
  vpTRACE("\tproject : computes  the point coordinates in the camera frame and its 2D coordinates"  ) ;
  P.track(cMo) ;

  // We also defined (again by forward projection) the desired position
  // of this point according to the desired camera position
  vpPoint Pd ;
  Pd.setWorldCoordinates(0,0,0) ;
  Pd.track(cdMo) ;
  
  // Nevertheless, a vpPoint is not a feature, this is just a "tracker"
  // from which the feature are built
  // a feature is juste defined by a vector s, a way to compute the
  // interaction matrix and the error, and if required a (or a vector of)
  // 3D information

  // for a point (x,y) Visp implements the vpFeaturePoint class.
  // we no defined a feature for x,y (and for (x*,y*))
  vpFeaturePoint p,pd ;
  
  // and we initialized the vector s=(x,y) of p from the tracker P
  // Z coordinates in p is also initialized, it will be used to compute
  // the interaction matrix
  vpFeatureBuilder::create(p,P)  ;
  vpFeatureBuilder::create(pd,Pd)  ;


  // This visual has to be regulated to zero
  vpTRACE("2nd feature ThetaUz and 3rd feature t") ;

  //------------------------------------------------------------------
  // 2nd feature ThetaUz and 3rd feature t
  // The thetaU feature is defined, tu represents the rotation that the camera
  // has to realized. t the translation.
  // the complete displacement is then defined by:
  //------------------------------------------------------------------
  vpHomogeneousMatrix cdMc ;
  vpTRACE("\tcompute the rotation that the camera has to realize "  ) ;
  cdMc = cdMo*cMo.inverse() ;

  // from this displacement, we extract the rotation cdRc represented by
  // the angle theta and the rotation axis u
  vpFeatureThetaU tuz(vpFeatureThetaU::cdRc) ;
  tuz.buildFrom(cdMc) ;
  // And the translations
  vpFeatureTranslation t(vpFeatureTranslation::cdMc) ;
  t.buildFrom(cdMc) ;
  
  // This visual has to be regulated to zero

  vpTRACE("\tsets the desired rotation (always zero !) ") ;
  vpTRACE("\tsince s is the rotation that the camera has to realize ") ;


  //------------------------------------------------------------------
  // Let us now the task itself
  //------------------------------------------------------------------

  vpTRACE("define the task") ;
  vpTRACE("\t we want an eye-in-hand control law") ;
  vpTRACE("\t robot is controlled in the camera frame") ;
  //  we choose to control the robot in the camera frame
  task.setServo(vpServo::EYEINHAND_CAMERA) ;
  // Interaction matrix is computed with the current value of s
  task.setInteractionMatrixType(vpServo::CURRENT) ;
  
  // we build the task by "stacking" the visual feature
  // previously defined
  task.addFeature(t) ;
  task.addFeature(p,pd) ;
  task.addFeature(tuz,vpFeatureThetaU::TUz) ; //selection of TUz
  
  // addFeature(X,Xd) means X should be regulated to Xd
  // addFeature(X) means that X should be regulated to 0
  // some features such as vpFeatureThetaU MUST be regulated to zero
  // (otherwise, it will results in an error at exectution level)

  vpTRACE("\t set the gain") ;
  task.setLambda(1) ;


  vpTRACE("Display task information " ) ;
  task.print() ;
  //------------------------------------------------------------------
  // An now the closed loop

  unsigned int iter=0 ;
  vpTRACE("\t loop") ;
  while(iter++<200)
  {
    std::cout << "---------------------------------------------" << iter <<std::endl ;
    vpColVector v ;

    if (iter==1) vpTRACE("\t\t get the robot position ") ;
    robot.getPosition(cMo) ;

    if (iter==1) vpTRACE("\t\t update the feature ") ;
    P.track(cMo) ;
    vpFeatureBuilder::create(p,P)  ;

    cdMc = cdMo*cMo.inverse() ;
    tuz.buildFrom(cdMc) ;
    t.buildFrom(cdMc) ;



    if (iter==1) vpTRACE("\t\t compute the control law ") ;
    // compute v = -lambda L^+(s-sd)
    v = task.computeControlLaw() ;

    if (iter==1) task.print() ;

    if (iter==1) vpTRACE("\t\t send the camera velocity to the controller ") ;
    robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;
    // Note that for vpRobotCamera, camera position cMo, is updated using the
    // exponential map.


    std::cout << ( task.getError() ).sumSquare() <<std::endl ; ;
  }

  vpTRACE("Display task information " ) ;
  task.print() ;
  task.kill();
  vpTRACE("Final camera location " ) ;
  std::cout << cMo << std::endl ;
}

