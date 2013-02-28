/****************************************************************************
 *
 * $Id: vpPioneerPan.h 3820 2012-06-27 13:13:29Z fspindle $
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
 * Common features for Pioneer unicycle mobile robots.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/
#ifndef VPPIONEERPAN_H
#define VPPIONEERPAN_H

#include <visp/vpRxyzVector.h>
#include <visp/vpTranslationVector.h>
#include <visp/vpUnicycle.h>


/*!

  \class vpPioneerPan

  \ingroup Pioneer

  \brief Generic functions for Pioneer mobile robots equiped with a pan head.

  This class provides common features for Pioneer mobile robots equiped with a pan head.

  This robot has three control velocities \f$(v_x, w_z, \dot{q_1})\f$, the translational and
  rotational velocities of the mobile platform, the pan head velocity respectively.

  The figure below shows the position of the frames that are used to model the robot.
  The end effector frame is here located at the pan axis.

  \image html pioneer-pan.png


*/
class VISP_EXPORT vpPioneerPan: public vpUnicycle
{
public:
  /*!
    Create a pioneer mobile robot equiped with a pan head.
    */
  vpPioneerPan() : vpUnicycle()
  {
    set_mtp();
    set_cMe();
    set_eJe(0.);
  }

  /*!
    Destructor that does nothing.
    */
  virtual ~vpPioneerPan() {};

  /*!
    Set the robot jacobian expressed at point E the end effector frame located on the pan head.

    Considering \f${\bf v} = {^e}{\bf J}_e \; [v_x, w_z, \dot{q_1}]\f$ with
    \f$(v_x, w_z)\f$ respectively the translational and rotational control velocities
    of the mobile platform, \f$\dot{q_1}\f$ the joint velocity of the pan head and \f$\bf v\f$
    the six dimention velocity skew expressed at point E in frame E,
    the robot jacobian is given by:

    \f[
    {^e}{\bf J}_e = \left(\begin{array}{ccc}
    c_1  & -c_1*p_y - s_1*p_x & 0   \\
    0  & 0 & 0 \\
    s_1  & -s_1*p_y + c_1*p_x & 0   \\
    0  & 0 & 0  \\
    0  & -1 & 1   \\
    0  & 0 & 0  \\
    \end{array}
    \right)
    \f]

    with \f$p_x, p_y\f$ the position of the head base frame in the mobile platform frame
    located at the middle point between the two weels.

  */
  void set_eJe(double q_pan)
  {
    double px = mtp_[0];
    double py = mtp_[1];
    double c1 = cos(q_pan);
    double s1 = sin(q_pan);

    eJe_.resize(6,3); // robot jacobian expressed at the pan head end effector

    eJe_ = 0;
    eJe_[0][0] = c1;
    eJe_[0][1] = -c1*py - s1*px;

    eJe_[2][0] = s1;
    eJe_[2][1] = -s1*py + c1*px;

    eJe_[4][1] = -1;
    eJe_[4][2] = 1;
  }

private:
  /*!
    Set the transformation between the camera frame and the pan head end effector frame.
    */
  void set_cMe()
  {
    // Position of the camera in the pan frame
    double cx = 0;
    double cy = -0.065; //-0.13;
    double cz = 0;
    vpTranslationVector etc(cx, cy, cz);
    vpRotationMatrix eRc;
    eRc[0][0] = eRc[1][1] = eRc[2][2] = 0;
    eRc[0][2] =  1;
    eRc[1][1] =  1;
    eRc[2][0] = -1;

    vpHomogeneousMatrix eMc;
    eMc.buildFrom(etc, eRc);

    cMe_ = eMc.inverse();

    std::cout << "cMe de pan: \n" << cMe_ << std::endl;
  }

  /*!
    Set the transformation between the mobile platform frame
    located at the middle point between the two weels and the base frame of the pan head.
    */
  void set_mtp()
  {
    // Position of the pan head in the mobile platform frame
    double px = 0.1; // distance between the pan frame and the robot frame
    double py = 0;
    double pz = 0.27;
    mtp_.set(px, py, pz);
  }

  vpTranslationVector mtp_;

};

#endif

