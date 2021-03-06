/****************************************************************************
 *
 * $Id: vpCalibration.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
 * Camera calibration.
 *
 * Authors:
 * Eric Marchand
 * Francois Chaumette
 * Anthony Saunier
 *
 *****************************************************************************/


/*!
  \file vpCalibration.cpp
  \brief Tools for camera calibration.
*/

#include <visp/vpCalibration.h>
#include <visp/vpDebug.h>
#include <visp/vpPose.h>
#include <visp/vpPixelMeterConversion.h>

double vpCalibration::threshold = 1e-10f;
unsigned int vpCalibration::nbIterMax = 4000;
double vpCalibration::gain = 0.25;
/*!
  Basic initialisation (called by the constructors)
*/
int vpCalibration::init()
{
  npt = 0 ;

  LoX.clear() ;
  LoY.clear() ;
  LoZ.clear() ;
  Lip.clear() ;

  return 0 ;
}

vpCalibration::vpCalibration()
{
  init() ;
}


/*!
  Destructor : delete the array of point (freed the memory)
*/
vpCalibration::~vpCalibration()
{
  clearPoint() ;
}

/*!
  = operator.

  \param twinCalibration : object to be copied
*/
void vpCalibration::operator=(vpCalibration& twinCalibration )
                             {
  npt = twinCalibration.npt ;
  LoX = twinCalibration.LoX ;
  LoY = twinCalibration.LoY ;
  LoZ = twinCalibration.LoZ ;
  Lip = twinCalibration.Lip ;

  residual = twinCalibration.residual;
  cMo = twinCalibration.cMo;
  residual_dist = twinCalibration.residual_dist;
  cMo_dist = twinCalibration.cMo_dist ;

  cam = twinCalibration.cam ;
  cam_dist = twinCalibration.cam_dist ;

  rMe = twinCalibration.rMe;

  eMc = twinCalibration.eMc;
  eMc_dist = twinCalibration.eMc_dist;
}


/*!
  Delete the array of points.
*/
int vpCalibration::clearPoint()
{
  LoX.clear() ;
  LoY.clear() ;
  LoZ.clear() ;
  Lip.clear() ;
  npt = 0 ;

  return 0 ;
}

/*!
  
  Add a new point in the array of points.
  \param  X,Y,Z : 3D coordinates of a point in the object frame
  \param ip : 2D Coordinates of the point in the camera frame.
*/
int vpCalibration::addPoint(double X, double Y, double Z, vpImagePoint &ip)
{
  LoX.push_back(X);
  LoY.push_back(Y);
  LoZ.push_back(Z);

  Lip.push_back(ip);

  npt++ ;

  return 0 ;
}

/*!
  Compute the pose cMo
  \param cam : camera intrinsic parameters used for computation
  \param cMo : computed pose
 */
void
    vpCalibration::computePose(const vpCameraParameters &cam, vpHomogeneousMatrix &cMo)
{
  // The vpPose class mainly contents a list of vpPoint (that is (X,Y,Z, x, y) )
  vpPose pose ;
  //  the list of point is cleared (if that's not done before)
  pose.clearPoint() ;
  // we set the 3D points coordinates (in meter !) in the object/world frame
  std::list<double>::const_iterator it_LoX = LoX.begin();
  std::list<double>::const_iterator it_LoY = LoY.begin();
  std::list<double>::const_iterator it_LoZ = LoZ.begin();
  std::list<vpImagePoint>::const_iterator it_Lip = Lip.begin();

  for (unsigned int i =0 ; i < npt ; i++)
  {
    vpPoint P;
    P.setWorldCoordinates(*it_LoX, *it_LoY, *it_LoZ);
    double x=0,y=0 ;
    vpPixelMeterConversion::convertPoint(cam, *it_Lip, x,y)  ;
    P.set_x(x) ;
    P.set_y(y) ;

    pose.addPoint(P);
    ++it_LoX;
    ++it_LoY;
    ++it_LoZ;
    ++it_Lip;
  }
  vpHomogeneousMatrix cMo_dementhon;  // computed pose with dementhon
  vpHomogeneousMatrix cMo_lagrange;  // computed pose with dementhon

  // compute the initial pose using Lagrange method followed by a non linear
  // minimisation method
  // Pose by Lagrange it provides an initialization of the pose
  pose.computePose(vpPose::LAGRANGE, cMo_lagrange) ;
  double residual_lagrange = pose.computeResidual(cMo_lagrange);

  // compute the initial pose using Dementhon method followed by a non linear
  // minimisation method
  // Pose by Dementhon it provides an initialization of the pose
  pose.computePose(vpPose::DEMENTHON, cMo_dementhon) ;
  double residual_dementhon = pose.computeResidual(cMo_dementhon);

  //we keep the better initialization 
  if (residual_lagrange < residual_dementhon)
    cMo = cMo_lagrange;
  else
    cMo = cMo_dementhon;
  
  // the pose is now refined using the virtual visual servoing approach
  // Warning: cMo needs to be initialized otherwise it may diverge
  pose.computePose(vpPose::VIRTUAL_VS, cMo) ;

}

/*!
  Compute and return the standard deviation expressed in pixel
  for pose matrix and camera intrinsic parameters for model without distortion.
  \param cMo : the matrix that defines the pose to be tested.
  \param cam : camera intrinsic parameters to be tested.
  \return the standard deviation by point of the error in pixel .
*/
double
    vpCalibration::computeStdDeviation(vpHomogeneousMatrix& cMo,
                                       vpCameraParameters& cam)
{
  double residual = 0 ;

  std::list<double>::const_iterator it_LoX = LoX.begin();
  std::list<double>::const_iterator it_LoY = LoY.begin();
  std::list<double>::const_iterator it_LoZ = LoZ.begin();
  std::list<vpImagePoint>::const_iterator it_Lip = Lip.begin();

  double u0 = cam.get_u0() ;
  double v0 = cam.get_v0() ;
  double px = cam.get_px() ;
  double py = cam.get_py() ;
  vpImagePoint ip;

  for (unsigned int i =0 ; i < npt ; i++)
  {

    double oX = *it_LoX;
    double oY = *it_LoY;
    double oZ = *it_LoZ;

    double cX = oX*cMo[0][0]+oY*cMo[0][1]+oZ*cMo[0][2] + cMo[0][3];
    double cY = oX*cMo[1][0]+oY*cMo[1][1]+oZ*cMo[1][2] + cMo[1][3];
    double cZ = oX*cMo[2][0]+oY*cMo[2][1]+oZ*cMo[2][2] + cMo[2][3];

    double x = cX/cZ ;
    double y = cY/cZ ;

    ip = *it_Lip;
    double u = ip.get_u() ;
    double v = ip.get_v() ;

    double xp = u0 + x*px;
    double yp = v0 + y*py;

    residual += (vpMath::sqr(xp-u) + vpMath::sqr(yp-v))  ;

    ++it_LoX;
    ++it_LoY;
    ++it_LoZ;
    ++it_Lip;
  }
  this->residual = residual ;
  return sqrt(residual/npt) ;
}
/*!
  Compute and return the standard deviation expressed in pixel
  for pose matrix and camera intrinsic parameters with pixel to meter model.
  \param cMo : the matrix that defines the pose to be tested.
  \param cam : camera intrinsic parameters to be tested.
  \return the standard deviation by point of the error in pixel .
*/
double
    vpCalibration::computeStdDeviation_dist(vpHomogeneousMatrix& cMo,
                                            vpCameraParameters& cam)
{
  double residual = 0 ;

  std::list<double>::const_iterator it_LoX = LoX.begin();
  std::list<double>::const_iterator it_LoY = LoY.begin();
  std::list<double>::const_iterator it_LoZ = LoZ.begin();
  std::list<vpImagePoint>::const_iterator it_Lip = Lip.begin();

  double u0 = cam.get_u0() ;
  double v0 = cam.get_v0() ;
  double px = cam.get_px() ;
  double py = cam.get_py() ;
  double kud = cam.get_kud() ;
  double kdu = cam.get_kdu() ;

  double inv_px = 1/px;
  double inv_py = 1/px;
  vpImagePoint ip;

  for (unsigned int i =0 ; i < npt ; i++)
  {

    double oX = *it_LoX;
    double oY = *it_LoY;
    double oZ = *it_LoZ;

    double cX = oX*cMo[0][0]+oY*cMo[0][1]+oZ*cMo[0][2] + cMo[0][3];
    double cY = oX*cMo[1][0]+oY*cMo[1][1]+oZ*cMo[1][2] + cMo[1][3];
    double cZ = oX*cMo[2][0]+oY*cMo[2][1]+oZ*cMo[2][2] + cMo[2][3];

    double x = cX/cZ ;
    double y = cY/cZ ;

    ip = *it_Lip;
    double u = ip.get_u() ;
    double v = ip.get_v() ;

    double r2ud = 1+kud*(vpMath::sqr(x)+vpMath::sqr(y)) ;

    double xp = u0 + x*px*r2ud;
    double yp = v0 + y*py*r2ud;

    residual += (vpMath::sqr(xp-u) + vpMath::sqr(yp-v))  ;

    double r2du = (vpMath::sqr((u-u0)*inv_px)+vpMath::sqr((v-v0)*inv_py)) ;

    xp = u0 + x*px - kdu*(u-u0)*r2du;
    yp = v0 + y*py - kdu*(v-v0)*r2du;

    residual += (vpMath::sqr(xp-u) + vpMath::sqr(yp-v))  ;
    ++it_LoX;
    ++it_LoY;
    ++it_LoZ;
    ++it_Lip;
  }
  residual /=2;

  this->residual_dist = residual;
  return sqrt(residual/npt) ;
}

/*!
  Compute and return the standard deviation expressed in pixel
  for pose matrix and camera intrinsic parameters.
  \param deviation   : the standard deviation computed for the model without distortion.
  \param deviation_dist : the standard deviation computed for the model with distortion.
*/
void
    vpCalibration::computeStdDeviation(double &deviation,double &deviation_dist)
{
  deviation   = computeStdDeviation(cMo,cam);
  deviation_dist = computeStdDeviation_dist(cMo_dist,cam_dist);
}


/*!
  Compute the calibration according to the desired method using one pose.

  \param method : Method that will be used to estimate the parameters.
  \param cMo : the homogeneous matrix that defines the pose.
  \param cam : intrinsic camera parameters.
  \param verbose : set at true if information about the residual at each loop
  of the algorithm is hoped.

  \return 0 if the calibration computation succeed.
*/
int vpCalibration::computeCalibration(vpCalibrationMethodType method,
                                      vpHomogeneousMatrix &cMo,
                                      vpCameraParameters &cam,
                                      bool verbose)
{
  try{
    computePose(cam,cMo);
    switch (method)
    {
    case CALIB_LAGRANGE :
    case CALIB_LAGRANGE_VIRTUAL_VS :
      {
	      calibLagrange(cam, cMo);
      }
      break;
    case CALIB_VIRTUAL_VS:
    case CALIB_VIRTUAL_VS_DIST:
    case CALIB_LAGRANGE_VIRTUAL_VS_DIST:
    default:
      break;
    }

    switch (method)
    {
    case CALIB_VIRTUAL_VS:
    case CALIB_VIRTUAL_VS_DIST:
    case CALIB_LAGRANGE_VIRTUAL_VS:
    case CALIB_LAGRANGE_VIRTUAL_VS_DIST:
      {
        if (verbose){std::cout << "start calibration without distortion"<< std::endl;}
        calibVVS(cam, cMo, verbose);
      }
      break ;
    case CALIB_LAGRANGE:
    default:
      break;
    }
    this->cMo = cMo;
    this->cMo_dist = cMo;

    //Print camera parameters
    if(verbose){
      //       std::cout << "Camera parameters without distortion :" << std::endl;
      cam.printParameters();
    }

    this->cam = cam;

    switch (method)
    {
    case CALIB_VIRTUAL_VS_DIST:
    case CALIB_LAGRANGE_VIRTUAL_VS_DIST:
      {
        if (verbose){std::cout << "start calibration with distortion"<< std::endl;}
        calibVVSWithDistortion(cam, cMo, verbose);
      }
      break ;
    case CALIB_LAGRANGE:
    case CALIB_VIRTUAL_VS:
    case CALIB_LAGRANGE_VIRTUAL_VS:
    default:
      break;
    }
    //Print camera parameters
    if(verbose){
      //       std::cout << "Camera parameters without distortion :" << std::endl;
      this->cam.printParameters();
      //       std::cout << "Camera parameters with distortion :" << std::endl;
      cam.printParameters();
    }

    this->cam_dist = cam ;

    this->cMo_dist = cMo;
    return 0 ;
  }
  catch(...){
    throw;
  }
}

/*!
  Compute the multi-images calibration according to the desired method using many poses.

  \param method : Method used to estimate the camera parameters.
  \param nbPose : number of images used to compute multi-images calibration
  \param table_cal : array of vpCalibration.
  \param cam : intrinsic camera parameters.
  \param verbose : set at true if information about the residual at each loop
  of the algorithm is hoped.

  \return 0 if the computation was managed succeed.
*/
int vpCalibration::computeCalibrationMulti(vpCalibrationMethodType method,
                                           unsigned int nbPose,
                                           vpCalibration table_cal[],
                                           vpCameraParameters& cam,
                                           bool verbose)
{
  try{
    for(unsigned int i=0;i<nbPose;i++){
      if(table_cal[i].get_npt()>3)
        table_cal[i].computePose(cam,table_cal[i].cMo);
    }
    switch (method) {   
    case CALIB_LAGRANGE :
      if(nbPose > 1){
        std::cout << "this calibration method is not available in" << std::endl
            << "vpCalibration::computeCalibrationMulti()" << std::endl;
        return -1 ;
      }
      else {
        table_cal[0].calibLagrange(cam,table_cal[0].cMo);
	      table_cal[0].cam = cam ;
        table_cal[0].cam_dist = cam ;
        table_cal[0].cMo_dist = table_cal[0].cMo ;
      }
      break;
    case CALIB_LAGRANGE_VIRTUAL_VS :
    case CALIB_LAGRANGE_VIRTUAL_VS_DIST :
      if(nbPose > 1){
        std::cout << "this calibration method is not available in" << std::endl
            << "vpCalibration::computeCalibrationMulti()" << std::endl
            << "with several images." << std::endl;
        return -1 ;
      }
      else {
	      table_cal[0].calibLagrange(cam,table_cal[0].cMo);
	      table_cal[0].cam = cam ;
        table_cal[0].cam_dist = cam ;
        table_cal[0].cMo_dist = table_cal[0].cMo ;
      }
    case CALIB_VIRTUAL_VS:
    case CALIB_VIRTUAL_VS_DIST:
      {
        calibVVSMulti(nbPose, table_cal, cam, verbose);
      }
      break ;
    }
    //Print camera parameters
    if(verbose){
      //       std::cout << "Camera parameters without distortion :" << std::endl;
      cam.printParameters();
    }

    switch (method)
    {
    case CALIB_LAGRANGE :
    case CALIB_LAGRANGE_VIRTUAL_VS :
    case CALIB_VIRTUAL_VS:
      verbose = false ;
      break;
    case CALIB_LAGRANGE_VIRTUAL_VS_DIST :
    case CALIB_VIRTUAL_VS_DIST:
      {
        if(verbose)
          std::cout << "Compute camera parameters with distortion"<<std::endl;

        calibVVSWithDistortionMulti(nbPose, table_cal, cam, verbose);
      }
      break ;
    }
    //Print camera parameters
    if(verbose){
      //       std::cout << "Camera parameters without distortion :" << std::endl;
      table_cal[0].cam.printParameters();
      //       std::cout << "Camera parameters with distortion:" << std::endl;
      cam.printParameters();
      std::cout<<std::endl;
    }
    return 0 ;
  }
  catch(...){ throw; }
}

/*!
  \brief Compute the multi-image calibration of effector-camera from R. Tsai and R. LorenzTsai.

  Compute extrinsic camera parameters : the constant transformation from
  the effector to the camera coordinates (eMc).

  R. Tsai, R. Lenz. -- A new technique for fully autonomous and efficient 3D
  robotics hand/eye calibration. -- IEEE Transactions on Robotics and
  Automation, 5(3):345--358, June 1989.

  \param nbPose : number of images used to compute multi-images calibration
  \param table_cal : array of vpCalibration. All the vpHomogeneousMatrix cMo
  and rMe of each vpCalibration have to be initialized.
  \param eMc : output: estimated pose of the camera in relation to the effector
  (camera support) with the camera model without distortion.
  \param eMc_dist : output: estimated pose of the camera in relation to the
  effector (camera support) with the model with distortion.
  \return 0 if the computation managed.
*/
int vpCalibration::computeCalibrationTsai(unsigned int nbPose,
                                          vpCalibration table_cal[],
                                          vpHomogeneousMatrix& eMc,
                                          vpHomogeneousMatrix& eMc_dist)
{
  vpHomogeneousMatrix* table_cMo = new vpHomogeneousMatrix[nbPose];
  vpHomogeneousMatrix* table_cMo_dist = new vpHomogeneousMatrix[nbPose];
  vpHomogeneousMatrix* table_rMe = new vpHomogeneousMatrix[nbPose];
  try{
    if (nbPose > 2){
      for(unsigned int i=0;i<nbPose;i++){
        table_cMo[i] = table_cal[i].cMo;
        table_cMo_dist[i] = table_cal[i].cMo_dist;
        table_rMe[i] = table_cal[i].rMe;
      }
      calibrationTsai(nbPose,table_cMo,table_rMe,eMc);
      calibrationTsai(nbPose,table_cMo_dist,table_rMe,eMc_dist);
      delete [] table_cMo;
      delete [] table_cMo_dist;
      delete [] table_rMe;
      return 0;
    }
    else{
      vpERROR_TRACE("Three images are needed to compute Tsai calibration !\n");
      delete [] table_cMo;
      delete [] table_cMo_dist;
      delete [] table_rMe;
      return -1;
    }
  }
  catch(...){
    delete [] table_cMo;
    delete [] table_cMo_dist;
    delete [] table_rMe;
    throw;
  }
}


/*!
  Write data into a file.

  data are organized as follow oX oY oZ u v

  \param filename : name of the file
*/
int vpCalibration::writeData(const char *filename)
{
  std::ofstream f(filename) ;
  vpImagePoint ip;

  std::list<double>::const_iterator it_LoX = LoX.begin();
  std::list<double>::const_iterator it_LoY = LoY.begin();
  std::list<double>::const_iterator it_LoZ = LoZ.begin();
  std::list<vpImagePoint>::const_iterator it_Lip = Lip.begin();

  f.precision(10);
  f.setf(std::ios::fixed,std::ios::floatfield);
  f << LoX.size() << std::endl ;

  for (unsigned int i =0 ; i < LoX.size() ; i++)
  {

    double oX = *it_LoX;
    double oY = *it_LoY;
    double oZ = *it_LoZ;

    ip = *it_Lip;
    double u = ip.get_u() ;
    double v = ip.get_v() ;

    f << oX <<" " << oY << " " << oZ << " " << u << "  " << v <<std::endl ;

    ++it_LoX;
    ++it_LoY;
    ++it_LoZ;
    ++it_Lip;

  }

  f.close() ;
  return 0 ;
}

/*!
  Read data from disk :
  data are organized as follow oX oY oZ u v

  \param filename : name of the file
*/
int vpCalibration::readData(const char* filename)
{
  vpImagePoint ip;
  std::ifstream f;
  f.open(filename);
  if (f != NULL){
    int n ;
    f >> n ;
    std::cout << "There are "<< n <<" point on the calibration grid " << std::endl ;

    clearPoint() ;
    for (int i=0 ; i < n ; i++)
    {
      double x, y, z, u, v ;
      f >> x >> y >> z >> u >> v ;
      std::cout << x <<" "<<y <<" "<<z<<" "<<u<<" "<<v <<std::endl ;
      ip.set_u( u );
      ip.set_v( v );
      addPoint(x, y, z, ip) ;
    }

    f.close() ;
    return 0 ;
  }
  else{
    return -1;
  }
}
/*!
  Read calibration grid coordinates from disk :
  data are organized as follow oX oY oZ

  \param filename : name of the file
  \param n : number of points in the calibration grid
  \param oX : List of oX coordinates
  \param oY : List of oY coordinates
  \param oZ : List of oZ coordinates

  \param verbose : Additionnal printings if true (number of points on
  the calibration grid and their respective coordinates in the object
  frame).

  \return 0 if success, -1 if an error occurs.
*/
int vpCalibration::readGrid(const char* filename, unsigned int &n,
                            std::list<double> &oX, std::list<double> &oY, std::list<double> &oZ, bool verbose)
{
  try{
    std::ifstream f;
    f.open(filename);
    if (f != NULL){

      f >> n ;
      if(verbose)   
        std::cout << "There are "<< n <<" points on the calibration grid " << std::endl ;
      int no_pt;
      double x,y,z;

      // clear the list
      oX.clear();
      oY.clear();
      oZ.clear();

      for (unsigned int i=0 ; i < n ; i++)
      {
        f >> no_pt >> x >> y >> z ;
        if(verbose){    
          std::cout << no_pt <<std::endl ;
          std::cout << x <<"  "<< y <<"  "<< z <<std::endl ;
        }     
        oX.push_back(x) ;
        oY.push_back(y) ;
        oZ.push_back(z) ;
      }

      f.close() ;
    }
    else{
      return -1;
    }
  }
  catch(...){return -1;}
  return 0 ;
}

/*!
  Display the data of the calibration (center of the tracked dots)
  \param I : Image where to display data.
  \param color : Color of the data.
  \param thickness : Thickness of the displayed data.

*/
int vpCalibration::displayData(vpImage<unsigned char> &I, vpColor color,
                               unsigned int thickness)
{

  for (std::list<vpImagePoint>::const_iterator it = Lip.begin(); it != Lip.end(); ++ it) {
    vpDisplay::displayCross(I, *it, 12, color, thickness) ;
  }
  return 0 ;
}

/*!
  Display estimated centers of dots using intrinsic camera parameters
  with model with distortion and the computed pose.
  \param I : Image where to display grid data.
  \param color : Color of the data.
  \param thickness : Thickness of the displayed data.
*/
int vpCalibration::displayGrid(vpImage<unsigned char> &I, vpColor color,
                               unsigned int thickness)
{
  double u0_dist = cam_dist.get_u0() ;
  double v0_dist = cam_dist.get_v0() ;
  double px_dist = cam_dist.get_px() ;
  double py_dist = cam_dist.get_py() ;
  double kud_dist = cam_dist.get_kud() ;
  //  double kdu_dist = cam_dist.get_kdu() ;

  //   double u0 = cam.get_u0() ;
  //   double v0 = cam.get_v0() ;
  //   double px = cam.get_px() ;
  //   double py = cam.get_py() ;

  std::list<double>::const_iterator it_LoX = LoX.begin();
  std::list<double>::const_iterator it_LoY = LoY.begin();
  std::list<double>::const_iterator it_LoZ = LoZ.begin();

  for (unsigned int i =0 ; i < npt ; i++)
  {

    double oX = *it_LoX;
    double oY = *it_LoY;
    double oZ = *it_LoZ;

    double cX = oX*cMo[0][0]+oY*cMo[0][1]+oZ*cMo[0][2] + cMo[0][3];
    double cY = oX*cMo[1][0]+oY*cMo[1][1]+oZ*cMo[1][2] + cMo[1][3];
    double cZ = oX*cMo[2][0]+oY*cMo[2][1]+oZ*cMo[2][2] + cMo[2][3];

    double x = cX/cZ ;
    double y = cY/cZ ;

    //     double xp = u0 + x*px ;
    //     double yp = v0 + y*py ;

    //     vpDisplay::displayCross(I,(int)vpMath::round(yp), (int)vpMath::round(xp),
    // 			    5,col) ;

    cX = oX*cMo_dist[0][0]+oY*cMo_dist[0][1]+oZ*cMo_dist[0][2]+cMo_dist[0][3];
    cY = oX*cMo_dist[1][0]+oY*cMo_dist[1][1]+oZ*cMo_dist[1][2]+cMo_dist[1][3];
    cZ = oX*cMo_dist[2][0]+oY*cMo_dist[2][1]+oZ*cMo_dist[2][2]+cMo_dist[2][3];

    x = cX/cZ ;
    y = cY/cZ ;

    double r2 = 1+kud_dist*(vpMath::sqr(x)+vpMath::sqr(y)) ;

    vpImagePoint ip;
    ip.set_u( u0_dist + x*px_dist*r2 );
    ip.set_v( v0_dist + y*py_dist*r2 );

    vpDisplay::displayCross(I, ip, 6, color, thickness) ;
    ///////////////////////////////////////

    //    std::cout << oX << "  " << oY <<  "  " <<oZ << std::endl ;
    //    I.getClick() ;
    ++it_LoX;
    ++it_LoY;
    ++it_LoZ;
  }
  return 0;
}

#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
/*!
  \deprecated This method is deprecated. You should use
  vpCalibration::readGrid(const char*,unsigned int &, vpList<double> &, vpList<double> &, vpList<double> &, bool) instead.

  Read calibration grid coordinates from disk :
  data are organized as follow oX oY oZ

  \param filename : name of the file
  \param n : number of points in the calibration grid
  \param oX : List of oX coordinates
  \param oY : List of oY coordinates
  \param oZ : List of oZ coordinates

  \param verbose : Additionnal printings if true (number of points on
  the calibration grid and their respective coordinates in the object
  frame).

  \return 0 : if success
*/
int vpCalibration::readGrid(const char* filename,unsigned int &n,
                            vpList<double> &oX,vpList<double> &oY,vpList<double> &oZ, bool verbose)
{
  try{
    std::ifstream f;
    f.open(filename);
    if (f != NULL){

      f >> n ;
      if(verbose)
        std::cout << "There are "<< n <<" points on the calibration grid " << std::endl ;
      int no_pt;
      double x,y,z;

      oX.front();
      oY.front();
      oZ.front();
      for (unsigned int i=0 ; i < n ; i++)
      {
        f >> no_pt >> x >> y >> z ;
        if(verbose){
          std::cout << no_pt <<std::endl ;
          std::cout << x <<"  "<< y <<"  "<< z <<std::endl ;
        }
        oX.addRight(x) ;
        oY.addRight(y) ;
        oZ.addRight(z) ;
      }

      f.close() ;
    }
    else{
      return -1;
    }
  }
  catch(...){return -1;}
  return 0 ;
}

#endif // VISP_BUILD_DEPRECATED_FUNCTIONS
