/*
 *Authors:
 ********BOUKRIS WALID***************
 ********SOW OUSMANE*****************
 ********Mini-shell: L3 MIAGE********
 *************2019-2020**************
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#include "readcmd.h"

//pour la commande setenv
extern char **environ;


/****************************************************************************
********Gestion de la reception du signal SGINT******************************
****************************************************************************/
void sg_int(int sig){
  fprintf(stderr, "Reception signal %d\n", sig);
  fprintf(stderr,"$$$$$$$$$> ");
}

/****************************************************************************
********Lire une ligne de commande cmd et la stocker Dans le tableau com*****
****************************************************************************/
void Lire_commande(char* cmd, char** com){

    com[0]=strtok(cmd," ");
    int i=1;
    while((com[i]=strtok(NULL," "))!=NULL){
              i++;
    }
}

/****************************************************************************
********Renvoie le nombre des pipes******************************************
********Exemple : ls -al | echo salut! | who Renvoie 2***********************
****************************************************************************/
int nbPipes(char** com){
  int i=0;
  int res = 0;
  //Parcours de tableau com!
  while (i<10 && com[i]!=NULL) {
    if(strcmp(com[i],"|")==0){
      res++;
    }
    i++;
  }
  return res;
}

/****************************************************************************
********Renvoie le fichier qu'on veut placer a 0*****************************
********NULL s'il y'a pas de redirection en entrée***************************
********Ex: ls | cat < fic1 renvoie fic1*************************************
****************************************************************************/
char *red_Entree(char** com){
  int i=0;
  //Parcours de tableau com!
  while (i<10 && com[i]!=NULL && strcmp(com[i],"<")!=0) {
      i++;
  }
  if(com[i]==NULL) return com[i];
  return  com[i+1];
}

/****************************************************************************
********Renvoie le fichier qu'on veut placer a 1*****************************
********NULL s'il y'a pas de redirection en sortie***************************
********Ex: ls | echo Salut !!!! > fic1 renvoie fic1*************************
****************************************************************************/
char *red_Sortie(char** com){
  int i=0;
  //Parcours de tableau com!
  while (i<10 && com[i]!=NULL && strcmp(com[i],">")!=0) {
      i++;
  }
  if(com[i]==NULL) return com[i];
  return com[i+1];
}

/****************************************************************************
********Renvoie une struct cmdline avec seq bien rempli**********************
********A partir de com******************************************************
********ET arrplan vaut 0****************************************************
****************************************************************************/
struct cmdline * readcmd(char** com){
  struct cmdline *s = malloc(sizeof(struct cmdline *));
  int i=0;
  char **ss=malloc(sizeof(char*)*10);
  char *s1=malloc(sizeof(char)*40);
  s->arrplan=0;
  int j=0;
  //Parcours de tableau com!
  while (i<10 && com[i]!=NULL) {
    if(i==0){
        strcpy(s1,com[i]);
    }else if(strcmp(com[i],";")==0){
        ss[j]=malloc(sizeof(char)*40);
        strcpy(ss[j],s1);
        j++;
        i++;
        if(com[i]!=NULL){
          strcpy(s1,com[i]);
        }
    }else{
      strcat(strcat(s1," "),com[i]);
    }
    i++;
  }

  ss[j]=malloc(sizeof(char)*40);
  strcpy(ss[j],s1);
  s->seq=ss;
  return s;
}

/****************************************************************************
********cette methode est appelé quand cmd contient plusieurs pipes**********
********Exemple : ls -al | echo salut! | who*********************************
********Dans la dernière version cette methode est toujours appelé quand
  il s'agit d'une commande extern *******************************************
****************************************************************************/
void n_pipe(struct cmdline * s,char **com){

  //si il existe une redirection > bout vaut 1 (idem pour bin avec <)
  int bout=0;int bin=0;
  //Pour ce bien positionner entre les pipes(ie quand il vaut 1 => il faut ce deplacer apres le 1er pipe)
  int indicePipe=0;
  //nombre de commandes a execute
  int nbc=nbPipes(com)+1;
  //Des vars pour faire les Parcours
  int i, j;
  //pid
	pid_t p;
  int tube[nbc-1][2];//tableau 2 dimensions : 2 cases pour chaque tube.
	for(i=0;i<nbc;i++){
    		if(i<nbc-1){ // si on n'est pas a la dernière commande!
			  	//Creation de tube
			  	pipe(tube[i]);
    		}

    		p=fork();
    		if(p==0){ //Fils
    			if(i>0){//si on n'est pas a la 1er commande on recopie le descripteur tube[i-1][0] dans le descripteur de l'entrée standard
    				dup2(tube[i-1][0], 0);
    			}

    			if(i<nbc-1){//si on n'est pas a la dernière commande on recopie le descripteur tube[i][0] dans le descripteur de la sortie standard
    				dup2(tube[i][1], 1);
    			}

    			for(j=0;j<nbc;j++){
    				close(tube[j][0]);
    				close(tube[j][1]);
    			}

          //Se positionner dans le bon endroit (ie aprés le bon pipe)
          int x=0;
          int iPipe=0;
          while (iPipe<indicePipe) {
            if(strcmp(com[x],"|")==0){
              iPipe++;
            }
            x++;
          }
          //Remplissage de s2
          int k=x;
          char* s2=malloc(sizeof(char)*40);
          char* sout=malloc(sizeof(char)*40);
          char* sin=malloc(sizeof(char)*40);
          if(com[x]!=NULL){
            strcpy(s2,com[x]);
            x++;
            while(x<10 && com[x]!=NULL && (strcmp(com[x],"|")!=0)&& (strcmp(com[x],"&")!=0)&&(strcmp(com[x],">")!=0)&&(strcmp(com[x],"<")!=0)){
              strcat(strcat(s2," "),com[x]);
              x++;
            }

            //si y'a une redirection >
            if(x<10 && com[x]!=NULL && strcmp(com[x],">")==0){
              bout=1;
              strcpy(sout,com[x+1]);
            }else{
              bout=0;
            }


            //si y'a une redirection <
            if(x<10 && com[x]!=NULL && strcmp(com[x],"<")==0){
              bin=1;
              strcpy(sin,com[x+1]);
              while(x<10 && com[x]!=NULL && (strcmp(com[x],"|")!=0)&&(strcmp(com[x],">")!=0)){
                x++;
              }

              //si y'a une redirection < et >
              if(x<10 && com[x]!=NULL && strcmp(com[x],">")==0){
                bout=1;
                strcpy(sout,com[x+1]);
              }else{
                bout=0;
              }
            }else{
              bin=0;
            }

            //si y'a un "&" a la fin!
            if(x<10 && com[x]!=NULL && strcmp(com[x],"&")==0){
              printf("LE & existe %s\n",com[x] );
              s->arrplan=1;
            }else{
              s->arrplan=0;
            }
          }

    			//redirection de sortie
          int fd1;
          if(bout){
            if ((fd1 = open(sout,O_CREAT|O_WRONLY, 0644)) < 0) {
                perror("Open >");
                exit(0);
            }
            dup2(fd1,1);
            close(fd1);
          }

          //redirection d'entrée
          int fd0;
          if(bin){
            if ((fd0 = open(sin, O_RDONLY, 0)) < 0) {
                   perror("Open <");
                   exit(0);
            }
            dup2(fd0, 0);
            close(fd0);
          }

          //Exec!!!!!
          if(strcmp(com[k],"echo")==0){
            execvp(com[k],com+k);
          }else if(com[k+1]!=NULL && strcmp(com[k],"ls")==0 && strcmp(com[k+1],"-al")==0){
            if(com[k+2]!=NULL && (strcmp(com[k+2],"|")!=0)&&(strcmp(com[k+2],">")!=0)&&(strcmp(com[k+2],"<")!=0)){
                chdir(strcat(strcat(getenv("PWD"),"/"),com[k+2]));
            }
            execlp(com[k],com[k],com[k+1],(char*)0);
          }else if(com[k+1]!=NULL && (strcmp(com[k],"grep")==0 || strcmp(com[k],"wc")==0)){
            execvp(com[k],com+k);
          }else{
            execlp(com[k],s2,(char*)0);
          }
          exit(0);
        }else{
      			close(tube[i][1]);
            //remettre la sortie et entrée standard a leur bon endroit!
            dup2(stdout,1);
            dup2(stdin,0);
            for (j = 0; j < nbc; j++){
              //le père attend que tous ces fils se terminent
              wait(NULL);
            }
            if(s->arrplan==1){
                printf("****%d\n",p);
            }
    		}
        indicePipe++;
	}
}

/****************************************************************************
********execute la ieme commande com*****************************************
*****************************************************************************
****************************************************************************/
void Execution(struct cmdline * s,char** com){
      int nbpipes=nbPipes(com);
      //int no, status;
      if(nbpipes>=1){
          n_pipe(s,com);
      }else if (strcmp(com[0],"cd")==0){
          if (com[1]==NULL){
                 chdir(getenv("HOME"));
          }else if (chdir(com[1])==-1){
                 perror(com[1]);
          }else{
                 chdir(strcat(strcat(getenv("PWD"),"/"),com[1]));
          }
      }else if(strcmp(com[0],"exit")==0){
          exit(-1);
      }else if (strcmp(com[0],"getenv")==0){
          if(com[1]!=NULL){
              printf("%s\n", getenv(com[1]));
          }
      }else if (strcmp(com[0],"setenv")==0){
          if((com[1]!=NULL) && (com[2] != NULL)){
            setenv(com[1],com[2],1);
          }else if(com[1]==NULL){
            int i = 0;
            while (environ[i]!=NULL) {
              printf("%s\n", environ[i]);
              i++;
            }
          }
      }else{
            /* Pour les autres :*/
            n_pipe(s,com);
      }
}

/****************************************************************************
********Methode PRINCIPALE***************************************************
*****************************************************************************
****************************************************************************/
void mon_shell(char *arge[]){

     char cmd[100];
     char *com[10];

     signal(SIGINT,sg_int);
     //signal(SIGINT,SIG_IGN);
     printf("Voici mon shell, taper Q pour sortir\n");
     printf("$$$$$$$$$> ");
     fgets(cmd,sizeof(char)*100,stdin);

     while ( strcmp(cmd,"\n")==0){//Tant que l'utilisateur tape que entree!
         printf("$$$$$$$$$> ");
         fgets(cmd,sizeof(char)*100,stdin);
     }
     //On remplace le dernier char de cmd qui vaut "\n" par 0.
     cmd[strlen(cmd)-1]=0;
     while ( strcmp(cmd,"Q")!=0){
         Lire_commande(cmd, com);
         struct cmdline * l=readcmd(com);
         int i=0;
         //Parcours des commandes separees par des ';'
         while(l->seq[i]!=0){
             char *s=l->seq[i];
             char *ss[10];
             Lire_commande(s,ss);
             Execution(l,ss);
             printf("\n");
             i++;
         }
         //ReInitialisation!
         printf("$$$$$$$$$> ");
         fgets(cmd,sizeof(char)*100,stdin);
         while(strcmp(cmd,"\n")==0){//Tant que l'utilisateur tape que entree!
             printf("$$$$$$$$$> ");
             fgets(cmd,sizeof(char)*100,stdin);
         }
         //On remplace le dernier char de cmd qui vaut "\n" par 0.
         cmd[strlen(cmd)-1]=0;
    }
}

/****************************************************************************
********LE MAIN**************************************************************
********Fait appel a la methode principale mon_shell*************************
****************************************************************************/
int main(int argc,char *argv[],char *arge[]){
     mon_shell(arge);
     return 0;
}

/****************Des Affichage pour tester le bon fonctionnement des methodes
et si la Structure est bien rempli*******************************************/


void afficher(char** com){
  printf("Vous avez Tapez:  \n");
  int i=0;
  //Parcours de tableau com + Affichage!
  while (i<10 && com[i]!=NULL) {
    printf("*%s* ",com[i]);
    i++;
  }
}

void afficher_cmd(struct cmdline *l){
    printf("Affichage\n");
    if(l->out!=NULL)printf("sortie :%s\n",l->out );
    if(l->in!=NULL)printf("entrée  :%s\n",l->in );
    char * s=malloc(sizeof(char)*40);
    int i=0;
    while(l->seq[i]!=0){
      s=l->seq[i];
      printf("seq [%d]  :  *%s*\n",i,s);
      i++;
    }
}
