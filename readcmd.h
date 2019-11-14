/*
 *Authors:
 ********BOUKRIS WALID***************
 ********SOW OUSMANE*****************
 ********Mini-shell: L3 MIAGE********
 *************2019-2020**************
 */

#ifndef __READCMD_H
#define __READCMD_H

/***************************************************************************
********Cree une Structure cmdline a partir de com**************************
****************************************************************************/
struct cmdline *readcmd(char **com);


/* Structure returned by readcmd() */
struct cmdline {

/**********Pour simplifier les choses in et out ne seront pas utilisées**********/

	char *in;	  /* If not null : name of file for input redirection. */
	char *out;	/* If not null : name of file for output redirection.*/

/**********Pour comprendre ce que seq contient Voici un Exemple : **********
**********la commande tapez : ls -al > ls ; cat < fic1 ; who | cat**********
**********seq[0] vaut : ls -al > ls ****************************************
**********seq[1] vaut : cat < fic1 *****************************************
**********seq[2] vaut : who | cat ****************************************/

	char **seq;	


/**********arrplan vaut 1 si il y'a un "&" sinon 0**********/
	int arrplan;
};

#endif
