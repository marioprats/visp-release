/****************************************************************************
 *
 * $Id: vpSkipio.c 3530 2012-01-03 10:52:12Z fspindle $
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
 * Le module "skipio.c" contient les procedures d'analyse
 * syntaxique du fichier "source" qui permettent de traiter
 * les commandes inconnues.
 *
 * Authors:
 * Jean-Luc CORRE
 *
 *****************************************************************************/






#include <visp/vpConfig.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include	<visp/vpMy.h>
#include	<visp/vpToken.h>
#include	<visp/vpLex.h>
#include	<visp/vpSkipio.h>
#include	<stdio.h>
/*
 * La procedure "skip_cmd" saute les structures d'une commande
 * jusqu'a reconnaitre le debut d'une nouvelle commande.
 * Entree :
 * f		Fichier en sortie.
 */
void skip_cmd (void)
{
	int	token;

	fprintf (stderr, "\n$ ");
	fwrite (mytext, mylength, 1, stderr);
	while ((token = lexecho (stderr, '$')) !=  T_EOF && token != '$');
	unlex ();
}

/*
 * La procedure "skip_keyword" saute les structures des articles 
 * jusqu'a reconnaitre le mot cle de jeton "token".
 * Entree :
 * token	Jeton du mot cle a reconnaitre.
 * err		Message d'erreur si le mot cle n'est pas reconnu.
 */
void skip_keyword (int token, char *err)
{
	int	t;

	switch (t = lex ()) {
	case T_IDENT :		/* saute le mot cle inconnu	*/
		while ((t = lex ()) != 0){ 
		  switch (t) {
		    case '$'   :	/* nouvelle commande		*/
		    case T_EOF :	/* fin de fichier		*/
		      lexerr ("start", err, NULL);
			    break;
		    default	:
			    if (t == token) return;
			    break;
		  }
    }
		break;
	default	:
		if (t == token) return;
		break;
	}
	lexerr ("start", err, NULL);
}

#endif

