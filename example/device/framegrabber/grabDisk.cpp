/****************************************************************************
 *
 * $Id: grabDisk.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Read an image sequence from the disk and display it.
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/


#include <visp/vpDebug.h>
#include <visp/vpConfig.h>
#include <stdlib.h>
#if (defined (VISP_HAVE_X11) || defined(VISP_HAVE_GTK))

#include <visp/vpDiskGrabber.h>
#include <visp/vpImage.h>
#include <visp/vpDisplay.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGTK.h>
#include <visp/vpTime.h>
#include <visp/vpParseArgv.h>

/*!
  \file grabDisk.cpp

  \brief Example of image sequence reading from the disk using vpDiskGrabber class.

  The sequence is made of separate images. Each image corresponds to a PGM
  file.
*/

// List of allowed command line options
#define GETOPTARGS	"b:de:f:i:hn:s:z:"

/*

  Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.
  \param ipath : Input image path.
  \param basename : Input image base name.
  \param ext : Input image extension.
  \param first : First image number to read.
  \param nimages : Number of images to read.
  \param step : Step between two successive images to read.
  \param nzero : Number of zero for the image number coding.

 */
void usage(const char *name, const char *badparam, std::string ipath, std::string basename,
	   std::string ext, int first, unsigned int nimages, int step,
	   unsigned int nzero)
{
  fprintf(stdout, "\n\
Read an image sequence from the disk. Display it using X11 or GTK.\n\
The sequence is made of separate images. Each image corresponds\n\
to a PGM file.\n\
\n\
SYNOPSIS\n\
  %s [-i <input image path>] [-b <base name>] [-e <extension>] \n\
   [-f <first frame>] [-n <number of images> [-s <step>] \n\
   [-z <number of zero>] [-d] [-h]\n", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -i <input image path>                                     %s\n\
     Set image input path.\n\
     From this path read \"ViSP-images/cube/image.%%04d.pgm\"\n\
     images.\n\
     Setting the VISP_INPUT_IMAGE_PATH environment\n\
     variable produces the same behaviour than using\n\
     this option.\n\
\n\
  -b <base name>                                 %s\n\
     Specify the base name of the files of the sequence\n\
     containing the images to process. \n\
     By image sequence, we mean one file per image.\n\
     The following image file formats PNM (PGM P5, PPM P6)\n\
     are supported. The format is selected by analysing \n\
     the filename extension.\n\
\n\
  -e <extension>                                            %s\n\
     Specify the extension of the files.\n\
     Not taken into account for the moment. Will be a\n\
     future feature...\n\
\n\
  -f <first frame>                                          %u\n\
     First frame number of the sequence\n\
\n\
  -n <number of images>                                     %u\n\
     Number of images to load from the sequence.\n\
\n\
  -s <step>                                                 %u\n\
     Step between two images.\n\
\n\
  -z <number of zero>                                       %u\n\
     Number of digits to encode the image number.\n\
\n\
  -d \n\
     Turn off the display.\n\
\n\
  -h \n\
     Print the help.\n\n",
	  ipath.c_str(), basename.c_str(), ext.c_str(), first,
	  nimages, step, nzero);

  if (badparam)
    fprintf(stdout, "\nERROR: Bad parameter [%s]\n", badparam);
}
/*!

  Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.
  \param ipath : Input image path.
  \param basename : Input image base name.
  \param ext : Input image extension.
  \param first : First image number to read.
  \param nimages : Number of images to read.
  \param step : Step between two successive images to read.
  \param nzero : Number of zero for the image number coding.
  \param display : Display activation.

  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, const char **argv, std::string &ipath, std::string &basename,
                std::string &ext, int &first, unsigned int &nimages,
                int &step, unsigned int &nzero, bool &display)
{
  const char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'b': basename = optarg; break;
    case 'd': display = false; break;
    case 'e': ext = optarg; break;
    case 'f': first = atoi(optarg); break;
    case 'i': ipath = optarg; break;
    case 'n': nimages = (unsigned) atoi(optarg); break;
    case 's': step = atoi(optarg); break;
    case 'z': nzero = (unsigned) atoi(optarg); break;
    case 'h': usage(argv[0], NULL, ipath, basename, ext, first, nimages,
                    step, nzero); return false; break;

    default:
      usage(argv[0], optarg, ipath, basename, ext, first, nimages,
            step, nzero);
      return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], NULL, ipath, basename, ext, first, nimages, step, nzero);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg << std::endl << std::endl;
    return false;
  }

  return true;
}


/*!
  \example grabDisk.cpp

  Example of image sequence reading from the disk using vpDiskGrabber class.

  Read an image sequence from the disk. The sequence is made of separate
  images. Each image corresponds to a PGM file. Display these images using X11
  or GTK.
*/
int
main(int argc, const char ** argv)
{
  std::string env_ipath;
  std::string opt_ipath;
  std::string ipath;
  std::string opt_basename = "ViSP-images/cube/image.";
  std::string opt_ext = "pgm";
  bool opt_display = true;

  int opt_first = 5;
  unsigned int opt_nimages = 70;
  int opt_step = 1;
  unsigned int opt_nzero = 4;

  // Get the VISP_IMAGE_PATH environment variable value
  char *ptenv = getenv("VISP_INPUT_IMAGE_PATH");
  if (ptenv != NULL)
    env_ipath = ptenv;

  // Set the default input path
  if (! env_ipath.empty())
    ipath = env_ipath;

  // Read the command line options
  if (getOptions(argc, argv, opt_ipath, opt_basename, opt_ext, opt_first,
                 opt_nimages, opt_step, opt_nzero, opt_display) == false) {
    exit (-1);
  }

  // Get the option values
  if (!opt_ipath.empty())
    ipath = opt_ipath;

  // Compare ipath and env_ipath. If they differ, we take into account
  // the input path comming from the command line option
  if (!opt_ipath.empty() && !env_ipath.empty()) {
    if (ipath != env_ipath) {
      std::cout << std::endl
                << "WARNING: " << std::endl;
      std::cout << "  Since -i <visp image path=" << ipath << "> "
                << "  is different from VISP_IMAGE_PATH=" << env_ipath << std::endl
                << "  we skip the environment variable." << std::endl;
    }
  }

  // Test if an input path is set
  if (opt_ipath.empty() && env_ipath.empty()){
    usage(argv[0], NULL, ipath, opt_basename, opt_ext, opt_first,
          opt_nimages, opt_step, opt_nzero);
    std::cerr << std::endl
              << "ERROR:" << std::endl;
    std::cerr << "  Use -i <visp image path> option or set VISP_INPUT_IMAGE_PATH "
              << std::endl
              << "  environment variable to specify the location of the " << std::endl
              << "  image path where test images are located." << std::endl << std::endl;
    exit(-1);
  }


  // Declare an image, this is a gray level image (unsigned char)
  // it size is not defined yet, it will be defined when the image will
  // read on the disk
  vpImage<unsigned char> I ;


  // Declare a framegrabber able to read a sequence of successive
  // images from the disk
  vpDiskGrabber g;

  // Set the path to the directory containing the sequence
  g.setDirectory(ipath.c_str());
  // Set the image base name. The directory and the base name constitute
  // the constant part of the full filename
  g.setBaseName(opt_basename.c_str());
  // Set the step between two images of the sequence
  g.setStep(opt_step);
  // Set the number of digits to build the image number
  g.setNumberOfZero(opt_nzero);
  // Set the first frame number of the sequence
  g.setImageNumber(opt_first);
  // Set the image extension
  g.setExtension(opt_ext.c_str());

  // Open the framegrabber by loading the first image of the sequence
  try {
    g.open(I) ;
  }
  catch (...) {
    vpERROR_TRACE("Cannot open the first image of the sequence... ") ;
    exit(-1);
  }

  std::cout << "Image size: width : " << I.getWidth() <<  " height: "
            << I.getHeight() << std::endl;

  // We open a window using either X11 or GTK.
  // Its size is automatically defined by the image (I) size
#if defined VISP_HAVE_X11
  vpDisplayX display;
#elif defined VISP_HAVE_GTK
  vpDisplayGTK display;
#endif

  if (opt_display) {
    try {
      display.init(I,100,100,"Disk Framegrabber");

      // display the image
      // The image class has a member that specify a pointer toward
      // the display that has been initialized in the display declaration
      // therefore is is no longuer necessary to make a reference to the
      // display variable.
      vpDisplay::display(I) ;
      vpDisplay::flush(I) ;
    }
    catch(...)
    {
      vpERROR_TRACE("Cannot display the image ") ;
      exit(-1);
    }
  }

  unsigned cpt = 1;
  // this is the loop over the image sequence
  try {
    while(cpt ++ < opt_nimages)
    {
      double tms = vpTime::measureTimeMs();
      // read the image and then increment the image counter so that the next
      // call to acquire(I) will get the next image
      g.acquire(I) ;
      if (opt_display) {
        // Display the image
        vpDisplay::display(I) ;
        // Flush the display
        vpDisplay::flush(I) ;
      }
      // Synchronise the loop to 40 ms
      vpTime::wait(tms, 40) ;

    }
  }
  catch(...) {
    vpERROR_TRACE("Error during the framegrabbing...");
  }
}

#else
int
main()
{
  vpERROR_TRACE("You do not have X11 or GTK display functionalities...");
}

#endif

