/****************************************************************************
 *
 * $Id: vpHomographyMalis.cpp 3530 2012-01-03 10:52:12Z fspindle $
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
  \file vpHomographyMalis.cpp

  This file implements the fonctions related with the homography
  estimation from non planar points using the Malis algorithm

  references

  E. Malis, F. Chaumette. 2 1/2 D visual servoing with respect to unknown
  objects through a new estimation scheme of camera displacement. Int. Journal
  of Computer Vision, 37(1):79-97, Juin 2000.

  \@article{Malis00b,
  Author = {Malis, E. and Chaumette, F.},
  Title = {2 1/2 D visual servoing with respect to unknown objects
  through a new estimation scheme of camera displacement},
  Journal = {Int. Journal of Computer Vision},
  Volume = {37},
  Number = {1},
  Pages = {79--97},
  Month = {June},
  Year = {2000}
  }

  paper can be obtained at this url :
  http://www.irisa.fr/lagadic/publi/publi/Malis00b-fra.html

  the algorithm for 2D scene implemented in this file is described in Ezio
  Malis PhD thesis

  E. Malis. Contributions � la mod�lisation et � la commande en asservissement
  visuel.
  Th�se de l'Universite de Rennes 1, T�l�communications et Traitement du Signal,
  Novembre 1998.
  http://www.irisa.fr/lagadic/publi/publi/TheseMalis-fra.html
*/

#include <visp/vpHomography.h>
#include <visp/vpDebug.h>
#include <visp/vpMatrixException.h>

#include <cmath>    // std::fabs
#include <limits>   // numeric_limits

#ifndef DOXYGEN_SHOULD_SKIP_THIS
const double eps = 1e-6 ;

/**************************************************************************
 * NOM :
 * changeFrame
 *
 * DESCRIPTION :
 * Changement de repere Euclidien.
 *
 ****************************************************************************
 * ENTREES :
 * int pts_ref[4]	: Definit quels sont les points de reference, ils ne
 *			  seront pas affectes par le changement de repere
 * int nb_pts		: nombre de points a changer de repere
 * double **pd	: La matrice des coordonnees des points desires
 * double **p	: La matrice des coordonnees des points courants
 *
 *
 * SORTIES :
 *
 * double **pt_des_nr 	: La matrice des coordonnees des points desires
 *			  dans le nouveau repere.
 * double **pt_cour_nr	: La matrice des coordonnees des points courants
 *			  dans le nouveau repere
 * double **M	: ??
 * double **Mpd	: pseudo inverse de M  ..
 *
 *
 ****************************************************************************
 */


void changeFrame(unsigned int *pts_ref,
		 unsigned int nb_pts,
		 vpMatrix &pd, vpMatrix &p,
		 vpMatrix &pnd, vpMatrix &pn,
		 vpMatrix &M, vpMatrix &Mdp)
{



  unsigned int i,j, k ;
  unsigned int cont_pts;		/* */
  double lamb_des[3];	/* */
  double lamb_cour[3] ; /* */



  /* Construction des matrices de changement de repere */
  vpMatrix Md(3,3) ;
  vpMatrix Mp(3,3) ;

  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      M[j][i] = p[pts_ref[i]][j] ;
      Md[j][i]  = pd[pts_ref[i]][j]  ;
    }
  }

  /*calcul de la pseudo inverse  */
  Mp= M.pseudoInverse(1e-16) ;
  Mdp = Md.pseudoInverse(1e-16) ;

  if (pts_ref[3] > 0) {
    for (i=0;i<3;i++) {
      for (j=0;j<3;j++) {
	lamb_cour[i] = Mp[i][j]*p[pts_ref[3]][j] ;
	lamb_des[i]  = Mdp[i][j]*pd[pts_ref[3]][j] ;
      }
    }

    for (i=0;i<3;i++) {
      for (j=0;j<3;j++) {
	M[i][j] = M[i][j]*lamb_cour[j] ;
	Md[i][j] = Md[i][j]*lamb_des[j] ;
      }
    }

    Mdp = Md.pseudoInverse(1e-16);
  }


  /* changement de repere pour tous les points autres que
     les trois points de reference */

  cont_pts = 0 ;
  for (k=0;k<nb_pts;k++) {
    if ((pts_ref[0] != k) && (pts_ref[1] != k) && (pts_ref[2] != k)) {
      for (i=0;i<3;i++) {
	pn[cont_pts][i] = 0.0 ;
	pnd[cont_pts][i]  = 0.0 ;
	for (j=0;j<3;j++) {
	  pn[cont_pts][i] = pn[cont_pts][i] + Mp[i][j]*p[k][j] ;
	  pnd[cont_pts][i]  = pnd[cont_pts][i]  + Mdp[i][j]*pd[k][j]   ;
	}
      }
      cont_pts = cont_pts + 1;
    }
  }


}


/**************************************************************************
 * NOM :
 * Homographie_CrvMafEstHomoPointsCible2D
 *
 * DESCRIPTION :
 * Calcul de l'homographie entre une image courante et une image desiree dans le
 * cas particulier d'une cible planaire de points (cible pi).
 * Cette procedure est appellee par "Homographie_CrvMafCalculHomographie".
 *
 ****************************************************************************
 * ENTREES :
 * int 	Nb_pts : nombre de points
 * double	**pd : tableau des coordonnees des points desires
 * couble	**p : tableau des coordonnees des points courants
 *
 * SORTIES :
 *
 * double **H 			matrice d homographie
 *
 ****************************************************************************
 * AUTEUR : BOSSARD Nicolas.  INSA Rennes 5eme annee.
 *
 * DATE DE CREATION : 02/12/98
 *
 * DATES DE MISE A JOUR :
 *
 ****************************************************************************/
void
HLM2D(unsigned int nb_pts,
      vpMatrix &points_des,
      vpMatrix &points_cour,
      vpMatrix &H)
{
  unsigned int i,j ;

  double  vals_inf ;
  unsigned int  contZeros, vect;

  /** allocation des matrices utilisees uniquement dans la procedure **/
  vpMatrix M(3*nb_pts,9) ;
  vpMatrix V(9,9) ;
  vpColVector sv(9) ;

  /** construction de la matrice M des coefficients dans le cas general **/
  for (j=0; j<nb_pts ;j++) {
    M[3*j][0] =  0 ;
    M[3*j][1] =  0 ;
    M[3*j][2] =  0 ;
    M[3*j][3] = -points_des[j][0]*points_cour[j][2] ;
    M[3*j][4] = -points_des[j][1]*points_cour[j][2] ;
    M[3*j][5] = -points_des[j][2]*points_cour[j][2] ;
    M[3*j][6] =  points_des[j][0]*points_cour[j][1] ;
    M[3*j][7] =  points_des[j][1]*points_cour[j][1] ;
    M[3*j][8] =  points_des[j][2]*points_cour[j][1] ;

    M[3*j+1][0] =  points_des[j][0]*points_cour[j][2] ;
    M[3*j+1][1] =  points_des[j][1]*points_cour[j][2] ;
    M[3*j+1][2] =  points_des[j][2]*points_cour[j][2] ;
    M[3*j+1][3] =  0 ;
    M[3*j+1][4] =  0 ;
    M[3*j+1][5] =  0 ;
    M[3*j+1][6] = -points_des[j][0]*points_cour[j][0] ;
    M[3*j+1][7] = -points_des[j][1]*points_cour[j][0] ;
    M[3*j+1][8] = -points_des[j][2]*points_cour[j][0] ;

    M[3*j+2][0] = -points_des[j][0]*points_cour[j][1] ;
    M[3*j+2][1] = -points_des[j][1]*points_cour[j][1] ;
    M[3*j+2][2] = -points_des[j][2]*points_cour[j][1] ;
    M[3*j+2][3] =  points_des[j][0]*points_cour[j][0] ;
    M[3*j+2][4] =  points_des[j][1]*points_cour[j][0] ;
    M[3*j+2][5] =  points_des[j][2]*points_cour[j][0] ;
    M[3*j+2][6] =  0 ;
    M[3*j+2][7] =  0 ;
    M[3*j+2][8] =  0 ;
  }

  /** calcul de la pseudo-inverse V de M et des valeurs singuli�res **/
  M.svd(sv,V);

  /*****
	La meilleure solution est le vecteur de V associe
	a la valeur singuliere la plus petite en valeur	absolu.
	Pour cela on parcourt la matrice des valeurs singulieres
	et on rep�re la plus petite valeur singuli�re, on en profite
	pour effectuer un controle sur le rang de la matrice : pas plus
	de 2 valeurs singuli�res quasi=0
  *****/
  vals_inf = fabs(sv[0]) ;
  vect = 0 ;
  contZeros = 0;
  if (fabs(sv[0]) < eps) {
    contZeros = contZeros + 1 ;
  }
  for (j=1; j<9; j++) {
    if (fabs(sv[j]) < vals_inf) {
      vals_inf = fabs(sv[j]);
      vect = j ;
    }
    if (fabs(sv[j]) < eps) {
      contZeros = contZeros + 1 ;
    }
  }


  /** cas d'erreur : plus de 2 valeurs singuli�res =0 **/
  if (contZeros > 2) {
    //vpERROR_TRACE("matrix is rank deficient");
    throw (vpMatrixException(vpMatrixException::matrixError,
			     "matrix is rank deficient"));
  }

  H.resize(3,3) ;
  /** construction de la matrice H **/
  for (i=0; i<3; i++) {
    for (j=0; j<3; j++){
      H[i][j] = V[3*i+j][vect];
    }
  }


}


/**************************************************************************
 * NOM :
 * Homographie_CrvMafEstHomoPointsC3DEzio
 *
 * DESCRIPTION :
 * Calcul de l'homographie entre une image courante et une image desiree dans le
 * cas particulier d'une cible non planaire de points (cible pi).
 * Cette procedure est appellee par "Homographie_CrvMafCalculHomographie".
 *
 *
 ****************************************************************************
 * ENTREES :
 * int 	Nb_pts : nombre de points
 * double	**pd : tableau des coordonnees des points desires
 * couble	**p : tableau des coordonnees des points courants
 *
 * SORTIES :
 *
 * double **H 			matrice d'homographie
 * double epipole[3]		epipole
 *
 ****************************************************************************
 **/
void
HLM3D(unsigned int nb_pts,
      vpMatrix &pd,
      vpMatrix &p,
      vpMatrix &H)
{
  unsigned int i,j,k,ii,jj ;
  unsigned int cont_pts;			/* Pour compter le nombre de points dans l'image */
  //unsigned int nl;			/*** Nombre de lignes ***/
  unsigned int nc ;			/*** Nombre de colonnes ***/
  unsigned int  pts_ref[4];		/*** d�finit lesquels des points de
				     l'image sont les points de r�f�rence***/
  /***  ***/
  int perm;			/***  Compte le nombre de permutations, quand le nombre
				      de permutations =0 arret de l'ordonnancement **/
  int  cont_zeros;		/*** pour compter les valeurs quasi= a zero	***/
  unsigned int  cont;
  unsigned int  vect;

  //int 	 prob;

  /***** Corps de la fonction	*****/

  /* allocation des matrices utilisees uniquement dans la procedure */
  //prob=0;

  vpMatrix M(3,3) ;
  vpMatrix Mdp(3,3) ;
  vpMatrix c(8,2) ; // matrice des coeff C

  vpColVector d(8) ;
  vpMatrix cp(2,8) ; //matrice pseudo-inverse de C


  vpMatrix H_int(3,3) ;
  vpMatrix pn((nb_pts-3),3) ; //points courant nouveau rep�re


  vpMatrix pnd((nb_pts-3),3) ; //points d�riv�s nouveau rep�re

  /* preparation du changement de repere */
  /****
       comme plan de reference on choisit pour le moment
       arbitrairement le plan contenant les points 1,2,3 du cinq
  ****/
  pts_ref[0] = 0 ;
  pts_ref[1] = 1 ;
  pts_ref[2] = 2 ;
  pts_ref[3] = 0 ;

  /* changement de repere pour tous les points autres que les trois points de reference */

  changeFrame(pts_ref,nb_pts,pd,p,pnd,pn,M,Mdp);


  cont_pts = nb_pts - 3 ;

  if (cont_pts < 5)
  {
    //vpERROR_TRACE(" not enough point to compute the homography ... ");
    throw (vpMatrixException(vpMatrixException::matrixError,
			     "Not enough point to compute the homography"));
  }

  //nl = cont_pts*(cont_pts-1)*(cont_pts-2)/6 ;
  nc = 7  ;

  /* Allocation matrice CtC */
  vpMatrix CtC(nc,nc) ;

  /* Initialisation matrice CtC */
  for (i=0;i<nc;i++) for (j=0;j<nc;j++) CtC[i][j] = 0.0;


  /* Allocation matrice M */
  vpColVector C(nc) ; //Matrice des coefficients

  /* construction de la matrice M des coefficients dans le cas general */
  /****
       inconnues du nouveau algorithme
       x1 = a  ; x2 = b  ; x3 = c ;
       x4 = ex ; x5 = ey ; x6 = ez ;
       qui deviennent apres changement :
       v1 = x1*x6 ; v2 = x1*x5 ;
       v3 = x2*x4 ; v4 = x2*x6 ;
       v5 = x3*x5 ; v6 = x3*x4 ;
  ****/
  cont = 0 ;
  for (i=0 ; i<nb_pts-5; i++) {
    for (j = i+1 ; j<nb_pts-4; j++) {
      for (k = j+1 ; k<nb_pts-3; k ++) {
	/* coeff a^2*b  */
	C[0] = pn[i][2]*pn[j][2]*pn[k][1]*pnd[k][0] //
	  * (pnd[j][0]*pnd[i][1] - pnd[j][1]*pnd[i][0])//
	  + pn[i][2]*pn[k][2]*pn[j][1]*pnd[j][0]//
	  *(pnd[i][0]*pnd[k][1] - pnd[i][1]*pnd[k][0])//
	  + pn[j][2]*pn[k][2]*pn[i][1]*pnd[i][0] //
	  *(pnd[k][0]*pnd[j][1] - pnd[k][1]*pnd[j][0]) ; //
	/* coeff a*b^2 */
	C[1] = pn[i][2]*pn[j][2]*pn[k][0]*pnd[k][1]//
	  *(pnd[i][0]*pnd[j][1] - pnd[i][1]*pnd[j][0])//
	  + pn[i][2]*pn[k][2]*pn[j][0]*pnd[j][1]//
	  *(pnd[k][0]*pnd[i][1] - pnd[k][1]*pnd[i][0])//
	  + pn[j][2]*pn[k][2]*pn[i][0]*pnd[i][1]//
	  *(pnd[j][0]*pnd[k][1] - pnd[j][1]*pnd[k][0]) ;//
	/* coeff a^2 */
	C[2] = 	  + pn[i][1]*pn[k][1]*pn[j][2]*pnd[j][0]//
	  *(pnd[k][2]*pnd[i][0] - pnd[k][0]*pnd[i][2])//
	  +pn[i][1]*pn[j][1]*pn[k][2]*pnd[k][0] //
	  *(pnd[i][2]*pnd[j][0] - pnd[i][0]*pnd[j][2])
	  + pn[j][1]*pn[k][1]*pn[i][2]*pnd[i][0] //
	  *(pnd[j][2]*pnd[k][0] - pnd[j][0]*pnd[k][2]) ; //



	/* coeff b^2 */
	C[3] = pn[i][0]*pn[j][0]*pn[k][2]*pnd[k][1] //
	  *(pnd[i][2]*pnd[j][1] - pnd[i][1]*pnd[j][2]) //
	  + pn[i][0]*pn[k][0]*pn[j][2]*pnd[j][1] //
	  *(pnd[k][2]*pnd[i][1] - pnd[k][1]*pnd[i][2]) //
	  + pn[j][0]*pn[k][0]*pn[i][2]*pnd[i][1] //
	  *(pnd[j][2]*pnd[k][1] - pnd[j][1]*pnd[k][2]) ; //

	/* coeff a */
	C[5] = pn[i][1]*pn[j][1]*pn[k][0]*pnd[k][2]//
	  *(pnd[i][0]*pnd[j][2] - pnd[i][2]*pnd[j][0])//
	  + pn[i][1]*pn[k][1]*pn[j][0]*pnd[j][2] //
	  *(pnd[k][0]*pnd[i][2] - pnd[k][2]*pnd[i][0])//
	  + pn[j][1]*pn[k][1]*pn[i][0]*pnd[i][2]//
	  *(pnd[j][0]*pnd[k][2] - pnd[j][2]*pnd[k][0]) ;//
	/* coeff b */
	C[6] = pn[i][0]*pn[j][0]*pn[k][1]*pnd[k][2] //
	  *(pnd[i][1]*pnd[j][2] - pnd[i][2]*pnd[j][1])//
	  + pn[i][0]*pn[k][0]*pn[j][1]*pnd[j][2]//
	  *(pnd[k][1]*pnd[i][2] - pnd[k][2]*pnd[i][1])//
	  + pn[j][0]*pn[k][0]*pn[i][1]*pnd[i][2]//
	  *(pnd[j][1]*pnd[k][2] - pnd[j][2]*pnd[k][1]) ;//
	/* coeff a*b */
	C[4] = pn[i][0]*pn[k][1]*pn[j][2] //
	  *(pnd[k][0]*pnd[j][1]*pnd[i][2] - pnd[j][0]*pnd[i][1]*pnd[k][2])//
	  + pn[k][0]*pn[i][1]*pn[j][2]//
	  *(pnd[j][0]*pnd[k][1]*pnd[i][2] - pnd[i][0]*pnd[j][1]*pnd[k][2])//
	  + pn[i][0]*pn[j][1]*pn[k][2]//
	  *(pnd[k][0]*pnd[i][1]*pnd[j][2] - pnd[j][0]*pnd[k][1]*pnd[i][2])//
	  + pn[j][0]*pn[i][1]*pn[k][2]//
	  *(pnd[i][0]*pnd[k][1]*pnd[j][2] - pnd[k][0]*pnd[j][1]*pnd[i][2])//
	  + pn[k][0]*pn[j][1]*pn[i][2]//
	  *(pnd[j][0]*pnd[i][1]*pnd[k][2] - pnd[i][0]*pnd[k][1]*pnd[j][2])//
	  + pn[j][0]*pn[k][1]*pn[i][2]//
	  *(pnd[i][0]*pnd[j][1]*pnd[k][2] - pnd[k][0]*pnd[i][1]*pnd[j][2]) ;//

	cont = cont+1 ;
	/* construction de la matrice CtC */
	for (ii=0;ii<nc;ii++) {
	  for (jj=ii;jj<nc;jj++) {
	    CtC[ii][jj] = CtC[ii][jj] + C[ii]*C[jj];
	  }
	}

      }
    }
  }



  /* calcul de CtC */
  for (i=0; i<nc ;i++) {
    for (j=i+1; j<nc ;j++) CtC[j][i] = CtC[i][j];
  }

  //nl = cont ;   /* nombre de lignes   */
  nc = 7 ;      /* nombre de colonnes */

  /* Creation de matrice CtC termine */
  /* Allocation matrice V */
  vpMatrix V(nc,nc) ;
  /*****
	Preparation au calcul de la svd (pseudo-inverse)
	pour obtenir une solution il faut au moins 5 equations independantes
	donc il faut au moins la mise en correspondence de 3+5 points
  *****/
  vpColVector sv(nc) ; //Vecteur contenant les valeurs singuli�res

  CtC.svd(sv,V) ;

  /*****
	Il faut un controle sur le rang de la matrice !!
	La meilleure solution est le vecteur de V associe
	a la valeur singuliere la plus petite en valeur
	absolu
  *****/

  vpColVector svSorted(nc) ; // sorted singular value

  // sorting the singular value
  for (i=0; i < nc ;i++) svSorted[i] = sv[i] ;
  perm = 1 ;
  double v_temp;
  while (perm != 0) {
    perm = 0;
    for (i=1; i < nc ;i++) {
      if (svSorted[i-1] > svSorted[i]) {
	v_temp = svSorted[i-1] ;
	svSorted[i-1] = svSorted[i] ;
	svSorted[i] = v_temp ;
	perm = perm + 1;
      }
    }
  }

  /*****
	Parcours de la matrice ordonn�e des valeurs singuli�res
	On note "cont_zeros" le nbre de valeurs quasi= � 0.
	On note "vect" le rang de la plus petite valeur singli�re
	en valeur absolu
  *****/

  vect = 0 ; cont_zeros = 0 ; cont = 0 ;
  for (j=0; j < nc; j++) {
    //if (fabs(sv[j]) == svSorted[cont]) vect = j ;
    if (std::fabs(sv[j]-svSorted[cont]) <= std::fabs(vpMath::maximum(sv[j],svSorted[cont]))) vect = j ;
    if (std::fabs(sv[j]/svSorted[nc-1]) < eps) cont_zeros = cont_zeros + 1 ;
  }

  if (cont_zeros > 5) {
    //    printf("erreur dans le rang de la matrice: %d \r\n ",7-cont_zeros);
    HLM2D(nb_pts,pd,p,H);
  }
  else
  {

    //     estimation de a = 1,b,c ; je cherche le min de somme(i=1:n) (0.5*(ei)^2)
    // 	  e1 = V[1][.] * b - V[3][.] = 0 ;
    // 	  e2 = V[2][.] * c - V[3][.] = 0 ;
    // 	  e3 = V[2][.] * b - V[3][.] * c = 0 ;
    // 	  e4 = V[4][.] * b - V[5][.] = 0 ;
    // 	  e5 = V[4][.] * c - V[6][.] = 0 ;
    // 	  e6 = V[6][.] * b - V[5][.] * c = 0 ;
    // 	  e7 = V[7][.] * b - V[8][.] = 0 ;
    // 	  e8 = V[7][.] * c - V[9][.] = 0 ;
    d[0] = V[2][vect] ;
    d[1] = V[4][vect] ;
    d[2] = V[1][vect] ;
    d[3] = V[0][vect] ;
    d[4] = V[3][vect] ;
    d[5] = V[4][vect] ;
    d[6] = V[0][vect] ;
    d[7] = V[1][vect] ;

    c[0][0] = V[5][vect] ; c[0][1] = 0.0 ;
    c[1][0] = V[6][vect] ; c[1][1] = 0.0 ;
    c[2][0] = V[3][vect] ; c[2][1] = 0.0 ;
    c[3][0] = V[4][vect] ; c[3][1] = 0.0
			     ;
    c[4][0] = 0.0        ; c[4][1] = V[6][vect] ;
    c[5][0] = 0.0        ; c[5][1] = V[5][vect] ;
    c[6][0] = 0.0        ; c[6][1] = V[2][vect] ;
    c[7][0] = 0.0        ; c[7][1] = V[4][vect] ;



    /// Calcul de la pseudo-inverse de C
    cp = c.pseudoInverse(1e-6) ;


    vpColVector H_nr(3), temp ;	// Homographie diagonale
    // Multiplication de la matrice H_nr par le vecteur cp
    temp =  cp * d;

    H_nr[0] = temp[0] ; H_nr[1] = temp[1] ;
    H_nr[2] = 1.0 ;

    vpMatrix T(9,3) ; T =0 ;
    T[0][0] = -V[1][vect] ; T[0][1] = V[0][vect] ;
    T[1][0] =  V[4][vect] ; T[1][2] = -V[2][vect] ;
    T[2][0] = -V[6][vect] ; T[2][1] = V[2][vect] ;
    T[3][0] =  V[6][vect] ; T[3][2] = -V[0][vect] ;
    T[4][0] = -V[3][vect] ; T[4][1] = V[6][vect] ;
    T[5][0] =  V[3][vect] ; T[5][2] = -V[1][vect] ;
    T[6][0] = -V[5][vect] ; T[6][1] = V[4][vect] ;
    T[7][0] =  V[5][vect] ; T[7][2] = -V[6][vect] ;
    T[8][1] =  -V[5][vect] ; T[8][2] = V[2][vect] ;


    vpMatrix Hd(3,3) ; //  diag(gu,gv,gw)
    for (i=0 ; i < 3 ; i++) Hd[i][i] = H_nr[i] ;

    // H = M diag(gu,gv,gw) M*-1
    H = M*Hd*Mdp ;



  }
}


/**************************************************************************
 * NOM :
 * Homographie_CrvMafCalculHomographie
 *
 * DESCRIPTION :
 * Calcul de l'homographie, en fonction de la cible d�sir�e et de la cible
 * en cours. C'est une estimation lin�aire.
 * Cette proc�dure n'effectue pas elle-m�me le calcul de l'homographie :
 * elle se contente d'appeler la bonne sous-proc�dure.
 * Cette proc�dure est appell�e par "crv_maf_calcul_tomographie".
 *
 ****************************************************************************
 * ENTREES :
 *  STR_CIBLE_ASSER   *cible_asser  	Pointeur sur structure contenant les
 *                                       commandes du robot, les donn�es de la
 *					carte...
 *					Voir "cvda/edixaa/param/robot.h"
 *	STR_VITESSE_ROBOT *data_common   Pointeur sur la structure d�crivant la
 *					cible d'asservissement.
 *					Voir "cvda/edixia/param/param.h"
 *	STR_MACH_DIV 		*machine_div   Pointeur sur structure contenant divers
 *					param�tres de configuration du robot.
 *					Voir "cvda/edixia/param/param.h"
 *
 * SORTIES :
 *
 * double **H 			matrice d'homographie

 *
 ****************************************************************************
 * AUTEUR : BOSSARD Nicolas.  INSA Rennes 5�me ann�e.
 *
 * DATE DE CREATION : 01/12/98
 *
 * DATES DE MISE A JOUR :
 *
 ****************************************************************************/
void
HLM(unsigned int q_cible,
    unsigned int nbpt,
    double *xm, double *ym,
    double *xmi, double *ymi,
    vpMatrix &H)
{
  unsigned int   i;

  /****
       on regarde si il y a au moins un point mais pour l'homographie
       il faut au moins quatre points
  ****/
  vpMatrix pd(nbpt,3) ;
  vpMatrix p(nbpt,3) ;

  for (i=0;i<nbpt;i++)  {
    /****
	 on assigne les points fournies par la structure robot
	 pour la commande globale
    ****/
    pd[i][0] = xmi[i];
    pd[i][1] = ymi[i];
    pd[i][2] = 1.0 ;
    p[i][0] = xm[i];
    p[i][1] = ym[i];
    p[i][2] = 1.0 ;
  }


  switch (q_cible) {
  case (1):
  case (2):
    /* La cible est planaire  de type points   */

    HLM2D(nbpt,pd,p,H);

    break;
  case (3) : /* cible non planaire : chateau */
    /* cible non planaire  de type points   */
    HLM3D(nbpt,pd,p,H);
    break;
  } /* fin switch */



} /* fin procedure calcul_homogaphie */


#endif

/*!
  \brief Computes the homography matrix from planar or non planar points
  using Ezio Malis linear method (HLM).

  Computes H such as  \f[
  ^a{\bf p} = ^a{\bf H}_b\; ^b{\bf p}
  \f]

  This function implements the fonctions related with the homography
  estimation from non coplanar points using the Malis algorithm

  References:

  E. Malis, F. Chaumette. 2 1/2 D visual servoing with respect to unknown
  objects through a new estimation scheme of camera displacement. Int. Journal
  of Computer Vision, 37(1):79-97, June 2000.

  \code
  @article{Malis00b,
  Author = {Malis, E. and Chaumette, F.},
  Title = {2 1/2 D visual servoing with respect to unknown objects
  through a new estimation scheme of camera displacement},
  Journal = {Int. Journal of Computer Vision},
  Volume = {37},
  Number = {1},
  Pages = {79--97},
  Month = {June},
  Year = {2000}
  }
  \endcode

  paper can be obtained at this url :
  http://www.irisa.fr/lagadic/publi/publi/Malis00b-fra.html

  the algorithm for 2D scene implemented in this file is described in Ezio
  Malis PhD thesis

  E. Malis. Contributions � la mod�lisation et � la commande en asservissement
  visuel.
  Th�se de l'Universite de Rennes 1, T�l�communications et Traitement du Signal,
  Novembre 1998.
  http://www.irisa.fr/lagadic/publi/publi/TheseMalis-fra.html


  if the boolean isplanar is true the point is assumed to be in a plane
  otherwise there are assumed to be planar.

  the reference planar is the plane build from the 3 first points

*/
void vpHomography::HLM(unsigned int n,
		       double *xb, double *yb,
		       double *xa, double *ya ,
		       bool isplanar,
		       vpHomography &aHb)
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS

  unsigned int i,j;
  unsigned int q_cible;
  vpMatrix H; // matrice d'homographie en metre

  aHb.setIdentity();


  if (isplanar)
    q_cible =1;
  else
    q_cible =3;

  ::HLM(q_cible,n, xa,ya,xb,yb,H) ;

  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
      aHb[i][j] = H[i][j];

#endif
}

