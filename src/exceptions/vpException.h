/****************************************************************************
 *
 * $Id: vpException.h 3530 2012-01-03 10:52:12Z fspindle $
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
 * Exception handling.
 *
 * Authors:
 * Nicolas Mansard
 *
 *****************************************************************************/


/* \file vpException.h
   \brief error that can be emited by the vp class and its derivates
 */

#ifndef __vpException_H
#define __vpException_H


/* --------------------------------------------------------------------- */
/* --- INCLUDE --------------------------------------------------------- */
/* --------------------------------------------------------------------- */

#include <visp/vpConfig.h>

/* Classes standards. */
#include <iostream>                /* Classe std::ostream.    */
#include <string>                  /* Classe string.     */

/* --------------------------------------------------------------------- */
/* --- CLASS ----------------------------------------------------------- */
/* --------------------------------------------------------------------- */


/* \class vpException
   \brief error that can be emited by the vp class and its derivates

    This class inherites from the standard std::exception contained in the C++
    STL.
    It is therefore possible to catch vpException with any other derivative of
    std::exception in the same catch.
 */
class VISP_EXPORT vpException : public std::exception
{

private:

  //! Contains the error code, see the errorCodeEnum table for details.
  int code;

  //! Contains an error message (can be empty)
  std::string message;

private:

  //!  forbid the empty constructor (private)
  vpException();

public:

  enum generalExceptionEnum
    {
      memoryAllocationError,
      memoryFreeError,
      functionNotImplementedError,
      ioError,
      cannotUseConstructorError,
      notImplementedError,
      divideByZeroError,
      dimensionError,
      fatalError,
      badValue, /*!< Used to indicate that a value is not in the allowed range. */
      notInitialized /*!< Used to indicate that a parameter is not initialized. */
    } ;

  vpException (const int code, const char * msg);
  vpException (const int code, const std::string & msg);
  vpException (const int code);

  /*!
    Basic destructor. Do nothing but implemented to fit the inheritance from
    std::exception
  */
  virtual ~vpException() throw() {}

  //!  send the object code
  int getCode (void);

  //! send a reference (constant) related the error message (can be empty)
  const std::string &getStringMessage (void);
  //! send a pointer on the array of  \e char related to the error string.
  //!Cannot be  \e NULL.
  const char *getMessage (void);

  //! print the error structure
  friend VISP_EXPORT std::ostream & operator << (std::ostream & os,
				const vpException & art);

  const char* what () const throw();

};





#endif /* #ifndef __vpException_H */


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
