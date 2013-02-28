/****************************************************************************
 *
 * $Id: vpFeatureMomentArea.h 3317 2011-09-06 14:14:47Z mbakthav $
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
 * Definition of vpFeatureMomentArea associated to vpMomentArea
 *
 * Authors:
 * Manikandan Bakthavatchalam
 *
 *****************************************************************************/
/*!
  \file vpFeatureMomentArea.h
  \brief Implementation of the interaction matrix computation for vpMomentArea.
*/
#ifndef __FEATUREMOMENTAREA_H__
#define __FEATUREMOMENTAREA_H__
#include <visp/vpFeatureMoment.h>

class vpMomentDatabase;

/*!
  \class vpFeatureMomentArea

*/

class VISP_EXPORT vpFeatureMomentArea : public vpFeatureMoment{
 public:
        /*!
        Initializes the feature with information about the database of moment primitives, the object plane and feature database.
        \param moments : Moment database. The database of moment primitives (first parameter) is mandatory. It is used to access different moment values later used to compute the final matrix.
        \param A : Plane coefficient in a \f$ A \times x+B \times y + C = \frac{1}{Z} \f$ plane.
        \param B : Plane coefficient in a \f$ A \times x+B \times y + C = \frac{1}{Z} \f$ plane.
        \param C : Plane coefficient in a \f$ A \times x+B \times y + C = \frac{1}{Z} \f$ plane.
        \param featureMoments : Feature database.

        */
        vpFeatureMomentArea(vpMomentDatabase& moments,double A, double B, double C,vpFeatureMomentDatabase* featureMoments=NULL) :
            vpFeatureMoment(moments,A,B,C,featureMoments,1){}

        void compute_interaction();
        /*!
          associated moment name
          */
        const char* momentName(){ return "vpMomentArea";}
        /*!
          feature name
          */
        const char* name(){ return "vpFeatureMomentArea";}

};
#endif
