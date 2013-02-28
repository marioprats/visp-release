/****************************************************************************
 *
 * $Id: vpMbtDistanceCylinder.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Description:
 * Make the complete tracking of an object by using its CAD model. Cylinder
 * tracking.
 *
 * Authors:
 * Nicolas Melchior
 * Romain Tallonneau
 * Eric Marchand
 * Bertrand Delabarre
 *
 *****************************************************************************/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/*!
 \file vpMbtDistanceCylinder.h
 \brief Make the complete tracking of an object by using its CAD model.
*/

#ifndef vpMbtDistanceCylinder_HH
#define vpMbtDistanceCylinder_HH

#include <visp/vpPoint.h>
#include <visp/vpMbtMeLine.h>
#include <visp/vpLine.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpFeatureLine.h>
#include <visp/vpCylinder.h>
#include <visp/vpCircle.h>

class VISP_EXPORT vpMbtDistanceCylinder
{
  private :
    std::string name;
    unsigned int index;
    vpCameraParameters cam;
    vpMe *me;
    double alpha;
    double wmean1;
    double wmean2;
    vpFeatureLine featureline1 ;
    vpFeatureLine featureline2 ;
    
  public: 
    //! The moving edge containers (first line of the cylinder)
    vpMbtMeLine *meline1;
    //! The moving edge containers (second line of the cylinder)
    vpMbtMeLine *meline2;

	  //! The upper circle limiting the cylinder
	  vpCircle *cercle1;
	  //! The lower circle limiting the cylinder
	  vpCircle *cercle2;

	  //! The radius of the cylinder
	  double radius;

    //! The first extremity on the axe
    vpPoint *p1;
    //! The second extremity on the axe
    vpPoint *p2;

    //! The interaction matrix
    vpMatrix L;
    //! The error vector
    vpColVector error;
    //! The number of moving edges
    unsigned int nbFeature;
    //! The number of moving edges on line 1
    unsigned int nbFeaturel1;
    //! The number of moving edges on line 2
    unsigned int nbFeaturel2;
    //! Indicates if the line has to be reinitialized
    bool Reinit;
    //! The cylinder
    vpCylinder *c;
    
  public:
    vpMbtDistanceCylinder() ;
    ~vpMbtDistanceCylinder() ;

    /*!
     Get the camera paramters.
   
     \param cam : The vpCameraParameters used to store the camera parameters.
    */
    inline void getCameraParameters(vpCameraParameters& cam) {cam = this->cam;}
    
    /*!
     Set the camera paramters.
     \param cam : The camera parameters.
    */
    inline void setCameraParameters(const vpCameraParameters& cam) {this->cam = cam;}
    
    /*!
     Get the mean weight of the first line. The mean weight is computed thanks to the weight of each moving edge.
     Those weights are computed by the robust estimation method used during the virtual visual servoing.
   
     \return The mean weight of the first line.
    */
    inline double getMeanWeight1() const {return wmean1;}

    /*!
     Set the mean weight of the first line.

     \param wmean : The mean weight of the first line.
    */
    inline void setMeanWeight1(const double wmean) {this->wmean1 = wmean;}

    /*!
     Get the mean weight of the second line. The mean weight is computed thanks to the weight of each moving edge.
     Those weights are computed by the robust estimation method used during the virtual visual servoing.

     \return The mean weight of the second line.
    */
    inline double getMeanWeight2() const {return wmean2;}
    
    /*!
     Set the mean weight of the second line.
   
     \param wmean : The mean weight of the second line.
    */
    inline void setMeanWeight2(const double wmean) {this->wmean2 = wmean;}
    
    /*!
      Set the index of the cylinder.
      
      \param i : The index number
    */
    inline void setIndex(const unsigned int i) {index = i;}
    
    /*!
      Get the index of the cylinder.
      
      \return Return the index of the line.
    */
    inline unsigned int getIndex() {return index ;}
    
    /*!
      Get the name of the cylinder.
      
      \return Return the name of the cylinder
    */
    inline std::string getName() const {return name;}
    
    /*!
      Set the name of the cylinder.
      
      \param name : The name of the cylinder.
    */
    inline void setName(const std::string& name) {this->name = name;}
    
    /*!
      Set the name of the cylinder.
      
      \param name : The name of the cylinder
    */
    inline void setName(const char* name) {this->name = name;}

    void setMovingEdge(vpMe *Me);
    
    void buildFrom(const vpPoint &_p1, const vpPoint &_p2, const double r);
    
    void initMovingEdge(const vpImage<unsigned char> &I, const vpHomogeneousMatrix &cMo);
    void trackMovingEdge(const vpImage<unsigned char> &I, const vpHomogeneousMatrix &cMo);
    void updateMovingEdge(const vpImage<unsigned char> &I, const vpHomogeneousMatrix &cMo);

    void initInteractionMatrixError();
    void computeInteractionMatrixError(const vpHomogeneousMatrix &cMo, const vpImage<unsigned char> &I);
    void display(const vpImage<unsigned char> &I, const vpHomogeneousMatrix &cMo, const vpCameraParameters &cam, const vpColor col, const unsigned int thickness = 1);
    void display(const vpImage<vpRGBa> &I, const vpHomogeneousMatrix &cMo, const vpCameraParameters &cam, const vpColor col, const unsigned int thickness = 1);
    void displayMovingEdges(const vpImage<unsigned char> &I);

    void reinitMovingEdge(const vpImage<unsigned char> &I, const vpHomogeneousMatrix &cMo);

  private:
    void project(const vpHomogeneousMatrix &cMo);

	  void getCylinderLineExtremity(double &i, double &j,double rho, double theta, vpCircle *circle);

} ;

#endif
#endif

