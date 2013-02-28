/****************************************************************************
 *
 * $Id: testPoseFeatures.cpp 3820 2012-06-27 13:13:29Z fspindle $
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
 * Compute the pose from visual features by virtual visual servoing.
 *
 * Authors:
 * Aurelien Yol
 *
 *****************************************************************************/

#include <visp/vpConfig.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpPoint.h>
#include <visp/vpDisplay.h>
#include <visp/vpDisplayX.h>
#include <visp/vpImage.h>
#include <visp/vpCameraParameters.h>
#include <visp/vpPoseFeatures.h>
#include <visp/vpPose.h>
#include <iostream>
#include <vector>
#include <visp/vpPose.h>
#include <limits> 

/*!
  \example testPoseFeatures.cpp

  Compute the pose from different features.

*/

#ifdef VISP_HAVE_CPP11_COMPATIBILITY
class vp_createPointClass{
public:
  int value;
  
  vp_createPointClass() : value(0){}
  
  int vp_createPoint(vpFeaturePoint &fp,const vpPoint &v){
    value += 1;
    vpFeatureBuilder::create(fp,v);
    return value;
  }
};

void vp_createPoint(vpFeaturePoint &fp,const vpPoint &v){
  vpFeatureBuilder::create(fp,v);
}

void vp_createLine(vpFeatureLine &fp,const vpLine &v){
  vpFeatureBuilder::create(fp,v);
}
#endif

int main()
{
  vpImage<unsigned char> I(600,600);
  
  vpHomogeneousMatrix cMo(0., 0., 1., vpMath::rad(0), vpMath::rad(0), vpMath::rad(60));
  
  std::cout << "cMo used for the projection : " << std::endl;
  std::cout << vpPoseVector(cMo).t() << std::endl;
  
  vpPoseFeatures pose;
  
  vpPoint pts[6];
    
  double val = 0.25;
  double val2 = 0.0;
  
  //2D Point Feature
  pts[0].setWorldCoordinates(0.0,-val,val2);
  pts[1].setWorldCoordinates(0.0,val,val2);
  pts[2].setWorldCoordinates(-val,val,val2);
  
  //Segment Feature
  pts[3].setWorldCoordinates(-val,-val/2.0,val2);
  pts[4].setWorldCoordinates(val,val/2.0,val2);
  
  //3D point Feature
  pts[5].setWorldCoordinates(0.0,0.0,-1.5);
  
  //Line Feature
  vpLine line;
  line.setWorldCoordinates(0.0,1.0,0.0,.0,
                           0.0,0.0,1.0,0.0);
  
  //Vanishing Point Feature
  vpLine l1;
  l1.setWorldCoordinates(0.0,1.0,0.2,0.0,
                         1.0,0.0,0.0,-0.25);
  
  vpLine l2;
  l2.setWorldCoordinates(0.0,1.0,0.2,0.0,
                           -1.0,0.0,0.0,-0.25);
  
  //Ellipse Feature
  vpCircle circle;
  circle.setWorldCoordinates(0.0, 0.0, 1.0 , 0.0, 0.0, 0.0, 0.25);
  
  pts[0].project(cMo);
  pts[1].project(cMo);
  pts[2].project(cMo);
  
  pts[3].project(cMo);
  pts[4].project(cMo);
  
  pts[5].project(cMo);
  
  line.project(cMo);
  
  l1.project(cMo);
  l2.project(cMo);
  
  circle.project(cMo);
  
  pose.addFeaturePoint(pts[0]);
//   pose.addFeaturePoint(pts[1]);
  pose.addFeaturePoint(pts[2]);
  
  pose.addFeaturePoint3D(pts[5]);
  
  pose.addFeatureVanishingPoint(l1,l2);
  
//   pose.addFeatureSegment(pts[3],pts[4]);
//   
//   pose.addFeatureLine(line);
  
  pose.addFeatureEllipse(circle);

#ifdef VISP_HAVE_CPP11_COMPATIBILITY
  vpFeaturePoint fp;
  vpFeatureLine fl;
  vpFeatureSegment fs;
  void (*ptr)(vpFeatureSegment&, vpPoint&, vpPoint&) = &vpFeatureBuilder::create;
  vp_createPointClass cpClass;
  int (vp_createPointClass::*ptrClass)(vpFeaturePoint&, const vpPoint&) = &vp_createPointClass::vp_createPoint;
  pose.addSpecificFeature(&cpClass, ptrClass, fp, pts[1]);
  pose.addSpecificFeature(&vp_createLine, fl, line);
  pose.addSpecificFeature(ptr, fs, pts[3], pts[4]);
#endif
  
  pose.setVerbose(true);
  pose.setLambda(0.6);
  pose.setVVSIterMax(200);
  pose.setCovarianceComputation(true);
  
  vpHomogeneousMatrix cMo2(0.4, 0.3, 1.5, vpMath::rad(0), vpMath::rad(0), vpMath::rad(0));
  std::cout << "cMo used as initialisation of the pose computation : " << std::endl;
  std::cout << vpPoseVector(cMo2).t() << std::endl;
  
  pose.computePose(cMo2);
//   pose.computePose(cMo2, vpPoseFeatures::ROBUST_VIRTUAL_VS);
  
  std::cout << "Resulting cMo : " << std::endl;
  std::cout << vpPoseVector(cMo2).t() << std::endl;
  
  std::cout << "Resulting covariance (Diag): " << std::endl;
  vpMatrix covariance = pose.getCovarianceMatrix();
  std::cout << covariance[0][0] << " " 
            << covariance[1][1] << " " 
            << covariance[2][2] << " "
            << covariance[3][3] << " "
            << covariance[4][4] << " "
            << covariance[5][5] << " " << std::endl;
  
  int result = 0; // Ok
  for(unsigned int i = 0 ; i < 6 ; i++){
    if(fabs(vpPoseVector(cMo2)[i] - vpPoseVector(cMo)[i]) > 1e-4){
      std::cout << "Bad pose estimation" << std::endl;
      result = -1;
      break;
    }
  }
  
  if (result == 0)
    std::cout << "Pose well estimed" << std::endl;
  
  return result;
}
