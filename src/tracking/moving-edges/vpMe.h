/****************************************************************************
 *
 * $Id: vpMe.h 3683 2012-04-16 10:17:05Z ayol $
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
 * Eric Marchand
 * Andrew Comport
 * Aurelien Yol
 *
 *****************************************************************************/

/*!
	\file vpMe.h
	\brief Moving edges
*/

// ====================================================================
/*!
  \class vpMe
  \ingroup TrackingImageME
  
  \brief Contains predetermined masks for sites and holds moving edges
  tracking parameters.
   
*/
// ====================================================================
//  Andrew Comport added functionality - replaced points_to_track with
//  sample step.

#ifndef vpMe_H
#define vpMe_H

#include <visp/vpMatrix.h>
#include <visp/vpMath.h>
#include <visp/vpImage.h>


class VISP_EXPORT vpMe
{
#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
public:
#else
private:
#endif
  double threshold ;//! Likelihood ratio threshold
  double mu1; //! Contrast continuity parameter (left boundary)
  double mu2; //! Contrast continuity parameter (right boundary)
  double min_samplestep;
  unsigned int anglestep;
  int mask_sign;
  unsigned int range; //! Seek range - on both sides of the reference pixel
  double sample_step; //! Distance between sampled points (in pixels)
  int ntotal_sample;
  int points_to_track;
  //! convolution masks' size in pixels (masks are square), \warning should not be public, use setMaskSize() and getMaskSize() instead (kept public for compatibility reasons).
  unsigned int mask_size;
  //! the number of convolution masks available for tracking ; defines resolution. \warning Should not be public, use setMaskNumber() and getMaskNumber() instead (kept public for compatibility reasons).
  unsigned int n_mask;
  //strip: defines a "security strip" such that Seek_Extremities()
  //cannot return a new extremity which is too close to the
  //frame borders which may cause Get_Sampling_Grid to refuse
  //the that extremity
  int strip;
  //int graph ;
  vpMatrix *mask ; //! Array of matrices defining the different masks (one for every angle step).

public:
  vpMe() ;
  vpMe(const vpMe &me) ;
  virtual ~vpMe() ;
  
  const vpMe& operator=(const vpMe &me);


  void initMask() ;// convolution masks - offset computation
  void checkSamplestep(double &a) { if(a < min_samplestep) a = min_samplestep ; }
  void print( ) ;
  
  /*!
    Get the matrix of the mask.

    \return the value of mask.
  */
  inline vpMatrix* getMask() const { return mask; }

  /*!
    Set the number of mask applied to determine the object contour. The number of mask determines the precision of
    the normal of the edge for every sample. If precision is 2deg, then there
    are 360/2 = 180 masks.

    \param a : the number of mask.
  */
  void setMaskNumber(const unsigned int &a) ;
  
  /*!
    Return the number of mask  applied to determine the object contour. The number of mask determines the precision of
    the normal of the edge for every sample. If precision is 2deg, then there
    are 360/2 = 180 masks.

    \return the current number of mask.
  */
  inline unsigned int getMaskNumber() const { return n_mask; }
  
  /*!
    Set the mask sign.
    
    \param a : new mask sign.
  */
  void setMaskSign(const int &a){ mask_sign = a ; }
  
  /*!
    Return the mask sign.

    \return Value of mask_sign.
  */
  inline int getMaskSign() const { return mask_sign; }
  
  /*!
    Set the mask size (in pixel) used to compute the image gradient and determine the object contour. 
    The mask size defines the size of the convolution mask used to detect an edge.
    
    \param a : new mask size.
  */
  void setMaskSize(const unsigned int &a);
  
  /*!
    Return the actual mask size (in pixel) used to compute the image gradient and determine the object contour. 
    The mask size defines the size of the convolution mask used to detect an edge.

    \return the current mask size.
  */
  inline unsigned int getMaskSize() const { return mask_size; }
  
  /*!
    Set the minimum image contrast allowed to detect a contour.
    
    \param mu1 : new mu1.
  */
  void setMu1(const double &mu1) { this->mu1 = mu1  ; }
  
  /*!
    Get the minimum image contrast allowed to detect a contour.

    \return Value of mu1.
  */
  inline double getMu1() const { return mu1; }
  
  /*!
    Set the maximum image contrast allowed to detect a contour.
    
    \param mu2 : new mu2.
  */
  void setMu2(const double &mu2) { this->mu2 = mu2  ; }
  
  /*!
    Get the maximum image contrast allowed to detect a contour.

    \return Value of mu2.
  */
  inline double getMu2() const { return mu2; }
  
  /*!
    Set how many discretizied points are used to track the feature.
    
    \param nb : new total number of sample.
  */
  void setNbTotalSample(const int &nb) { ntotal_sample = nb; }
  
  /*!
    Get how many discretizied points are used to track the feature.

    \return Value of ntotal_sample.
  */
  inline int getNbTotalSample() const { return ntotal_sample; }
  
  /*! 
    Set the number of points to track.
    
    \param n : new number of points to track.
    
    \warning This method is useful only for the vpMeNurbsTracker.  
  */
  void setPointsToTrack(const int &n) { points_to_track = n; }
  
  /*!
    Return the number of points to track.

    \return Value of points_to_track.
  */
  inline int getPointsToTrack() const { return points_to_track ; }
  
  /*!
    Set the seek range on both sides of the reference pixel.
    
    \param r : new range.
  */
  void setRange(const unsigned int &r) { range = r  ; }
  
  /*!
    Return the seek range on both sides of the reference pixel.

    \return Value of range.
  */
  inline unsigned int getRange() const { return range; }
  
  /*!
    Set the angle step.
    
    \param a : new angle step.
  */
  void setAngleStep(const unsigned int &a) { anglestep = a ; }
  
  /*!
    Return the angle step.

    \return Value of anglestep.
  */
  inline unsigned int getAngleStep() const { return anglestep; }
  
  /*!
    Set the minimum allowed sample step. Useful to specify a lower bound when the sample step is changed.
    
    \param min : new minimum sample step.
  */
  void setMinSampleStep(const double &min) { min_samplestep = min ; }
  #ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
  /*!
    \deprecated Use setMinSampleStep() instead.
  */
  vp_deprecated void setMinSamplestep(const double &min) { min_samplestep = min ; } //Little mistake in the method name "step" should be "Step" 
  #endif
  
  /*!
    Get the minimum allowed sample step. Useful to specify a lower bound when the sample step is changed.

    \return Value of min_samplestep.
  */
  inline double getMinSampleStep() const { return min_samplestep; }
  
  /*!
    Set the minimum distance in pixel between two discretized points.
    
    \param s : new sample_step.
  */
  void setSampleStep(const double &s) { sample_step = s ; }
  
  /*!
    Get the minimum distance in pixel between two discretized points.

    \return Value of sample_step.
  */
  inline double getSampleStep() const { return sample_step ; }
  
  /*!
    Set the number of pixels that are ignored around the image borders.
    
    \param a : new strip.
  */
  void setStrip(const int &a) { strip = a ; }
  
  /*!
    Get the number of pixels that are ignored around the image borders.

    \return the value of strip.
  */
  inline int getStrip() const { return strip; }
  
  /*!
    Set the likelihood threshold used to determined if the moving edge is valid or not.
    
    \param t : new threshold.
  */
  void setThreshold(const double &t) { threshold = t ; }
  
  /*!
    Return the likelihood threshold used to determined if the moving edge is valid or not.

    \return Value of threshold.
  */
  inline double getThreshold() const { return threshold; }
  
#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
public: 
  double aberration;
  double init_aberration;
  
  /*!
    \deprecated since it wasn't used.
    
    Set aberration value.
    
    \param a : new value.
  */
  vp_deprecated void setAberration(const double &a) { aberration = a ; }
  
  /*!
    \deprecated since it wasn't used.
   
    Set initial aberration value.
    
    \param a : new value.
  */
  vp_deprecated void setInitAberration(const double &a) { init_aberration = a ; }
#endif
};


#endif


