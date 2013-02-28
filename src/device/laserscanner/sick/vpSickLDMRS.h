/****************************************************************************
 *
 * $Id: vpSickLDMRS.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Sick LD-MRS laser driver.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/
#ifndef vpSickLDMRS_h
#define vpSickLDMRS_h

#include <visp/vpConfig.h>

#ifdef UNIX

#include <arpa/inet.h>
#include <iostream>
#include <vector>


#include <visp/vpScanPoint.h>
#include <visp/vpLaserScan.h>
#include <visp/vpLaserScanner.h>
#include <visp/vpColVector.h>

/*!

  \file vpSickLDMRS.h

  \brief Driver for the Sick LD-MRS laser scanner.
*/

/*!

  \class vpSickLDMRS

  \ingroup LaserDriver

  \brief Driver for the Sick LD-MRS laser scanner.

  \warning For the moment, this driver works only on UNIX platform.

  The code below shows how the four laser scan provided by the Sick
  LD-MRS could be acquired.

  \code
#include "visp/vpSickLDMRS.h"

int main()
{
#ifdef UNIX
  std::string ip = "131.254.12.119";

  vpSickLDMRS laser;
  laser.setIpAddress(ip);
  laser.setup();
    
  vpLaserScan laserscan[4];
  for ( ; ; ) {
    // Get the measured points in the four layers
    laser.measure(laserscan);

    // Prints all the measured points 
    for (int layer=0; layer<4; layer++) {
      std::vector<vpScanPoint> pointsInLayer = laserscan[layer].getScanPoints();
      vpScanPoint p;
    
      for (unsigned int i=0; i < pointsInLayer.size(); i++) {
	std::cout << pointsInLayer[i] << std::endl; 
      }
    }
  }
#endif
}
  \endcode
*/
class VISP_EXPORT vpSickLDMRS : public vpLaserScanner
{
 public:
  enum MagicWord {
    MagicWordC2 = 0xAFFEC0C2   ///< The magic word that allows to identify the messages that are sent by the Sick LD-MRS.
  };
  enum DataType {
    MeasuredData = 0x2202      ///< Flag to indicate that the body of a message contains measured data.
  };
  vpSickLDMRS();
  /*! Copy constructor. */
  vpSickLDMRS(const vpSickLDMRS &sick) : vpLaserScanner(sick) {
    socket_fd = sick.socket_fd;
    body = new unsigned char [104000];
  };
  virtual ~vpSickLDMRS();
  bool setup(std::string ip, int port);
  bool setup();
  bool measure(vpLaserScan laserscan[4]);

 protected:
#ifdef WIN32
  SOCKET socket_fd;
#else
  int socket_fd;  
#endif
 private:
  unsigned char *body;
  vpColVector vAngle; // constant vertical angle for each layer
  double time_offset;
  bool isFirstMeasure;
 };

#endif

#endif
