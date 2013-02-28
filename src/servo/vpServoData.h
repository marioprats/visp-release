/****************************************************************************
 *
 * $Id: vpServoData.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Save data during the task execution.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/

#ifndef vpServoData_H
#define vpServoData_H

/*!
  \file vpServoData.h
  \brief  save data during the task execution
*/



// Servo
#include <visp/vpServo.h>

#include <iostream>

/*!
  \class vpServoData
  \ingroup VsTask
  \brief Save data during the task execution.
*/
class VISP_EXPORT vpServoData
{

private:
  char baseDirectory[FILENAME_MAX] ;

  std::ofstream velocityFile ;
  std::ofstream errorFile ;
  std::ofstream errorNormFile ;
  std::ofstream sFile ;
  std::ofstream sStarFile ;
  std::ofstream vNormFile ;

  //! flag to known if velocity should be output in cm and degrees (true)
  //! or in m/rad
  bool cmDeg ;



public:

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  vpServoData(const vpServoData &) {
    throw vpException(vpException::functionNotImplementedError,"Not implemented!");
  }      
  void operator=(const vpServoData &){
    throw vpException(vpException::functionNotImplementedError,"Not implemented!");
  }
#endif

  vpServoData() { ; }
  virtual ~vpServoData() { ; }

  //! velocity output in cm and deg
  void setCmDeg() ;
  //! velocity output in meter and deg (default)
  void setMeterRad() ;

  void save(const vpServo &task) ;
  void open(const char *baseDirectory) ;
  void close() ;

  void empty() ;
  void push() ;
  void display(vpImage<unsigned char> &I) ;

} ;



#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
