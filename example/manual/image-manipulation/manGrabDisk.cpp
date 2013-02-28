/****************************************************************************
 *
 * $Id: manGrabDisk.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Images grabbing example.
 *
 * Authors:
 * Anthony Saunier
 * Fabien Spindler
 *
 *****************************************************************************/
/*!
  \file manGrabDisk.cpp

  \brief Images grabbing example with the vpDiskGrabber class.

 */
/*!
  \example manGrabDisk.cpp

  \brief Images grabbing example with the vpDiskGrabber class.

 */

#include <visp/vpConfig.h>

#include <visp/vpImage.h>
#include <visp/vpDiskGrabber.h>

int main()
{
  vpImage<unsigned char> I; // Grey level image

  // Declare a framegrabber able to read a sequence of successive
  // images from the disk
  vpDiskGrabber g;

  // Set the path to the directory containing the sequence
  g.setDirectory("/tmp");
  // Set the image base name. The directory and the base name constitute
  // the constant part of the full filename
  g.setBaseName("image");
  // Set the step between two images of the sequence
  g.setStep(3);
  // Set the number of digits to build the image number
  g.setNumberOfZero(4);
  // Set the first frame number of the sequence
  g.setImageNumber(1);
  // Set the file extension of the images of the sequence
  g.setExtension("pgm");

  // Open the framegrabber by loading the first image of the sequence
  g.open(I) ;

  // this is the loop over the image sequence
  for(int cpt = 0; cpt < 100; cpt++)
  {
    // read the image and then increment the image counter so that the next
    // call to acquire(I) will get the next image
    g.acquire(I) ;
  }

  return 0;
}
