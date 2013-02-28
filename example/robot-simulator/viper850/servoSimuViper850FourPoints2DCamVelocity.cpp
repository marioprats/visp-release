/****************************************************************************
 *
 * $Id: servoSimuFourPoints2DPolarCamVelocityDisplay.cpp 2503 2010-02-16 18:55:01Z fspindle $
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
 * Simulation of a 2D visual servoing using 4 points with polar
 * coordinates as visual feature.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/


/*!
  \example servoSimuViper850FourPoints2DCamVelocity.cpp

  \brief Simulation of a 2D visual servoing:

  Simulation of a 2D visual servoing:
  - servo on 4 points with cartesian coordinates,
  - eye-in-hand control law,
  - camera velocities are computed,
  - display internal camera view and an external view.

  Interaction matrix is computed as the mean of the current and desired
  interaction matrix.

*/

#include <visp/vpDebug.h>
#include <visp/vpConfig.h>

#if (defined(WIN32) || defined(VISP_HAVE_PTHREAD)) && (defined (VISP_HAVE_X11) || defined(VISP_HAVE_OPENCV) || defined(VISP_HAVE_GDI))

// We need to use threading capabilities. Thus on Unix-like
// platforms, the libpthread third-party library need to be
// installed. On Windows, we use the native threading capabilities.

#include <stdlib.h>
#include <stdio.h>

#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpServo.h>
#include <visp/vpDebug.h>
#include <visp/vpFeatureBuilder.h>
#include <visp/vpFeaturePoint.h>
#include <visp/vpSimulatorViper850.h>

#include <visp/vpMeterPixelConversion.h>

#include <visp/vpImage.h>
#include <visp/vpImagePoint.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGTK.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpParseArgv.h>
#include <visp/vpIoTools.h>

// List of allowed command line options
#define GETOPTARGS	"cdh"

/*!

Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.

*/
void usage(const char *name, const char *badparam)
{
  fprintf(stdout, "\n\
Tests a control law with the following characteristics:\n\
- eye-in-hand control\n\
- articular velocity are computed\n\
- servo on 4 points,\n\
- internal and external camera view displays.\n\
          \n\
SYNOPSIS\n\
  %s [-c] [-d] [-h]\n", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -c\n\
     Disable the mouse click. Useful to automaze the \n\
     execution of this program without humain intervention.\n\
                  \n\
  -d \n\
     Turn off the display.\n\
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
  \param display : Display activation.
  \param click_allowed : Click activation.

  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, const char **argv, bool &click_allowed, bool &display)
{
  const char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'c': click_allowed = false; break;
    case 'd': display = false; break;
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
  bool opt_click_allowed = true;
  bool opt_display = true;

  // Read the command line options
  if (getOptions(argc, argv, opt_click_allowed, opt_display) == false) {
    exit (-1);
  }

  // We open two displays, one for the internal camera view, the other one for
  // the external view, using either X11, GTK or GDI.
#if defined VISP_HAVE_X11
  vpDisplayX displayInt;
#elif defined VISP_HAVE_GDI
  vpDisplayGDI displayInt;
#elif defined VISP_HAVE_OPENCV
  vpDisplayOpenCV displayInt;
#endif

  // open a display for the visualization

  vpImage<unsigned char> Iint(480, 640, 255);

  if (opt_display) {
    displayInt.init(Iint,700,0, "Internal view") ;
  }

  int i;
  vpServo task;


  std::cout << std::endl ;
  std::cout << "----------------------------------------------" << std::endl ;
  std::cout << " Test program for vpServo "  <<std::endl ;
  std::cout << " Eye-in-hand task control, articular velocity are computed" 
            << std::endl ;
  std::cout << " Simulation " << std::endl ;
  std::cout << " task : servo 4 points " << std::endl ;
  std::cout << "----------------------------------------------" << std::endl ;
  std::cout << std::endl ;


  vpTRACE("sets the initial camera location " ) ;
  vpHomogeneousMatrix cMo(-0.05,-0.05,0.7,
                          vpMath::rad(10),  vpMath::rad(10),  vpMath::rad(-30));


  vpTRACE("sets the point coordinates in the object frame "  ) ;
  vpPoint point[4] ;
  point[0].setWorldCoordinates(-0.045,-0.045,0) ;
  point[3].setWorldCoordinates(-0.045,0.045,0) ;
  point[2].setWorldCoordinates(0.045,0.045,0) ;
  point[1].setWorldCoordinates(0.045,-0.045,0) ;

  vpTRACE("project : computes  the point coordinates in the camera frame and its 2D coordinates"  ) ;
  for (i = 0 ; i < 4 ; i++)
    point[i].track(cMo) ;

  vpTRACE("sets the desired position of the point ") ;
  vpFeaturePoint p[4] ;
  for (i = 0 ; i < 4 ; i++)
    vpFeatureBuilder::create(p[i],point[i])  ;  //retrieve x,y and Z of the vpPoint structure
  
  vpTRACE("sets the desired position of the feature point s*") ;
  vpFeaturePoint pd[4] ;
  
  //Desired pose
  vpHomogeneousMatrix cdMo(vpHomogeneousMatrix(0.0,0.0,0.8,vpMath::rad(0),vpMath::rad(0),vpMath::rad(0)));
  
  // Projection of the points
  for (int i = 0 ; i < 4 ; i++)
    point[i].track(cdMo);
  
  for (int i = 0 ; i < 4 ; i++)
    vpFeatureBuilder::create(pd[i], point[i]);

  vpTRACE("define the task") ;
  vpTRACE("\t we want an eye-in-hand control law") ;
  vpTRACE("\t articular velocity are computed") ;
  task.setServo(vpServo::EYEINHAND_CAMERA);
  task.setInteractionMatrixType(vpServo::DESIRED) ;

  vpTRACE("\t we want to see a point on a point..") ;
  for (i = 0 ; i < 4 ; i++)
    task.addFeature(p[i],pd[i]) ;

  vpTRACE("\t set the gain") ;
  task.setLambda(0.8) ;
  
  /*Declaration of the robot*/
  vpSimulatorViper850 robot(opt_display);
  
  /*Initialise the robot and especially the camera*/
  robot.init(vpViper850::TOOL_PTGREY_FLEA2_CAMERA,vpCameraParameters::perspectiveProjWithoutDistortion);
  robot.setRobotState(vpRobot::STATE_VELOCITY_CONTROL);
  
  /*Initialise the object for the display part*/
  robot.initScene(vpWireFrameSimulator::PLATE, vpWireFrameSimulator::D_STANDARD);
  
  /*Initialise the position of the object relative to the pose of the robot's camera*/
  robot.initialiseObjectRelativeToCamera(cMo);
  
  /*Set the desired position (for the displaypart)*/
  robot.setDesiredCameraPosition(cdMo);
  
  /*Get the internal robot's camera parameters*/
  vpCameraParameters cam;
  robot.getCameraParameters(cam,Iint);
  
  if (opt_display)
  { 
    //Get the internal view
	  vpDisplay::display(Iint);
    robot.getInternalView(Iint);
    vpDisplay::flush(Iint);
  }


  vpTRACE("Display task information " ) ;
  task.print() ;

  unsigned int iter=0 ;
  vpTRACE("\t loop") ;
  while(iter++<500)
  {
    std::cout << "---------------------------------------------" << iter <<std::endl ;
    vpColVector v ;

    //Get the Time at the beginning of the loop
    double t = vpTime::measureTimeMs();

    //Get the current pose of the camera
    cMo = robot.get_cMo();

    if (iter==1) {
      std::cout <<"Initial robot position with respect to the object frame:\n";
      cMo.print();
    }

    if (iter==1) vpTRACE("\t new point position ") ;
    for (i = 0 ; i < 4 ; i++)
    {
      point[i].track(cMo) ;
      //retrieve x,y and Z of the vpPoint structure
      try {
        vpFeatureBuilder::create(p[i],point[i])  ;
      }
      catch(...)
      {
        break;
      }
    }

    if (opt_display)
    {
      /*Get the internal view and display it*/
      vpDisplay::display(Iint) ;
      robot.getInternalView(Iint);
      vpDisplay::flush(Iint);
    }

    if (opt_display && opt_click_allowed && iter == 1)
    {
      // suppressed for automate test
      vpTRACE("\n\nClick in the internal view window to continue...");
      vpDisplay::getClick(Iint) ;
    }

    if (iter==1) vpTRACE("\t\t compute the control law ") ;
    v = task.computeControlLaw() ;

    if (iter==1) vpTRACE("\t\t send the camera velocity to the controller ") ;
    robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;

    vpTRACE("\t\t || s - s* || ") ;
    std::cout << ( task.getError() ).sumSquare() <<std::endl ;

    /* The main loop as a duration of 10 ms at minimum*/
    vpTime::wait(t,10);
  }

  vpTRACE("Display task information " ) ;
  task.print() ;
  task.kill();


  std::cout <<"Final robot position with respect to the object frame:\n";
  cMo.print();

  if (opt_display && opt_click_allowed) 
  {
    // suppressed for automate test
    vpTRACE("\n\nClick in the internal view window to end...");
    vpDisplay::getClick(Iint) ;
  }
}
#else
int
main()
{
  vpERROR_TRACE("You do not have X11, OpenCV or GDI display functionalities or threading capabilities...");
}

#endif
