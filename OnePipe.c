/*
 *Authors:
 ********BOUKRIS WALID***************
 ********SOW OUSMANE*****************
 ********Mini-shell: L3 MIAGE********
 *************2019-2020**************
 */
///Premiers version de pipe!


/****************************************************************************
********cette methode est appelé quand cmd contient un pipe******************
********Exemple : ls -al | echo salut!***************************************
****************************************************************************/
void one_pipe(char** com){

  int i;
  int f1[2];
  char* s1=malloc(sizeof(char)*40);//pour stocker la suite d'une commande Ex: "echo salut ça va" ---> s1="salut ça va"
  char* s2=malloc(sizeof(char)*40);//Meme chose que s1
	//Creation de tube
	pipe(f1);
  switch (fork()){
     case -1 :
         perror("fork");
         exit(-1);
     case 0 :
           /**********************FILS**********************/
           //Remplissage de s1
           i=0;
           i++;
           if(strcmp(com[i],"|")!=0){
             strcpy(s1,com[i]);
             i++;
             while(i<10 && strcmp(com[i],"|")!=0){
               strcat(strcat(s1," "),com[i]);
               i++;
             }
           }
           //Gestion des Entrees/sorties
           close(1);
           dup(f1[1]);
           close(f1[1]);
           close(f1[0]);
           //Exec!!!!!
           if(strcmp(com[1],"|")!=0){
             if(execlp(com[0],com[0],s1,(char*)0)<1){
                perror("execvp 1");
                exit(-1);
             }
           }else{
             if(execlp(com[0],com[0],(char*)0)<1){
                perror("execvp 1");
                exit(-1);
             }
           }
     default :
          /**********************PERE**********************/
          switch (fork()){
            case -1 :
                perror("fork");
                exit(-1);
            case 0 :
                //Gestion des Entrees/sorties
                close(0);
                dup(f1[0]);
                close(f1[0]);
                close(f1[1]);
                //Se positionner dans le bon endroit (ie aprés le pipe)
                i=0;
                while(strcmp(com[i],"|")!=0){
                  i++;
                }
                //Remplissage de s2
                i++;
                int j=i;
                i++;
                if(com[i]!=NULL){
                  strcpy(s2,com[i]);
                  i++;
                  while(i<10 && com[i]!=NULL){
                    strcat(strcat(s2," "),com[i]);
                    i++;
                  }
                }
                //Exec!!!!!!!
                if(com[j+1]==NULL){
                  if(execlp(com[j],com[j],(char*)0)<1){
                      perror("execvp 2");
                      exit(-1);
                  }
                }else{

                  if(execlp(com[j],com[j],s2,(char*)0)<1){
                      perror("execvp 2");
                      exit(-1);
                  }
                }
            default:
                //On attend que tous les fils meurent!
                while (wait(NULL)!=-1);
                close(f1[1]);
                close(f1[0]);
         }
      }
}
