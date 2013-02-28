/****************************************************************************
 *
 * $Id: vpFeatureVanishingPoint.h 3653 2012-03-28 12:43:23Z fspindle $
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
 * 2D vanishing point visual feature (Z coordinate in 3D space is infinity)
 *
 * Authors:
 * Odile Bourquardez
 *
 *****************************************************************************/


#ifndef vpFeatureVanishingPoint_H
#define vpFeatureVanishingPoint_H



/*!
  \file vpFeatureVanishingPoint.h \brief Class that defines 2D vanishing
  point visual feature (Z coordinate in 3D space is infinity)
*/

#include <visp/vpMatrix.h>
#include <visp/vpBasicFeature.h>
#include <visp/vpPoint.h>

#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpRGBa.h>



/*!
  \class vpFeatureVanishingPoint
  \ingroup VsFeature2

  \brief Class that defines 2D vanishing point visual feature (Z
  coordinate in 3D space is infinity).
*/
class VISP_EXPORT vpFeatureVanishingPoint : public vpBasicFeature
{
public:
  typedef enum
    {
      X = 1,   // x coordinates
      Y = 2    // y coordinates
    } vpFeatureVanishingPointType;

  /*
    attributes and members directly related to the vpBasicFeature needs
    other functionalities ar useful but not mandatory
  */
  //no Z required

public:
  //! basic construction
  void init() ;
  //! basic constructor
  vpFeatureVanishingPoint() ;
  //! destructor
  virtual ~vpFeatureVanishingPoint() { if (flags != NULL) delete [] flags; }


public:
  /*
    /section Set coordinates
  */


   //! set the point x-coordinates
  void set_x(const double _x) ;
  //! get the point x-coordinates
  double get_x()  const ;
  //! set the point y-coordinates
  void set_y(const double _y) ;
  //! get the point y-coordinates
  double get_y()   const ;
  //! set the point xy coordinates
  void set_xy(const double _x, const double _y) ;
   // void buildFrom(const vpPoint &p) ;
  void buildFrom(const double _x, const double _y) ;



  public:
  /*
    vpBasicFeature method instantiation
  */
  // feature selection
  inline static unsigned int selectX()  { return FEATURE_LINE[0] ; }
  inline static unsigned int selectY()  { return FEATURE_LINE[1] ; }


    //! compute the interaction matrix from a subset a the possible features
  vpMatrix  interaction(const unsigned int select = FEATURE_ALL);
  //! compute the error between two visual features from a subset
  //! a the possible features
  vpColVector error(const vpBasicFeature &s_star,
                    const unsigned int select = FEATURE_ALL)  ;
  //! compute the error between a visual features and zero
  vpColVector error(const unsigned int select = FEATURE_ALL)  ;
  //! print the name of the feature
  void print(const unsigned int select = FEATURE_ALL ) const ;

  //! feature duplication
  vpFeatureVanishingPoint *duplicate() const ;

public:
  void display(const vpCameraParameters &cam,
               const vpImage<unsigned char> &I,
               const vpColor &color=vpColor::green,
               unsigned int thickness=1) const;
  void display(const vpCameraParameters &cam,
               const vpImage<vpRGBa> &I,
               const vpColor &color=vpColor::green,
               unsigned int thickness=1) const;

} ;



#endif
