/****************************************************************************
 *
 * $Id: parse-argv1.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Example of  command line parsing.
 *
 * Author:
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \file parse-argv1.cpp

  \brief Parsing command line arguments.
*/


/*!
  \example parse-argv1.cpp

  Example of command line parsing.
*/



#include <visp/vpDebug.h>
#include <visp/vpParseArgv.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>
// List of allowed command line options
#define GETOPTARGS	"d:f:i:h"

/*!

  Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.
  \param i_val : An integer.
  \param f_val : A float.
  \param d_val : A double.

*/
void usage(const char *name, const char *badparam, int i_val, float f_val, double d_val)
{
  fprintf(stdout, "\n\
Parsing command line arguments example.\n\
\n\
SYNOPSIS\n\
  %s [-i <integer>] [-f <float>] [-d <double> [-h]\n\
", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -i <integer>                                         %d\n\
     An integer value.\n\
\n\
  -f <float>                                           %f\n\
     A float value.\n\
\n\
  -d <double>                                          %g\n\
     A double value.\n\
\n\
  -h\n\
     Print the help.\n\n",
	  i_val, f_val, d_val);

  if (badparam) {
    fprintf(stderr, "ERROR: \n" );
    fprintf(stderr, "\nBad parameter [%s]\n", badparam);
  }
}

/*!

  Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.
  \param i_val : An integer.
  \param f_val : A float.
  \param d_val : A double.
  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, const char **argv, int &i_val, float &f_val, double &d_val)
{
  const char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'd': d_val = atof(optarg); break;
    case 'f': f_val = (float) atof(optarg); break;
    case 'i': i_val = atoi(optarg); break;
    case 'h': usage(argv[0], NULL, i_val, f_val, d_val); return false; break;

    default:
      usage(argv[0], optarg, i_val, f_val, d_val); return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], NULL, i_val, f_val, d_val);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg << std::endl << std::endl;
    return false;
  }

  return true;
}

int
main(int argc, const char ** argv)
{
  using ::std::cout;
  using ::std::endl;

  int    i_val = 3;
  float  f_val = 3.14f;
  double d_val = 3.1415;

  // Read the command line options
  if (getOptions(argc, argv, i_val, f_val, d_val) == false) {
    return (-1);
  }

  cout << "Your parameters: " << endl;
  cout << "  Integer value: " << i_val << endl;
  cout << "  Float   value: " << f_val << endl;
  cout << "  Double  value: " << d_val << endl << endl;
  cout << "Call  " << argv[0]
       << " -h to see how to change these parameters." << endl;

  return 0;
}



/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
