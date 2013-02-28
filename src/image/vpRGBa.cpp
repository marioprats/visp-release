/****************************************************************************
 *
 * $Id: vpRGBa.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
 * RGBA pixel.
 *
 * Authors:
 * Eric Marchand
 * Fabien Spindler
 *
 *****************************************************************************/

/*!
  \file vpRGBa.cpp
  \brief Define the object vpRGBa that is used to build color
  images (it define a RGB 32 bits structure, fourth byte is not used - yet -)
*/


#include <visp/vpRGBa.h>
#include <visp/vpColor.h>
#include <visp/vpDebug.h>
#include <visp/vpException.h>


/*!
  Copy operator (from an unsigned char value)

  \param v : Input color ( R = G = B = v )
*/
void
vpRGBa::operator=(const unsigned char &v)
{
  this->R = v;
  this->G = v;
  this->B = v;
  this->A = v;
}

/*!
  Copy operator.
*/
void
vpRGBa::operator=(const vpRGBa &v)
{
  this->R = v.R;
  this->G = v.G;
  this->B = v.B;
  this->A = v.A;
}

/*!
  Cast a vpColVector in a vpRGBa

  \param v : Input vector. v[0], v[1], v[2], v[3] are to make into
  relation with respectively R, G, B and A.

  \exception vpException::dimensionError : If v is not a 4 four
  dimention vector.
*/
void
vpRGBa::operator=(const vpColVector &v)
{
  if (v.getRows() != 4) {
    vpERROR_TRACE("Bad vector dimension ") ;
    throw(vpException(vpException::dimensionError, "Bad vector dimension "));
  }
  R = (unsigned char)v[0];
  G = (unsigned char)v[1];
  B = (unsigned char)v[2];
  A = (unsigned char)v[3];
}

/*!
  Compare two RGBa values.

  \return true if the values are the same, false otherwise.
*/
bool vpRGBa::operator==(const vpRGBa &v)
{
  if (R != v.R)
    return false;
  if (G != v.G)
    return false;
  if (B != v.B)
    return false;
  if (A != v.A)
    return false;

  return true ;
}
/*!
  Compare two color pixels.

  \return true if the images are different, false if they are the same.
*/
bool vpRGBa::operator!=(const vpRGBa &v)
{
  if (R == v.R)
    return false;
  if (G == v.G)
    return false;
  if (B == v.B)
    return false;
  if (A == v.A)
    return false;

  return true ;
}

/*!
  Substraction operator : "this" - v.
  \param v : Color to substract to the current object "this".
  \return "this" - v
*/
vpColVector
vpRGBa::operator-(const vpRGBa &v) const
{
  vpColVector n(4); // new color
  n[0] = (double)R - (double)v.R;
  n[1] = (double)G - (double)v.G;
  n[2] = (double)B - (double)v.B;
  n[3] = (double)A - (double)v.A;
  return n;
}

/*!
  Addition operator : "this" + v.
  \param v : Color to add to the current object "this".
  \return "this" + v
  \warning in case of overflow : e.g. 128+128 returns 0 for all 4 channels
*/
vpRGBa
vpRGBa::operator+(const vpRGBa &v) const
{
  vpRGBa n; // new color
  n.R = static_cast<unsigned char>( R + v.R );
  n.G = static_cast<unsigned char>( G + v.G );
  n.B = static_cast<unsigned char>( B + v.B );
  n.A = static_cast<unsigned char>( A + v.A );
  return n;
}

/*!
  Substraction operator : "this" - v.
  \param v : Color to substract to the current object "this".
  \return "this" - v
*/
vpColVector
vpRGBa::operator-(const vpColVector &v) const
{
  vpColVector n(4); // new color
  n[0] = R - v[0];
  n[1] = G - v[1];
  n[2] = B - v[2];
  n[3] = A - v[3];
  return n;
}

/*!
  Addition operator : "this" + v.
  \param v : Color to add to the current object "this".
  \return "this" + v
*/
vpColVector
vpRGBa::operator+(const vpColVector &v) const
{
  vpColVector n(4); // new color
  n[0] = R + v[0];
  n[1] = G + v[1];
  n[2] = B + v[2];
  n[3] = A + v[3];
  return n;
}

/*!
  Multiplication operator : v * "this".
  \param v : Value to multiply.
  \return v * "this"
*/
vpColVector
vpRGBa::operator*(const float &v) const
{
  vpColVector n(4);
  n[0] = R * v;
  n[1] = G * v;
  n[2] = B * v;
  n[3] = A * v;
  return n;
}

/*!
  Multiplication operator : v * "this".
  \param v : Value to multiply.
  \return v * "this"
*/
vpColVector
vpRGBa::operator*(const double &v) const
{
  vpColVector n(4);
  n[0] = R * v;
  n[1] = G * v;
  n[2] = B * v;
  n[3] = A * v;
  return n;
}

bool
vpRGBa::operator<(const vpRGBa &v) const
{
	double gray1 = 0.2126*R+0.7152*G+0.0722*B;
	double gray2 = 0.2126*v.R+0.7152*v.G+0.0722*v.B;

	return (gray1 < gray2);
}

bool
vpRGBa::operator>(const vpRGBa &v) const
{
	double gray1 = 0.2126*R+0.7152*G+0.0722*B;
	double gray2 = 0.2126*v.R+0.7152*v.G+0.0722*v.B;

	return (gray1 > gray2);
}

vpRGBa operator*(const double &x, const vpRGBa  &rgb)
{
	return rgb*x;
}

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
