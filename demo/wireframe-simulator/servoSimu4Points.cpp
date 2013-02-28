/****************************************************************************
 *
 * $Id: servoSimu4Points.cpp 3625 2012-03-13 19:47:29Z fspindle $
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
 * Demonstration of the wireframe simulator with a simple visual servoing
 *
 * Authors:
 * Nicolas Melchior
 *
 *****************************************************************************/

/*!
  \file servoSimu4Points.cpp

  \brief Demonstration of the wireframe simulator with a simple visual servoing.
*/

/*!
  \example servoSimu4Points.cpp

  Demonstration of the wireframe simulator with a simple visual servoing.
*/


#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGTK.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpDisplayD3D.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpTime.h>

#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpFeaturePoint.h>
#include <visp/vpServo.h>
#include <visp/vpRobotCamera.h>
#include <visp/vpFeatureBuilder.h>
#include <visp/vpParseArgv.h>
#include <visp/vpIoTools.h>
#include <visp/vpVelocityTwistMatrix.h>
#include <visp/vpWireFrameSimulator.h>
#include <stdlib.h>
#define GETOPTARGS	"dh"

#if (defined (VISP_HAVE_X11) || defined(VISP_HAVE_OPENCV) || defined(VISP_HAVE_GDI) || defined(VISP_HAVE_D3D9) || defined(VISP_HAVE_GTK))

/*!

  Print the program options.

  \param name : Program name.
  \param ipath : Input image path.
  \param badparam : Bad parameter name.

*/
void usage(const char *name, std::string ipath, const char *badparam)
{
  fprintf(stdout, "\n\
Demonstration of the wireframe simulator with a simple visual servoing.\n\
          \n\
The visual servoing consists in bringing the camera at a desired \n\
position from the object.\n\
          \n\
The visual features used to compute the pose of the camera and \n\
thus the control law are four points.\n\
          \n\
This demonstration explains also how to move the object around a world\n\
reference frame. Here, the movement is a rotation around the x and y axis\n\
at a given distance from the world frame. In fact the object trajectory\n\
is on a sphere whose center is the origin of the world frame.\n\
          \n\
SYNOPSIS\n\
  %s [-d] [-h]\n", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -i <input image path>                                %s\n\
     Set mire.pgm image input path.\n\
     From this path read \"ViSP-images/mire/mire.pgm\" video.\n\
     Setting the VISP_INPUT_IMAGE_PATH environment variable \n\
     produces the same behaviour than using this option.\n\
            \n\
  -d \n\
     Turn off the display.\n\
            \n\
  -h\n\
     Print the help.\n", ipath.c_str());

  if (badparam)
    fprintf(stdout, "\nERROR: Bad parameter [%s]\n", badparam);
}


/*!

  Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.
  \param ipath : Input image path.
  \param display : Display activation.

  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, const char **argv, std::string &ipath, bool &display)
{
  const char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'i': ipath = optarg; break;
    case 'd': display = false; break;
    case 'h': usage(argv[0],ipath, NULL); return false; break;

    default:
      usage(argv[0],ipath, optarg);
      return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], ipath, NULL);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg << std::endl << std::endl;
    return false;
  }

  return true;
}


int
main(int argc, const char ** argv)
{
  bool opt_display = true;
  std::string opt_ipath;
  std::string env_ipath;
  std::string ipath ;
  std::string filename;
  
  // Read the command line options
  if (getOptions(argc, argv, opt_ipath, opt_display) == false) {
    exit (-1);
  }
  
  vpImage<vpRGBa> Iint(480,640,255);
  vpImage<vpRGBa> Iext1(480,640,255);
  vpImage<vpRGBa> Iext2(480,640,255);

#if defined VISP_HAVE_X11
  vpDisplayX display[3];
#elif defined VISP_HAVE_OPENCV
  vpDisplayOpenCV display[3];
#elif defined VISP_HAVE_GDI
  vpDisplayGDI display[3];
#elif defined VISP_HAVE_D3D9
  vpDisplayD3D display[3];
#elif defined VISP_HAVE_GTK
  vpDisplayGTK display[3];
#endif
  
  if (opt_display)
  {
    try
    {
      // Display size is automatically defined by the image (I) size
      display[0].init(Iint, 100, 100,"The internal view") ;
      display[1].init(Iext1, 100, 100,"The first external view") ;
      display[2].init(Iext2, 100, 100,"The second external view") ;
      vpDisplay::setWindowPosition (Iint, 0, 0);
      vpDisplay::setWindowPosition (Iext1, 700, 0);
      vpDisplay::setWindowPosition (Iext2, 0, 550);
      vpDisplay::display(Iint);
      vpDisplay::flush(Iint);
      vpDisplay::display(Iext1);
      vpDisplay::flush(Iext1);
      vpDisplay::display(Iext2);
      vpDisplay::flush(Iext2);
    }
    catch(...)
    {
      vpERROR_TRACE("Error while displaying the image") ;
      exit(-1);
    }
  }

  vpServo task;
  vpRobotCamera robot ;
  float sampling_time = 0.040f; // Sampling period in second
  robot.setSamplingTime(sampling_time);

  //cMo initial
  vpPoseVector cMoi(0,0.1,2.0,vpMath::rad(35),vpMath::rad(25),0);

  vpHomogeneousMatrix cMo(cMoi);
  
  //The four point used as visual features
  vpPoint point[4] ;
  point[0].setWorldCoordinates(-0.1,-0.1,0) ;
  point[3].setWorldCoordinates(-0.1,0.1,0) ;
  point[2].setWorldCoordinates(0.1,0.1,0) ;
  point[1].setWorldCoordinates(0.1,-0.1,0) ;
  
  // Projection of the points
  for (int i = 0 ; i < 4 ; i++)
    point[i].track(cMo);

  //Set the current visual feature
  vpFeaturePoint p[4];
  for (int i = 0 ; i < 4 ; i++)
    vpFeatureBuilder::create(p[i], point[i]);
  
  //cMo desired
  vpHomogeneousMatrix cdMo(vpHomogeneousMatrix(0.0,0.0,1.0,vpMath::rad(0),vpMath::rad(0),vpMath::rad(0)));
  
  // Projection of the points
  for (int i = 0 ; i < 4 ; i++)
    point[i].track(cdMo);

  vpFeaturePoint pd[4];
  for (int i = 0 ; i < 4 ; i++)
    vpFeatureBuilder::create(pd[i], point[i]);

  task.setServo(vpServo::EYEINHAND_L_cVe_eJe);
  task.setInteractionMatrixType(vpServo::DESIRED);
  
  vpHomogeneousMatrix cMe;
  vpVelocityTwistMatrix cVe(cMe);
  task.set_cVe(cVe);

  vpMatrix eJe;
  robot.get_eJe(eJe);
  task.set_eJe(eJe);
  
  for (int i = 0 ; i < 4 ; i++)
    task.addFeature(p[i],pd[i]) ;
  
  task.setLambda(10);
  
  std::list<vpImageSimulator> list;
  vpImageSimulator imsim;
  
  vpColVector X[4];
  for (int i = 0; i < 4; i++) X[i].resize(3);
  X[0][0] = -0.2;
  X[0][1] = -0.2;
  X[0][2] = 0;
  
  X[1][0] = 0.2;
  X[1][1] = -0.2;
  X[1][2] = 0;
  
  X[2][0] = 0.2;
  X[2][1] = 0.2;
  X[2][2] = 0;
  
  X[3][0] = -0.2;
  X[3][1] = 0.2;
  X[3][2] = 0;
  
  char *ptenv = getenv("VISP_INPUT_IMAGE_PATH");
  if (ptenv != NULL)
    env_ipath = ptenv;
  
  if (! env_ipath.empty())
    ipath = env_ipath;
  
  if (!opt_ipath.empty())
    ipath = opt_ipath;
  
  filename = ipath +  vpIoTools::path("/ViSP-images/mire/mire.pgm");

  try
  {
    imsim.init(filename.c_str(), X);
  }
  catch(...)
  {
    vpTRACE("You need the ViSP data ");
    task.kill();
    return 1;
  }
  
  list.push_back(imsim);
  
  vpWireFrameSimulator sim;
  
  //Set the scene
  sim.initScene(vpWireFrameSimulator::PLATE, vpWireFrameSimulator::D_STANDARD,list);
  
  //Set the initial and the desired position of the camera.
  sim.setCameraPositionRelObj(cMoi) ;
  sim.setDesiredCameraPosition(cdMo);
  
  //Set the External camera position
  vpHomogeneousMatrix camMf(vpHomogeneousMatrix(0.0,0,3.5,vpMath::rad(0),vpMath::rad(30),0));
  sim.setExternalCameraPosition(camMf);
  
  //Move the object in the world reference frame
  sim.set_fMo(vpHomogeneousMatrix(0.0,0.0,0.2,0,0,0));
  
  //Computes the position of a camera which is fixed in the object frame
  vpHomogeneousMatrix camoMf;
  vpHomogeneousMatrix temp(vpHomogeneousMatrix(0,0.0,1.5,0,vpMath::rad(140),0)*(sim.get_fMo().inverse()));
  vpTranslationVector T;
  vpRotationMatrix R;
  temp.extract(T);
  temp.extract(R);
  camoMf.buildFrom(T,R);
  
  //Set the parameters of the cameras (internal and external)
  vpCameraParameters camera(1000,1000,320,240);
  sim.setInternalCameraParameters(camera);
  sim.setExternalCameraParameters(camera);
  
  int stop = 10;
  
  if (opt_display)
  {
    stop = 2500;
    
    //Get the internal and external views
    sim.getInternalImage(Iint);
    sim.getExternalImage(Iext1);
    sim.getExternalImage(Iext2,camoMf);

    //Display the object frame (current and desired position)
    vpDisplay::displayFrame(Iint,cMo,camera,0.2,vpColor::none);
    vpDisplay::displayFrame(Iint,cdMo,camera,0.2,vpColor::none);

    //Display the object frame the world reference frame and the camera frame
    vpDisplay::displayFrame(Iext1,camMf*sim.get_fMo()*cMo.inverse(),camera,0.2,vpColor::none);
    vpDisplay::displayFrame(Iext1,camMf*sim.get_fMo(),camera,0.2,vpColor::none);
    vpDisplay::displayFrame(Iext1,camMf,camera,0.2,vpColor::none);

    //Display the world reference frame and the object frame
    vpDisplay::displayFrame(Iext2,camoMf,camera,0.2,vpColor::none);
    vpDisplay::displayFrame(Iext2,camoMf*sim.get_fMo(),camera,0.05,vpColor::none);

    vpDisplay::flush(Iint);
    vpDisplay::flush(Iext1);
    vpDisplay::flush(Iext2);
    
    std::cout << "Click on a display" << std::endl;
    while (!vpDisplay::getClick(Iint,false) && !vpDisplay::getClick(Iext1,false) && !vpDisplay::getClick(Iext2,false)){};
  }

  robot.setPosition(sim.get_cMo());
  //Print the task
  task.print() ;

  int iter = 0;
  vpColVector v ;

  while(iter++ < stop)
  {
    if (opt_display)
    {
      vpDisplay::display(Iint) ;
      vpDisplay::display(Iext1) ;
      vpDisplay::display(Iext2) ;
    }

    double t = vpTime::measureTimeMs();

    robot.get_eJe(eJe) ;
    task.set_eJe(eJe) ;

    robot.getPosition(cMo) ;
    for (int i = 0 ; i < 4 ; i++)
    {
      point[i].track(cMo) ;
      vpFeatureBuilder::create(p[i],point[i])  ;
    }

    v = task.computeControlLaw() ;
    robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;

    //Get the camera position
    robot.getPosition(cMo) ;
    
    //Compute the movement of the object around the world reference frame.
    vpHomogeneousMatrix a(0.0,0.0,0.2,vpMath::rad(0*iter),0,0);
    vpHomogeneousMatrix b(0,0.0,0.0,vpMath::rad(1.5*iter),0,0);
    vpHomogeneousMatrix c(0,0.0,0.0,0,vpMath::rad(2.5*iter),0);

    vpHomogeneousMatrix cMf = cMo*sim.get_fMo().inverse(); //The camera position in the world frame

    sim.set_fMo(b*c*a);  //Move the object in the simulator

    //Indicates to the task the movement of the object
    cMo = cMf*b*c*a;
    robot.setPosition(cMo);
    sim.setCameraPositionRelObj(cMo);

    //Compute the position of the external view which is fixed in the object frame
    vpHomogeneousMatrix temp(vpHomogeneousMatrix(0,0.0,1.5,0,vpMath::rad(150),0)*(sim.get_fMo().inverse()));
    vpTranslationVector T;
    vpRotationMatrix R;
    temp.extract(T);
    temp.extract(R);
    camoMf.buildFrom(T,R);

    if (opt_display)
    {
      //Get the internal and external views
      sim.getInternalImage(Iint);
      sim.getExternalImage(Iext1);
      sim.getExternalImage(Iext2,camoMf);

      //Display the object frame (current and desired position)
      vpDisplay::displayFrame(Iint,cMo,camera,0.2,vpColor::none);
      vpDisplay::displayFrame(Iint,cdMo,camera,0.2,vpColor::none);

      //Display the object frame the world reference frame and the camera frame
      vpDisplay::displayFrame(Iext1,sim.getExternalCameraPosition()*sim.get_fMo()*cMo.inverse(),camera,0.2,vpColor::none);
      vpDisplay::displayFrame(Iext1,sim.getExternalCameraPosition()*sim.get_fMo(),camera,0.2,vpColor::none);

      vpDisplay::displayFrame(Iext1,sim.getExternalCameraPosition(),camera,0.2,vpColor::none);

      //Display the world reference frame and the object frame
      vpDisplay::displayFrame(Iext2,camoMf,camera,0.2,vpColor::none);
      vpDisplay::displayFrame(Iext2,camoMf*sim.get_fMo(),camera,0.05,vpColor::none);

      vpDisplay::flush(Iint);
      vpDisplay::flush(Iext1);
      vpDisplay::flush(Iext2);
    }

    vpTime::wait(t, sampling_time * 1000); // Wait 40 ms

    //vpTRACE("\t\t || s - s* || ") ;
    //std::cout << ( task.getError() ).sumSquare() <<std::endl ;
  }

  task.print() ;
  task.kill() ;

  return 0;
}
#else
int
main()
{
  vpERROR_TRACE("You do not have X11, OpenCV, GDI, D3D9 or GTK display functionalities...");
}

#endif
