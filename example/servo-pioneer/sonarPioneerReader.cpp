/****************************************************************************
 *
 * $Id: sonarPioneerReader.cpp 3820 2012-06-27 13:13:29Z fspindle $
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
 * Example that shows how to control a Pioneer mobile robot in ViSP.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/

#include <iostream>

#include <visp/vpRobotPioneer.h> // Include before vpDisplayX to avoid build issues
#include <visp/vpConfig.h>
#include <visp/vpDisplay.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpDisplayX.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpTime.h>

#ifndef VISP_HAVE_PIONEER
int main()
{
  std::cout << "\nThis example requires Aria 3rd party library. You should install it.\n"
            << std::endl;
  return 0;
}

#else

ArSonarDevice sonar;
vpRobotPioneer *robot;
#if defined(VISP_HAVE_X11)
vpDisplayX *d;
#elif defined (VISP_HAVE_GDI)
vpDisplayGDI *d;
#endif
vpImage<unsigned char> I;
static int isInitialized = false;
static int half_size = 256*2;

void sonarPrinter(void)
{
  fprintf(stdout, "in sonarPrinter()\n"); fflush(stdout);
  double scale = (double)half_size / (double)sonar.getMaxRange();

  /*
  ArSonarDevice *sd;

  std::list<ArPoseWithTime *>::iterator it;
  std::list<ArPoseWithTime *> *readings;
  ArPose *pose;

  sd = (ArSonarDevice *)robot->findRangeDevice("sonar");
  if (sd != NULL)
  {
    sd->lockDevice();
    readings = sd->getCurrentBuffer();
    if (readings != NULL)
    {
      for (it = readings->begin(); it != readings->end(); ++it)
      {
        pose = (*it);
        //pose->log();
      }
    }
    sd->unlockDevice();
  }
*/
  double range;
  double angle;

  /*
   * example to show how to find closest readings within polar sections
   */
  printf("Closest readings within polar sections:\n");

  double start_angle = -45;
  double end_angle = 45;
  range = sonar.currentReadingPolar(start_angle, end_angle, &angle);
  printf(" front quadrant: %5.0f  ", range);
  if (range != sonar.getMaxRange())
    printf("%3.0f ", angle);
  printf("\n");
#if defined(VISP_HAVE_X11) || defined (VISP_HAVE_GDI)
  if (isInitialized && range != sonar.getMaxRange())
  {
    double x = range * cos(vpMath::rad(angle)); // position of the obstacle in the sensor frame
    double y = range * sin(vpMath::rad(angle));

    // Conversion in pixels so that the robot frame is in the middle of the image
    double j = -y * scale + half_size; // obstacle position
    double i = -x * scale + half_size;

    vpDisplay::display(I);
    vpDisplay::displayLine(I, half_size, half_size, 0, 0, vpColor::red, 5);
    vpDisplay::displayLine(I, half_size, half_size, 0, 2*half_size-1, vpColor::red, 5);
    vpDisplay::displayLine(I, half_size, half_size, i, j, vpColor::green, 3);
    vpDisplay::displayCross(I, i, j, 7, vpColor::blue);
  }
#endif

  range = sonar.currentReadingPolar(-135, -45, &angle);
  printf(" right quadrant: %5.0f ", range);
  if (range != sonar.getMaxRange())
    printf("%3.0f ", angle);
  printf("\n");

  range = sonar.currentReadingPolar(45, 135, &angle);
  printf(" left quadrant: %5.0f ", range);
  if (range != sonar.getMaxRange())
    printf("%3.0f ", angle);
  printf("\n");

  range = sonar.currentReadingPolar(-135, 135, &angle);
  printf(" back quadrant: %5.0f ", range);
  if (range != sonar.getMaxRange())
    printf("%3.0f ", angle);
  printf("\n");

  /*
   * example to show how get all sonar sensor data
   */
  ArSensorReading *reading;
  for (int sensor = 0; sensor < robot->getNumSonar(); sensor++)
  {
    reading = robot->getSonarReading(sensor);
    if (reading != NULL)
    {
      angle = reading->getSensorTh();
      range = reading->getRange();
      double sx = reading->getSensorX(); // position of the sensor in the robot frame
      double sy = reading->getSensorY();
      double ox = range * cos(vpMath::rad(angle)); // position of the obstacle in the sensor frame
      double oy = range * sin(vpMath::rad(angle));
      double x = sx + ox; // position of the obstacle in the robot frame
      double y = sy + oy;

      // Conversion in pixels so that the robot frame is in the middle of the image
      double sj = -sy * scale + half_size; // sensor position
      double si = -sx * scale + half_size;
      double j = -y * scale + half_size; // obstacle position
      double i = -x * scale + half_size;

//      printf("%d x: %.1f y: %.1f th: %.1f d: %d\n", sensor, reading->getSensorX(),
//             reading->getSensorY(), reading->getSensorTh(), reading->getRange());

#if defined(VISP_HAVE_X11) || defined (VISP_HAVE_GDI)
      if (isInitialized && range != sonar.getMaxRange())
      {
        vpDisplay::displayLine(I, si, sj, i, j, vpColor::blue, 2);
        vpDisplay::displayCross(I, si, sj, 7, vpColor::blue);
        char legend[15];
        sprintf(legend, "%d: %1.2fm", sensor, float(range)/1000);
        vpDisplay::displayCharString(I, i-7, j+7, legend, vpColor::blue);
      }
#endif
    }

  }

#if defined(VISP_HAVE_X11) || defined (VISP_HAVE_GDI)
  if (isInitialized)
    vpDisplay::flush(I);
#endif

  fflush(stdout);
}

/*!
  \example sonarPioneerReader.cpp example showing how to connect and read
  sonar data from a Pioneer mobile robot->

*/
int main(int argc, char **argv)
{
  ArArgumentParser parser(&argc, argv);
  parser.loadDefaultArguments();

  robot = new vpRobotPioneer;

  // ArRobotConnector connects to the robot, get some initial data from it such as type and name,
  // and then loads parameter files for this robot.
  ArRobotConnector robotConnector(&parser, robot);
  if(!robotConnector.connectRobot())
  {
    ArLog::log(ArLog::Terse, "Could not connect to the robot");
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
  
  std::cout << "Robot connected" << std::endl;

#if defined(VISP_HAVE_X11) || defined (VISP_HAVE_GDI)
  // Create a display to show sensor data
  if (isInitialized == false)
  {
    I.resize(half_size*2, half_size*2);
    I = 255;

#if defined(VISP_HAVE_X11)
    d = new vpDisplayX;
#elif defined (VISP_HAVE_GDI)
    d = new vpDisplayGDI;
#endif
    d->init(I, -1, -1, "Sonar range data");
    isInitialized = true;
  }
#endif

  // Activates the sonar
  ArGlobalFunctor sonarPrinterCB(&sonarPrinter);
  robot->addRangeDevice(&sonar);
  robot->addUserTask("Sonar printer", 50, &sonarPrinterCB);

  robot->useSonar(true); // activates the sonar device usage

  // Robot velocities
  vpColVector v_mes(2);

  for (int i=0; i < 1000; i++)
  {
    double t = vpTime::measureTimeMs();

    v_mes = robot->getVelocity(vpRobot::REFERENCE_FRAME);
    std::cout << "Trans. vel= " << v_mes[0] << " m/s, Rot. vel=" << vpMath::deg(v_mes[1]) << " deg/s" << std::endl;
    v_mes = robot->getVelocity(vpRobot::ARTICULAR_FRAME);
    std::cout << "Left wheel vel= " << v_mes[0] << " m/s, Right wheel vel=" << v_mes[1] << " m/s" << std::endl;
    std::cout << "Battery=" << robot->getBatteryVoltage() << std::endl;

#if defined(VISP_HAVE_X11) || defined (VISP_HAVE_GDI)
  if (isInitialized) {
    // A mouse click to exit
    if (vpDisplay::getClick(I, false) == true) {
      {
        vpImage<vpRGBa> C;
        vpDisplay::getImage(I, C);
        vpImageIo::write(C, "/tmp/sonar.png");
      }
      break;
    }
  }
#endif

    vpTime::wait(t, 40);
  }

  ArLog::log(ArLog::Normal, "simpleMotionCommands: Stopping.");
  robot->lock();
  robot->stop();
  robot->unlock();
  ArUtil::sleep(1000);

  robot->lock();
  ArLog::log(ArLog::Normal, "simpleMotionCommands: Pose=(%.2f,%.2f,%.2f), Trans. Vel=%.2f, Rot. Vel=%.2f, Battery=%.2fV",
             robot->getX(), robot->getY(), robot->getTh(), robot->getVel(), robot->getRotVel(), robot->getBatteryVoltage());
  robot->unlock();

  std::cout << "Ending robot thread..." << std::endl;
  robot->stopRunning();

#if defined(VISP_HAVE_X11) || defined (VISP_HAVE_GDI)
  if (isInitialized) {
    if (d != NULL)
      delete d;
  }
#endif

  // wait for the thread to stop
  robot->waitForRunExit();

  delete robot;

  // exit
  ArLog::log(ArLog::Normal, "simpleMotionCommands: Exiting.");
  return 0;
}

#endif

