/****************************************************************************
 *
 * $Id: testKalmanAcceleration.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Tests some vpLinearKalmanFilterInstantiation functionalities.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \example testKalmanAcceleration.cpp

  \brief Test some vpLinearKalmanFilterInstantiation functionalities
  with constant acceleration state model.
*/

#include <visp/vpLinearKalmanFilterInstantiation.h>
#include <iostream>
#include <fstream>

int
main()
{
  unsigned int nsignal = 1; // Number of signal to filter
  unsigned int niter = 100;

  std::string filename = "/tmp/log.dat";
  std::ofstream flog(filename.c_str());

  vpLinearKalmanFilterInstantiation kalman;

  vpLinearKalmanFilterInstantiation::vpStateModel model;
  model = vpLinearKalmanFilterInstantiation::stateConstAccWithColoredNoise_MeasureVel;
  kalman.setStateModel(model);

  unsigned int size_state_vector = kalman.getStateSize()*nsignal;
  unsigned int size_measure_vector = kalman.getMeasureSize()*nsignal;

  vpColVector sigma_measure(size_measure_vector);
  for (unsigned int signal=0; signal < nsignal; signal ++) 
    sigma_measure = 0.0001;
  vpColVector sigma_state(size_state_vector);
  for (unsigned int signal=0; signal < nsignal; signal ++) { 
    sigma_state[3*signal] = 0.; // not used
    sigma_state[3*signal+1] = 0.000001;
    sigma_state[3*signal+2] = 0.000001;
  }
  
  vpColVector velocity_measure(size_measure_vector);

  double rho = 0.9; // correlation
  double dt = 0.2; // sampling period

  for (unsigned int signal=0; signal < nsignal; signal ++) 
    velocity_measure[signal] = 3+2*signal;

  kalman.verbose(false);
  kalman.initFilter(nsignal, sigma_state, sigma_measure, rho, dt);


  for (unsigned int iter=0; iter <= niter; iter++) {
    std::cout << "-------- iter " << iter << " ------------" << std::endl;
    for (unsigned int signal=0; signal < nsignal; signal ++) {
      velocity_measure[signal] = 3+2*signal 
          + 0.3*sin(vpMath::rad(360./niter*iter));
    }
    std::cout << "measure : " << velocity_measure.t() << std::endl;

    flog << velocity_measure.t();

    //    kalman.prediction();
    kalman.filter(velocity_measure);
    flog << kalman.Xest.t();
    flog << kalman.Xpre.t();

    std::cout << "Xest: " << kalman.Xest.t() << std::endl;
    std::cout << "Xpre: " << kalman.Xpre.t() << std::endl;

    flog <<  std::endl;
  }

  flog.close();
  return 0;
}
