/****************************************************************************
 *
 * $Id: vpBiclops.h 3698 2012-05-03 15:20:05Z fspindle $
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
 * Interface for the Biclops robot.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/

#ifndef __vpBiclops_H
#define __vpBiclops_H

/* ----------------------------------------------------------------------- */
/* --- INCLUDES -------------------------------------------------------- */
/* --------------------------------------------------------------------- */

/* --- ViSP --- */
#include <visp/vpMatrix.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpPoseVector.h>
#include <visp/vpMath.h>
#include <visp/vpRxyzVector.h>
#include <visp/vpTranslationVector.h>
#include <visp/vpVelocityTwistMatrix.h>

/* --- GENERAL --- */
#include <iostream>

/*!

  \class vpBiclops

  \ingroup Biclops

  \brief Jacobian, geometric model functionnalities... for biclops, pan, tilt
  head.

  Two different Denavit Hartenberg representations of the robot are implemented.
  As mentionned in vpBiclops::DenavitHartenbergModel they differ in the orientation
  of the tilt axis. Use setDenavitHartenbergModel() to select the representation.

  See http://www.traclabs.com/tracbiclops.htm for more details concerning the hardware.

*/

class VISP_EXPORT vpBiclops
{
public:
  /*!
    Two different Denavit Hartenberg representations of the robot are implemented.
    They differ in the orientation of the tilt axis.

    The first representation, vpBiclops::DH1 is given by:

    \f[
    \begin{tabular}{|c|c|c|c|c|}
    \hline
    Joint & $a_i$ & $d_i$ & $\alpha_i$ & $\theta_i$ \\
    \hline
    1 & 0 & 0 & $-\pi/2$ & $q_1^*$ \\
    2 & 0 & 0 & $ \pi/2$ & $q_2^* + \pi/2$ \\
    \hline
    \end{tabular}
    \f]

    The second one, vpBiclops::DH2 is given by:

    \f[
    \begin{tabular}{|c|c|c|c|c|}
    \hline
    Joint & $a_i$ & $d_i$ & $\alpha_i$ & $\theta_i$ \\
    \hline
    1 & 0 & 0 & $ \pi/2$ & $q_1^*$ \\
    2 & 0 & 0 & $ -\pi/2$ & $q_2^* - \pi/2$ \\
    \hline
    \end{tabular}
    \f]

    where \f$q_1^*, q_2^*\f$ are respectively the pan and tilt joint positions.

    In those representations, the pan is oriented from left to right, while the tilt is oriented
    - in vpBiclops::DH1 from down to top,
    - in vpBiclops::DH2 from top to down.


    */
  typedef enum
  {
    DH1, /*!< First Denavit Hartenberg representation. */
    DH2  /*!< Second Denavit Hartenberg representation. */
  } DenavitHartenbergModel;

public: /* Constants */

  static const unsigned int ndof;  /*!< Number of dof */

  /* Geometric model */
  static const float h;

  static const float panJointLimit;
  static const float tiltJointLimit;
  static const float speedLimit;


protected:
  DenavitHartenbergModel dh_model_;
  vpHomogeneousMatrix cMe_; // Camera frame to mobile platform frame

public:
  vpBiclops (void);
  void init (void);

  void computeMGD (const vpColVector &q, vpHomogeneousMatrix & fMc);

  vpHomogeneousMatrix computeMGD (const vpColVector & q);
  void computeMGD (const vpColVector &q,  vpPoseVector &fvc);

  /*!
    Return the tranformation \f${^c}{\bf M}_e\f$ between the camera frame and the end
    effector frame.
    */
  vpHomogeneousMatrix get_cMe() const
  {
    return cMe_;
  }

  void get_cVe(vpVelocityTwistMatrix &_cVe) ;
  void get_fMc (const vpColVector &q, vpHomogeneousMatrix &fMc);
  void get_fMc (const vpColVector &q,  vpPoseVector &fvc);
  vpHomogeneousMatrix get_fMc (const vpColVector &q);
  vpHomogeneousMatrix get_fMe (const vpColVector &q);

  void get_eJe(const vpColVector &q, vpMatrix &eJe);
  void get_fJe(const vpColVector &q, vpMatrix &fJe);

  /*!
    Return the Denavit Hartenberg representation used to model the head.
    \sa vpBiclops::DenavitHartenbergModel
    */
  inline vpBiclops::DenavitHartenbergModel getDenavitHartenbergModel()
  {
    return dh_model_;
  }

  void set_cMe() ;
  /*!
    Set the transformation between the camera frame and the end effector frame.
    */
  void set_cMe(const vpHomogeneousMatrix &cMe)
  {
    cMe_ = cMe;
  }
  /*!
    Set the Denavit Hartenberg representation used to model the head.

    \sa vpBiclops::DenavitHartenbergModel
    */
  inline void setDenavitHartenbergModel(vpBiclops::DenavitHartenbergModel m=vpBiclops::DH1)
  {
    dh_model_ = m;
  }

  friend std::ostream & operator << (std::ostream & os,
				     const vpBiclops & constant);
};


#endif
