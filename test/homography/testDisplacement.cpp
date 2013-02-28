/****************************************************************************
 *
 * $Id: testDisplacement.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Tests transformation within various representations of rotation.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


/*!
  \file testDisplacement.cpp
  \brief Tests transformation within various representations of rotation
*/

/*!
  \example testDisplacement.cpp


*/

#include <visp/vpMath.h>
#include <visp/vpRotationMatrix.h>
#include <visp/vpHomography.h>
#include <visp/vpDebug.h>
#include <visp/vpThetaUVector.h>
#include <visp/vpParseArgv.h>
#include <stdlib.h>
#include <stdio.h>

// List of allowed command line options
#define GETOPTARGS	"h"

/*!

  Print the program options.

*/
void usage(const char *name, const char *badparam)
{
  fprintf(stdout, "\n\
Tests transformation within various representations of rotation.\n\
\n\
SYNOPSIS\n\
  %s [-h]\n", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -h\n\
     Print the help.\n");

  if (badparam)
    fprintf(stdout, "\nERROR: Bad parameter [%s]\n", badparam);
}
/*!

  Set the program options.

  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, const char **argv)
{
  const char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'h': usage(argv[0], NULL); return false; break;

    default:
      usage(argv[0], optarg);
      return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], NULL);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg << std::endl << std::endl;
    return false;
  }

  return true;
}


int
main(int argc, const char ** argv)
{
  // Read the command line options
  if (getOptions(argc, argv) == false) {
    exit (-1);
  }

  {
    vpThetaUVector tu(vpMath::rad(90), vpMath::rad(120), vpMath::rad(45)) ;

    std::cout << "Initialization " <<std::endl ;
    // std::cout << tu << std::endl ;


    std::cout << "From vpThetaUVector to vpRotationMatrix " << std::endl ;
    vpRotationMatrix R(tu)  ;


    // pure rotation
    vpHomogeneousMatrix M ;
    M.insert(R) ;



    std::cout << "M" <<std::endl <<M << std::endl ;
    vpPlane p(0,0,1,1) ;

    vpHomography H(M,p) ;

    vpTRACE(" ") ;
    std::cout << "H" <<std::endl <<H << std::endl ;


    vpTRACE(" ") ;

    vpColVector n ;
    vpTranslationVector T ;

    H.computeDisplacement(R,T,n) ;

    std::cout << "R" <<std::endl << R ;
    std::cout << "T" <<std::endl << T.t()  ;
    std::cout << "n" <<std::endl << n.t()  ;
    vpTRACE(" ") ;
    vpTRACE(" ") ;
  }
  std::cout <<"------------------------------------------------------" << std::endl ;

  {
    vpThetaUVector tu(vpMath::rad(90), vpMath::rad(120), vpMath::rad(45)) ;

    std::cout << "Initialization " <<std::endl ;
    // std::cout << tu << std::endl ;


    std::cout << "From vpThetaUVector to vpRotationMatrix " << std::endl ;
    vpRotationMatrix R(tu)  ;


    // pure rotation
    vpHomogeneousMatrix M ;
    M.insert(R) ;


    M[0][3] = 0.21 ;
    M[1][3] = 0.31 ;
    M[2][3] = 0.5 ;


    std::cout << "M" <<std::endl <<M << std::endl ;
    vpPlane p(0,0,1,1) ;

    vpHomography H(M,p) ;

    vpTRACE(" ") ;
    std::cout << "H" <<std::endl <<H << std::endl ;


    vpTRACE(" ") ;

    vpColVector n ;
    vpTranslationVector T ;

    H.computeDisplacement(R,T,n) ;

    std::cout << "R" <<std::endl << R ;
    std::cout << "T" <<std::endl << T.t()  ;
    std::cout << "n" <<std::endl << n.t()  ;
    vpTRACE(" ") ;
    vpTRACE(" ") ;
  }

  std::cout <<"------------------------------------------------------" << std::endl ;
  {
    vpThetaUVector  tu(vpMath::rad(-190), vpMath::rad(12), vpMath::rad(-45)) ;

    vpRotationMatrix R(tu)  ;


    // pure rotation
    vpHomogeneousMatrix M ;
    M.insert(R) ;

    M[0][3] = 0.21 ;
    M[1][3] =- 0.31 ;
    M[2][3] = 0.5 ;

    std::cout << "M" <<std::endl <<M << std::endl ;
    vpPlane p(0.4,-0.5,0.5,1) ;

    vpHomography H(M,p) ;

    vpTRACE(" ") ;
    std::cout << "H" <<std::endl <<H << std::endl ;

    vpTRACE(" ") ;
    vpColVector n ;
    vpTranslationVector T ;
    H.computeDisplacement(R,T,n) ;

    std::cout << "R" <<std::endl << R ;
    std::cout << "T" <<std::endl << T.t()  ;
    std::cout << "n" <<std::endl << n.t()  ;

    vpPlane p1(n[0],n[1],n[2],1.0) ;
    H.buildFrom(R,T,p1) ;
    std::cout << "H" <<std::endl <<H << std::endl ;

  }
}
