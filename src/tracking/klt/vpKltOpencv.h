/****************************************************************************
 *
 * $Id: vpKltOpencv.h 3797 2012-06-21 07:44:05Z fspindle $
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
 * Wrapper for the KLT (Kanade-Lucas-Tomasi) feature tracker implemented
 * with opencv.
 *
 * Authors:
 * Fabien Servant
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \file vpKltOpencv.h

  \brief Wrapper for the KLT (Kanade-Lucas-Tomasi) feature tracker
  implemented with opencv.
*/

#ifndef vpKltOpencv_h
#define vpKltOpencv_h

#include <visp/vpConfig.h>

#ifdef VISP_HAVE_OPENCV

#ifdef _CH_
#pragma package <opencv>
#endif

#if (VISP_HAVE_OPENCV_VERSION >= 0x020101) // Require opencv >= 2.1.1
#  include <opencv2/imgproc/imgproc.hpp>
#  include <opencv2/video/tracking.hpp>
#  include <opencv2/legacy/legacy.hpp>
#  include <opencv2/highgui/highgui.hpp>
#else
#  ifndef _EiC
#    include <cv.h>
#    include <highgui.h>
#    include <stdio.h>
#    include <ctype.h>
#    endif
#endif

#include <visp/vpConfig.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplay.h>
#include <visp/vpDebug.h>
#include <visp/vpException.h>
#include <visp/vpTrackingException.h>

typedef int (*funccheck)(int,double,double);
typedef void (*funcinfo)(int,int,int,double,double);
typedef void (*funcevent)(int);

/*!
  \class vpKltOpencv

  \ingroup TrackingImagePoint

  \brief Wrapper for the KLT (Kanade-Lucas-Tomasi) feature tracker
  implemented with OpenCV.

  The following example shows how to use the main functions of the class:

  \code
#include <visp/vpConfig.h>
#include <visp/vpImage.h>
#include <visp/vpDisplay.h>
#include <visp/vpKltOpencv.h>
#include <visp/vpImageConvert.h>

int main()
{
#if VISP_HAVE_OPENCV_VERSION >= 0x010100 // KLT only available since OpenCV-1.1.0
  vpImage<unsigned char> I;
  IplImage* Icv = NULL;
  vpKltOpencv klt;

  //First grab the initial image I

  //Convert the image I to the IplImage format.
  vpImageConvert::convert(I, Icv);

  //Initialise the tracking on the whole image.
  klt.initTracking(Icv, NULL);

  while(true)
  {
    // Grab a new image and convert it to the OpenCV format.
    vpImageConvert::convert(I, Icv);

    // Track the features on the current image.
    klt.track(Icv);

    // Display the features tracked at the current iteration.
    klt.display(I);
  }

  cvReleaseImage(&Icv);
#else
  std::cout << "vpKltOpencv requires ViSP with OpenCV." << std::endl;
#endif
  return(0);
}
  \endcode

*/
class VISP_EXPORT vpKltOpencv
{
 private:
  int initialized; //Is the tracker ready ?

  int maxFeatures; //Maximum number of features to track (Default 50)
  int countFeatures; //Currently tracked features
  int countPrevFeatures; //Previously tracked features
  int globalcountFeatures; //Keep over time for ID

  int flags; //Flags for tracking (internal)

  int win_size; //Size of search window for tracker (default 10)
  double quality; //Multiplier for the maxmin eigenvalue; specifies minimal accepted quality of image corners (default 0.01)
  double min_distance; //Limit, specifying minimum possible distance between returned corners; Euclidian distance is used. (default 10)
  double harris_free_parameter; //Harris detector free parameter. (default 0.04)
  int block_size; //Size of the averaging block used by the corner detector (default 3)
  int use_harris; //0 use a simple Minimum EigenValue Detector, != 0  use Harris (default 1)
  int pyramid_level; //Number of level for the tracker's gaussian pyramid data (default 3)
  int _tid; //tracker id for multiple trackers

  IplImage *image; //Image buffer
  IplImage *prev_image; //Image buffer for the previous iteration
  IplImage *pyramid; //Gaussian pyramid data
  IplImage *prev_pyramid; //Gaussian pyramid data for the previous iteration
  IplImage *swap_temp; //Internal

  CvPoint2D32f *features; //List of features
  CvPoint2D32f *prev_features; //List of features for previous iteration
  
  long *featuresid; //Array of ids for features
  long *prev_featuresid; //Array of ids for previous features
  bool *lostDuringTrack; // Result of the tracker for every feature : 1 = lost, 0 = still present
  char *status; //Result of the tracker for every features : 0 = lost, 1 = found



  //EVENT FUNCTION POINTERS
  funcevent OnInitialize;
  funcinfo OnFeatureLost;
  funcinfo OnNewFeature;
  funcinfo OnMeasureFeature;
  funccheck IsFeatureValid;

 private:

  //Internal
  void clean();
  void cleanAll();
  void reset();

 public:
  vpKltOpencv();
  vpKltOpencv(const vpKltOpencv& copy);
  virtual ~vpKltOpencv();

  //Detect corners in the image. Initialize the tracker
  void initTracking(const IplImage *I, const IplImage *mask = NULL);

  //Track !
  void track(const IplImage *I);

  //Draw the tracked features on the given image
  void display(const vpImage<unsigned char> &I,
	       vpColor color = vpColor::red);

  //Seters
  
  /* Should be used only before initTracking */
  void setMaxFeatures(const int input);

  /*!
    Set the window size for the sub-pixel computation.

    \warning The tracker must be re-initialised using the method initTracking().

    \param input : The new number of maximum features.
  */
  void setWindowSize(const int input) {initialized = 0; win_size=input;}
  void setQuality(double input) {initialized = 0; quality=input;}

  /*!
    Set the minimal distance between two points during the initialisation.

    \warning The tracker must be re-initialised using the method initTracking().

    \param input : The new minimal distance between two points.
  */
  void setMinDistance(double input) {initialized = 0; min_distance=input;}

  /*!
    Set the Harris parameter (The \e k value).

    \warning The tracker must be re-initialised using the method initTracking().

    \param input : The new Harris parameter.
  */
  void setHarrisFreeParameter(double input) {initialized = 0; harris_free_parameter=input;}

  /*!
    Set the size of the averaging block used to track the features. 
    
    \warning The input is a signed integer to be compatible with OpenCV. However, 
    it must be a positive integer.
    
    \param input : The new size of the block.
  */
  void setBlockSize(const int input) {initialized = 0; block_size=input;}
  void setUseHarris(const int input) {initialized = 0; use_harris=input;}

  /*!
    Set the maximal pyramid level. If the level is zero, then no pyramid is
    computed for the optical flow.

    \warning The tracker must be re-initialised using the method initTracking().

    \param input : The new maximal pyramid level.
  */
  void setPyramidLevels(const int input) {initialized = 0; pyramid_level=input;}
  void setTrackerId(int tid) {_tid = tid;}

  //Functors

  //Event when tracker is initialized -> event(id_tracker)
  void setOnInitialize(funcevent input) {OnInitialize = input;}
  //Event when a feature is lost -> event(id_tracker, index, uid, x, y)
  void setOnFeatureLost(funcinfo input) {OnFeatureLost = input;}
  //Event when a new feature is found -> event(id_tracker, index, uid, x, y)
  void setOnNewFeature(funcinfo input) {OnNewFeature = input;}
  //Event when a feature is found while tracking -> event(id_tracker, index, uid, x, y)
  void setOnMeasureFeature(funcinfo input) {OnMeasureFeature = input;}
  //Is a feature valid (e.g. : test if not too close to borders) -> event(id_tracker, x, y)
  void setIsFeatureValid(funccheck input) {IsFeatureValid = input;}

  //! Get the list of lost feature
  bool *getListOfLostFeature() const { return lostDuringTrack; }
  //! Get the list of features
  CvPoint2D32f* getFeatures() const {return features;}
  //! Get the list of features id
  long* getFeaturesId() const {return featuresid;}
  //! Get the list of features
  CvPoint2D32f* getPrevFeatures() const {return prev_features;}
  //! Get the list of features id
  long* getPrevFeaturesId() const {return prev_featuresid;}
  //! Get the current number of features
  int getNbFeatures() const { return countFeatures; }
  //! Get the previous number of features
  int getNbPrevFeatures() const { return countPrevFeatures; }
  //! Get Max number of features
  int getMaxFeatures() const {return maxFeatures;}

  void getFeature(int index, int &id, float &x, float &y) const;
  void getPrevFeature(int index, int &id, float &x, float &y) const;
  void addFeature(const int &id, const float &x, const float &y);
  void suppressFeature(int index);
  
//Static Functions
public: 
  static void display(const vpImage<unsigned char>& I,const CvPoint2D32f* features_list, 
		      const int &nbFeatures, vpColor color = vpColor::green, 
		      unsigned int thickness=1);
  
  static void display(const vpImage<unsigned char>& I,const CvPoint2D32f* features_list, 
		      const long *featuresid_list, const int &nbFeatures, 
		      vpColor color = vpColor::green, unsigned int thickness=1);
  
};

#endif
#endif
