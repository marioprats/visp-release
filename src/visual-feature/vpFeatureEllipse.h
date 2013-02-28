/****************************************************************************
 *
 * $Id: vpFeatureEllipse.h 3653 2012-03-28 12:43:23Z fspindle $
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
 * 2D ellipse visual feature.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/



#ifndef vpFeatureEllipse_H
#define vpFeatureEllipse_H

/*!
  \file vpFeatureEllipse.h
  \brief Class that defines 2D ellipse visual feature
*/

#include <visp/vpMatrix.h>
#include <visp/vpBasicFeature.h>

#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpRGBa.h>


/*!
  \class vpFeatureEllipse
  \ingroup VsFeature2
  \brief Class that defines 2D ellipse visual feature.
*/
class VISP_EXPORT vpFeatureEllipse : public vpBasicFeature
{
  /*
    attributes and members directly related to the vpBasicFeature needs
    other functionalities are useful but not mandatory
  */
private:
  //! FeatureEllipse depth (required to compute the interaction matrix)
  //! default Z = 1m
  double A,B,C ;


public:
  //! basic construction
  void init() ;
  //! basic constructor
  vpFeatureEllipse() ;
  //! destructor
  virtual ~vpFeatureEllipse() { if (flags != NULL) delete [] flags; }

public:
  /*!
    \section Set coordinates
  */
 //! basic constructor
  vpFeatureEllipse(const double x, const double y,
		   const double mu20, const double mu11, const double mu02);

  // void buildFrom(const vpEllipse &p) ;
  void buildFrom(const double x, const double y,
		 const double mu20, const double mu11, const double mu02) ;
  void buildFrom(const double x, const double y,
		 const double mu20, const double mu11, const double mu02,
		 const double A, const double B, const double C) ;
  void set_x(const double x);
  void set_y(const double y);
  void set_xy(const double x,const double y);
  void setABC(const double A, const double B, const double C) ;
  void setMu(const double mu20, const double mu11, const double mu02) ;
  
  double get_x() const {return s[0];}
  double get_y() const {return s[1];}
  double getMu20() const {return s[2];}
  double getMu11() const {return s[3];}
  double getMu02() const {return s[4];}

public:
  /*!
    vpBasicFeature method instantiation
  */

  // feature selection
  inline static unsigned int selectX()  { return FEATURE_LINE[0] ; }
  inline static unsigned int selectY()  { return FEATURE_LINE[1] ; }
  inline static unsigned int selectMu20()  { return FEATURE_LINE[2] ; }
  inline static unsigned int selectMu11()  { return FEATURE_LINE[3] ; }
  inline static unsigned int selectMu02()  { return FEATURE_LINE[4] ; }

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
  vpFeatureEllipse *duplicate() const ;


public:
  void display(const vpCameraParameters &cam,
               const vpImage<unsigned char> &I,
               const vpColor &color=vpColor::green,
               unsigned int thickness=1) const ;
  void display(const vpCameraParameters &cam,
               const vpImage<vpRGBa> &I,
               const vpColor &color=vpColor::green,
               unsigned int thickness=1) const ;
} ;



#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
