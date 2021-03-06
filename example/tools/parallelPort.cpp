/****************************************************************************
 *
 * $Id: parallelPort.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Example of keybord management.
 *
 * Author:
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \example parallelPort.cpp

  Send a data to the parallel port.
*/

#include <visp/vpConfig.h>
#include <visp/vpDebug.h>

#if defined VISP_HAVE_PARPORT
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <signal.h>

#include <visp/vpParallelPort.h>
#include <visp/vpParseArgv.h>

// List of allowed command line options
#define GETOPTARGS	"d:h"

/*!

  Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.
  \param data : Data to send to the parallel port.

 */
void usage(const char *name, const char *badparam, unsigned char &data)
{
  fprintf(stdout, "\n\
Send a data to the parallel port.\n\
\n\
SYNOPSIS\n\
  %s [-d <data>] [-h]\n\
", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -d <data>                                              %d\n\
     Data to send to the parallel port.\n\
     Value should be in [0:255].\n\
\n\
  -h\n\
     Print the help.\n\n",
	  data);

  if (badparam) {
    fprintf(stderr, "ERROR: \n" );
    fprintf(stderr, "\nBad parameter [%s]\n", badparam);
  }

}

/*!

  Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.
  \param data : Data to send to the parallel port.

  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, const char **argv, unsigned char &data)
{
  const char *optarg;
  int	c;

  int value;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'd': {
      value = atoi(optarg);
      if ((value < 0) || (value > 255)) {
        usage(argv[0], optarg, data);
        std::cerr << "ERROR: " << std::endl;
        std::cerr << "  Bad value \"-d " << optarg << "\""
                  << std::endl << std::endl;
        return false;
      }
      else {
        data = (unsigned char) value;
      }
      break;
    }
    case 'h': usage(argv[0], NULL, data); return false; break;

    default:
      usage(argv[0], optarg, data); return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], NULL, data);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg << std::endl << std::endl;
    return false;
  }

  return true;
}

/*!

  Send a data to the parallel port.

*/
int
main(int argc, const char **argv)
{
  // data to send to the parallel port
  unsigned char data = 0;

  // Read the command line options
  if (getOptions(argc, argv, data) == false) {
    exit (-1);
  }
  try {

    vpParallelPort parport;

    printf("Send data \"%d\" to the parallel port\n", data);
    parport.sendData(data);


  }
  catch (vpParallelPortException e) {
    switch(e.getCode()) {
    case vpParallelPortException::opening:
      printf("Can't open the parallel port\n");
      break;
    case vpParallelPortException::closing:
      printf("Can't close the parallel port\n");
      break;
    }
  }
  catch(...) {
    printf("An error occurs...\n");
  }
  return 0;
}
#else
int
main()
{
  vpTRACE("Sorry, for the moment, vpParallelPort class works only on unix...");
  return 0;
}
#endif
