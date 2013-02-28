/****************************************************************************
 *
 * $Id: vpSimulatorAfma6.cpp 2595 2010-06-02 08:50:59Z nmelchio $
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
 * Class which provides a simulator for the robot Afma6.
 *
 * Authors:
 * Nicolas Melchior
 *
 *****************************************************************************/



#include <visp/vpConfig.h>
#if defined(WIN32) || defined(VISP_HAVE_PTHREAD)
#include <visp/vpSimulatorAfma6.h>
#include <visp/vpTime.h>
#include <visp/vpImagePoint.h>
#include <visp/vpPoint.h>
#include <visp/vpMeterPixelConversion.h>
#include <visp/vpRobotException.h>
#include <visp/vpIoTools.h>
#include <cmath>    // std::fabs
#include <limits>   // numeric_limits
#include <string>
const double vpSimulatorAfma6::defaultPositioningVelocity = 25.0;


/*!
  Basic constructor
*/
vpSimulatorAfma6::vpSimulatorAfma6():vpRobotSimulator(), vpAfma6()
{
  init();
  initDisplay();
  
  tcur = vpTime::measureTimeMs();

  #if defined(WIN32)
  mutex_fMi = CreateMutex(NULL,FALSE,NULL);
  mutex_artVel = CreateMutex(NULL,FALSE,NULL);
  mutex_artCoord = CreateMutex(NULL,FALSE,NULL);
  mutex_velocity = CreateMutex(NULL,FALSE,NULL);
  mutex_display = CreateMutex(NULL,FALSE,NULL);


  DWORD   dwThreadIdArray;
  hThread = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
			launcher,               // thread function name
            this,                   // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray);     // returns the thread identifier 
  #elif defined (VISP_HAVE_PTHREAD)
  pthread_mutex_init(&mutex_fMi, NULL);
  pthread_mutex_init(&mutex_artVel, NULL);
  pthread_mutex_init(&mutex_artCoord, NULL);
  pthread_mutex_init(&mutex_velocity, NULL);
  pthread_mutex_init(&mutex_display, NULL);
  
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  
  pthread_create(&thread, NULL, launcher, (void *)this);
  #endif
  
  compute_fMi();
}

/*!
  Constructor used to enable or disable the external view of the robot.
*/
vpSimulatorAfma6::vpSimulatorAfma6(bool disp):vpRobotSimulator(disp)
{
  init();
  initDisplay();
    
  tcur = vpTime::measureTimeMs();
  
    #if defined(WIN32)
  mutex_fMi = CreateMutex(NULL,FALSE,NULL);
  mutex_artVel = CreateMutex(NULL,FALSE,NULL);
  mutex_artCoord = CreateMutex(NULL,FALSE,NULL);
  mutex_velocity = CreateMutex(NULL,FALSE,NULL);
  mutex_display = CreateMutex(NULL,FALSE,NULL);


  DWORD   dwThreadIdArray;
  hThread = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
			launcher,               // thread function name
            this,                   // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray);      // returns the thread identifier 
  #elif defined(VISP_HAVE_PTHREAD)
  pthread_mutex_init(&mutex_fMi, NULL);
  pthread_mutex_init(&mutex_artVel, NULL);
  pthread_mutex_init(&mutex_artCoord, NULL);
  pthread_mutex_init(&mutex_velocity, NULL);
  pthread_mutex_init(&mutex_display, NULL);
  
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  
  pthread_create(&thread, NULL, launcher, (void *)this);
  #endif
  
  compute_fMi();
}

/*!
  Basic destructor
*/
vpSimulatorAfma6::~vpSimulatorAfma6()
{
  robotStop = true;
  
  #if defined(WIN32)
  WaitForSingleObject(hThread,INFINITE);
  CloseHandle(hThread);
  CloseHandle(mutex_fMi);
  CloseHandle(mutex_artVel);
  CloseHandle(mutex_artCoord);
  CloseHandle(mutex_velocity);
  CloseHandle(mutex_display);
  #elif defined(VISP_HAVE_PTHREAD)
  pthread_attr_destroy(&attr);
  pthread_join(thread, NULL);
  pthread_mutex_destroy(&mutex_fMi);
  pthread_mutex_destroy(&mutex_artVel);
  pthread_mutex_destroy(&mutex_artCoord);
  pthread_mutex_destroy(&mutex_velocity);
  pthread_mutex_destroy(&mutex_display);
  #endif
  
  if (robotArms != NULL)
  {
    for(int i = 0; i < 6; i++)
      free_Bound_scene (&(robotArms[i]));
  }

  delete[] robotArms;
  delete[] fMi;
}

/*!
  Method which initialises the parameters linked to the robot caracteristics.

  Set the path to the arm files (*.bnd and *.sln) used by the
  simulator.  If the path set in vpConfig.h in VISP_ROBOT_ARMS_DIR macro is
  not valid, the path is set from the VISP_ROBOT_ARMS_DIR environment
  variable that the user has to set.
*/
void
vpSimulatorAfma6::init()
{
  // set arm_dir from #define VISP_ROBOT_ARMS_DIR if it exists
  if (vpIoTools::checkDirectory(VISP_ROBOT_ARMS_DIR) == true) // directory exists
    arm_dir = VISP_ROBOT_ARMS_DIR;
  else {
    try {
      arm_dir = vpIoTools::getenv("VISP_ROBOT_ARMS_DIR");
      std::cout << "The simulator uses data from VISP_ROBOT_ARMS_DIR=" << arm_dir << std::endl;
    }
    catch (...) {
      std::cout << "Cannot get VISP_ROBOT_ARMS_DIR environment variable" << std::endl;
    }
  }

  this->init(vpAfma6::TOOL_CCMOP);
  toolCustom = false;

  size_fMi = 8;
  fMi = new vpHomogeneousMatrix[8];
  artCoord.resize(njoint);
  artVel.resize(njoint);

  zeroPos.resize(njoint);
  zeroPos = 0;
  reposPos.resize(njoint);
  reposPos = 0;
  reposPos[1] = -M_PI/2; reposPos[2] = M_PI; reposPos[4] = M_PI/2;
  
  artCoord = zeroPos;
  artVel = 0;
  
  q_prev_getdis.resize(njoint);
  q_prev_getdis = 0;
  first_time_getdis = true;
  
  positioningVelocity  = defaultPositioningVelocity ;
  
  setRobotFrame (vpRobot::ARTICULAR_FRAME);
  this->setRobotState(vpRobot::STATE_STOP);
  
  // Software joint limits in radians
  //_joint_min.resize(njoint);
  _joint_min[0] = -0.6501;
  _joint_min[1] = -0.6001;
  _joint_min[2] = -0.5001;
  _joint_min[3] = -2.7301;
  _joint_min[4] = -0.1001;
  _joint_min[5] = -1.5901;
  //_joint_max.resize(njoint);
  _joint_max[0] = 0.7001;
  _joint_max[1] = 0.5201;
  _joint_max[2] = 0.4601;
  _joint_max[3] = 2.7301;
  _joint_max[4] = 2.4801;
  _joint_max[5] = 1.5901;
}

/*!
  Method which initialises the parameters linked to the display part.
*/
void
vpSimulatorAfma6::initDisplay()
{
  robotArms = NULL;
  robotArms = new Bound_scene[6];
  initArms();
  setExternalCameraPosition(vpHomogeneousMatrix(0,0,0,0,0,vpMath::rad(180))*vpHomogeneousMatrix(-0.1,0,4,vpMath::rad(90),0,0));
  cameraParam.initPersProjWithoutDistortion (558.5309599, 556.055053, 320, 240);
  setExternalCameraParameters(cameraParam);
  vpCameraParameters tmp;
  getCameraParameters(tmp,640,480);
  px_int = tmp.get_px();
  py_int = tmp.get_py();
  sceneInitialized = true;
}


/*!

  Initialize the robot kinematics with the extrinsic calibration
  parameters associated to a specific camera.

  The eMc parameters depend on the camera.
  
  \warning Only perspective projection without distorsion is available!

  \param tool : Tool to use. Note that the generic camera is not handled.

  \param projModel : Projection model associated to the camera.

  \sa vpCameraParameters, init()
*/
void
vpSimulatorAfma6::init (vpAfma6::vpAfma6ToolType tool,
		       vpCameraParameters::vpCameraParametersProjType projModel)
{
  this->projModel = projModel;
  
  // Use here default values of the robot constant parameters.
  switch (tool) {
  case vpAfma6::TOOL_CCMOP: {
      _erc[0] = vpMath::rad(164.35); // rx
      _erc[1] = vpMath::rad( 89.64); // ry
      _erc[2] = vpMath::rad(-73.05); // rz
      _etc[0] = 0.0117; // tx
      _etc[1] = 0.0033; // ty
      _etc[2] = 0.2272; // tz
      
      setCameraParameters(vpCameraParameters(1109.5735473989, 1112.1520168160, 320, 240));
      
      if (robotArms != NULL)
      {
        while (get_displayBusy()) vpTime::wait(2);
        free_Bound_scene (&(robotArms[5]));
        char name_arm[FILENAME_MAX];
        strcpy(name_arm, arm_dir.c_str());
        strcat(name_arm,"/afma6_tool_ccmop.bnd");
        set_scene(name_arm, robotArms+5, 1.0);
        set_displayBusy(false);
      }
      break;
    }
  case vpAfma6::TOOL_GRIPPER: {
      _erc[0] = vpMath::rad( 88.33); // rx
      _erc[1] = vpMath::rad( 72.07); // ry
      _erc[2] = vpMath::rad(  2.53); // rz
      _etc[0] = 0.0783; // tx
      _etc[1] = 0.1234; // ty
      _etc[2] = 0.1638; // tz
      
      setCameraParameters(vpCameraParameters(852.6583228197, 854.8084224761, 320, 240));
      
      if (robotArms != NULL)
      {
        while (get_displayBusy()) vpTime::wait(2);
        free_Bound_scene (&(robotArms[5]));
        char name_arm[FILENAME_MAX];
        strcpy(name_arm, arm_dir.c_str());
        strcat(name_arm,"/afma6_tool_gripper.bnd");
        set_scene(name_arm, robotArms+5, 1.0);
        set_displayBusy(false);
      }
      break;
    }
  case vpAfma6::TOOL_VACUUM: {
      _erc[0] = vpMath::rad( 90.40); // rx
      _erc[1] = vpMath::rad( 75.11); // ry
      _erc[2] = vpMath::rad(  0.18); // rz
      _etc[0] = 0.0038; // tx
      _etc[1] = 0.1281; // ty
      _etc[2] = 0.1658; // tz
      
      setCameraParameters(vpCameraParameters(853.4876600807, 856.0339170706, 320, 240));
      
      if (robotArms != NULL)
      {
        while (get_displayBusy()) vpTime::wait(2);
        free_Bound_scene (&(robotArms[5]));
        char name_arm[FILENAME_MAX];
        strcpy(name_arm, arm_dir.c_str());
        strcat(name_arm,"/afma6_tool_vacuum.bnd");
        set_scene(name_arm, robotArms+5, 1.0);
        set_displayBusy(false);
      }
      break;
    }
  case vpAfma6::TOOL_GENERIC_CAMERA: {
      std::cout << "The generic camera is not handled in vpSimulatorAfma6.cpp" << std::endl;
    }
  }
  
  vpRotationMatrix eRc(_erc);
  this->_eMc.buildFrom(_etc, eRc);
  
  setToolType(tool);
  return ;
}

/*!
  Get the current intrinsic camera parameters obtained by calibration.

  \param cam : In output, camera parameters to fill.
  \param image_width : Image width used to compute camera calibration.
  \param image_height : Image height used to compute camera calibration.
  
  \warning The image size must be : 640x480 !
*/

void
vpSimulatorAfma6::getCameraParameters (vpCameraParameters &cam,
				 const unsigned int &image_width,
				 const unsigned int &image_height)
{
  if (toolCustom)
  {
    vpCameraParameters(px_int,py_int,image_width/2,image_height/2);
  }
  // Set default parameters
  switch (getToolType()) {
  case vpAfma6::TOOL_CCMOP: {
    // Set default intrinsic camera parameters for 640x480 images
    if (image_width == 640 && image_height == 480) 
    {
      std::cout << "Get default camera parameters for camera \"" 
		<< vpAfma6::CONST_CCMOP_CAMERA_NAME << "\"" << std::endl;
	cam.initPersProjWithoutDistortion(1109.5735473989, 1112.1520168160, 320, 240);
    }
    else {
      vpTRACE("Cannot get default intrinsic camera parameters for this image resolution");
    }
    break;
  }
  case vpAfma6::TOOL_GRIPPER: {
    // Set default intrinsic camera parameters for 640x480 images
    if (image_width == 640 && image_height == 480) {
      std::cout << "Get default camera parameters for camera \"" 
		<< vpAfma6::CONST_GRIPPER_CAMERA_NAME << "\"" << std::endl;
	cam.initPersProjWithoutDistortion(852.6583228197, 854.8084224761, 320, 240);
    }
    else {
      vpTRACE("Cannot get default intrinsic camera parameters for this image resolution");
    }
    break;
  }
  case vpAfma6::TOOL_VACUUM: {
      std::cout << "The generic camera is not handled in vpSimulatorAfma6.cpp" << std::endl;
      break;
  }
  default: 
    vpERROR_TRACE ("This error should not occur!");
    break;
  }
  return;
}

/*!
  Get the current intrinsic camera parameters obtained by calibration.

  \param cam : In output, camera parameters to fill.
  \param I : A B&W image send by the current camera in use.

  \warning The image size must be : 640x480 !
*/
void
vpSimulatorAfma6::getCameraParameters (vpCameraParameters &cam,
			      const vpImage<unsigned char> &I)
{
  getCameraParameters(cam,I.getWidth(),I.getHeight());
}

/*!
  Get the current intrinsic camera parameters obtained by calibration.

  \param cam : In output, camera parameters to fill.
  \param I : A B&W image send by the current camera in use.

  \warning The image size must be : 640x480 !
*/
void
vpSimulatorAfma6::getCameraParameters (vpCameraParameters &cam,
				 const vpImage<vpRGBa> &I)
{
  getCameraParameters(cam,I.getWidth(),I.getHeight());
}


/*!
  Set the intrinsic camera parameters.
      
  \param cam : The desired camera parameters.
*/
void
vpSimulatorAfma6::setCameraParameters(const vpCameraParameters cam) 
{
  px_int = cam.get_px();
  py_int = cam.get_py();
  toolCustom = true;
}


/*!
  Method lauched by the thread to compute the position of the robot in the articular frame.
*/
void
vpSimulatorAfma6::updateArticularPosition()
{
  double tcur_1 = tcur;// temporary variable used to store the last time since the last command 

  while (!robotStop)
  {
    //Get current time
    tprev = tcur_1;
    tcur = vpTime::measureTimeMs();
    
    if(setVelocityCalled || !constantSamplingTimeMode){
      setVelocityCalled = false;
    
      computeArticularVelocity();
    
      double ellapsedTime = tcur - tprev;
      if(constantSamplingTimeMode){//if we want a constant velocity, we force the ellapsed time to the given samplingTime
        ellapsedTime = samplingTime;
      }
    
      vpColVector articularCoordinates = get_artCoord();
      articularCoordinates = get_artCoord();
      vpColVector articularVelocities = get_artVel();
    
      if (jointLimit)
      {
        double art = articularCoordinates[jointLimitArt-1] + ellapsedTime*articularVelocities[jointLimitArt-1]*1e-3;
        if (art <= _joint_min[jointLimitArt-1] || art >= _joint_max[jointLimitArt-1])
          articularVelocities = 0.0;
        else
          jointLimit = false;
      }
    
      articularCoordinates[0] = articularCoordinates[0] + ellapsedTime*articularVelocities[0]*1e-3;
      articularCoordinates[1] = articularCoordinates[1] + ellapsedTime*articularVelocities[1]*1e-3;
      articularCoordinates[2] = articularCoordinates[2] + ellapsedTime*articularVelocities[2]*1e-3;
      articularCoordinates[3] = articularCoordinates[3] + ellapsedTime*articularVelocities[3]*1e-3;
      articularCoordinates[4] = articularCoordinates[4] + ellapsedTime*articularVelocities[4]*1e-3;
      articularCoordinates[5] = articularCoordinates[5] + ellapsedTime*articularVelocities[5]*1e-3;
      
      int jl = isInJointLimit();
      
      if (jl != 0 && jointLimit == false)
      {
        if (jl < 0)
          ellapsedTime = (_joint_min[(unsigned int)(-jl-1)] - articularCoordinates[(unsigned int)(-jl-1)])/(articularVelocities[(unsigned int)(-jl-1)]*1e-3);
        else
          ellapsedTime = (_joint_max[(unsigned int)(jl-1)] - articularCoordinates[(unsigned int)(jl-1)])/(articularVelocities[(unsigned int)(jl-1)]*1e-3);
      
        for (unsigned int i = 0; i < 6; i++)
          articularCoordinates[i] = articularCoordinates[i] + ellapsedTime*articularVelocities[i]*1e-3;
      
        jointLimit = true;
        jointLimitArt = (unsigned int)fabs((double)jl);
      }

      set_artCoord(articularCoordinates);
      set_artVel(articularVelocities);
    
      compute_fMi();
   
      if (displayAllowed)
      {
        vpDisplay::display(I);
        vpDisplay::displayFrame(I,getExternalCameraPosition (),cameraParam,0.2,vpColor::none);
        vpDisplay::displayFrame(I,getExternalCameraPosition ()*fMi[7],cameraParam,0.1,vpColor::none);
      }
    
      if (displayType == MODEL_3D && displayAllowed)
      {
        while (get_displayBusy()) vpTime::wait(2);
        vpSimulatorAfma6::getExternalImage(I);
        set_displayBusy(false);
      }
        
    
      if (0/*displayType == MODEL_DH && displayAllowed*/)
      {
        vpHomogeneousMatrix fMit[8];
        get_fMi(fMit);
      
      //vpDisplay::displayFrame(I,getExternalCameraPosition ()*fMi[6],cameraParam,0.2,vpColor::none);

        vpImagePoint iP, iP_1;
        vpPoint pt;
        pt.setWorldCoordinates (0,0,0);
      
        pt.track(getExternalCameraPosition ());
        vpMeterPixelConversion::convertPoint (cameraParam, pt.get_x(), pt.get_y(), iP_1);
        pt.track(getExternalCameraPosition ()*fMit[0]);
        vpMeterPixelConversion::convertPoint (cameraParam, pt.get_x(), pt.get_y(), iP);
        vpDisplay::displayLine(I,iP_1,iP,vpColor::green);
        for (unsigned int k = 1; k < 7; k++)
        {
          pt.track(getExternalCameraPosition ()*fMit[k-1]);
          vpMeterPixelConversion::convertPoint (cameraParam, pt.get_x(), pt.get_y(), iP_1);
        
          pt.track(getExternalCameraPosition ()*fMit[k]);
          vpMeterPixelConversion::convertPoint (cameraParam, pt.get_x(), pt.get_y(), iP);
        
          vpDisplay::displayLine(I,iP_1,iP,vpColor::green);
        }
        vpDisplay::displayCamera(I,getExternalCameraPosition ()*fMit[7],cameraParam,0.1,vpColor::green);
      }
    
      vpDisplay::flush(I);
    
      
      vpTime::wait(tcur,samplingTime);
      tcur_1 = tcur;
    }else{
      vpTime::wait(tcur, 4);
    }
  }
}

/*!
  Compute the pose between the robot reference frame and the frames used used to compute the Denavit-Hartenberg representation. The last element of the table corresponds to the pose between the reference frame and the camera frame.
  
  To compute the diferent homogeneous matrices, this function needs the articular coordinates as input.
  
  Finally the output is a table of 8 elements : \f$ fM1 \f$,\f$ fM2 \f$,\f$ fM3 \f$,\f$ fM4 \f$,\f$ fM5 \f$,\f$ fM6 = fMw \f$,\f$ fM7 = fMe \f$ and \f$ fMc \f$ - where w is for wrist and e for effector-.
*/
void 
vpSimulatorAfma6::compute_fMi()
{
  //vpColVector q = get_artCoord();
  vpColVector q(6);//; = get_artCoord();
  q = get_artCoord();
  
  vpHomogeneousMatrix fMit[8];
  
  double q1 = q[0];
  double q2 = q[1];
  double q3 = q[2];
  double q4 = q[3];
  double q5 = q[4];
  double q6 = q[5];
  
  double c4 = cos(q4);
  double s4 = sin(q4);
  double c5 = cos(q5);
  double s5 = sin(q5);
  double c6 = cos(q6);
  double s6 = sin(q6);
  
  fMit[0][0][0] = 1;
  fMit[0][1][0] = 0;
  fMit[0][2][0] = 0;
  fMit[0][0][1] = 0;
  fMit[0][1][1] = 1;
  fMit[0][2][1] = 0;
  fMit[0][0][2] = 0;
  fMit[0][1][2] = 0;
  fMit[0][2][2] = 1;
  fMit[0][0][3] = q1;
  fMit[0][1][3] = 0;
  fMit[0][2][3] = 0;
  
  fMit[1][0][0] = 1;
  fMit[1][1][0] = 0;
  fMit[1][2][0] = 0;
  fMit[1][0][1] = 0;
  fMit[1][1][1] = 1;
  fMit[1][2][1] = 0;
  fMit[1][0][2] = 0;
  fMit[1][1][2] = 0;
  fMit[1][2][2] = 1;
  fMit[1][0][3] = q1;
  fMit[1][1][3] = q2;
  fMit[1][2][3] = 0;
  
  fMit[2][0][0] = 1;
  fMit[2][1][0] = 0;
  fMit[2][2][0] = 0;
  fMit[2][0][1] = 0;
  fMit[2][1][1] = 1;
  fMit[2][2][1] = 0;
  fMit[2][0][2] = 0;
  fMit[2][1][2] = 0;
  fMit[2][2][2] = 1;
  fMit[2][0][3] = q1;
  fMit[2][1][3] = q2;
  fMit[2][2][3] = q3;
  
  fMit[3][0][0] = s4;
  fMit[3][1][0] = -c4;
  fMit[3][2][0] = 0;
  fMit[3][0][1] = c4;
  fMit[3][1][1] = s4;
  fMit[3][2][1] = 0;
  fMit[3][0][2] = 0;
  fMit[3][1][2] = 0;
  fMit[3][2][2] = 1;
  fMit[3][0][3] = q1;
  fMit[3][1][3] = q2;
  fMit[3][2][3] = q3;
  
  fMit[4][0][0] = s4*s5;
  fMit[4][1][0] = -c4*s5;
  fMit[4][2][0] = c5;
  fMit[4][0][1] = s4*c5;
  fMit[4][1][1] = -c4*c5;
  fMit[4][2][1] = -s5;
  fMit[4][0][2] = c4;
  fMit[4][1][2] = s4;
  fMit[4][2][2] = 0;
  fMit[4][0][3] = c4*this->_long_56+q1;
  fMit[4][1][3] = s4*this->_long_56+q2;
  fMit[4][2][3] = q3;
  
  fMit[5][0][0] = s4*s5*c6+c4*s6;
  fMit[5][1][0] = -c4*s5*c6+s4*s6;
  fMit[5][2][0] = c5*c6;
  fMit[5][0][1] = -s4*s5*s6+c4*c6;
  fMit[5][1][1] = c4*s5*s6+s4*c6;
  fMit[5][2][1] = -c5*s6;
  fMit[5][0][2] = -s4*c5;
  fMit[5][1][2] = c4*c5;
  fMit[5][2][2] = s5;
  fMit[5][0][3] = c4*this->_long_56+q1;
  fMit[5][1][3] = s4*this->_long_56+q2;
  fMit[5][2][3] = q3;
  
  fMit[6][0][0] = fMit[5][0][0];
  fMit[6][1][0] = fMit[5][1][0];
  fMit[6][2][0] = fMit[5][2][0];
  fMit[6][0][1] = fMit[5][0][1];
  fMit[6][1][1] = fMit[5][1][1];
  fMit[6][2][1] = fMit[5][2][1];
  fMit[6][0][2] = fMit[5][0][2];
  fMit[6][1][2] = fMit[5][1][2];
  fMit[6][2][2] = fMit[5][2][2];
  fMit[6][0][3] = fMit[5][0][3];
  fMit[6][1][3] = fMit[5][1][3];
  fMit[6][2][3] = fMit[5][2][3];
  
//   vpHomogeneousMatrix cMe;
//   get_cMe(cMe);
//   cMe = cMe.inverse();
//   fMit[7] = fMit[6] * cMe;
  get_fMc(q,fMit[7]);
  
  #if defined(WIN32)
  WaitForSingleObject(mutex_fMi,INFINITE);
  for (int i = 0; i < 8; i++)
    fMi[i] = fMit[i];
  ReleaseMutex(mutex_fMi);
  #elif defined(VISP_HAVE_PTHREAD)
  pthread_mutex_lock (&mutex_fMi);
  for (int i = 0; i < 8; i++)
    fMi[i] = fMit[i];
  pthread_mutex_unlock (&mutex_fMi);
  #endif
}


/*!
Change the robot state.

\param newState : New requested robot state.
*/
vpRobot::vpRobotStateType
vpSimulatorAfma6::setRobotState(vpRobot::vpRobotStateType newState)
{
  switch (newState) {
  case vpRobot::STATE_STOP: {
    // Start primitive STOP only if the current state is Velocity
    if (vpRobot::STATE_VELOCITY_CONTROL == getRobotState ()) {
      stopMotion();
    }
    break;
  }
  case vpRobot::STATE_POSITION_CONTROL: {
    if (vpRobot::STATE_VELOCITY_CONTROL == getRobotState ()) {
      std::cout << "Change the control mode from velocity to position control.\n";
      stopMotion();
    }
    else {
      //std::cout << "Change the control mode from stop to position control.\n";
    }
    break;
  }
  case vpRobot::STATE_VELOCITY_CONTROL: {
    if (vpRobot::STATE_VELOCITY_CONTROL != getRobotState ()) {
      std::cout << "Change the control mode from stop to velocity control.\n";
    }
    break;
  }
  case vpRobot::STATE_ACCELERATION_CONTROL:
  default:
    break ;
  }

  return vpRobot::setRobotState (newState);
}

/*!
  Apply a velocity to the robot.

  \param frame : Control frame in which the velocity is expressed. Velocities
  could be expressed in articular, camera frame, reference frame or mixt frame.

  \param vel : Velocity vector. Translation velocities are expressed
  in m/s while rotation velocities in rad/s. The size of this vector
  is always 6.

  - In articular, \f$ vel = [\dot{q}_1, \dot{q}_2, \dot{q}_3, \dot{q}_4,
  \dot{q}_5, \dot{q}_6]^t \f$ correspond to joint velocities.

  - In camera frame, \f$ vel = [^{c} v_x, ^{c} v_y, ^{c} v_z, ^{c}
  \omega_x, ^{c} \omega_y, ^{c} \omega_z]^t \f$ is expressed in the
  camera frame.

  - In reference frame, \f$ vel = [^{r} v_x, ^{r} v_y, ^{r} v_z, ^{r}
  \omega_x, ^{r} \omega_y, ^{r} \omega_z]^t \f$ is expressed in the
  reference frame.

  - In mixt frame, \f$ vel = [^{r} v_x, ^{r} v_y, ^{r} v_z, ^{c} \omega_x,
  ^{c} \omega_y, ^{c} \omega_z]^t \f$.  In mixt frame, translations \f$ v_x,
  v_y, v_z \f$ are expressed in the reference frame and rotations \f$
  \omega_x, \omega_y, \omega_z \f$ in the camera frame.
  
  \exception vpRobotException::wrongStateError : If a the robot is not
  configured to handle a velocity. The robot can handle a velocity only if the
  velocity control mode is set. For that, call setRobotState(
  vpRobot::STATE_VELOCITY_CONTROL) before setVelocity().

  \warning Velocities could be saturated if one of them exceed the
  maximal autorized speed (see vpRobot::maxTranslationVelocity and
  vpRobot::maxRotationVelocity). To change these values use
  setMaxTranslationVelocity() and setMaxRotationVelocity().

  \code
#include <visp/vpConfig.h>
#include <visp/vpSimulatorAfma6.h>
#include <visp/vpColVector.h>
#include <visp/vpMath.h>

int main()
{
  vpSimulatorAfma6 robot;

  vpColVector qvel(6);
  // Set a joint velocity
  qvel[0] = 0.1;             // Joint 1 velocity in m/s
  qvel[1] = 0.1;             // Joint 2 velocity in m/s
  qvel[2] = 0.1;             // Joint 3 velocity in m/s
  qvel[3] = M_PI/8;          // Joint 4 velocity in rad/s
  qvel[4] = 0;               // Joint 5 velocity in rad/s
  qvel[5] = 0;               // Joint 6 velocity in rad/s
  
  // Initialize the controller to position control
  robot.setRobotState(vpRobot::STATE_VELOCITY_CONTROL);

  for ( ; ; ) {
    // Apply a velocity in the joint space
    robot.setVelocity(vpRobot::ARTICULAR_FRAME, qvel);

    // Compute new velocities qvel...
  }
  // Stop the robot
  robot.setRobotState(vpRobot::STATE_STOP);
}
  \endcode
*/
void
vpSimulatorAfma6::setVelocity (const vpRobot::vpControlFrameType frame, const vpColVector & vel)
{
  if (vpRobot::STATE_VELOCITY_CONTROL != getRobotState ()) {
    vpERROR_TRACE ("Cannot send a velocity to the robot "
		   "use setRobotState(vpRobot::STATE_VELOCITY_CONTROL) first) ");
    throw vpRobotException (vpRobotException::wrongStateError,
			    "Cannot send a velocity to the robot "
			    "use setRobotState(vpRobot::STATE_VELOCITY_CONTROL) first) ");
  }
  
  vpColVector vel_sat(6);

  double scale_trans_sat = 1;
  double scale_rot_sat   = 1;
  double scale_sat       = 1;
  double vel_trans_max = getMaxTranslationVelocity();
  double vel_rot_max   = getMaxRotationVelocity();
  
  double vel_abs; // Absolute value
  
  // Velocity saturation
  switch(frame)
  {
    // saturation in cartesian space
    case vpRobot::CAMERA_FRAME :
    case vpRobot::REFERENCE_FRAME :
    {
      if (vel.getRows() != 6)
      {
	vpERROR_TRACE ("The velocity vector must have a size of 6 !!!!");
	throw;
      }
      
      for (unsigned int i = 0 ; i < 3; ++ i)
      {
        vel_abs = fabs (vel[i]);
        if (vel_abs > vel_trans_max && !jointLimit)
        {
	  vel_trans_max = vel_abs;
	  vpERROR_TRACE ("Excess velocity %g m/s in TRANSLATION "
		         "(axis nr. %d).", vel[i], i+1);
        }
      
        vel_abs = fabs (vel[i+3]);
        if (vel_abs > vel_rot_max && !jointLimit) {
	  vel_rot_max = vel_abs;
	  vpERROR_TRACE ("Excess velocity %g rad/s in ROTATION "
		       "(axis nr. %d).", vel[i+3], i+4);
        }
      }
    
      if (vel_trans_max > getMaxTranslationVelocity())                     
        scale_trans_sat = getMaxTranslationVelocity() / vel_trans_max;
    
      if (vel_rot_max > getMaxRotationVelocity())
        scale_rot_sat = getMaxRotationVelocity() / vel_rot_max; 
    
      if ( (scale_trans_sat < 1) || (scale_rot_sat < 1) )
      {
        if (scale_trans_sat < scale_rot_sat)  
	  scale_sat = scale_trans_sat;                    
        else                        
	  scale_sat = scale_rot_sat;
      }
      break;
    }
    
    // saturation in joint space
    case vpRobot::ARTICULAR_FRAME :
    {
      if (vel.getRows() != 6)
      {
	vpERROR_TRACE ("The velocity vector must have a size of 6 !!!!");
	throw;
      }
      for (unsigned int i = 0 ; i < 6; ++ i)
      {
        vel_abs = fabs (vel[i]);
        if (vel_abs > vel_rot_max && !jointLimit)
        {
	  vel_rot_max = vel_abs;
	  vpERROR_TRACE ("Excess velocity %g rad/s in ROTATION "
		       "(axis nr. %d).", vel[i], i+1);
        }
      }
      if (vel_rot_max > getMaxRotationVelocity())
        scale_rot_sat = getMaxRotationVelocity() / vel_rot_max; 
      if ( scale_rot_sat < 1 ) 
        scale_sat = scale_rot_sat;
      break;
    }
    case vpRobot::MIXT_FRAME :
    {
      break;
    }
  }
  
  set_velocity (vel * scale_sat);
  setRobotFrame (frame);
  setVelocityCalled = true;
}


/*!
  Compute the articular velocity relative to the velocity in another frame.
*/
void
vpSimulatorAfma6::computeArticularVelocity()
{
  vpRobot::vpControlFrameType frame = getRobotFrame();
  
  double scale_rot_sat   = 1;
  double scale_sat       = 1;
  double vel_rot_max   = getMaxRotationVelocity();
  double vel_abs;
  
  vpColVector articularCoordinates = get_artCoord();
  vpColVector velocityframe = get_velocity();
  vpColVector articularVelocity;
  
  switch(frame)
  {
    case vpRobot::CAMERA_FRAME :
    {
      vpMatrix eJe;
      vpVelocityTwistMatrix eVc(_eMc);
      vpAfma6::get_eJe(articularCoordinates,eJe);
      eJe = eJe.pseudoInverse();
      if (singularityManagement)
        singularityTest(articularCoordinates,eJe);
      articularVelocity = eJe*eVc*velocityframe;
      set_artVel (articularVelocity);
      break;
    }
    case vpRobot::REFERENCE_FRAME :
    {
      vpMatrix fJe;
      vpAfma6::get_fJe(articularCoordinates,fJe);
      fJe = fJe.pseudoInverse();
      if (singularityManagement)
        singularityTest(articularCoordinates,fJe);
      articularVelocity = fJe*velocityframe;
      set_artVel (articularVelocity);
      break;
    }
    case vpRobot::ARTICULAR_FRAME :
    {
      articularVelocity = velocityframe;
      set_artVel (articularVelocity);
      break;
    }
    case vpRobot::MIXT_FRAME :
    {
      break;
    }
  }
  
  
  
  switch(frame)
  {
    case vpRobot::CAMERA_FRAME :
    case vpRobot::REFERENCE_FRAME :
    {
      for (unsigned int i = 0 ; i < 6; ++ i)
      {
        vel_abs = fabs (articularVelocity[i]);
        if (vel_abs > vel_rot_max && !jointLimit)
        {
          vel_rot_max = vel_abs;
          vpERROR_TRACE ("Excess velocity %g rad/s in ROTATION "
             "(axis nr. %d).", articularVelocity[i], i+1);
        }
      }
      if (vel_rot_max > getMaxRotationVelocity())
        scale_rot_sat = getMaxRotationVelocity() / vel_rot_max; 
      if ( scale_rot_sat < 1 ) 
        scale_sat = scale_rot_sat;
    
      set_artVel(articularVelocity * scale_sat);
      break;
    }
    case vpRobot::ARTICULAR_FRAME :
    case vpRobot::MIXT_FRAME :
    {
      break;
    }
  }
}


/*!
  Get the robot velocities.

  \param frame : Frame in wich velocities are mesured.

  \param vel : Measured velocities. Translations are expressed in m/s
  and rotations in rad/s.

  \warning In camera frame, reference frame and mixt frame, the representation
  of the rotation is ThetaU. In that cases, \f$velocity = [\dot x, \dot y, \dot
  z, \dot {\theta U}_x, \dot {\theta U}_y, \dot {\theta U}_z]\f$.

  \code
#include <visp/vpConfig.h>
#include <visp/vpSimulatorAfma6.h>
#include <visp/vpColVector.h>

int main()
{
  // Set requested joint velocities
  vpColVector q_dot(6);
  q_dot[0] = 0.1;    // Joint 1 velocity in m/s
  q_dot[1] = 0.2;    // Joint 2 velocity in m/s
  q_dot[2] = 0.3;    // Joint 3 velocity in m/s
  q_dot[3] = M_PI/8; // Joint 4 velocity in rad/s
  q_dot[4] = M_PI/4; // Joint 5 velocity in rad/s
  q_dot[5] = M_PI/16;// Joint 6 velocity in rad/s

  vpSimulatorAfma6 robot;
  
  robot.setRobotState(vpRobot::STATE_VELOCITY_CONTROL);

  // Moves the joint in velocity
  robot.setVelocity(vpRobot::ARTICULAR_FRAME, q_dot);

  // Initialisation of the velocity measurement
  vpColVector q_dot_mes; // Measured velocities

  for ( ; ; ) {
    robot.getVelocity(vpRobot::ARTICULAR_FRAME, q_dot_mes);
     vpTime::wait(40); // wait 40 ms
     // here q_dot_mes is equal to [0.1, 0.2, 0.3, M_PI/8, M_PI/4, M_PI/16]
  }
}
  \endcode
*/
void
vpSimulatorAfma6::getVelocity (const vpRobot::vpControlFrameType frame, vpColVector & vel)
{
  vel.resize(6);
  
  vpColVector articularCoordinates = get_artCoord();
  vpColVector articularVelocity = get_artVel();
  
  switch(frame)
  {
    case vpRobot::CAMERA_FRAME : 
    {
      vpMatrix eJe;
      vpVelocityTwistMatrix cVe(_eMc);
      vpAfma6::get_eJe(articularCoordinates,eJe);
      vel = cVe*eJe*articularVelocity;
      break ;
    }
    case vpRobot::ARTICULAR_FRAME : 
    {
      vel = articularVelocity;
      break ;
    }
    case vpRobot::REFERENCE_FRAME : 
    {
      vpMatrix fJe;
      vpAfma6::get_fJe(articularCoordinates,fJe);
      vel = fJe*articularVelocity;
      break ;
    }
    case vpRobot::MIXT_FRAME : 
    {
      break ;
    }
    default: 
    {
      vpERROR_TRACE ("Error in spec of vpRobot. "
		   "Case not taken in account.");
      return;
    }
  }
}

/*!
  Get the robot velocities.

  \param frame : Frame in wich velocities are mesured.

  \return Measured velocities. Translations are expressed in m/s
  and rotations in rad/s.

  \code
#include <visp/vpConfig.h>
#include <visp/vpSimulatorAfma6.h>
#include <visp/vpColVector.h>

int main()
{
  // Set requested joint velocities
  vpColVector q_dot(6);
  q_dot[0] = 0.1;    // Joint 1 velocity in rad/s
  q_dot[1] = 0.2;    // Joint 2 velocity in rad/s
  q_dot[2] = 0.3;    // Joint 3 velocity in rad/s
  q_dot[3] = M_PI/8; // Joint 4 velocity in rad/s
  q_dot[4] = M_PI/4; // Joint 5 velocity in rad/s
  q_dot[5] = M_PI/16;// Joint 6 velocity in rad/s

  vpSimulatorAfma6 robot;
  
  robot.setRobotState(vpRobot::STATE_VELOCITY_CONTROL);

  // Moves the joint in velocity
  robot.setVelocity(vpRobot::ARTICULAR_FRAME, q_dot);

  // Initialisation of the velocity measurement
  vpColVector q_dot_mes; // Measured velocities

  for ( ; ; ) {
     q_dot_mes = robot.getVelocity(vpRobot::ARTICULAR_FRAME);
     vpTime::wait(40); // wait 40 ms
     // here q_dot_mes is equal to [0.1, 0.2, 0.3, M_PI/8, M_PI/4, M_PI/16]
  }
}
  \endcode
*/
vpColVector
vpSimulatorAfma6::getVelocity (vpRobot::vpControlFrameType frame)
{
  vpColVector velocity(6);
  getVelocity (frame, velocity);

  return velocity;
}


void 
vpSimulatorAfma6::findHighestPositioningSpeed(vpColVector &q)
{
  double vel_rot_max   = getMaxRotationVelocity();
  double velmax =  fabs(q[0]);
  for (unsigned int i = 1; i < 6; i++)
  {
    if (velmax < fabs(q[i]))
      velmax =  fabs(q[i]);
  }
  
  double alpha = (getPositioningVelocity() * vel_rot_max) / (velmax*100);
  q = q * alpha;
}

/*!
  Move to an absolute position with a given percent of max velocity.
  The percent of max velocity is to set with setPositioningVelocity().
  The position to reach can be specified in joint coordinates, in the
  camera frame or in the reference frame.

  \warning This method is blocking. It returns only when the position
  is reached by the robot.

  \param q : A six dimension vector corresponding to the
  position to reach. All the positions are expressed in meters for the
  translations and radians for the rotations. If the position is out
  of range, an exception is provided.

  \param frame : Frame in which the position is expressed.

  - In the joint space, positions are respectively X, Y, Z, A, B, C,
  with X,Y,Z the translations, and A,B,C the rotations of the
  end-effector.

  - In the camera and the reference frame, rotations are
  represented by a vpRxyzVector.

  - Mixt frame is not implemented. By mixt frame we mean, translations
  expressed in the reference frame, and rotations in the camera
  frame.

  \exception vpRobotException::lowLevelError : vpRobot::MIXT_FRAME not
  implemented.

  \exception vpRobotException::positionOutOfRangeError : The requested
  position is out of range.

  \code
#include <visp/vpConfig.h>
#include <visp/vpSimulatorAfma6.h>
#include <visp/vpColVector.h>

int main()
{
  vpColVector position(6);
  // Set positions in the camera frame
  position[0] = 0.1;    // x axis, in meter
  position[1] = 0.2;    // y axis, in meter
  position[2] = 0.3;    // z axis, in meter
  position[3] = M_PI/8; // rotation around x axis, in rad
  position[4] = M_PI/4; // rotation around y axis, in rad
  position[5] = M_PI;   // rotation around z axis, in rad

  vpSimulatorAfma6 robot;
  
  robot.setRobotState(vpRobot::STATE_POSITION_CONTROL);

  // Set the max velocity to 20%
  robot.setPositioningVelocity(20);

  // Moves the robot in the camera frame
  robot.setPosition(vpRobot::CAMERA_FRAME, position);
}
  \endcode

  To catch the exception if the position is out of range, modify the code like:

  \code
  try {
    robot.setPosition(vpRobot::CAMERA_FRAME, position);
  }
  catch (vpRobotException e) {
    if (e.getCode() == vpRobotException::positionOutOfRangeError) {
    std::cout << "The position is out of range" << std::endl;
  }
  \endcode

*/
void 
vpSimulatorAfma6::setPosition(const vpRobot::vpControlFrameType frame,const vpColVector &q)
{
  if (vpRobot::STATE_POSITION_CONTROL != getRobotState ())
  {
    vpERROR_TRACE ("Robot was not in position-based control\n"
		     "Modification of the robot state");
   // setRobotState(vpRobot::STATE_POSITION_CONTROL) ;
  }
  
  vpColVector articularCoordinates = get_artCoord();
  
  vpColVector error(6);
  double errsqr = 0;
  switch(frame)
  {
    case vpRobot::CAMERA_FRAME : 
    {
      int nbSol;
      vpColVector qdes(6);

      vpTranslationVector txyz;
      vpRxyzVector rxyz;
      for (unsigned int i=0; i < 3; i++)
      {
        txyz[i] = q[i];
        rxyz[i] = q[i+3];
      }

      vpRotationMatrix cRc2(rxyz);
      vpHomogeneousMatrix cMc2(txyz, cRc2);

      vpHomogeneousMatrix fMc;
      vpAfma6::get_fMc(articularCoordinates, fMc);
	
      vpHomogeneousMatrix fMc2 = fMc * cMc2;
	
      do
      {
        articularCoordinates = get_artCoord();
        qdes = articularCoordinates;
        nbSol = getInverseKinematics(fMc2, qdes);
        setVelocityCalled = true;
        if (nbSol > 0)
        {
          error = qdes - articularCoordinates;
          errsqr = error.sumSquare();
          //findHighestPositioningSpeed(error);
          set_artVel(error);
          if (errsqr < 1e-4)
          {
            set_artCoord (qdes);
            error = 0;
            set_artVel(error);
            set_velocity(error);
            break;
          }
        }
        else
        {
          vpERROR_TRACE ("Positionning error.");
          throw vpRobotException (vpRobotException::positionOutOfRangeError,
			    "Position out of range.");
        }
      }while (errsqr > 1e-8 && nbSol > 0);

      break ;
    }
      
    case vpRobot::ARTICULAR_FRAME:
    {
      do
      {
        articularCoordinates = get_artCoord();
        error = q - articularCoordinates;
        errsqr = error.sumSquare();
        //findHighestPositioningSpeed(error);
        set_artVel(error);
        setVelocityCalled = true;
        if (errsqr < 1e-4)
        {
          set_artCoord (q);
          error = 0;
          set_artVel(error);
          set_velocity(error);
          break;
        }
      }while (errsqr > 1e-8);
      break ;
    }
      
    case vpRobot::REFERENCE_FRAME:
    {
      int nbSol;
      vpColVector qdes(6);

      vpTranslationVector txyz;
      vpRxyzVector rxyz;
      for (unsigned int i=0; i < 3; i++)
      {
        txyz[i] = q[i];
        rxyz[i] = q[i+3];
      }

      vpRotationMatrix fRc(rxyz);
      vpHomogeneousMatrix fMc(txyz, fRc);

      do
      {
        articularCoordinates = get_artCoord();
        qdes = articularCoordinates;
        nbSol = getInverseKinematics(fMc, qdes);
        setVelocityCalled = true;
        if (nbSol > 0)
        {
          error = qdes - articularCoordinates;
          errsqr = error.sumSquare();
          //findHighestPositioningSpeed(error);
          set_artVel(error);
          if (errsqr < 1e-4)
          {
            set_artCoord (qdes);
            error = 0;
            set_artVel(error);
            set_velocity(error);
            break;
          }
        }
        else
          vpERROR_TRACE ("Positionning error. Position unreachable");
      }while (errsqr > 1e-8 && nbSol > 0);
      break ;
    }
    case vpRobot::MIXT_FRAME:
    {
      vpERROR_TRACE ("Positionning error. Mixt frame not implemented");
      throw vpRobotException (vpRobotException::lowLevelError,
			      "Positionning error: "
			      "Mixt frame not implemented.");
      break ;
    }
  }
}

/*!
  Move to an absolute position with a given percent of max velocity.
  The percent of max velocity is to set with setPositioningVelocity().
  The position to reach can be specified in joint coordinates, in the
  camera frame or in the reference frame.

  This method overloads setPosition(const
  vpRobot::vpControlFrameType, const vpColVector &).

  \warning This method is blocking. It returns only when the position
  is reached by the robot.

  \param pos1, pos2, pos3, pos4, pos5, pos6 : The six coordinates of
  the position to reach. All the positions are expressed in meters for
  the translations and radians for the rotations.

  \param frame : Frame in which the position is expressed.

  - In the joint space, positions are respectively X (pos1), Y (pos2),
  Z (pos3), A (pos4), B (pos5), C (pos6), with X,Y,Z the
  translations, and A,B,C the rotations of the end-effector.

  - In the camera and the reference frame, rotations [pos4, pos5, pos6] are
  represented by a vpRxyzVector.

  - Mixt frame is not implemented. By mixt frame we mean, translations
  expressed in the reference frame, and rotations in the camera
  frame.

  \exception vpRobotException::lowLevelError : vpRobot::MIXT_FRAME not
  implemented.

  \exception vpRobotException::positionOutOfRangeError : The requested
  position is out of range.

  \code
#include <visp/vpConfig.h>
#include <visp/vpSimulatorAfma6.h>

int main()
{
  // Set positions in the camera frame
  double pos1 = 0.1;    // x axis, in meter
  double pos2 = 0.2;    // y axis, in meter
  double pos3 = 0.3;    // z axis, in meter
  double pos4 = M_PI/8; // rotation around x axis, in rad
  double pos5 = M_PI/4; // rotation around y axis, in rad
  double pos6 = M_PI;   // rotation around z axis, in rad

  vpSimulatorAfma6 robot;

  robot.setRobotState(vpRobot::STATE_POSITION_CONTROL);

  // Set the max velocity to 20%
  robot.setPositioningVelocity(20);

  // Moves the robot in the camera frame
  robot.setPosition(vpRobot::CAMERA_FRAME, pos1, pos2, pos3, pos4, pos5, pos6);
}
  \endcode

  \sa setPosition()
*/
void vpSimulatorAfma6::setPosition (const vpRobot::vpControlFrameType frame,
				   const double pos1,
				   const double pos2,
				   const double pos3,
				   const double pos4,
				   const double pos5,
				   const double pos6)
{
  try{
    vpColVector position(6) ;
    position[0] = pos1 ;
    position[1] = pos2 ;
    position[2] = pos3 ;
    position[3] = pos4 ;
    position[4] = pos5 ;
    position[5] = pos6 ;

    setPosition(frame, position) ;
  }
  catch(...)
    {
      vpERROR_TRACE("Error caught");
      throw ;
    }
}

/*!
  Move to an absolute joint position with a given percent of max
  velocity. The robot state is set to position control.  The percent
  of max velocity is to set with setPositioningVelocity(). The
  position to reach is defined in the position file.

  \param filename : Name of the position file to read. The
  readPosFile() documentation shows a typical content of such a
  position file.

  This method has the same behavior than the sample code given below;
  \code
#include <visp/vpConfig.h>
#include <visp/vpSimulatorAfma6.h>
#include <visp/vpColVector.h>

int main()
{
  vpColVector q;
  vpSimulatorAfma6 robot;

  robot.readPosFile("MyPositionFilename.pos", q);
  robot.setRobotState(vpRobot::STATE_POSITION_CONTROL);
  robot.setPosition(vpRobot::ARTICULAR_FRAME, q);
}
  \endcode

  \exception vpRobotException::lowLevelError : vpRobot::MIXT_FRAME not
  implemented.

  \exception vpRobotException::positionOutOfRangeError : The requested
  position is out of range.

  \sa setPositioningVelocity()
*/
void vpSimulatorAfma6::setPosition(const char *filename)
{
  vpColVector q;
  bool ret;

  ret = this->readPosFile(filename, q);

  if (ret == false) {
    vpERROR_TRACE ("Bad position in \"%s\"", filename);
    throw vpRobotException (vpRobotException::lowLevelError,
			    "Bad position in filename.");
  }
  this->setRobotState(vpRobot::STATE_POSITION_CONTROL);
  this->setPosition(vpRobot::ARTICULAR_FRAME, q);
}

/*!
  Get the current position of the robot.

  \param frame : Control frame type in which to get the position, either :
  - in the camera cartesien frame,
  - joint (articular) coordinates of each axes
  - in a reference or fixed cartesien frame attached to the robot base
  - in a mixt cartesien frame (translation in reference
  frame, and rotation in camera frame)

  \param q : Measured position of the robot:
  - in camera cartesien frame, a 6 dimension vector, set to 0.

  - in articular, a 6 dimension vector corresponding to the joint
  position of each dof in radians.

  - in reference frame, a 6 dimension vector, the first 3 values correspond to
  the translation tx, ty, tz in meters (like a vpTranslationVector), and the
  last 3 values to the rx, ry, rz rotation (like a vpRxyzVector). The code
  below show how to convert this position into a vpHomogenousMatrix:

  \code
#include <visp/vpConfig.h>
#include <visp/vpSimulatorAfma6.h>
#include <visp/vpColVector.h>
#include <visp/vpTranslationVector.h>
#include <visp/vpRxyzVector.h>
#include <visp/vpRotationMatrix.h>
#include <visp/vpHomogeneousMatrix.h>

int main()
{
  vpSimulatorAfma6 robot;

  vpColVector position;
  robot.getPosition(vpRobot::REFERENCE_FRAME, position);

  vpTranslationVector ftc; // reference frame to camera frame translations
  vpRxyzVector frc; // reference frame to camera frame rotations

  // Update the transformation between reference frame and camera frame
  for (int i=0; i < 3; i++) {
    ftc[i] = position[i];   // tx, ty, tz
    frc[i] = position[i+3]; // ry, ry, rz
  }

  // Create a rotation matrix from the Rxyz rotation angles
  vpRotationMatrix fRc(frc); // reference frame to camera frame rotation matrix

  // Create the camera to fix frame transformation in terms of a
  // homogenous matrix
  vpHomogeneousMatrix fMc(fRc, ftc);
}
  \endcode

  \sa setPosition(const vpRobot::vpControlFrameType frame, const
  vpColVector & r)

*/
void 
vpSimulatorAfma6::getPosition(const vpRobot::vpControlFrameType frame, vpColVector &q)
{
  q.resize(6);
  
  switch(frame)
  {
    case vpRobot::CAMERA_FRAME : 
    {
      q = 0;
      break ;
    }
      
    case vpRobot::ARTICULAR_FRAME:
    {
      q = get_artCoord();
      break ;
    }
      
    case vpRobot::REFERENCE_FRAME:
    {
      vpHomogeneousMatrix fMc;
      get_fMc (get_artCoord(), fMc);
      
      vpRotationMatrix fRc;
      fMc.extract(fRc);
      vpRxyzVector rxyz(fRc);
      
      vpTranslationVector txyz;
      fMc.extract(txyz);
      
      for (unsigned int i=0; i <3; i++)
      {
        q[i] = txyz[i];
	q[i+3] = rxyz[i];
      }
      break ;
    }
    
    case vpRobot::MIXT_FRAME:
    {
      vpERROR_TRACE ("Positionning error. Mixt frame not implemented");
      throw vpRobotException (vpRobotException::lowLevelError,
			      "Positionning error: "
			      "Mixt frame not implemented.");
      break ;
    }
  }
}


/*!
  Get the current position of the robot.

  Similar as getPosition(const vpRobot::vpControlFrameType frame, vpColVector &)

  The difference is here that the position is returned using a ThetaU
  representation.

  \sa getPosition(const vpRobot::vpControlFrameType frame, vpColVector &)
*/
void 
vpSimulatorAfma6::getPosition (const vpRobot::vpControlFrameType frame,   
                            vpPoseVector &position)
{
  vpColVector posRxyz;
  //recupere  position en Rxyz
  this->getPosition(frame,posRxyz);
  
  //recupere le vecteur thetaU correspondant
  vpThetaUVector RtuVect(posRxyz[3],posRxyz[4],posRxyz[5]);

  //remplit le vpPoseVector avec translation et rotation ThetaU
  for(unsigned int j=0;j<3;j++)
  {
    position[j]=posRxyz[j];
    position[j+3]=RtuVect[j];
  }
}

/*!
  This method enables to set the minimum and maximum joint limits for all the six axis of the robot. The three first values have to be given in meter and the others in radian.
  
  \param limitMin : The minimum joint limits are given in a vector of size 6. The three first values have to be given in meter and the others in radian.
  \param limitMax : The maximum joint limits are given in a vector of size 6. The three first values have to be given in meter and the others in radian.
*/
void 
vpSimulatorAfma6::setJointLimit(vpColVector limitMin, vpColVector limitMax)
{
  if (limitMin.getRows() != 6 || limitMax.getRows() != 6)
  {
    vpTRACE("Joint limit vector has not a size of 6 !");
    return;
  }
  
  _joint_min[0] = limitMin[0];
  _joint_min[1] = limitMin[1];
  _joint_min[2] = limitMin[2];
  _joint_min[3] = limitMin[3];
  _joint_min[4] = limitMin[4];
  _joint_min[5] = limitMin[5];

  _joint_max[0] = limitMax[0];
  _joint_max[1] = limitMax[1];
  _joint_max[2] = limitMax[2];
  _joint_max[3] = limitMax[3];
  _joint_max[4] = limitMax[4];
  _joint_max[5] = limitMax[5];

}

/*! 
  Test to detect if the robot is near one of its singularities.
  
  The goal is to avoid the problems du to such configurations.
*/
bool
vpSimulatorAfma6::singularityTest(const vpColVector q, vpMatrix &J)
{
  double q5 = q[4];
  
  bool cond = fabs(q5-M_PI/2) < 1e-1;
  
  if(cond)
  {
    J[0][3] = 0;
    J[0][4] = 0;
    J[1][3] = 0;
    J[1][4] = 0;
    J[3][3] = 0;
    J[3][4] = 0;
    J[5][3] = 0;
    J[5][4] = 0;
    return true;
  }
  
  return false;
}

/*! 
  Method used to check if the robot reached a joint limit.
*/
int
vpSimulatorAfma6::isInJointLimit ()
{
  int artNumb = 0;
  double diff = 0;
  double difft = 0;
  
  vpColVector articularCoordinates = get_artCoord();
  
  for (unsigned int i = 0; i < 6; i++)
  {
    if (articularCoordinates[i] <= _joint_min[i])
    {
      difft = _joint_min[i] - articularCoordinates[i];
      if (difft > diff)
      {
	diff = difft;
	artNumb = -(int)i-1;
      }
    }
  }
  
  for (unsigned int i = 0; i < 6; i++)
  {
    if (articularCoordinates[i] >= _joint_max[i])
    {
      difft = articularCoordinates[i] - _joint_max[i];
      if (difft > diff)
      {
	diff = difft;
	artNumb = (int)(i+1);
      }
    }
  }
  
  if (artNumb != 0)
    std::cout << "\nWarning: Velocity control stopped: axis " << fabs((float)artNumb) << " on joint limit!" <<std::endl;
  
  return artNumb;
}

/*!
  Get the robot displacement expressed in the camera frame since the last call
  of this method.

  \param displacement : The measured displacement in the camera frame. The
  dimension of \e displacement is 6 (tx, ty, ty, rx, ry,
  rz). Translations are expressed in meters, rotations in radians with
  the Euler Rxyz representation.

  \sa getDisplacement(), getArticularDisplacement()
*/
void
vpSimulatorAfma6::getCameraDisplacement(vpColVector &displacement)
{
  getDisplacement(vpRobot::CAMERA_FRAME, displacement);
}

/*!
  Get the robot joint displacement since the last call of this method.

  \param displacement : The measured joint displacement. The dimension
  of \e displacement is 6 (the robot joint number). All the values are
  expressed in radians.

  \sa getDisplacement(), getCameraDisplacement()
*/
void
vpSimulatorAfma6::getArticularDisplacement(vpColVector  &displacement)
{
  getDisplacement(vpRobot::ARTICULAR_FRAME, displacement);
}

/*!
  Get the robot displacement since the last call of this method.

  \warning This functionnality is not implemented for the moment in the
  cartesian space. It is only available in the joint space
  (vpRobot::ARTICULAR_FRAME).

  \param frame : The frame in which the measured displacement is expressed.

  \param displacement : The measured displacement since the last call
  of this method. The dimension of \e displacement is always
  6. Translations are expressed in meters, rotations in radians.

  In camera or reference frame, rotations are expressed with the
  Euler Rxyz representation.

  \sa getArticularDisplacement(), getCameraDisplacement()
*/
void
vpSimulatorAfma6::getDisplacement(vpRobot::vpControlFrameType frame,
				 vpColVector &displacement)
{
  displacement.resize (6);
  displacement = 0;
  vpColVector q_cur(6);

  q_cur = get_artCoord();

  if ( ! first_time_getdis ) 
  {
    switch (frame) 
    {
      case vpRobot::CAMERA_FRAME: 
      {
        std::cout << "getDisplacement() CAMERA_FRAME not implemented\n";
        return;
        break ;
      }

      case vpRobot::ARTICULAR_FRAME: 
      {
        displacement = q_cur - q_prev_getdis;
        break ;
      }

      case vpRobot::REFERENCE_FRAME: 
      {
        std::cout << "getDisplacement() REFERENCE_FRAME not implemented\n";
        return;
        break ;
      }

      case vpRobot::MIXT_FRAME: 
      {
        std::cout << "getDisplacement() MIXT_FRAME not implemented\n";
        return;
        break ;
      }
    }
  }
  else 
  {
    first_time_getdis = false;
  }

  // Memorize the joint position for the next call
  q_prev_getdis = q_cur;
}

/*!
Read joint positions in a specific Afma6 position file.

This position file has to start with a header. The six joint positions
are given after the "R:" keyword. The first 3 values correspond to the
joint translations X,Y,Z expressed in meters. The 3 last values
correspond to the joint rotations A,B,C expressed in degres to be more
representative for the user. Theses values are then converted in
radians in \e q. The character "#" starting a line indicates a
comment.

A typical content of such a file is given below:

\code
#AFMA6 - Position - Version 2.01
# file: "myposition.pos "
#
# R: X Y Z A B C
# Joint position: X, Y, Z: translations in meters
#                 A, B, C: rotations in degrees
#

R: 0.1 0.3 -0.25 -80.5 80 0
\endcode

\param filename : Name of the position file to read.

\param q : Joint positions: X,Y,Z,A,B,C. Translations X,Y,Z are
expressed in meters, while joint rotations A,B,C in radians.

\return true if the positions were successfully readen in the file. false, if
an error occurs.

The code below shows how to read a position from a file and move the robot to this position.
\code
vpSimulatorAfma6 robot;
vpColVector q;        // Joint position
robot.readPosFile("myposition.pos", q); // Set the joint position from the file
robot.setRobotState(vpRobot::STATE_POSITION_CONTROL);

robot.setPositioningVelocity(5); // Positioning velocity set to 5%
robot.setPosition(vpRobot::ARTICULAR_FRAME, q); // Move to the joint position
\endcode

\sa savePosFile()
*/
bool
vpSimulatorAfma6::readPosFile(const char *filename, vpColVector &q)
{

  FILE * fd ;
  fd = fopen(filename, "r") ;
  if (fd == NULL)
    return false;

  char line[FILENAME_MAX];
  char dummy[FILENAME_MAX];
  char head[] = "R:";
  bool sortie = false;

  do {
    // Saut des lignes commencant par #
    if (fgets (line, FILENAME_MAX, fd) != NULL) {
      if ( strncmp (line, "#", 1) != 0) {
	// La ligne n'est pas un commentaire
	if ( strncmp (line, head, sizeof(head)-1) == 0) {
	  sortie = true; 	// Position robot trouvee.
	}
// 	else
// 	  return (false); // fin fichier sans position robot.
      }
    }
    else {
      return (false);		/* fin fichier 	*/
    }

  }
  while ( sortie != true );

  // Lecture des positions
  q.resize(njoint);
  sscanf(line, "%s %lf %lf %lf %lf %lf %lf",
	 dummy,
	 &q[0], &q[1], &q[2],
	 &q[3], &q[4], &q[5]);

  // converts rotations from degrees into radians
  //q.deg2rad();
  
  q[3] = vpMath::rad(q[3]);
  q[4] = vpMath::rad(q[4]);
  q[5] = vpMath::rad(q[5]);

  fclose(fd) ;
  return (true);
}

/*!
  Save joint (articular) positions in a specific Afma6 position file.

  This position file starts with a header on the first line. After
  convertion of the rotations in degrees, the joint position \e q is
  written on a line starting with the keyword "R: ". See readPosFile()
  documentation for an example of such a file.

  \param filename : Name of the position file to create.

  \param q : Joint positions [X,Y,Z,A,B,C] to save in the
  filename. Translations X,Y,Z are expressed in meters, while
  rotations A,B,C in radians.

  \warning The joint rotations A,B,C written in the file are converted
  in degrees to be more representative for the user.

  \return true if the positions were successfully saved in the file. false, if
  an error occurs.

  \sa readPosFile()
*/
bool
vpSimulatorAfma6::savePosFile(const char *filename, const vpColVector &q)
{

  FILE * fd ;
  fd = fopen(filename, "w") ;
  if (fd == NULL)
    return false;

  fprintf(fd, "\
#AFMA6 - Position - Version 2.01\n\
#\n\
# R: X Y Z A B C\n\
# Joint position: X, Y, Z: translations in meters\n\
#                 A, B, C: rotations in degrees\n\
#\n\
#\n\n");

  // Save positions in mm and deg
  fprintf(fd, "R: %lf %lf %lf %lf %lf %lf\n",
	  q[0],
	  q[1],
	  q[2],
	  vpMath::deg(q[3]),
	  vpMath::deg(q[4]),
	  vpMath::deg(q[5]));

  fclose(fd) ;
  return (true);
}

/*!
  Moves the robot to the joint position specified in the filename.

  \param filename: File containing a joint position.

  \sa readPosFile
*/
void
vpSimulatorAfma6::move(const char *filename)
{
  vpColVector q;

  try
  {
    this->readPosFile(filename, q);
    this->setRobotState(vpRobot::STATE_POSITION_CONTROL);
    this->setPosition ( vpRobot::ARTICULAR_FRAME,  q);
  }
  catch(...) 
  {
    throw;
  }
}

/*!
  Get the geometric transformation \f$^c{\bf M}_e\f$ between the
  camera frame and the end-effector frame. This transformation is
  constant and correspond to the extrinsic camera parameters estimated
  by calibration.

  \param cMe : Transformation between the camera frame and the
  end-effector frame.
*/
void
vpSimulatorAfma6::get_cMe(vpHomogeneousMatrix &cMe)
{
  vpAfma6::get_cMe(cMe) ;
}

/*!
  Get the twist transformation \f$^c{\bf V}_e\f$ from camera frame to
  end-effector frame.  This transformation allows to compute a
  velocity expressed in the end-effector frame into the camera frame.

  \param cVe : Twist transformation.
*/
void
vpSimulatorAfma6::get_cVe(vpVelocityTwistMatrix &cVe)
{
  vpHomogeneousMatrix cMe ;
  vpAfma6::get_cMe(cMe) ;

  cVe.buildFrom(cMe) ;
}

/*!
  Get the robot jacobian expressed in the end-effector frame.

  To compute \f$^e{\bf J}_e\f$, we communicate with the low level
  controller to get the joint position of the robot.

  \param eJe : Robot jacobian \f$^e{\bf J}_e\f$ expressed in the
  end-effector frame.
*/
void
vpSimulatorAfma6::get_eJe(vpMatrix &eJe)
{
  try
  {
    vpAfma6::get_eJe(get_artCoord(), eJe) ;
  }
  catch(...)
  {
    vpERROR_TRACE("catch exception ") ;
    throw ;
  }
}

/*!
  Get the robot jacobian expressed in the robot reference frame also
  called fix frame.

  To compute \f$^f{\bf J}_e\f$, we communicate with the low level
  controller to get the joint position of the robot.

  \param fJe : Robot jacobian \f$^f{\bf J}_e\f$ expressed in the
  reference frame.
*/
void
vpSimulatorAfma6::get_fJe(vpMatrix &fJe)
{
  try
  {
    vpColVector articularCoordinates = get_artCoord(); 
    vpAfma6::get_fJe(articularCoordinates, fJe) ;
  }
  catch(...)
  {
    vpERROR_TRACE("Error caught");
    throw ;
  }
}

/*!
  Stop the robot.
*/
void 
vpSimulatorAfma6::stopMotion()
{
  if (getRobotState() != vpRobot::STATE_VELOCITY_CONTROL)
    return;
  
  vpColVector stop(6);
  stop = 0;
  set_artVel(stop);
  set_velocity(stop);
  setRobotState (vpRobot::STATE_STOP);
}



/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/

/*!
  Initialise the display of the robot's arms.

  Set the path to the scene files (*.bnd and *.sln) used by the
  simulator.  If the path set in vpConfig.h in VISP_SCENES_DIR macro is
  not valid, the path is set from the VISP_SCENES_DIR environment
  variable that the user has to set.
*/
void
vpSimulatorAfma6::initArms()
{
  // set scene_dir from #define VISP_SCENE_DIR if it exists
  std::string scene_dir;
  if (vpIoTools::checkDirectory(VISP_SCENES_DIR) == true) // directory exists
    scene_dir = VISP_SCENES_DIR;
  else {
    try {
      scene_dir = vpIoTools::getenv("VISP_SCENES_DIR");
      std::cout << "The simulator uses data from VISP_SCENES_DIR=" << scene_dir << std::endl;
    }
    catch (...) {
      std::cout << "Cannot get VISP_SCENES_DIR environment variable" << std::endl;
    }
  }

  char name_cam[FILENAME_MAX];

  strcpy(name_cam, scene_dir.c_str());
  strcat(name_cam,"/camera.bnd");
  set_scene(name_cam,&camera,cameraFactor);
  
  char name_arm[FILENAME_MAX];
  strcpy(name_arm, arm_dir.c_str());
  strcat(name_arm,"/afma6_gate.bnd");
  set_scene(name_arm, robotArms, 1.0);
  strcpy(name_arm, arm_dir.c_str());
  strcat(name_arm,"/afma6_arm1.bnd");
  set_scene(name_arm, robotArms+1, 1.0);
  strcpy(name_arm, arm_dir.c_str());
  strcat(name_arm,"/afma6_arm2.bnd");
  set_scene(name_arm, robotArms+2, 1.0);
  strcpy(name_arm, arm_dir.c_str());
  strcat(name_arm,"/afma6_arm3.bnd");
  set_scene(name_arm, robotArms+3, 1.0);
  strcpy(name_arm, arm_dir.c_str());
  strcat(name_arm,"/afma6_arm4.bnd");
  set_scene(name_arm, robotArms+4, 1.0);
  
  vpAfma6::vpAfma6ToolType tool;
  tool = getToolType();
  strcpy(name_arm, arm_dir.c_str());
  switch (tool) {
  case vpAfma6::TOOL_CCMOP: {
      strcat(name_arm,"/afma6_tool_ccmop.bnd");
      break;
    }
  case vpAfma6::TOOL_GRIPPER: {
      strcat(name_arm,"/afma6_tool_gripper.bnd");
      break;
    }
  case vpAfma6::TOOL_VACUUM: {
      strcat(name_arm,"/afma6_tool_vacuum.bnd");
      break;
    }
  case vpAfma6::TOOL_GENERIC_CAMERA: {
      std::cout << "The generic camera is not handled in vpSimulatorAfma6.cpp" << std::endl;
      break;
    }
  }
  set_scene(name_arm, robotArms+5, 1.0);
  
  add_rfstack(IS_BACK);

  add_vwstack ("start","depth", 0.0, 100.0);
  add_vwstack ("start","window", -0.1,0.1,-0.1,0.1);
  add_vwstack ("start","type", PERSPECTIVE);
// 
//   sceneInitialized = true;
//   displayObject = true;
  displayCamera = true;
}


void 
vpSimulatorAfma6::getExternalImage(vpImage<vpRGBa> &I)
{
  bool changed = false;
  vpHomogeneousMatrix displacement = navigation(I,changed);

  //if (displacement[2][3] != 0)
  if (std::fabs(displacement[2][3]) > std::numeric_limits<double>::epsilon())
    camMf2 = camMf2*displacement;

  f2Mf = camMf2.inverse()*camMf;

  camMf = camMf2* displacement * f2Mf;

  double u;
  double v;
  //if(px_ext != 1 && py_ext != 1)
  // we assume px_ext and py_ext > 0
  if( (std::fabs(px_ext-1.) > vpMath::maximum(px_ext,1.)*std::numeric_limits<double>::epsilon()) 
      && (std::fabs(py_ext-1) > vpMath::maximum(py_ext,1.)*std::numeric_limits<double>::epsilon()))
  {
    u = (double)I.getWidth()/(2*px_ext);
    v = (double)I.getHeight()/(2*py_ext);
  }
  else
  {
    u = (double)I.getWidth()/(vpMath::minimum(I.getWidth(),I.getHeight()));
    v = (double)I.getHeight()/(vpMath::minimum(I.getWidth(),I.getHeight()));
  }

  float w44o[4][4],w44cext[4][4],x,y,z;

  vp2jlc_matrix(camMf.inverse(),w44cext);

  add_vwstack ("start","cop", w44cext[3][0],w44cext[3][1],w44cext[3][2]);
  x = w44cext[2][0] + w44cext[3][0];
  y = w44cext[2][1] + w44cext[3][1];
  z = w44cext[2][2] + w44cext[3][2];
  add_vwstack ("start","vrp", x,y,z);
  add_vwstack ("start","vpn", w44cext[2][0],w44cext[2][1],w44cext[2][2]);
  add_vwstack ("start","vup", w44cext[1][0],w44cext[1][1],w44cext[1][2]);
  add_vwstack ("start","window", -u, u, -v, v);
  
  vpHomogeneousMatrix fMit[8];
  get_fMi(fMit);
  
  vp2jlc_matrix(vpHomogeneousMatrix(0,0,0,0,0,0),w44o);
  display_scene(w44o,robotArms[0],I, curColor);
  
  vp2jlc_matrix(fMit[0],w44o);
  display_scene(w44o,robotArms[1],I, curColor);
  
  vp2jlc_matrix(fMit[2],w44o);
  display_scene(w44o,robotArms[2],I, curColor);
  
  vp2jlc_matrix(fMit[3],w44o);
  display_scene(w44o,robotArms[3],I, curColor);
  
  vp2jlc_matrix(fMit[4],w44o);
  display_scene(w44o,robotArms[4],I, curColor);
  
  vp2jlc_matrix(fMit[5],w44o);
  display_scene(w44o,robotArms[5],I, curColor);

  if (displayCamera)
  {
    vpHomogeneousMatrix cMe;
    get_cMe(cMe);
    cMe = cMe.inverse();
    cMe = fMit[6] * cMe;
    vp2jlc_matrix(cMe,w44o);
    display_scene(w44o,camera, I, camColor);
  }
  
  if (displayObject)
  {
    vp2jlc_matrix(fMo,w44o);
    display_scene(w44o,scene,I, curColor);
  }
}

/*!
  This method enables to initialise the articular coordinates of the robot in order to position the camera relative to the object.
  
  Before using this method it is advised to set the position of the object thanks to the set_fMo() method.
  
  \param cMo : the desired pose of the camera.
*/
void 
vpSimulatorAfma6::initialiseCameraRelativeToObject(vpHomogeneousMatrix cMo)
{
  vpColVector stop(6);
  stop = 0;
  set_artVel(stop);
  set_velocity(stop);
  vpHomogeneousMatrix fMc;
  fMc = fMo * cMo.inverse();
  
  vpColVector articularCoordinates = get_artCoord();
  int nbSol = getInverseKinematics(fMc, articularCoordinates);
  
  if (nbSol == 0)
    vpERROR_TRACE ("Positionning error. Position unreachable");
  
  set_artCoord(articularCoordinates);
  
  compute_fMi();
}

/*!
  This method enables to initialise the pose between the object and the refrence frame, in order to position the object relative to the camera.
  
  Before using this method it is advised to set the articular coordinates of the robot.
  
  \param cMo : the desired pose of the camera.
*/
void 
vpSimulatorAfma6::initialiseObjectRelativeToCamera(vpHomogeneousMatrix cMo)
{
  vpColVector stop(6);
  stop = 0;
  set_artVel(stop);
  set_velocity(stop);
  vpHomogeneousMatrix fMit[8];
  get_fMi(fMit);
  fMo = fMit[7] * cMo;
}

/*!
  This method enable to move the robot with respect to the initialized object.
  The robot trajectory is a straight line from the current position to the one corresponding to the desired pose (3D visual servoing).

  \param cdMo : the desired pose of the camera wrt. the object
  \param Iint : pointer to the image where the internal view is displayed
  \param errMax : maximum error to consider the pose is reached

  \return True is the pose is reached, False else
*/
bool
vpSimulatorAfma6::setPosition(const vpHomogeneousMatrix &cdMo, vpImage<unsigned char> *Iint, const double &errMax)
{
	// get rid of max velocity
	double vMax = getMaxTranslationVelocity();
	double wMax = getMaxRotationVelocity();
	setMaxTranslationVelocity(10.*vMax);
	setMaxRotationVelocity(10.*wMax);

	vpColVector v(3),w(3),vel(6);
	vpHomogeneousMatrix cdMc;
	vpTranslationVector cdTc;vpRotationMatrix cdRc;vpThetaUVector cdTUc;
	vpColVector err(6);err=1.;
	const double lambda = 5.;
	double t;

	vpVelocityTwistMatrix cVe;
	vpMatrix eJe;

	unsigned int i,iter=0;
	while((iter++<300) & (err.euclideanNorm()>errMax))
		{
		t = vpTime::measureTimeMs();

		// update image
		if(Iint != NULL)
		{
			vpDisplay::display(*Iint);
			getInternalView(*Iint);
			vpDisplay::flush(*Iint);
		}

		// update pose error
		cdMc = cdMo*get_cMo().inverse();
		cdMc.extract(cdRc);
		cdMc.extract(cdTc);
		cdTUc.buildFrom(cdRc);

		// compute v,w and velocity
		v = -lambda*cdRc.t()*cdTc;
		w = -lambda*cdTUc;
		for(i=0;i<3;++i)
		{
			vel[i] = v[i];
			vel[i+3] = w[i];
			err[i] = cdTc[i];
			err[i+3] = cdTUc[i];
		}

		// update feat
		setVelocity(vpRobot::CAMERA_FRAME,vel);

		// wait for it
		vpTime::wait(t,10);
		}
	vel=0.;
	set_velocity(vel);
	set_artVel(vel);
	setMaxTranslationVelocity(vMax);
	setMaxRotationVelocity(wMax);

	//std::cout << "setPosition: final error " << err.t() << std::endl;
	return(err.euclideanNorm()<= errMax);
}

#endif
