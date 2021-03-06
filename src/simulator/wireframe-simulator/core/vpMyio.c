/****************************************************************************
 *
 * $Id: vpMyio.c 3530 2012-01-03 10:52:12Z fspindle $
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
 * Description:
 * Le module "myio.c" contient les procedures d'entree/sortie
 * des types definis dans le module "my.h".
 * Les entrees non specifiees sont effectuees
 * sur le fichier "source" du module "lex.c".
 * Pour les mots cles des "fprintf_..." voir "token.c".
 *
 * Authors:
 * Jean-Luc CORRE
 *
 *****************************************************************************/




#include	<visp/vpMy.h>
#include	<visp/vpToken.h>
#include	<visp/vpLex.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#ifndef DOXYGEN_SHOULD_SKIP_THIS


extern	char	*mytext;	/* chaine du symbole courant	*/


/*
 * La procedure "fscanf_float" lit en ascii un nombre flottant.
 * Entree :
 * fp		Nombre flottant a lire.
 */
void fscanf_float (float *fp)
{
	int	t;

	if ((t = lex ()) != T_FLOAT && t != T_INT)
	  lexerr ("start", "float expected", NULL);
	*fp = (t == T_INT) ? (float) myint : myfloat;
}

/*
 * La procedure "fscanf_Index" lit en ascii un indice.
 * Entree :
 * ip		Indice a lire.
 */
void fscanf_Index (Index *ip)
{
	if (lex () != T_INT)
		lexerr ("start", "integer expected", NULL);
	*ip = (Index) myint;
}

/*
 * La procedure "fscanf_int" lit en ascii un nombre entier.
 * Entree :
 * ip		Nombre entier a lire.
 */
void fscanf_int (int *ip)
{
	if (lex () != T_INT)
		lexerr ("start", "integer expected", NULL);
	*ip = myint;
}

/*
 * La procedure "fscanf_string" lit en ascii une chaine de caracteres.
 * Entree :
 * str		Chaine a lire.
 */
void fscanf_string (char **str)
{
	if (lex () != T_STRING)
		lexerr ("start", "string expected", NULL);
	if (*str == NULL)
		*str = (char *) malloc ((mylength + 1) * sizeof (char));
	else	*str = (char *) realloc (*str, (mylength + 1) * sizeof (char));
	strncpy (*str, mytext, mylength);
}

/*
 * La procedure "fscanf_Type" lit en ascii un octet.
 * Entree :
 * ip		Type a lire.
 */
void fscanf_Type (Type *ip)
{
	if (lex () != T_INT)
		lexerr ("start", "integer expected", NULL);
	*ip = (Type ) myint;
}

#endif
