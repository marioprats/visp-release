/****************************************************************************
 *
 * $Id: vpFeatureEllipse.cpp 3653 2012-03-28 12:43:23Z fspindle $
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


/*!
  \file vpFeatureEllipse.cpp
  \brief Class that defines 2D ellipse visual feature
*/


#include <visp/vpBasicFeature.h>
#include <visp/vpFeatureEllipse.h>

// Exception
#include <visp/vpException.h>
#include <visp/vpMatrixException.h>
#include <visp/vpFeatureException.h>

// Debug trace
#include <visp/vpDebug.h>

// math
#include <visp/vpMath.h>



#include <visp/vpFeatureDisplay.h>


/*



attributes and members directly related to the vpBasicFeature needs
other functionalities ar useful but not mandatory





*/

void
vpFeatureEllipse::init()
{
    //feature dimension
    dim_s = 5 ;
    nbParameters = 8;

    // memory allocation
    s.resize(dim_s) ;
    if (flags == NULL)
      flags = new bool[nbParameters];
    for (unsigned int i = 0; i < nbParameters; i++) flags[i] = false;

    //default depth values
    A = B = 0;
    C =1 ;

}

vpFeatureEllipse::vpFeatureEllipse() : vpBasicFeature()
{
    init() ;
}



//! compute the interaction matrix from a subset a the possible features
vpMatrix
vpFeatureEllipse::interaction(const unsigned int select)
{
  vpMatrix L ;

  L.resize(0,6) ;

  if (deallocate == vpBasicFeature::user)
  {
    for (unsigned int i = 0; i < nbParameters; i++)
    {
      if (flags[i] == false)
      {
        switch(i){
        case 0:
          vpTRACE("Warning !!!  The interaction matrix is computed but x was not set yet");
        break;
        case 1:
          vpTRACE("Warning !!!  The interaction matrix is computed but y was not set yet");
        break;
        case 2:
          vpTRACE("Warning !!!  The interaction matrix is computed but mu20 was not set yet");
        break;
        case 3:
          vpTRACE("Warning !!!  The interaction matrix is computed but mu11 was not set yet");
        break;
        case 4:
          vpTRACE("Warning !!!  The interaction matrix is computed but mu02 was not set yet");
        break;
        case 5:
          vpTRACE("Warning !!!  The interaction matrix is computed but A was not set yet");
        break;
        case 6:
          vpTRACE("Warning !!!  The interaction matrix is computed but B was not set yet");
        break;
        case 7:
          vpTRACE("Warning !!!  The interaction matrix is computed but C was not set yet");
        break;
        default:
          vpTRACE("Problem during the reading of the variable flags");
        }
      }
    }
    resetFlags();
  }

  double xc = s[0] ;
  double yc = s[1] ;
  double mu20 = s[2] ;
  double mu11 = s[3] ;
  double mu02 = s[4] ;

  //eq 39
  double Z = 1/(A*xc + B*yc + C) ;



  if (vpFeatureEllipse::selectX() & select )
  {
    vpMatrix H(1,6) ; H = 0;


    H[0][0] = -1/Z;
    H[0][1] = 0 ;
    H[0][2] = xc/Z + A*mu20 + B*mu11;
    H[0][3] = xc*yc + mu11;
    H[0][4] = -1-vpMath::sqr(xc)-mu20;
    H[0][5] = yc;


    L = vpMatrix::stackMatrices(L,H) ;
  }

  if (vpFeatureEllipse::selectY() & select )
  {
    vpMatrix H(1,6) ; H = 0;


    H[0][0] = 0 ;
    H[0][1] = -1/Z;
    H[0][2] = yc/Z + A*mu11 + B*mu02;
    H[0][3] = 1+vpMath::sqr(yc)+mu02;
    H[0][4] = -xc*yc - mu11;
    H[0][5] = -xc;

    L = vpMatrix::stackMatrices(L,H) ;
  }

  if (vpFeatureEllipse::selectMu20() & select )
  {
    vpMatrix H(1,6) ; H = 0;

    H[0][0] = -2*(A*mu20+B*mu11);
    H[0][1] = 0 ;
    H[0][2] = 2*((1/Z+A*xc)*mu20+B*xc*mu11) ;
    H[0][3] = 2*(yc*mu20+xc*mu11);
    H[0][4] = -4*mu20*xc;
    H[0][5] = 2*mu11;

    L = vpMatrix::stackMatrices(L,H) ;
  }

  if (vpFeatureEllipse::selectMu11() & select )
  {
    vpMatrix H(1,6) ; H = 0;

    H[0][0] = -A*mu11-B*mu02;
    H[0][1] = -A*mu20-B*mu11;
    H[0][2] = A*yc*mu20+(3/Z-C)*mu11+B*xc*mu02;
    H[0][3] = 3*yc*mu11+xc*mu02;
    H[0][4] = -yc*mu20-3*xc*mu11;
    H[0][5] = mu02-mu20;

    L = vpMatrix::stackMatrices(L,H) ;
  }

  if (vpFeatureEllipse::selectMu02() & select )
  {
    vpMatrix H(1,6) ; H = 0;

    H[0][0] = 0 ;
    H[0][1] = -2*(A*mu11+B*mu02);
    H[0][2] = 2*((1/Z+B*yc)*mu02+A*yc*mu11);
    H[0][3] = 4*yc*mu02;
    H[0][4] = -2*(yc*mu11 +xc*mu02) ;
    H[0][5] = -2*mu11 ;
    L = vpMatrix::stackMatrices(L,H) ;
  }


  return L ;
}

//! compute the error between two visual features from a subset
//! a the possible features
vpColVector
vpFeatureEllipse::error(const vpBasicFeature &s_star,
			const unsigned int select)
{
  vpColVector e(0) ;

  try{
    if (vpFeatureEllipse::selectX() & select )
    {
      vpColVector ex(1) ;
      ex[0] = s[0] - s_star[0] ;

      e = vpMatrix::stackMatrices(e,ex) ;
    }

    if (vpFeatureEllipse::selectY() & select )
    {
      vpColVector ey(1) ;
      ey[0] = s[1] - s_star[1] ;
      e =  vpMatrix::stackMatrices(e,ey) ;
    }

     if (vpFeatureEllipse::selectMu20() & select )
    {
      vpColVector ex(1) ;
      ex[0] = s[2] - s_star[2] ;

      e = vpMatrix::stackMatrices(e,ex) ;
    }

    if (vpFeatureEllipse::selectMu11() & select )
    {
      vpColVector ey(1) ;
      ey[0] = s[3] - s_star[3] ;
      e =  vpMatrix::stackMatrices(e,ey) ;
    }

    if (vpFeatureEllipse::selectMu02() & select )
    {
      vpColVector ey(1) ;
      ey[0] = s[4] - s_star[4] ;
      e =  vpMatrix::stackMatrices(e,ey) ;
    }

  }
  catch(vpMatrixException me)
  {
    vpERROR_TRACE("caught a Matrix related error") ;
    std::cout <<std::endl << me << std::endl ;
    throw(me) ;
  }
  catch(vpException me)
  {
    vpERROR_TRACE("caught another error") ;
    std::cout <<std::endl << me << std::endl ;
    throw(me) ;
  }


  return e ;

}


void
vpFeatureEllipse::print(const unsigned int select ) const
{

  std::cout <<"Ellipse:  " << std::endl ;
  if (vpFeatureEllipse::selectX() & select )
    std::cout << " x=" << s[0] <<std::endl ;;
  if (vpFeatureEllipse::selectY() & select )
    std::cout << " y=" << s[1] <<std::endl ;
  if (vpFeatureEllipse::selectMu20() & select )
    std::cout << " mu20=" << s[2] <<std::endl ;
  if (vpFeatureEllipse::selectMu11() & select )
    std::cout << " mu11=" << s[3] <<std::endl ;
  if (vpFeatureEllipse::selectMu02() & select )
    std::cout << " mu02=" << s[4] <<std::endl ;
  std::cout << "A = "<<A <<" B = "<<B <<" C = "<<C << std::endl ;
}


void
vpFeatureEllipse::buildFrom(const double x, const double y,
			    const double mu20, const double mu11,
			    const double mu02)
{

  s[0] = x ;
  s[1] = y ;
  s[2] = mu20 ;
  s[3] = mu11 ;
  s[4] = mu02 ;

  for( int i = 0; i < 5; i++) flags[i] = true;

}

void
vpFeatureEllipse::buildFrom(const double x, const double y,
			    const double mu20, const double mu11,
			    const double mu02,
			    const double A, const double B, const double C)
{

  s[0] = x ;
  s[1] = y ;
  s[2] = mu20 ;
  s[3] = mu11 ;
  s[4] = mu02 ;

  this->A = A ;
  this->B = B ;
  this->C = C ;

  for( unsigned int i = 0; i < nbParameters; i++) flags[i] = true;
}

void
vpFeatureEllipse::set_x(const double x)
{
  s[0] = x ;
  flags[0] = true;
}

void
vpFeatureEllipse::set_y(const double y)
{
  s[1] = y ;
  flags[1] = true;
}

void
vpFeatureEllipse::set_xy(const double x,const double y)
{
  s[0] = x ;
  s[1] = y ;
  for( int i = 0; i < 2; i++) flags[i] = true;
}

void
vpFeatureEllipse::setABC(const double A, const double B, const double C)
{
  this->A = A ;
  this->B = B ;
  this->C = C ;
  for( unsigned int i = 5; i < nbParameters; i++) flags[i] = true;
}


void
vpFeatureEllipse::setMu(const double mu20, const double mu11,
			const double mu02)
{

  s[2] = mu20 ;
  s[3] = mu11 ;
  s[4] = mu02 ;
  for( int i = 2; i < 5; i++) flags[i] = true;

}



/*!

  Display ellipse feature.

  \param cam : Camera parameters.
  \param I : Image on which features have to be displayed.
  \param color : Color used to display the feature.
  \param thickness : Thickness of the feature representation.
*/
void
vpFeatureEllipse::display(const vpCameraParameters &cam,
                          const vpImage<unsigned char> &I,
                          const vpColor &color,
                          unsigned int thickness ) const
{
  try{
      double x = s[0] ;
      double y = s[1] ;

      double mu20 = s[2] ;
      double mu11 = s[3] ;
      double mu02 = s[4] ;

      vpFeatureDisplay::displayEllipse(x, y, mu20, mu11, mu02,
				       cam, I, color, thickness) ;

  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}

/*!

  Display ellipse feature.

  \param cam : Camera parameters.
  \param I : Color image on which features have to be displayed.
  \param color : Color used to display the feature.
  \param thickness : Thickness of the feature representation.
*/
void
vpFeatureEllipse::display(const vpCameraParameters &cam,
                          const vpImage<vpRGBa> &I,
                          const vpColor &color,
                          unsigned int thickness ) const
{
  try{
    double x = s[0] ;
    double y = s[1] ;

    double mu20 = s[2] ;
    double mu11 = s[3] ;
    double mu02 = s[4] ;

    vpFeatureDisplay::displayEllipse(x, y, mu20, mu11, mu02,
                                     cam, I, color, thickness) ;

  }
  catch(...)
  {
    vpERROR_TRACE("Error caught") ;
    throw ;
  }
}


//! for memory issue (used by the vpServo class only)
vpFeatureEllipse *vpFeatureEllipse::duplicate() const
{
  vpFeatureEllipse *feature = new vpFeatureEllipse ;
  return feature ;
}


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
