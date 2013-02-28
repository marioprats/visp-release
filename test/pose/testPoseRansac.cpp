/****************************************************************************
 *
 * $Id: testPoseRansac.cpp 3780 2012-06-07 08:56:03Z fspindle $
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
 * Compute the pose of a 3D object using the Dementhon method. Assuming that
 * the correspondance between 2D points and 3D points is not done, we use
 * the RANSAC algorithm to achieve this task
 *
 * Authors:
 * Aurelien Yol
 *
 *****************************************************************************/

#include <visp/vpPose.h>
#include <visp/vpPoint.h>
#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>

#include <stdlib.h>
#include <stdio.h>

#define L 0.1


/*!
  \example testPoseRansac.cpp

  Compute the pose of a 3D object using the Ransac method.

*/

int
main()
{
    std::cout << "Pose computation with matched points" << std::endl;
    int size = 8;
    vpPoint *P = new vpPoint [size]  ;  //  Point to be tracked

    P[0].setWorldCoordinates(-L,-L, 0 ) ;
    P[1].setWorldCoordinates(L,-L, 0 ) ;
    P[2].setWorldCoordinates(L,L, 0 ) ;
    P[3].setWorldCoordinates(-L,L, 0 ) ;
    
    double L2 = L*2.0;
    P[4].setWorldCoordinates(-L2,-L2, 0 ) ;
    P[5].setWorldCoordinates(L2,-L2, 0 ) ;
    P[6].setWorldCoordinates(L2,L2, 0 ) ;
    P[7].setWorldCoordinates(-L2,L2, 0 ) ;
    
    //P[4].setWorldCoordinates(-0,0, L ) ; //ERREUR DANS LAGRANGE ET DEMENTHON

    vpHomogeneousMatrix cMo_ref(0,0.2,1,0,0,0) ;
    for(int i=0 ; i < size ; i++)
    {      
      P[i].project(cMo_ref) ;
      P[i].print() ;
      std::cout << std::endl;
    }

    //Introduce an error
    double error = 0.01;
    P[3].set_y(P[3].get_y() + 2*error);
    P[6].set_x(P[6].get_x() + error);
    
    vpPose pose;
    for(int i=0 ; i < size ; i++)
      pose.addPoint(P[i]);
    
    unsigned int nbInlierToReachConsensus = (unsigned int)(75.0 * (double)size / 100.0);
    double threshold = 0.01;
    
    pose.setRansacNbInliersToReachConsensus(nbInlierToReachConsensus);
    pose.setRansacThreshold(threshold);
    
    vpHomogeneousMatrix cMo ;
    //vpPose::ransac(lp,lP, 5, 1e-6, ninliers, lPi, cMo) ;
    pose.computePose(vpPose::RANSAC, cMo);
    
    std::vector<vpPoint> inliers = pose.getRansacInliers();
    
    std::cout << "Inliers: " << std::endl;
    for (unsigned int i = 0; i < inliers.size() ; i++)
    {
      inliers[i].print() ;
      std::cout << std::endl;
    }

    std::cout << "cMo :\n" << vpPoseVector(cMo).t() << std::endl << std::endl;

    delete [] P;
}
