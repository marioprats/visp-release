/****************************************************************************
 *
 * $Id: vpMeNurbs.h 3587 2012-03-02 16:26:32Z fspindle $
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
 * Moving edges.
 *
 * Authors:
 * Nicolas Melchior
 *
 *****************************************************************************/

/*!
  \file vpMeNurbs.h
  \brief Moving edges on a form reprsented by a NURBS (Non Uniform Rational B-Spline)
*/

#ifndef vpMeNurbs_HH
#define vpMeNurbs_HH

#include <visp/vpMatrix.h>
#include <visp/vpMath.h>
#include <visp/vpMeTracker.h>
#include <visp/vpNurbs.h>

#include <math.h>
#include <iostream>
#include <list>

/*!
  \class vpMeNurbs

  \ingroup TrackingImageME

  \brief Class that tracks in an image a edge defined by a Nurbs.

  The advantage of this class is that it enables to track an edge whose equation is
  not known in advance. At each iteration, the Nurbs corresponding to the edge is computed.
  
  It is possible to have a direct access to the nurbs. It is indeed a public parameter.

  The code below shows how to use this class.
\code
#include <visp/vpImage.h>
#include <visp/vpMeNurbs.h>
#include <visp/vpImagePoint.h>

int main()
{
  vpImage<unsigned char> I(240, 320);

  // Fill the image with a black rectangle
  I = 0;
  for (int i = 100; i < 180; i ++) {
    for (int j = 0; j < 320; j ++) {
      I[i][j] = 255;
    }
  }

  // Set the moving-edges tracker parameters
  vpMe me;
  me.setRange(25);
  me.setPointsToTrack(20);
  me.setThreshold(15000);
  me.setSampleStep(10);

  // Initialize the moving-edges tracker parameters
  vpMeNurbs meNurbs;
  meNurbs.setNbControlPoints(4);
  meNurbs.setMe(&me);

  // Initialize the location of the edge to track (here a horizontal line
  std::list<vpImagePoint> ipList; //List of points belonginig to the edge
  ipList.push_back(vpImagePoint(110,119));
  ipList.push_back(vpImagePoint(140,119));
  ipList.push_back(vpImagePoint(160,119));
  ipList.push_back(vpImagePoint(170,119));

  meNurbs.initTracking(I, ipList);

  while ( 1 )
  {
    // ... Here the code to read or grab the next image.

    // Track the line.
    meNurbs.track(I);
  }
  return 0;
}
\endcode

  \note It is possible to display the nurbs as an overlay. For that you 
  must use the display function of the class vpMeNurbs.
  
  \note In case of an edge which is not smooth, it can be interesting to use the
  canny detection to find the extremities. In this case, use the method
  setEnableCannyDetection to enable it. Warning : This function requires OpenCV.
*/

class VISP_EXPORT vpMeNurbs : public vpMeTracker
{
  #ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
	public:
	#else
	private:
	#endif
    //! The Nurbs which represents the tracked edge.
    vpNurbs nurbs;
    
	private:
    //! Size of the nurbs
    double dist;
    //! Number of control points used to compute the Nurbs.
    unsigned int nbControlPoints;
    //! counter used to trigger the canny edge detection at the beginning of the Nurbs.
    int beginPtFound;
    //! counter used to trigger the canny edge detection at the end of the Nurbs.
    int endPtFound;
    //! True if the canny detection has to be used during the extremities search.
    bool enableCannyDetection;
    //! First canny threshold
    double cannyTh1;
    //! Second canny threshold
    double cannyTh2;

  public:
    vpMeNurbs();
    vpMeNurbs(const vpMeNurbs &menurbs);
    virtual ~vpMeNurbs();
    
    /*!
      Sets the number of control points used to compute the Nurbs.

      \param nbControlPoints : The number of control points used to compute the Nurbs.
    */
    void setNbControlPoints(const unsigned int nbControlPoints) {this->nbControlPoints = nbControlPoints;}
    
    /*!
      Enables or disables the canny detection used during the extremities search.

      \param enableCannyDetection : if true it enables the canny detection.
    */
    void setEnableCannyDetection(const bool enableCannyDetection) {this->enableCannyDetection =enableCannyDetection;}
    
    /*!
      Enables to set the two thresholds use by the canny detection.
      
      \param th1 : The first threshold;
      \param th2 : The second threshold;
    */
    void setCannyThreshold(const double th1, const double th2)
    {
      this->cannyTh1 = th1;
      this->cannyTh2 = th2;
    }
    
    void initTracking(const vpImage<unsigned char> &I) ;
    void initTracking(const vpImage<unsigned char> &I,
                      const std::list<vpImagePoint> &ptList) ;

    void track(const vpImage<unsigned char>& Im);

    void sample(const vpImage<unsigned char>&image);
    void reSample(const vpImage<unsigned char> &I) ;
    void updateDelta();
    void setExtremities() ;
    void seekExtremities(const vpImage<unsigned char> &I) ;
    void seekExtremitiesCanny(const vpImage<unsigned char> &I) ;
    void suppressPoints() ;

    void supressNearPoints();
    void localReSample(const vpImage<unsigned char> &I);
    
    /*!
      Gets the nurbs;
    */
    inline vpNurbs getNurbs() const {return nurbs; }
    
    void display(const vpImage<unsigned char>& I, vpColor col) ;
    
#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
    /*!
      @name Deprecated functions
    */
    vp_deprecated void initTracking(const vpImage<unsigned char> &I,
                                    vpList<vpImagePoint> &ptList) ;
#endif

  private:
    bool computeFreemanChainElement(const vpImage<unsigned char> &I,
				   vpImagePoint &iP,
				   unsigned int &element);

    bool hasGoodLevel(const vpImage<unsigned char>& I,
			  const vpImagePoint& iP) const;

    bool isInImage(const vpImage<unsigned char>& I, const vpImagePoint &iP) const;
    
    void computeFreemanParameters( unsigned int element, vpImagePoint &diP);
    
    bool farFromImageEdge(const vpImage<unsigned char>& I, const vpImagePoint& iP);
    
public:
  static void display(const vpImage<unsigned char>& I, vpNurbs &n, vpColor color = vpColor::green);
};

#endif
