/****************************************************************************
 *
 * $Id: vpMatrix.h 3842 2012-07-13 22:21:42Z fspindle $
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
 * Matrix manipulation.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/



#ifndef vpMatrix_H
#define vpMatrix_H

#include <visp/vpConfig.h>
#include <visp/vpTime.h>

#ifdef VISP_HAVE_GSL
#  include <gsl/gsl_math.h>
#  include <gsl/gsl_eigen.h>
#endif

#include <iostream>
#include <math.h>

class vpRowVector;
class vpColVector;
class vpTranslationVector;


class vpColVector;
class vpTranslationVector;
class vpRowVector;



/*!
  \file vpMatrix.h

  \brief Definition of matrix class as well as a set of operations on
  these matrices.
*/



/*!
  \class vpMatrix
  \ingroup Matrix

  \brief Definition of the vpMatrix class.

  vpMatrix class provides a data structure for the matrices as well
  as a set of operations on these matrices

  \author Eric Marchand (IRISA - INRIA Rennes)

  \warning Note the matrix in the class (*this) will be noted A in the comment

  \ingroup libmath

  \sa vpRowVector, vpColVector, vpHomogeneousMatrix, vpRotationMatrix,
  vpTwistMatrix, vpHomography
*/
class VISP_EXPORT vpMatrix
{
 public:
  /*!
    Method used to compute the determinant of a square matrix.
    \sa det()
  */
  typedef enum {
    LU_DECOMPOSITION     /*!< LU decomposition method. */
  } vpDetMethod;


protected:
  //! number of rows
  unsigned int rowNum;
  //! number of columns
  unsigned int colNum;

 public:
  //! address of the first element of the data array
  double *data;
 protected:
  //! address of the first element of each rows
  double **rowPtrs;

  //! Current size (rowNum * colNum)
  unsigned int dsize;
  //! Total row space
  unsigned int trsize;

 public:
  //! Basic constructor
  vpMatrix() ;
  //! Constructor. Initialization of A as an r x c matrix with 0.
  vpMatrix(unsigned int r, unsigned int c) ;

  //! sub vpMatrix constructor
  vpMatrix(const vpMatrix &m, unsigned int r, unsigned int c, 
	   unsigned int nrows, unsigned int ncols) ;

  //! Destructor (Memory de-allocation)
  virtual ~vpMatrix();

  //! Initialization of the object matrix
  void  init() ;

  //! Destruction of the matrix  (Memory de-allocation)
  void kill() ;

  // Initialize an identity matrix n-by-n
  void eye(unsigned int n) ;
  // Initialize an identity matrix m-by-n
  void eye(unsigned int m, unsigned int n) ;
  void setIdentity(const double & val=1.0) ;

  //---------------------------------
  // Set/get Matrix size
  //---------------------------------
  /** @name Set/get Matrix size  */
  //@{
  //! Return the number of rows of the matrix
  inline unsigned int getRows() const { return rowNum ;}
  //! Return the number of columns of the matrix
  inline unsigned int getCols() const { return colNum; }

  // Set the size of the matrix A, initialization with a zero matrix
  void resize(const unsigned int nrows, const unsigned int ncols, 
	      const bool nullify = true);
  
  double getMinValue() const;
  
  double getMaxValue() const;
  //@}

  //---------------------------------
  // Printing
  //---------------------------------

  friend VISP_EXPORT std::ostream &operator << (std::ostream &s,const vpMatrix &m);
  /** @name Printing  */
  //@{

  int print(std::ostream& s, unsigned int length, char const* intro=0);
  std::ostream & matlabPrint(std::ostream & os);
  std::ostream & cppPrint(std::ostream & os, const char * matrixName = NULL, bool octet = false);

  void printSize() { std::cout << getRows() <<" x " << getCols() <<"  " ; }
  //@}
  
  static bool saveMatrix(const char *filename, const vpMatrix &M, const bool binary = false, const char *Header = "");
  static bool loadMatrix(const char *filename, vpMatrix &M, const bool binary = false, char *Header = NULL);

  /*!
    Save a matrix to a file.

    \param filename : absolute file name
    \param M : matrix to be saved
    \param binary :If true the matrix is save in a binary file, else a text file.
    \param Header : optional line that will be saved at the beginning of the file

    \return Returns true if no problem appends.

    Warning : If you save the matrix as in a text file the precision is less than if you save it in a binary file.
  */
  static inline bool saveMatrix(std::string filename, const vpMatrix &M, 
				const bool binary = false, 
				const char *Header = "")
  {
    return vpMatrix::saveMatrix(filename.c_str(), M, binary, Header);
  }

  /*!
    Load a matrix to a file.

    \param filename : absolute file name
    \param M : matrix to be loaded
    \param binary :If true the matrix is load from a binary file, else from a text file.
    \param Header : Header of the file is load in this parameter

    \return Returns true if no problem appends.
  */
  static inline bool loadMatrix(std::string filename, vpMatrix &M, 
				const bool binary = false, char *Header = NULL)
  {
    return vpMatrix::loadMatrix(filename.c_str(), M, binary, Header);
  }

  //---------------------------------
  // Copy / assignment
  //---------------------------------
  /** @name Copy / assignment  */
  //@{
  //! Copy constructor
  vpMatrix (const vpMatrix& m);

  // Assignment from an array
  vpMatrix &operator<<(double*);

  //! Copy operator.   Allow operation such as A = B
  vpMatrix &operator=(const vpMatrix &B);
  //! Set all the element of the matrix A to x
  vpMatrix &operator=(const double x);
  void diag(const vpColVector &A);
  //@}

  //---------------------------------
  // Access/modification operators
  //---------------------------------
  /** @name Access/modification operators  */
  //@{
  //! write elements Aij (usage : A[i][j] = x )
  inline double *operator[](unsigned int n) { return rowPtrs[n]; }
  //! read elements Aij  (usage : x = A[i][j] )
  inline double *operator[](unsigned int n) const {return rowPtrs[n];}
  //@}

  //---------------------------------
  // Matrix operations (Static).
  //---------------------------------

  static void mult2Matrices(const vpMatrix &A, const vpMatrix &B, vpMatrix &C);
  static void add2Matrices(const vpMatrix &A, const vpMatrix &B, vpMatrix &C);
  static void add2WeightedMatrices(const vpMatrix &A, const double &wA, const vpMatrix &B,const double &wB, vpMatrix &C);
  static void sub2Matrices(const vpMatrix &A, const vpMatrix &B, vpMatrix &C);
  static void negateMatrix(const vpMatrix &A, vpMatrix &C);
  static void multMatrixVector(const vpMatrix &A, const vpColVector &b, vpColVector &c);
  
  static vpMatrix computeCovarianceMatrix(const vpMatrix &A, const vpColVector &x, const vpColVector &b);
  static vpMatrix computeCovarianceMatrix(const vpMatrix &A, const vpColVector &x, const vpColVector &b, const vpMatrix &w);

  //---------------------------------
  // Matrix operations.
  //---------------------------------
  /** @name Matrix operations  */
  //@{
  // operation A = A + B
  vpMatrix &operator+=(const vpMatrix &B);
  // operation A = A - B
  vpMatrix &operator-=(const vpMatrix &B);

  vpMatrix operator*(const vpMatrix &B) const;
  vpMatrix operator+(const vpMatrix &B) const;
  vpMatrix operator-(const vpMatrix &B) const;
  vpMatrix operator-() const;

  //---------------------------------
  // Matrix/vector operations.
  //---------------------------------

  vpColVector operator*(const vpColVector &b) const;
  // operation c = A * b (A is unchanged, c and b are translation vectors)
  vpTranslationVector operator*(const vpTranslationVector  &b) const;
  //---------------------------------
  // Matrix/real operations.
  //---------------------------------

  //! Add x to all the element of the matrix : Aij = Aij + x
  vpMatrix &operator+=(const double x);
  //! Substract x to all the element of the matrix : Aij = Aij - x
  vpMatrix &operator-=(const double x);
  //! Multiply  all the element of the matrix by x : Aij = Aij * x
  vpMatrix &operator*=(const double x);
  //! Divide  all the element of the matrix by x : Aij = Aij / x
  vpMatrix &operator/=(double x);

  // Cij = Aij * x (A is unchanged)
  vpMatrix operator*(const double x) const;
  // Cij = Aij / x (A is unchanged)
  vpMatrix operator/(const double x) const;

  //!return sum of the Aij^2 (for all i, for all j)
  double sumSquare() const;

  // return the determinant of the matrix.
  double det(vpDetMethod method = LU_DECOMPOSITION) const;
  
  //Compute the exponential matrix of a square matrix
  vpMatrix expm();

  //-------------------------------------------------
  // Columns, Rows extraction, SubMatrix
  //-------------------------------------------------
  /** @name Columns, Rows extraction, Submatrix  */
  //@{
  //! Row extraction
  vpRowVector row(const unsigned int i);
  //! Column extraction
  vpColVector column(const unsigned int j);
  //! subvpMatrix extraction
  void init(const vpMatrix &m, unsigned int r, unsigned int c, 
	    unsigned int nrows, unsigned int ncols);
  //@}

  //-------------------------------------------------
  // transpose
  //-------------------------------------------------
  /** @name Transpose, Identity  */
  //@{
  // Compute the transpose C = A^T
  vpMatrix t() const;

  // Compute the transpose C = A^T
  vpMatrix transpose()const;
  void  transpose(vpMatrix & C )const;
    
  vpMatrix AAt() const;
  void AAt(vpMatrix &B) const;
   
  vpMatrix AtA() const;
  void AtA(vpMatrix &B) const;
  //@}


  //-------------------------------------------------
  // Kronecker product
  //-------------------------------------------------
  /** @name Kronecker product  */
  //@{
  
  // Stacks columns of a matrix in a vector
  void stackColumns(vpColVector  &out );

  // Stacks columns of a matrix in a vector
  vpColVector stackColumns();

  // Stacks columns of a matrix in a vector
  void stackRows(vpRowVector  &out );

  // Stacks columns of a matrix in a vector
  vpRowVector stackRows();
  
  // Compute Kronecker product matrix 
  void kron(const vpMatrix  &m1 , vpMatrix  &out);
  
  // Compute Kronecker product matrix 
  vpMatrix kron(const vpMatrix  &m1);
  //@}
  
  // Compute Kronecker product matrix 
  static void kron(const vpMatrix  &m1, const vpMatrix  &m2 , vpMatrix  &out);

  // Compute Kronecker product matrix 
  static vpMatrix kron(const vpMatrix  &m1, const vpMatrix  &m2 );


  //-------------------------------------------------
  // LU decomposition
  //-------------------------------------------------
  /** @name LU decomposition  */
  //@{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  //! LU Decomposition
  void LUDcmp(unsigned int* perm, int& d);
  //! solve AX = B using the LU Decomposition
  void LUBksb(unsigned int* perm, vpColVector& b);
#endif // doxygen should skip this

  // inverse matrix A using the LU decomposition 
  vpMatrix inverseByLU() const;
  //@}

  //-------------------------------------------------
  // SVD decomposition
  //-------------------------------------------------

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  void svdFlake(vpColVector& w, vpMatrix& v);
  void svdNr(vpColVector& w, vpMatrix& v);
#ifdef VISP_HAVE_GSL
  void svdGsl(vpColVector& w, vpMatrix& v);
#endif
#if (VISP_HAVE_OPENCV_VERSION >= 0x020100) // Require opencv >= 2.1.0
  void svdOpenCV(vpColVector& w, vpMatrix& v);
#endif
#ifdef VISP_HAVE_LAPACK
  void svdLapack(vpColVector& W, vpMatrix& V);
#endif
  //! solve AX=B using the SVD decomposition
  void SVBksb(const vpColVector& w,
  	      const vpMatrix& v,
  	      const vpColVector& b, vpColVector& x);
#endif

  /** @name SVD decomposition  */
  //@{
  // singular value decomposition SVD

  void svd(vpColVector& w, vpMatrix& v);

  // solve Ax=B using the SVD decomposition (usage A = solveBySVD(B,x) )
  void solveBySVD(const vpColVector &B, vpColVector &x) const ;
  // solve Ax=B using the SVD decomposition (usage  x=A.solveBySVD(B))
  vpColVector solveBySVD(const vpColVector &B) const ;

  //! Compute the pseudo inverse of the matrix using the SVD.
  vpMatrix pseudoInverse(double svThreshold=1e-6)  const;
  //! Compute the pseudo inverse of the matrix using the SVD.
  //! return the rank
  unsigned int pseudoInverse(vpMatrix &Ap, double svThreshold=1e-6)  const;
  //! Compute the pseudo inverse of the matrix using the SVD.
  //! return the rank and the singular value
  unsigned int pseudoInverse(vpMatrix &Ap, vpColVector &sv, double svThreshold=1e-6) const ;
  //! Compute the pseudo inverse of the matrix using the SVD.
  //! return the rank and the singular value, image
  unsigned int pseudoInverse(vpMatrix &Ap,
			     vpColVector &sv, double svThreshold,
			     vpMatrix &ImA,
			     vpMatrix &ImAt) const ;
  //! Compute the pseudo inverse of the matrix using the SVD.
  //! return the rank and the singular value, image, kernel.
  unsigned int pseudoInverse(vpMatrix &Ap,
			     vpColVector &sv, double svThreshold,
			     vpMatrix &ImA,
			     vpMatrix &ImAt,
			     vpMatrix &kerA) const ;

  unsigned int kernel(vpMatrix &KerA, double svThreshold=1e-6);
  //@}

  //-------------------------------------------------
  // Eigen values and vectors
  //-------------------------------------------------

  /** @name Eigen values  */

  //@{
  // compute the eigen values using the Gnu Scientific library
  vpColVector eigenValues();
  void eigenValues(vpColVector &evalue, vpMatrix &evector);
  //@}

  //! Stack two Matrices C = [ A B ]^T
  static vpMatrix stackMatrices(const vpMatrix &A,const  vpMatrix &B) ;
  //! Stack two Matrices C = [ A B ]^T
  static void stackMatrices(const vpMatrix &A,const  vpMatrix &B, vpMatrix &C) ;
  // Juxtapose to matrices C = [ A B ]
  static vpMatrix juxtaposeMatrices(const vpMatrix &A,const  vpMatrix &B) ;
  // Juxtapose to matrices C = [ A B ]
  static void juxtaposeMatrices(const vpMatrix &A,const  vpMatrix &B, vpMatrix &C) ;

  // Create a diagonal matrix with the element of a vector DAii = Ai
  static void createDiagonalMatrix(const vpColVector &A, vpMatrix &DA)  ;
  
  // Stack the matrix A below the current one, copy if not initialized this = [ this A ]^T
  void stackMatrices(const vpMatrix &A);

  // Insert matrix A in the current matrix at the given position (r, c). 
  void insert(const vpMatrix&A, const unsigned int r, const unsigned int c);
  // Insert matrix B in matrix A at the given position (r, c). 
  static vpMatrix insert(const vpMatrix &A,const  vpMatrix &B, const unsigned int r, const unsigned int c) ;
  // Insert matrix B in matrix A (not modified) at the given position (r, c), the result is given in matrix C. 
  static void insert(const vpMatrix &A, const vpMatrix &B, vpMatrix &C, const unsigned int r, const unsigned int c) ;

  // -------------------------
  // Norms
  // -------------------------
  /** @name Norms  */
  //@{
  // Euclidean norm ||x||=sqrt(sum(x_i^2))
  double euclideanNorm () const;
  // Infinity norm ||x||=max(sum(fabs(x_i)))
  double infinityNorm () const;
  //@}

 private:
  double detByLU() const;

};


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////


//! multiplication by a scalar C = x*A
VISP_EXPORT vpMatrix operator*(const double &x, const vpMatrix &A) ;

  //! multiplication by a scalar C = x*A
VISP_EXPORT vpColVector operator*(const double &x, const vpColVector &A) ;



#endif


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
