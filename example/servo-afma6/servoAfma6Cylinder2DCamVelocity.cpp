/****************************************************************************
 *
 * $Id: servoAfma6Cylinder2DCamVelocity.cpp 3668 2012-04-04 09:07:10Z fspindle $
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
 *   tests the control law
 *   eye-in-hand control
 *   velocity computed in the camera frame
 *
 * Authors:
 * Nicolas Melchior
 *
 *****************************************************************************/

/*!

  \file servoAfma6Cylinder2DCamVelocity.cpp

  \example servoAfma6Cylinder2DCamVelocity.cpp

  Example of eye-in-hand control law. We control here a real robot,
  the Afma6 robot (cartesian robot, with 6 degrees of freedom). The
  velocity is computed in the camera frame. Visual features are the
  two lines corresponding to the edges of a cylinder.

*/



#include <visp/vpConfig.h>
#include <visp/vpDebug.h> // Debug trace
#include <stdlib.h>
#include <cmath>    // std::fabs
#include <limits>   // numeric_limits
#if (defined (VISP_HAVE_AFMA6) && defined (VISP_HAVE_DC1394_2))

#include <visp/vp1394TwoGrabber.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplay.h>
#include <visp/vpDisplayX.h>

#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpFeatureLine.h>
#include <visp/vpMeLine.h>
#include <visp/vpCylinder.h>
#include <visp/vpServo.h>
#include <visp/vpFeatureBuilder.h>

#include <visp/vpRobotAfma6.h>

// Exception
#include <visp/vpException.h>
#include <visp/vpMatrixException.h>
#include <visp/vpServoDisplay.h>

int
main()
{
  try
  {
    vpImage<unsigned char> I ;

    vp1394TwoGrabber g;
    g.setVideoMode(vp1394TwoGrabber::vpVIDEO_MODE_640x480_MONO8);
    g.setFramerate(vp1394TwoGrabber::vpFRAMERATE_60);
    g.open(I) ;

    g.acquire(I) ;

    vpDisplayX display(I,100,100,"testDisplayX.cpp ") ;
    vpTRACE(" ") ;

    vpDisplay::display(I) ;
    vpDisplay::flush(I) ;

    vpServo task ;

    std::cout << std::endl ;
    std::cout << "-------------------------------------------------------" << std::endl ;
    std::cout << " Test program for vpServo "  <<std::endl ;
    std::cout << " Eye-in-hand task control, velocity computed in the camera frame" << std::endl ;
    std::cout << " Simulation " << std::endl ;
    std::cout << " task : servo a point " << std::endl ;
    std::cout << "-------------------------------------------------------" << std::endl ;
    std::cout << std::endl ;


    int i ;
    int nbline =2 ;
    vpMeLine line[nbline] ;

    vpMe me ;
    me.setRange(10) ;
    me.setPointsToTrack(100) ;
    me.setThreshold(30000) ;
    me.setSampleStep(10);

    //Initialize the tracking of the two edges of the cylinder
    for (i=0 ; i < nbline ; i++)
    {
      line[i].setDisplay(vpMeSite::RANGE_RESULT) ;
      line[i].setMe(&me) ;

      line[i].initTracking(I) ;
      line[i].track(I) ;
    }

    vpRobotAfma6 robot ;
    //robot.move("zero.pos") ;

    vpCameraParameters cam ;
    // Update camera parameters
    robot.getCameraParameters (cam, I);

    vpTRACE("sets the current position of the visual feature ") ;
    vpFeatureLine p[nbline] ;
    for (i=0 ; i < nbline ; i++)
      vpFeatureBuilder::create(p[i],cam, line[i])  ;

    vpTRACE("sets the desired position of the visual feature ") ;
    vpCylinder cyld(0,1,0,0,0,0,0.04);

    vpHomogeneousMatrix cMo(0,0,0.4,0,0,vpMath::rad(0));

    cyld.project(cMo);

    vpFeatureLine pd[nbline] ;
    vpFeatureBuilder::create(pd[0],cyld,vpCylinder::line1);
    vpFeatureBuilder::create(pd[1],cyld,vpCylinder::line2);

    //Those lines are needed to keep the conventions define in vpMeLine (Those in vpLine are less restrictive)
    //Another way to have the coordinates of the desired features is to learn them before executing the program.
    pd[0].setRhoTheta(-fabs(pd[0].getRho()),0);
    pd[1].setRhoTheta(-fabs(pd[1].getRho()),M_PI);

    vpTRACE("define the task") ;
    vpTRACE("\t we want an eye-in-hand control law") ;
    vpTRACE("\t robot is controlled in the camera frame") ;
    task.setServo(vpServo::EYEINHAND_CAMERA) ;
    task.setInteractionMatrixType(vpServo::DESIRED, vpServo::PSEUDO_INVERSE);

    vpTRACE("\t we want to see a two lines on two lines..") ;
    std::cout << std::endl ;
    for (i=0 ; i < nbline ; i++)
      task.addFeature(p[i],pd[i]) ;

    vpTRACE("\t set the gain") ;
    task.setLambda(0.2) ;


    vpTRACE("Display task information " ) ;
    task.print() ;


    robot.setRobotState(vpRobot::STATE_VELOCITY_CONTROL) ;

    unsigned int iter=0 ;
    vpTRACE("\t loop") ;
    vpColVector v ;
    vpImage<vpRGBa> Ic ;
    double lambda_av =0.05;
    double alpha = 0.2 ;
    double beta =3 ;
    for ( ; ; )
    {
      std::cout << "---------------------------------------------" << iter <<std::endl ;

      try {
        g.acquire(I) ;
        vpDisplay::display(I) ;

        //Track the two edges and update the features
        for (i=0 ; i < nbline ; i++)
	      {
		      line[i].track(I) ;
		      line[i].display(I, vpColor::red) ;

		      vpFeatureBuilder::create(p[i],cam,line[i]);
		      vpTRACE("%f %f ",line[i].getRho(), line[i].getTheta()) ;

		      p[i].display(cam, I,  vpColor::red) ;
		      pd[i].display(cam, I,  vpColor::green) ;
	      }

        vpDisplay::flush(I) ;

        //Adaptative gain
        double gain ;
        {
          if (std::fabs(alpha) <= std::numeric_limits<double>::epsilon())
            gain = lambda_av ;
          else
          {
            gain = alpha * exp (-beta * ( task.getError() ).sumSquare() ) +  lambda_av ;
          }
        }
        task.setLambda(gain) ;

        v = task.computeControlLaw() ;

        if (iter==0)  vpDisplay::getClick(I) ;
        robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;
      }
      catch(...)
	    {
	      v =0 ;
	      robot.setVelocity(vpRobot::CAMERA_FRAME, v) ;
	      robot.stopMotion() ;
	      exit(1) ;
	    }

      vpTRACE("\t\t || s - s* || = %f ", ( task.getError() ).sumSquare()) ;
      iter++;
    }

    vpTRACE("Display task information " ) ;
    task.print() ;
    task.kill();
  }
  catch (...)
  {
    vpERROR_TRACE(" Test failed") ;
    return 0;
  }
}

#else
int
main()
{
  vpERROR_TRACE("You do not have an afma6 robot or a firewire framegrabber connected to your computer...");
}

#endif
