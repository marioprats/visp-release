/****************************************************************************
 *
 * $Id: vpFeatureMomentImpl.h 3317 2011-09-06 14:14:47Z fnovotny $
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
 * Implementation for all supported moment features.
 *
 * Authors:
 * Filip Novotny
 *
 *****************************************************************************/
/*!
  \file vpFeatureMomentCentered.h
  \brief Implementation of the interaction matrix computation for vpMomentCentered.
*/
#ifndef __FEATUREMOMENTCENTERED_H__
#define __FEATUREMOMENTCENTERED_H__
#include <visp/vpFeatureMoment.h>
#ifdef VISP_MOMENTS_COMBINE_MATRICES
class vpMomentDatabase;
/*!
  \class vpFeatureMomentCentered

  \ingroup VsFeature2

  \brief Functionality computation for centered moment feature. Computes the interaction matrix associated with vpMomentCentered.

  The interaction matrix for the feature is defined in [1] "Point-based and region-based image moments for visual servoing of planar objects" by Omar Tahri and Francois Chaumette, equation (17).
  This vpFeatureMoment, as well as it's corresponding moment primitive is double-indexed.
  The interaction matrix \f$ L_{\mu_{ij}} \f$ is obtained by calling vpFeatureMomentBasic::interaction (i,j) and is associated to \f$ \mu_{ij} \f$ obtained by vpFeatureMomentCentered::get (i,j).

  vpFeatureMomentCentered computes interaction matrices all interaction matrices up to vpMomentObject::getOrder()-1.
  \attention The maximum order reached by vpFeatureMomentBasic is NOT the maximum order of the vpMomentObject, it is one unit smaller.
  For example if you define your vpMomentObject up to order n then vpFeatureMomentBasic will be able to compute interaction matrices up to order n-1 that is
  \f$ L_{m_{ij}} \f$ with \f$ i+j<=n-1 \f$.

    This feature depends on:
        - vpFeatureMomentBasic
        - vpFeatureMomentGravityCenter
        - vpMomentGravityCenter
*/
class VISP_EXPORT vpFeatureMomentCentered : public vpFeatureMoment{
private:
    unsigned int order;
 public:
        vpFeatureMomentCentered(vpMomentDatabase& moments,double A, double B, double C,vpFeatureMomentDatabase* featureMoments=NULL);
        void compute_interaction();
        /*!
        Interaction matrix corresponding to \f$ \mu_{ij} \f$ moment
        \param select_one : first index (i)
        \param select_two : second index (j)
        \return Interaction matrix corresponding to the moment
        */
        vpMatrix 	interaction (unsigned int select_one,unsigned int select_two);

        /*!
          associated moment name
          */
        const char* momentName(){ return "vpMomentCentered";}
        /*!
          feature name
          */
        const char* name(){ return "vpFeatureMomentCentered";}


};

#else
class vpMomentDatabase;
/*!
  \class vpFeatureMomentCentered

  \ingroup VsFeature2

  \brief Functionality computation for centered moment feature. Computes the interaction matrix associated with vpMomentCentered.

  The interaction matrix for the feature is defined in [1] "Point-based and region-based image moments for visual servoing of planar objects" by Omar Tahri and Francois Chaumette, equation (17).
  This vpFeatureMoment, as well as it's corresponding moment primitive is double-indexed.
  The interaction matrix \f$ L_{\mu_{ij}} \f$ is obtained by calling vpFeatureMomentBasic::interaction (i,j) and is associated to \f$ \mu_{ij} \f$ obtained by vpFeatureMomentCentered::get (i,j).

  vpFeatureMomentCentered computes interaction matrices all interaction matrices up to vpMomentObject::getOrder()-1.
  \attention The maximum order reached by vpFeatureMomentBasic is NOT the maximum order of the vpMomentObject, it is one unit smaller.
  For example if you define your vpMomentObject up to order n then vpFeatureMomentBasic will be able to compute interaction matrices up to order n-1 that is
  \f$ L_{m_{ij}} \f$ with \f$ i+j<=n-1 \f$.

    This feature depends on:
      - vpMomentGravityCenter
      - vpMomentCentered
*/
class VISP_EXPORT vpFeatureMomentCentered : public vpFeatureMoment{
private:
    unsigned int order;
 public:
        vpFeatureMomentCentered(vpMomentDatabase& moments,double A, double B, double C,vpFeatureMomentDatabase* featureMoments=NULL);
        void compute_interaction();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
        /* Add function due to pure virtual definition in vpBasicFeature.h */
        vpMatrix interaction(const unsigned int /* select = FEATURE_ALL */){
          throw vpException(vpException::functionNotImplementedError,"Not implemented!");
        }
#endif

        vpMatrix 	interaction (unsigned int select_one,unsigned int select_two);

        /*!
          associated moment name
          */
        const char* momentName(){ return "vpMomentCentered";}
        /*!
          feature name
          */
        const char* name(){ return "vpFeatureMomentCentered";}


};

#endif
#endif
