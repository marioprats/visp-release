/****************************************************************************
 *
 * $Id: vpParallelPort.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Parallel port management.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/


#ifndef vpParallelPort_h
#define vpParallelPort_h

/*!
  \file vpParallelPort.h
  \brief Parallel port management under unix.
*/

#include <visp/vpConfig.h>

#ifdef VISP_HAVE_PARPORT

#include <iostream>
#  include <stdio.h>
#  include <errno.h>
#  include <linux/parport.h>
#  include <linux/ppdev.h>

#  include <visp/vpParallelPortException.h>

/*!

  \class vpParallelPort
  \ingroup ParallelPort
  \brief Parallel port management under unix.

  The code below shows how to send a data over the parallel port.
  \code
#include <visp/vpConfig.h>
#include <visp/vpParallelPort.h>

int main()
{
#ifdef VISP_HAVE_PARPORT
  vpParallelPort parport;

  unsigned char data = 5; // 0x00000101 = 5 in decimal
  parport.sendData(data); // D0 and D2 are set to logical level 1
#endif
}
  \endcode

*/
class VISP_EXPORT vpParallelPort
{

public:
  vpParallelPort();
  ~vpParallelPort();

  void sendData(unsigned char &data);
  unsigned char getData();

private:
  void open();
  void close();

private:
  int fd; // parallel port descriptor
  char device[FILENAME_MAX];
} ;

#endif

#endif
