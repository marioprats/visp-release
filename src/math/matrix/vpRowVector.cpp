/****************************************************************************
 *
 * $Id: vpRowVector.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
 * Operation on row vectors.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


/*!
  \file vpRowVector.cpp
  \brief Definition of vpRowVector class member
*/


#include <visp/vpMatrix.h>
#include <visp/vpMatrixException.h>
#include <visp/vpRowVector.h>
#include <visp/vpColVector.h>
#include <visp/vpDebug.h>
#include <string.h>
#include <stdlib.h>
//! Copy operator.   Allow operation such as A = v
vpRowVector & vpRowVector::operator=(const vpRowVector &v)
{
  if (colNum==0)
    resize(v.getCols());

  for (unsigned int i=0; i<rowNum; i++) {
    for (unsigned int j=0; j<colNum; j++) {
      rowPtrs[i][j] = v.rowPtrs[i][j];
    }
  }
  return *this;
}

/*!
  \brief copy from a matrix
  \warning  Handled with care m should be a 1 column matrix
*/
vpRowVector & vpRowVector::operator=(const vpMatrix &m)
{
  if (m.getCols() != colNum)
    resize(m.getCols());

  memcpy(data, m.data, colNum*sizeof(double)) ;
  /*
  for (int i=0; i<rowNum; i++) {
    for (int j=0; j<colNum; j++) {
      rowPtrs[i][j] = m.rowPtrs[i][j];
    }
    }*/
  return *this;
}

//! initialisation each element of the vector is x
vpRowVector & vpRowVector::operator=(double x)
{
  for (unsigned int i=0; i<rowNum; i++) {
    for (unsigned int j=0; j<colNum; j++) {
      rowPtrs[i][j] = x;
    }
  }
  return *this;
}

/*!

  Multiply a row vector by a column vector.

  \param x : Column vector.

  \warning The number of elements of the two vectors must be equal.

  \exception vpMatrixException::matrixError : If the number of elements of the
  two vectors is not the same.

  \return A scalar.

*/
double vpRowVector::operator*(const vpColVector &x) const
{
  unsigned int nelements = x.getRows();
  if (getCols() != nelements ) {
    vpERROR_TRACE("\n\t\t Illegal matrix operation") ;
    throw(vpMatrixException(vpMatrixException::matrixError,
			    "\n\t\t Illegal matrix operation, bad vector size")) ;
  }

  double scalar = 0.0;

  for (unsigned int i=0; i<nelements; i++) {
    scalar += (*this)[i] * x[i];
  }
  return scalar;
}
/*!

  Multiply a row vector by a Matrix.

  \param A : Matrix.

  \warning The number of elements of the rowVector must be equal to the number
  of rows of the matrix.

  \exception vpMatrixException::matrixError : If the number of elements of the
  rowVector is not equal to the number of rows of the matrix.

  \return A vpRowVector.

*/
vpRowVector vpRowVector::operator*(const vpMatrix &A) const
{

  vpRowVector c(A.getCols());

  if (colNum != A.getRows())
  {
    vpERROR_TRACE("vpMatrix mismatch in vpRowVector/matrix multiply") ;
    throw(vpMatrixException::incorrectMatrixSizeError) ;
  }

  c = 0.0;

  for (unsigned int i=0;i<colNum;i++) {
    {
      double bi = data[i] ; // optimization em 5/12/2006
      for (unsigned int j=0;j<A.getCols();j++) {
        c[j]+=bi*A[i][j];
      }
    }
  }

  return c ;
}

/*!
  \brief Transpose the row vector A

  A is defined inside the class

  \return  A^T
*/
vpColVector vpRowVector::t() const
{
  vpColVector tmp(colNum);
  memcpy(tmp.data, data, colNum*sizeof(double)) ;
  /*
  for (int i=0;i<colNum;i++)
      tmp[i] = (*this)[i];
  */
  return tmp;
}

//! copy constructor
vpRowVector::vpRowVector (const vpRowVector &v) : vpMatrix(v)
{
}

//! Constructor  (Take line i of matrix m)
vpRowVector::vpRowVector (vpMatrix &m, unsigned int i) : vpMatrix(m, i, 0, 1, m.getCols())
{
}

/*!
  \relates vpRowVector
  \brief normalise the vector

  \f[
  {\bf x}_i = \frac{{\bf x}_i}{\sqrt{\sum_{i=1}^{n}x^2_i}}
  \f]
*/
vpRowVector &vpRowVector::normalize(vpRowVector &x) const
{
  x = x/sqrt(x.sumSquare());

  return x;
}


/*!
  \relates vpRowVector
  \brief normalise the vector

  \f[
  {\bf x}_i = \frac{{\bf x}_i}{\sqrt{\sum_{i=1}^{n}x^2_i}}
  \f]
*/
vpRowVector &vpRowVector::normalize()
{

  double sum = sumSquare() ;
  *this /= sum ;

  return *this;
}

/*!
  \brief reshape the row vector in a matrix
  \param nrows : number of rows of the matrix
  \param ncols : number of columns of the matrix
  \return a vpMatrix
*/
vpMatrix vpRowVector::reshape(const unsigned int &nrows,const unsigned int &ncols){
  vpMatrix m(nrows,ncols);
  reshape(m,nrows,ncols);
  return m;
}

/*!
  \brief reshape the row vector in a matrix
  \param m : the reshaped Matrix
  \param nrows : number of rows of the matrix
  \param ncols : number of columns of the matrix
*/
void vpRowVector::reshape(vpMatrix & m,const unsigned int &nrows,const unsigned int &ncols){
  if(dsize!=nrows*ncols)
  {
    vpERROR_TRACE("\n\t\t vpSubRowVector mismatch size for reshape vpSubColVector in a vpMatrix") ;
    throw(vpMatrixException(vpMatrixException::incorrectMatrixSizeError,
			    "\n\t\t \n\t\t vpSubRowVector mismatch size for reshape vpSubColVector in a vpMatrix")) ;
  }
  try 
  {
    if ((m.getRows() != nrows) || (m.getCols() != ncols)) m.resize(nrows,ncols);
  }
  catch(vpException me)
  {
    vpERROR_TRACE("Error caught") ;
    std::cout << me << std::endl ;
    throw ;
  }
     for(unsigned int i =0; i< nrows; i++)
         for(unsigned int j =0; j< ncols; j++)
         	  m[i][j]=data[i*nrows+j];
}
