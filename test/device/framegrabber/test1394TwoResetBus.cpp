/****************************************************************************
 *
 * $Id: test1394TwoResetBus.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Firewire cameras video capture.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/


/*!
  \file test1394TwoResetBus.cpp

  \brief Resets the IEEE1394 bus using libdc1394-2.x library.

*/



#include <visp/vpConfig.h>
#include <visp/vpDebug.h>

#include <iostream>

#if defined(VISP_HAVE_DC1394_2)

#include <visp/vp1394TwoGrabber.h>
#include <visp/vpImage.h>
#include <visp/vpImageIo.h>


/*!
  \example test1394TwoResetBus.cpp

  Resets the IEEE1394 bus which first camera is attached to. Resetting
  the bus is "rude" to other devices because it causes them to
  re-enumerate on the bus and may cause a temporary disruption in
  their current activities.  Thus, use it sparingly.  Its primary use
  is if a program shuts down uncleanly and needs to free leftover ISO
  channels or bandwidth.  A bus reset will free those things as a side
  effect.

*/
int
main()
{
  try  {
    std::cout << "IEEE1394 bus reset in progress..." << std::endl;
    vp1394TwoGrabber g;
    g.resetBus(); // Reset the bus attached to the first camera found
    std::cout << "IEEE1394 bus was reset." << std::endl;

    vpImage<unsigned char> I;
    g.acquire(I);    
    //     std::cout << "write /tmp/test.pgm" << std::endl;
    //     vpImageIo::writePGM(I, "/tmp/test.pgm");
  }
  catch (...) {
    vpCERROR << "Failure: exit" << std::endl;
  }
}
#else
int
main()
{
  vpTRACE("Ieee 1394 grabber capabilities are not available...\n"
          "You should install libdc1394-2 to use this binary.") ;
}

#endif

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
