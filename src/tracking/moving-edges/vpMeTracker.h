/****************************************************************************
 *
 * $Id: vpMeTracker.h 3683 2012-04-16 10:17:05Z ayol $
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
 * Andrew Comport
 * Aurelien Yol
 *
 *****************************************************************************/

/*!
  \file vpMeTracker.h
  \brief Contains abstract elements for a Distance to Feature type feature.
*/

// ===================================================================
/*!
  \class vpMeTracker
  \ingroup TrackingImageME
  \brief Contains abstract elements for a Distance to Feature type feature.

  2D state = list of points, 3D state = feature
  
 */
// ===================================================================

#ifndef vpMeTracker_HH
#define vpMeTracker_HH

#include <visp/vpColVector.h>
#include <visp/vpMeSite.h>
#include <visp/vpMe.h>
#include <visp/vpTracker.h>

#include <math.h>
#include <iostream>
#include <list>

class VISP_EXPORT vpMeTracker : public vpTracker
{
#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
public:
#else
protected:
#endif
  //! Tracking dependent variables/functions
  //! List of tracked moving edges points.
  std::list<vpMeSite> list ;
  //! Moving edges initialisation parameters
  vpMe *me ;
  unsigned int init_range;
  int nGoodElement;
  
protected:
  vpMeSite::vpMeSiteDisplayType selectDisplay ;

public:
  // Constructor/Destructor
  vpMeTracker() ;
  vpMeTracker(const vpMeTracker& meTracker) ;
  virtual ~vpMeTracker() ;
  
  void init() ;
  void initTracking(const vpImage<unsigned char>& I);
  
  //! Track sampled pixels.
  void track(const vpImage<unsigned char>& I);

  unsigned int numberOfSignal() ;
  unsigned int totalNumberOfSignal() ;
  
  virtual void  display(const vpImage<unsigned char> &I, vpColor col)=0;
  virtual void  display(const vpImage<unsigned char>& I);
  void          display(const vpImage<unsigned char>& I, vpColVector &w, unsigned int &index_w);
  
  void setDisplay(vpMeSite::vpMeSiteDisplayType select)  { 
    selectDisplay = select ;
  }
  
  vpMeTracker& operator =(vpMeTracker& f);
  
  int outOfImage( int i , int j , int half , int rows , int cols) ;
  int outOfImage( vpImagePoint iP , int half , int rows , int cols) ;
  
  //!Sample pixels at a given interval
  virtual void sample(const vpImage<unsigned char> &image)=0;
  
  
  /*!
    Set the initial range.
  
    \param r : initial range.
  */
  void setInitRange(const unsigned int &r) { init_range = r; }
  
  /*!
    Return the initial range.
  
    \return Value of init_range.
  */
  inline unsigned int getInitRange() { return init_range; }
  
  /*!
    Set the moving edges initialisation parameters
  
    \param me : Moving Edges.
  */
  void setMe(vpMe *me) { this->me = me ; }
  
  /*!
    Return the moving edges initialisation parameters
  
    \return Moving Edges.
  */
  inline vpMe* getMe(){ return me; }
  
  /*!
    Set the list of moving edges
  
    \param l : list of Moving Edges.
  */
  void setMeList(const std::list<vpMeSite> &l) { list = l; }
 
  /*!
    Return the list of moving edges
  
    \return List of Moving Edges.
  */
  inline std::list<vpMeSite>& getMeList() { return list; }
  inline std::list<vpMeSite> getMeList() const { return list; }
  
  /*!
    Return the number of points that has not been suppressed.
  
    \return Number of good points.
  */
  inline int getNbPoints() const { return nGoodElement; }
  
#ifdef VISP_BUILD_DEPRECATED_FUNCTIONS
public:
  int query_range;
  bool display_point;// if 1 (TRUE) displays the line that is being tracked
#endif
};


#endif


