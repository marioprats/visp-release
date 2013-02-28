/****************************************************************************
 *
 * $Id: servoPioneerPanSegment3D.cpp 3842 2012-07-13 22:21:42Z fspindle $
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
 * IBVS on Pioneer P3DX mobile platform
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/
#include <iostream>

#include <visp/vpConfig.h>

#include <visp/vpRobotPioneer.h> // Include before vpDisplayX to avoid build issues
#include <visp/vpRobotBiclops.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDot2.h>
#include <visp/vpFeatureBuilder.h>
#include <visp/vpFeatureSegment.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpImage.h>
#include <visp/vp1394TwoGrabber.h>
#include <visp/vp1394CMUGrabber.h>
#include <visp/vpV4l2Grabber.h>
#include <visp/vpPioneerPan.h>
#include <visp/vpPlot.h>
#include <visp/vpServo.h>
#include <visp/vpVelocityTwistMatrix.h>

#define USE_REAL_ROBOT
#define USE_PLOTTER

/*!
  \example servoPioneerPanSegment3D.cpp

  Example that shows how to control the Pioneer mobile robot by IBVS visual servoing with respect to a segment.
  The segment consists in two horizontal dots. The current visual features that are used are
  \f${\bf s} = (x_n, l_n, \alpha)\f$. The desired one are \f${\bf s^*} = (0, l_n*, 0)\f$, with:
  - \f$x_n\f$ the normalized abscisse of the point corresponding to segment
  - \f$l_n\f$ the normalized segment length
  - \f$\alpha\f$ the segment orientation.

  The degrees of freedom that are controlled are \f$(v_x, w_z, \dot{q})\f$, the translational and rotational
  velocity of the mobile platform at point M located at the middle between the two wheels, the head pan
  velocity respectively.

  The depth of the points is estimated from the surface of the blob.

  */
#if defined(VISP_HAVE_PIONEER) && defined(VISP_HAVE_BICLOPS)
int main(int argc, char **argv)
{
#if defined(VISP_HAVE_DC1394_2) || defined(VISP_HAVE_V4L2) || defined(VISP_HAVE_CMU1394)
#if defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI)
  vpImage<unsigned char> I; // Create a gray level image container
  double lambda = 0.05;
  // Scale parameter used to estimate the depth Z of the blob from its surface
  double coef = 0.9/14.85;  // At 0.9m, the blob has a surface of 14.85
  double L = 0.21; // 3D horizontal segment length
  double Z_d = 1.; // Desired distance along Z between camera and segment

  // Warning: To have a non singular task of rank 3, Y_d should be different from 0 so that
  // the optical axis doesn't intersect the horizontal segment
  double Y_d = .18;   // Desired distance along Y between camera and segment.
  vpColVector qm(2); // Measured head position
  qm = 0;
  double qm_pan = 0; // Measured pan position (tilt is not handled in that example)

#ifdef USE_REAL_ROBOT
  // Initialize the biclops head

  vpRobotBiclops biclops("/usr/share/BiclopsDefault.cfg");
  biclops.setDenavitHartenbergModel(vpBiclops::DH1);

  // Move to the initial position
  vpColVector q(2);

  q=0;
  q[0] = vpMath::rad(63);
  q[1] = vpMath::rad(12); // introduce a tilt angle to compensate camera sphere tilt so that the camera is parallel to the plane

  biclops.setRobotState(vpRobot::STATE_POSITION_CONTROL) ;
  biclops.setPosition( vpRobot::ARTICULAR_FRAME, q );
  //biclops.setPositioningVelocity(50);
  biclops.getPosition(vpRobot::ARTICULAR_FRAME, qm);
  qm_pan = qm[0];

  // Now the head will be controlled in velocity
  biclops.setRobotState(vpRobot::STATE_VELOCITY_CONTROL) ;

  // Initialize the pioneer robot
  vpRobotPioneer pioneer;
  ArArgumentParser parser(&argc, argv);
  parser.loadDefaultArguments();

  // ArRobotConnector connects to the robot, get some initial data from it such as type and name,
  // and then loads parameter files for this robot.
  ArRobotConnector robotConnector(&parser, &pioneer);
  if(!robotConnector.connectRobot())
  {
    ArLog::log(ArLog::Terse, "Could not connect to the pioneer robot.");
    if(parser.checkHelpAndWarnUnparsed())
    {
      Aria::logOptions();
      Aria::exit(1);
    }
  }
  if (!Aria::parseArgs())
  {
    Aria::logOptions();
    Aria::shutdown();
    return false;
  }

  pioneer.useSonar(false); // disable the sonar device usage

  std::cout << "Pioneer robot connected" << std::endl;
#endif

  vpPioneerPan robot_pan; // Generic robot that computes the velocities for the pioneer and the biclops head

  // Create the camera framegrabber
#if defined(VISP_HAVE_V4L2)
  // Create a grabber based on v4l2 third party lib (for usb cameras under Linux)
  vpV4l2Grabber g;
  g.setScale(1);
  g.setInput(0);
  g.setDevice("/dev/video1");
  g.open(I);
#elif defined(VISP_HAVE_DC1394_2)
  // Create a grabber based on libdc1394-2.x third party lib (for firewire cameras under Linux)
  vp1394TwoGrabber g(false);
  g.setVideoMode(vp1394TwoGrabber::vpVIDEO_MODE_640x480_MONO8);
  g.setFramerate(vp1394TwoGrabber::vpFRAMERATE_30);
#elif defined(VISP_HAVE_CMU1394)
  // Create a grabber based on CMU 1394 third party lib (for firewire cameras under windows)
  vp1394CMUGrabber g;
  g.setVideoMode(0, 5); // 640x480 MONO8
  g.setFramerate(4);    // 30 Hz
  g.open(I);
#endif

  // Acquire an image from the grabber
  g.acquire(I);

  // Create an image viewer
#if defined(VISP_HAVE_X11)
  vpDisplayX d(I, 10, 10, "Current frame");
#elif defined(VISP_HAVE_GDI)
  vpDisplayGDI d(I, 10, 10, "Current frame");
#endif
  vpDisplay::display(I);
  vpDisplay::flush(I);

  // The 3D segment consists in two horizontal dots
  vpDot2 dot[2];
  for (int i=0; i <2; i++)
  {
    dot[i].setGraphics(true);
    dot[i].setComputeMoments(true);
    dot[i].setEllipsoidShapePrecision(0.);  // to track a blob without any constraint on the shape
    dot[i].setGrayLevelPrecision(0.9);  // to set the blob gray level bounds for binarisation
    dot[i].setEllipsoidBadPointsPercentage(0.5); // to be accept 50% of bad inner and outside points with bad gray level
    dot[i].initTracking(I);
    vpDisplay::flush(I);
  }

  // Camera parameters. In this experiment we don't need a precise calibration of the camera
  vpCameraParameters cam;
  cam.initPersProjWithoutDistortion(558, 555, 312, 210);

  vpServo task;
  task.setServo(vpServo::EYEINHAND_L_cVe_eJe) ;
  task.setInteractionMatrixType(vpServo::DESIRED, vpServo::PSEUDO_INVERSE) ;
  task.setLambda(lambda) ;
  vpVelocityTwistMatrix cVe ; // keep to identity
  cVe = robot_pan.get_cVe() ;
  task.set_cVe(cVe) ;

  std::cout << "cVe: \n" << cVe << std::endl;

  vpMatrix eJe;

  // Update the robot jacobian that depends on the pan position
  robot_pan.set_eJe(qm_pan);
  // Get the robot jacobian
  eJe = robot_pan.get_eJe() ;
  task.set_eJe(eJe) ;
  std::cout << "eJe: \n" << eJe << std::endl;

  // Define a 3D horizontal segment an its cordinates in the image plane
  vpPoint P[2];
  P[0].setWorldCoordinates(-L/2, 0, 0);
  P[1].setWorldCoordinates( L/2, 0, 0);
  // Define the desired camera position
  vpHomogeneousMatrix cMo(0, Y_d, Z_d, 0, 0, 0);  // Here we are in front of the segment
  for (int i=0; i <2; i++)
  {
    P[i].changeFrame(cMo);
    P[i].project(); // Here the x,y parameters obtained by perspective projection are computed
  }

  // Estimate the depth of the segment extremity points
  double surface[2];
  double Z[2]; // Depth of the segment points
  for (int i=0; i<2; i++)
  {
    // Surface of the blob estimated from the image moment m00 and converted in meters
    surface[i] = 1./sqrt(dot[i].m00/(cam.get_px()*cam.get_py()));

    // Initial depth of the blob
    Z[i] = coef * surface[i] ;
  }

  // Use here a feature segment builder
  bool normalized = true; // segment normilized features are used
  vpFeatureSegment s_segment(normalized), s_segment_d(normalized); // From the segment feature we use only alpha
  vpFeatureBuilder::create(s_segment, cam, dot[0], dot[1]);
  s_segment.setZ1(Z[0]);
  s_segment.setZ2(Z[1]);
  // Set the desired feature
  vpFeatureBuilder::create(s_segment_d, P[0], P[1]);
  s_segment.setZ1( P[0].get_Z() ); // Desired depth
  s_segment.setZ2( P[1].get_Z() );

  task.addFeature(s_segment, s_segment_d,
                  vpFeatureSegment::selectXc() |
                  vpFeatureSegment::selectL() |
                  vpFeatureSegment::selectAlpha());

#ifdef USE_PLOTTER
  //Create a window (500 by 500) at position (700, 10) with two graphics
  vpPlot graph(2, 500, 500, 700, 10, "Curves...");

  //The first graphic contains 3 curve and the second graphic contains 3 curves
  graph.initGraph(0,3);
  graph.initGraph(1,3);
  graph.setTitle(0, "Velocities");
  graph.setTitle(1, "Error s-s*");
  graph.setLegend(0, 0, "vx");
  graph.setLegend(0, 1, "wz");
  graph.setLegend(0, 2, "w_pan");
  graph.setLegend(1, 0, "xm/l");
  graph.setLegend(1, 1, "1/l");
  graph.setLegend(1, 2, "alpha");
#endif

  vpColVector v; // vz, wx
  unsigned int iter = 0;
  try
  {
    while(1)
    {
#ifdef USE_REAL_ROBOT
      // Get the new pan position
      biclops.getPosition(vpRobot::ARTICULAR_FRAME, qm);
#endif
      qm_pan = qm[0];

      // Acquire a new image
      g.acquire(I);
      // Set the image as background of the viewer
      vpDisplay::display(I);

      // Display the desired position of the segment
      for (int i=0; i<2; i++)
        P[i].display(I, cam, vpColor::red, 3);

      // Does the blob tracking
      for (int i=0; i<2; i++)
        dot[i].track(I);

      for (int i=0; i<2; i++)
      {
        // Surface of the blob estimated from the image moment m00 and converted in meters
        surface[i] = 1./sqrt(dot[i].m00/(cam.get_px()*cam.get_py()));

        // Initial depth of the blob
        Z[i] = coef * surface[i] ;
      }

      // Update the features
      vpFeatureBuilder::create(s_segment, cam, dot[0], dot[1]);
      // Update the depth of the point. Useful only if current interaction matrix is used
      // when task.setInteractionMatrixType(vpServo::CURRENT, vpServo::PSEUDO_INVERSE) is set
      s_segment.setZ1(Z[0]);
      s_segment.setZ2(Z[1]);

      robot_pan.get_cVe(cVe);
      task.set_cVe(cVe);

      // Update the robot jacobian that depends on the pan position
      robot_pan.set_eJe(qm_pan);
      // Get the robot jacobian
      eJe = robot_pan.get_eJe();
      // Update the jacobian that will be used to compute the control law
      task.set_eJe(eJe);

      // Compute the control law. Velocities are computed in the mobile robot reference frame
      v = task.computeControlLaw();

//      std::cout << "-----" << std::endl;
//      std::cout << "v: " << v.t() << std::endl;
//      std::cout << "error: " << task.getError().t() << std::endl;
//      std::cout << "L:\n " << task.getInteractionMatrix() << std::endl;
//      std::cout << "eJe:\n " << task.get_eJe() << std::endl;
//      std::cout << "cVe:\n " << task.get_cVe() << std::endl;
//      std::cout << "L_cVe_eJe:\n" << task.getInteractionMatrix() * task.get_cVe() * task.get_eJe() << std::endl;
//      task.print() ;
      if (task.getTaskRank() != 3)
        std::cout << "Warning: task is of rank " << task.getTaskRank() << std::endl;

#ifdef USE_PLOTTER
      graph.plot(0, iter, v); // plot velocities applied to the robot
      graph.plot(1, iter, task.getError()); // plot error vector
#endif

#ifdef USE_REAL_ROBOT
      // Send the velocity to the robot
      vpColVector v_pioneer(2); // vx, wz
      v_pioneer[0] = v[0];
      v_pioneer[1] = v[1];
      vpColVector v_biclops(2); // qdot pan and tilt
      v_biclops[0] = v[2];
      v_biclops[1] = 0;

      pioneer.setVelocity(vpRobot::REFERENCE_FRAME, v_pioneer);
      biclops.setVelocity(vpRobot::ARTICULAR_FRAME, v_biclops) ;
#endif

      // Draw a vertical line which corresponds to the desired x coordinate of the dot cog
      vpDisplay::displayLine(I, 0, cam.get_u0(), 479, cam.get_u0(), vpColor::red);
      vpDisplay::flush(I);

      // A click in the viewer to exit
      if ( vpDisplay::getClick(I, false) )
        break;

      iter ++;
      //break;
    }
  }
  catch(...)
  {
  }

#ifdef USE_REAL_ROBOT
  std::cout << "Ending robot thread..." << std::endl;
  pioneer.stopRunning();

  // wait for the thread to stop
  pioneer.waitForRunExit();
#endif

  // Kill the servo task
  task.print() ;
  task.kill();

#endif
#endif
}
#else
int main()
{
  std::cout << "ViSP is not able to control the Pioneer robot" << std::endl;
  return 0;
}
#endif
