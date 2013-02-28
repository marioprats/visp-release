/****************************************************************************
 *
 * $Id: vpNoise.h 3634 2012-03-15 09:24:47Z fspindle $
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
 * Generation of random number with uniform and normal probability density.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


#ifndef vpNoise_hh
#define vpNoise_hh


/*!
  \file vpNoise.h
  \brief Class for generating random number
  with uniform and normal probability density

  The algorithms and notations used are described in

  James E. Gentle, Random Number Generation and Monte Carlo Methods,
  Springer 1998
 */

#include <visp/vpConfig.h>


/*!
  \class vpUniRand

  \ingroup Random
  \brief Class for generating random numbers with uniform probability density.

  The algorithms and notations used are described in
  Random Number Generation and Monte Carlo Methods
  James E. Gentle, Springer 1998

 */
class VISP_EXPORT vpUniRand
{


	/*unsigned*/ long    a  ;
	/*unsigned*/ long    m ; //2^31-1
	/*unsigned*/ long    q ; //integer part of m/a
	/*unsigned*/ long    r ;//r=m mod a
	double    normalizer ; //we use a normalizer > m to ensure ans will never be 1 (it is the case if x = 739806647)


private:
	void draw0();
protected:
	long x;
	double draw1();
	void init()
	{
		a = 16807 ;
		m = /*(unsigned long)*/2147483647 ; //2^31-1
		q = 127773 ; //integer part of m/a
		r = 2836 ;//r=m mod a
		//we use a normalizer > m to ensure ans will never be
		// 1 (it is the case if x = 739806647)
		normalizer = 2147484721.0 ;
	}

public:
	vpUniRand(const long seed = 0):x((seed)? seed : 739806647)
	{
		init() ;
	}
	double operator()() {return draw1();}

};

/*!
  \class vpGaussRand
  \ingroup Random
  \brief Class for generating random number with normal probability density.

  The algorithms and notations used are described in "Random Number Generation and Monte Carlo Methods",
  James E. Gentle, Springer 1998

  The code below shows how to use the random generator.

  \code
#include <iostream>

#include <visp/vpNoise.h>

int main()
{
  vpGaussRand noise(0.5, 0);
  for(int i=0; i< 10; i++) {
    std::cout << "noise " << i << ": " << noise() << std::endl;
  }
  return 0;
}
  \endcode
 */
class vpGaussRand : public vpUniRand
{
	double mean;
	double sigma;

public:

	// Initialization
	vpGaussRand() {init();mean=0;sigma=0;x=739806647;}
	vpGaussRand(const double sqrtvariance,
			const double _mean,
			const long seed = 0):mean(_mean), sigma(sqrtvariance)
	{
		init() ;
		mean = 0 ;
		if (seed) x=seed; else x=739806647;
	}
	/*!
      Set the standard deviation and mean for gaussian noise

      \param _s new standard deviation
      \param _m new mean
	 */
	inline void setSigmaMean(const double _s, const double _m) {mean=_m;sigma=_s;}
	/*!
        Set the seed of the noise

        \param seed new seed
	 */
	inline void seed(const long seed) {x=seed;}
	inline double operator()() {return sigma*gaussianDraw()+mean;}

private :
	double gaussianDraw();
};

#endif
