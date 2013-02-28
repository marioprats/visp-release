/****************************************************************************
 *
 * $Id: vpHomographyDLT.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
 * Homography estimation.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


/*!
  \file vpHomographyDLT.cpp

  This file implements the fonctions related with the homography
  estimation using the DLT algorithm
*/

#include <visp/vpHomography.h>

#include <cmath>    // std::fabs
#include <limits>   // numeric_limits

void
vpHomography::HartleyNormalization(unsigned int n,
				   double *x, double *y,
				   double *xn, double *yn,
				   double &xg, double &yg,
				   double &coef)
{
  unsigned int i;
  xg = 0 ;
  yg = 0 ;

  for (i =0 ; i < n ; i++)
  {
    xg += x[i] ;
    yg += y[i] ;
  }
  xg /= n ;
  yg /= n ;

  //Changement d'origine : le centre de gravit� doit correspondre
  // � l'origine des coordonn�es
  double distance=0;
  for(i=0; i<n;i++)
  {
    double xni=x[i]-xg;
    double yni=y[i]-yg;
    xn[i] = xni ;
    yn[i] = yni ;
    distance+=sqrt(vpMath::sqr(xni)+vpMath::sqr(yni));
  }//for translation sur tous les points

  //Changement d'�chelle
  distance/=n;
  //calcul du coef de changement d'�chelle
  //if(distance ==0)
  if(std::fabs(distance) <= std::numeric_limits<double>::epsilon())
    coef=1;
  else
    coef=sqrt(2.0)/distance;

  for(i=0; i<n;i++)
  {
    xn[i] *= coef;
    yn[i] *= coef;
  }

}

//---------------------------------------------------------------------------------------

void
vpHomography::HartleyDenormalization(vpHomography &aHbn,
				     vpHomography &aHb,
				     double xg1, double yg1, double coef1,
				     double xg2, double yg2, double coef2 )
{

  //calcul des transformations � appliquer sur M_norm pour obtenir M
  //en fonction des deux normalisations effectu�es au d�but sur
  //les points: aHb = T2^ aHbn T1
  vpMatrix T1(3,3);
  vpMatrix T2(3,3);
  vpMatrix T2T(3,3);

  T1.setIdentity();
  T2.setIdentity();
  T2T.setIdentity();

  T1[0][0]=T1[1][1]=coef1;
  T1[0][2]=-coef1*xg1 ;
  T1[1][2]=-coef1*yg1 ;

  T2[0][0]=T2[1][1]=coef2;
  T2[0][2]=-coef2*xg2 ;
  T2[1][2]=-coef2*yg2 ;


  T2T=T2.pseudoInverse(1e-16) ;

  vpMatrix maHb=T2T*(vpMatrix)aHbn*T1;

  for (unsigned int i=0 ; i < 3 ; i++)
    for (unsigned int j=0 ; j < 3 ; j++) aHb[i][j] = maHb[i][j] ;

}


/*!

  \brief Computes the homography matrix using the DLT (Direct Linear
  Transform) algorithm on normalized data.
 
  Normalizes data, computes H wrt. these normalized data and denormalizes
  the result. The normalization carried out is the one preconized by Hartley .
  At least 4 correspondant points couples are needed.

  \sa DLT()
*/
void
vpHomography::HartleyDLT(unsigned int n,
			 double *xb, double *yb,
			 double *xa, double *ya ,
			 vpHomography &aHb)
{
  try{
    //initialise les donn�es initiales
    // code_retour =InitialData(n, p1,p2);

    // normalize points
    double *xbn;
    double *ybn;
    xbn = new double [n];
    ybn = new double [n];

    double xg1, yg1, coef1 ;
    vpHomography::HartleyNormalization(n,
				       xb,yb,
				       xbn,ybn,
				       xg1, yg1,coef1);

    double *xan;
    double *yan;
    xan = new double [n];
    yan = new double [n];

    double xg2, yg2, coef2 ;
    vpHomography::HartleyNormalization(n,
				       xa,ya,
				       xan,yan,
				       xg2, yg2,coef2);

    vpHomography aHbn ;
    //compute the homography using the DLT from normalized data
    vpHomography::DLT(n,xbn,ybn,xan,yan,aHbn);

    //H d�normalis�e
    vpHomography::HartleyDenormalization(aHbn,aHb,xg1,yg1,coef1,xg2,yg2, coef2);

    delete [] xbn;
    delete [] ybn;
    delete [] xan;
    delete [] yan;

  }
  catch(...)
  {
    vpTRACE(" ") ;
    throw ;
  }
}


/*!
  \brief Computes the homography matrix wrt. the
  data using the DLT (Direct Linear Transform) algorithm.

  Computes H such as  \f[
  ^a{\bf p} = ^a{\bf H}_b\; ^b{\bf p}
  \f]

  To do so, we use the DLT algorithm on the data,
  ie we resolve the linear system  by SDV : \f$\bf{Ah} =0\f$.
  \f$\bf{h}\f$ is the vector with the terms of \f$\mathbf{H}\f$,

  \f$\mathbf{A}\f$ depends on the  points coordinates.

  At least 4 correspondant points couples are needed.


  For each point, in homogeneous coordinates we have:
  \f[
  \mathbf{p}_{a}= \mathbf{H}\mathbf{p}_{b}
  \f]
  which is equivalent to:
  \f[
  \mathbf{p}_{a} \times \mathbf{H}\mathbf{p}_{b}  =0
  \f]
  If we note \f$\mathbf{h}_j^T\f$ the  \f$j^{\textrm{th}}\f$ line of  \f$\mathbf{H}\f$, we can write:
  \f[
  \mathbf{H}\mathbf{p}_{b}  = \left( \begin{array}{c}\mathbf{h}_1^T\mathbf{p}_{b} \\\mathbf{h}_2^T\mathbf{p}_{b} \\\mathbf{h}_3^T \mathbf{p}_{b} \end{array}\right)
  \f]

  Setting \f$\mathbf{p}_{a}=(x_{a},y_{a},w_{a})\f$, the cross product  can be rewritten by:
  \f[
  \mathbf{p}_{a} \times \mathbf{H}\mathbf{p}_{b}  =\left( \begin{array}{c}y_{a}\mathbf{h}_3^T\mathbf{p}_{b}-w_{a}\mathbf{h}_2^T\mathbf{p}_{b} \\w_{a}\mathbf{h}_1^T\mathbf{p}_{b} -x_{a}\mathbf{h}_3^T \mathbf{p}_{b} \\x_{a}\mathbf{h}_2^T \mathbf{p}_{b}- y_{a}\mathbf{h}_1^T\mathbf{p}_{b}\end{array}\right)
  \f]


  \f[
  \underbrace{\left( \begin{array}{ccc}\mathbf{0}^T & -w_{a} \mathbf{p}_{b}^T
  & y_{a} \mathbf{p}_{b}^T     \\     w_{a}
  \mathbf{p}_{b}^T&\mathbf{0}^T & -x_{a} \mathbf{p}_{b}^T      \\
  -y_{a} \mathbf{p}_{b}^T & x_{a} \mathbf{p}_{b}^T &
  \mathbf{0}^T\end{array}\right)}_{\mathbf{A}_i (3\times 9)}
  \underbrace{\left( \begin{array}{c}\mathbf{h}_{1}^{T}      \\
  \mathbf{h}_{2}^{T}\\\mathbf{h}_{3}^{T}\end{array}\right)}_{\mathbf{h} (9\times 1)}=0
  \f]

  leading to an homogeneous system to be solve:   \f$\mathbf{A}\mathbf{h}=0\f$ with
  \f$\mathbf{A}=\left(\mathbf{A}_1^T, ..., \mathbf{A}_i^T, ..., \mathbf{A}_n^T \right)^T\f$.

  It can be solved using an SVD decomposition:
  \f[\bf A = UDV^T \f]
  <b>h</b> is the column of <b>V</b> associated with the smalest singular value of <b>A
  </b>

*/
void vpHomography::DLT(unsigned int n,
		       double *xb, double *yb,
		       double *xa, double *ya ,
		       vpHomography &aHb)
{

  // 4 point are required
  if(n<4)
  {
    vpTRACE("there must be at least 4 points in the both images\n") ;
    throw ;
  }

  try{
    vpMatrix A(2*n,9);
    vpColVector h(9);
    vpColVector D(9);
    vpMatrix V(9,9);
    unsigned int i, j;
    
    // We need here to compute the SVD on a (n*2)*9 matrix (where n is
    // the number of points). if n == 4, the matrix has more columns
    // than rows. This kind of matrix is not supported by GSL for
    // SVD. The solution is to add an extra line with zeros
    if (n == 4) 
      A.resize(2*n+1,9);

    // build matrix A
    for(i=0; i<n;i++)
    {

      A[2*i][0]=0;
      A[2*i][1]=0;
      A[2*i][2]=0;
      A[2*i][3]=-xb[i] ;
      A[2*i][4]=-yb[i] ;
      A[2*i][5]=-1;
      A[2*i][6]=xb[i]*ya[i] ;
      A[2*i][7]=yb[i]*ya[i];
      A[2*i][8]=ya[i];


      A[2*i+1][0]=xb[i] ;
      A[2*i+1][1]=yb[i] ;
      A[2*i+1][2]=1;
      A[2*i+1][3]=0;
      A[2*i+1][4]=0;
      A[2*i+1][5]=0;
      A[2*i+1][6]=-xb[i]*xa[i];
      A[2*i+1][7]=-yb[i]*xa[i];
      A[2*i+1][8]=-xa[i] ;
    }

    // Add an extra line with zero.
    if (n == 4) {
      for (int i=0; i < 9; i ++) {
	A[2*n][i] = 0;
      }
    }      

    // solve Ah = 0
    // SVD  Decomposition A = UDV^T (destructive wrt A)
    A.svd(D,V);

    // on en profite pour effectuer un controle sur le rang de la matrice :
    // pas plus de 2 valeurs singuli�res quasi=0
    int rank=0;
    for(i = 0; i<9;i++) if(D[i]>1e-7) rank++;
    if(rank <7)
    {
      vpTRACE(" le rang est de : %d, shoud be 8", rank);
      throw ;
    }
    //h = is the column of V associated with the smallest singular value of A

    // since  we are not sure that the svd implemented sort the
    // singular value... we seek for the smallest
    double smallestSv = 1e30 ;
    unsigned int indexSmallestSv  = 0 ;
    for (i=0 ; i < 9 ; i++)
      if ((D[i] < smallestSv) ){ smallestSv = D[i] ;indexSmallestSv = i ; }


    h=V.column(indexSmallestSv+1);

    // build the homography
    for(i =0;i<3;i++)
    {
      for(j=0;j<3;j++)
	aHb[i][j]=h[3*i+j];
    }

  }
  catch(...)
  {
    vpTRACE(" ") ;
    throw ;
  }
}
