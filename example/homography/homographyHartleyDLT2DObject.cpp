/****************************************************************************
 *
 * $Id: homographyHartleyDLT2DObject.cpp 3619 2012-03-09 17:28:57Z fspindle $
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
 * Example of the HartleyDLT homography estimation algorithm.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/
/*!
  \file homographyHartleyDLT2DObject.cpp

  \brief Example of the HartleyDLT homography estimation algorithm using
  vpHomography class.

*/
/*!
  \example homographyHartleyDLT2DObject.cpp

  Example of the HartleyDLT homography estimation algorithm using vpHomography
  class.

*/



#include <visp/vpMath.h>
#include <visp/vpRotationMatrix.h>
#include <visp/vpHomography.h>
#include <visp/vpDebug.h>
#include <visp/vpThetaUVector.h>

#include <visp/vpPoint.h>
#include <visp/vpMath.h>
#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpDebug.h>
#include <visp/vpParseArgv.h>
#include <stdlib.h>
// List of allowed command line options
#define GETOPTARGS	"h"

#define L 0.1
#define nbpt 5

/*!

  Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.


*/
void usage(const char *name, const char *badparam)
{
  fprintf(stdout, "\n\
Test the HartleyDLT homography estimation algorithm.\n\
\n\
SYNOPSIS\n\
  %s [-h]\n", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -h\n\
     Print the help.\n");

  if (badparam) {
    fprintf(stderr, "ERROR: \n" );
    fprintf(stderr, "\nBad parameter [%s]\n", badparam);
  }
}
/*!

  Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.


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

  int i ;

  vpPoint P[nbpt]  ;  //  Point to be tracked
  double xa[nbpt], ya[nbpt] ;
  double xb[nbpt], yb[nbpt] ;

  vpPoint aP[nbpt]  ;  //  Point to be tracked
  vpPoint bP[nbpt]  ;  //  Point to be tracked

  P[0].setWorldCoordinates(-L,-L, 0 ) ;
  P[1].setWorldCoordinates(2*L,-L, 0 ) ;
  P[2].setWorldCoordinates(L,L, 0 ) ;
  P[3].setWorldCoordinates(-L,3*L, 0 ) ;
  P[4].setWorldCoordinates(0,0, 0 ) ;
  /*
    P[5].setWorldCoordinates(10,20, 0 ) ;
    P[6].setWorldCoordinates(-10,12, 0 ) ;
  */
  vpHomogeneousMatrix bMo(0,0,1, 0,0,0) ;
  vpHomogeneousMatrix aMb(1,0,0.0,vpMath::rad(10),0,vpMath::rad(40)) ;
  vpHomogeneousMatrix aMo =aMb*bMo ;
  for(i=0 ; i < nbpt ; i++)
  {
    P[i].project(aMo) ;
    aP[i] = P[i] ;
    xa[i] = P[i].get_x() ;
    ya[i] = P[i].get_y() ;
  }

  for(i=0 ; i < nbpt ; i++)
  {
    P[i].project(bMo) ;
    bP[i] = P[i] ;
    xb[i] = P[i].get_x() ;
    yb[i] = P[i].get_y() ;
  }
  std::cout << "-------------------------------" <<std::endl ;
  std::cout << "aMb "<<std::endl <<aMb << std::endl ;
  std::cout << "-------------------------------" <<std::endl ;
  vpHomography aHb ;

  vpHomography::HartleyDLT(nbpt,xb,yb,xa,ya,aHb) ;

  vpTRACE("aHb computed using the DLT algorithm") ;
  aHb /= aHb[2][2] ;
  std::cout << std::endl << aHb<< std::endl ;

  vpRotationMatrix aRb  ;
  vpTranslationVector aTb ;
  vpColVector n ;

  std::cout << "-------------------------------" <<std::endl ;
  vpTRACE("extract R, T and n ") ;
  aHb.computeDisplacement(aRb, aTb, n) ;
  std::cout << "Rotation: aRb" <<std::endl ;
  std::cout << aRb << std::endl ;
  std::cout << "Translation: aTb" <<std::endl;
  std::cout << (aTb).t() <<std::endl   ;
  std::cout << "Normal to the plane: n" <<std::endl;
  std::cout << (n).t() <<std::endl ;


  std::cout << "-------------------------------" <<std::endl ;
  vpTRACE("Compare with built homoraphy H = R + t/d ") ;
  vpPlane bp(0,0,1,1) ;
  vpHomography aHb_built(aMb,bp) ;
  vpTRACE( "aHb built from the displacement ") ;
  std::cout <<  std::endl <<aHb_built/aHb_built[2][2] << std::endl ;

  aHb_built.computeDisplacement(aRb, aTb, n) ;
  std::cout << "Rotation: aRb" <<std::endl ;
  std::cout << aRb << std::endl ;
  std::cout << "Translation: aTb" <<std::endl;
  std::cout << (aTb).t() <<std::endl ;
  std::cout << "Normal to the plane: n" <<std::endl;
  std::cout << (n).t() <<std::endl ;

  std::cout << "-------------------------------" <<std::endl ;
  vpTRACE("test if ap = aHb bp") ;

  for(i=0 ; i < nbpt ; i++)
  {
    std::cout << "Point "<< i<< std::endl ;
    vpPoint p ;
    std::cout << "(" ;
    std::cout << aP[i].get_x()/aP[i].get_w()<<", "<< aP[i].get_y()/aP[i].get_w() ;
    std::cout <<") =  (" ;
    p = aHb*bP[i] ;
    std::cout << p.get_x() /p.get_w()<<",  "<< p.get_y()/ p.get_w() <<")"<<std::endl ;
  } 
}
