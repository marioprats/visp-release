/****************************************************************************
 *
 * $Id: vpParser.c 3530 2012-01-03 10:52:12Z fspindle $
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
 * Le module "parser.c" contient les procedures de gestion
 * de l'analyse syntaxique d'un fichier source dont la grammaire
 * possede les symboles terminaux de "lex.c".
 *
 * Authors:
 * Jean-Luc CORRE
 *
 *****************************************************************************/




#include	<visp/vpMy.h>
#include	<visp/vpArit.h>
#include	<visp/vpView.h>
#include	<visp/vpBound.h>
#include	<visp/vpToken.h>
#include	<visp/vpLex.h>
#include	<visp/vpSkipio.h>

#include	<stdio.h>
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef	used
extern	Byte		*get_remove (void);
extern	View_parameters	*get_view_parameters (void);
#endif	/* used	*/

/*
 * La procedure "parser" fait l'analyse syntaxique du fichier source.
 * Entree/Sortie :
 * bsp		Scene surfacique polygonale a lire.
 */
void parser (Bound_scene *bsp)
{
	int	token;

	while ((token = lex ()) != T_EOF) 
	switch (token) {
	case '$' :
		switch (lex ()) {
		case T_IDENT	:	/* saute la commande inconnue	*/
			skip_cmd (/* stderr */);
			unlex ();
			break;
		case T_EXIT	:
			return;
			break;
		case T_BOUND	:
			if (bsp->bound.nbr == BOUND_NBR) {
				fprintf (stderr, "mire: too much bound\n");
				return;
			}
			fscanf_Bound (
			&(bsp->bound.ptr[bsp->bound.nbr++]));
			break;
#ifdef	used
		case T_REMOVE	:
			fscanf_Remove (get_remove ());
			break;
		case T_VIEW	:
			fscanf_View_parameters (get_view_parameters ());
			set_projection (void);
			break;
#endif	/* used	*/
		default		:
		  lexerr ("start", "keyword expected", NULL); 
			break;
		}
		break;
	default	:
	  lexerr ("start", "symbol '$' expected", NULL);
		break;
	}
}

#endif
