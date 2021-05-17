/**
 * @file Nostromo.c
 * @project Alien
 * @author Priscilla Tissot
 * @date 11 janvier 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "stdbool.h"
#include <fcntl.h>
#include <signal.h>
#include <string.h>

/**
 * @struct Personnage
 * Caractéristiques d'un personnage
 */
struct Personnage {
    char *nom;
    char *race;
};

/**
 * @struct pid_name
 * Association entre un passager et son PID
 */
struct pid_name {
    pid_t pid;
    char *nom;
};


typedef struct pid_name pid_n;

int nombrePassager = 8; //nombre de passagers à mettre à jour progressivement
struct Personnage *personnage_courant; //hack pour acceder au passager dans le handler
int nbPassager; //nombre de passagers
bool stillalive = true;  //statut de l'alien, permet de sortir le père de la boucle

/**
 * Handler pere : On annonce la fin de la partie et on sort de la boucle.
 * 
 * @param sig   valeur signal
 * 
 */
void
handlerPere( int sig ){

    //Annonce la fin de la partie
    printf("L'alien n'est pas un imposteur\n");
    //Sort le père de sa boucle
    stillalive = false;

}

/**
 * Handler humain : l'humain écrit son testament et meurt.
 * 
 * @param sig   valeur signal
 * 
 */
void
handlerHuman( int sig ){

   char pid[20];

   //On afiche le pid
   sprintf(pid, "%d",getpid());

   //On crée le fichier <pid>.txt
   FILE *f = fopen(strcat(pid,".txt"),"w");

   char texte[100];

   //On écris dans le fichier <pid>.txt le testament de l'humain qui meurt
   sprintf(texte, "Je suis %s et ma fin est proche",personnage_courant->nom);
   fputs(texte,f);

   //On ferme le fichier
   fclose(f);

   //On affiche la mort du passager humain dans le terminal
   printf("Passager n°%d : RIP %s (%ld)\n",nbPassager,  personnage_courant->nom, (long)getpid());

   //Le processus du passager humain se termine
   exit(0);
}

/**
 * Handler alien : l'alien affiche son décès sur le terminal et envoie un signal au père
 * 
 * @param sig   valeur signal
 * 
 */
void
handlerAlien( int sig ){
    
    //Affiche la mort de l'alien dans le terminal
    printf("- RIP l'alien -\n");

    //On envoie un signal au père pour terminer la partie
    kill(getppid(), SIGUSR1);

    //Le processus de l'alien se termine
    exit(0);
}

/**
 * Simulation avec 8 passagers.
 */
int
genererPassagers( void )
{
    //On génère les personnages
    struct Personnage personnages[]= {
        {.nom = "Alien", .race = "alien"},
        {.nom = "Ash", .race = "humain"},
        {.nom = "Bret", .race = "humain"},
        {.nom = "Parker", .race = "humain"},
        {.nom = "Lambert", .race = "humain"},
        {.nom = "Dallas", .race = "humain"},
        {.nom = "Toby", .race = "humain"},
        {.nom = "Helen Ripley", .race = "humain"}
    };

    //Déclaration du tableau de pid_name et allocation de mémoire
    struct pid_name *tab_pid_name = malloc(nombrePassager*sizeof(pid_n));

    //On commence la partie
    printf("* Dans l'espace lointain... \n"); 

    for ( int i = 0; i < nombrePassager; i++ ) {
    
      //TODO : Génération des passagers et gestion des handlers des fils
      //TODO : Création du tableau "passager / PID" du père
      
      //On rempli le tableau "passager / PID" tab_pid_name avec les noms des personnages
      (tab_pid_name+i)->nom = personnages[i].nom;

        //On génère le processus du passager
        if((((tab_pid_name+i)->pid) = fork()) == 0){

            //On attribue le personnage courant dans la boucle à la variable personnage_courant
            personnage_courant = &personnages[i]; 

            //On test si ce personnage est l'alien
            if (strcmp(personnages[i].nom,"Alien")==0){

                //Affectation de l'action handlerAlien associé au signal SIGUSR1
                struct sigaction alien;
                alien.sa_handler = handlerAlien;
                sigaction(SIGUSR1,&alien,0);

            //On test si le personnage est Helen Ripley
            } else if (strcmp(personnages[i].nom, "Helen Ripley")==0){

                struct sigaction HR;
                HR.sa_handler = SIG_IGN;//Le signal est ignoré
                sigaction(SIGUSR1,&HR,0);

            //Si ce n'est ni l'Alien, ni Helen Ripley
            } else {
                
                //Affectation de l'action handlerHuman associé au signal SIGUSR1
                nbPassager = i;
                struct sigaction humain;
                humain.sa_handler = handlerHuman;
                sigaction(SIGUSR1,&humain,0);

            }
            //Le processus est mis en pause jusqu'à la réception d'un signal externe
            pause();

        }
    }

    //Affectation de l'action handlerAlien associé au signal SIGUSR1 dans le processus père
    struct sigaction pere;
    pere.sa_handler = handlerPere;
    sigaction(SIGUSR1,&pere,0);
    sleep(3);

    //Tant que l'alien est toujours vivant
    while( stillalive ) {
        
        int nb = (rand()%getpid())%nombrePassager;

        //On envoie un signal au passager
        kill((tab_pid_name+nb)->pid,SIGUSR1);
        
        if (strcmp((tab_pid_name+nb)->nom, "Helen Ripley")!=0){

            //On décrémente le nombre de passager à chaque mort d'un passager 
            nombrePassager--;

            for (int i = nb ; i <nombrePassager; i++){

                //On met à jour les tableaux à chaque mort d'un passager
                personnages[i].nom = personnages[i+1].nom;
                personnages[i].race = personnages[i+1].race;
                (tab_pid_name+i)->nom = (tab_pid_name+(i+1))->nom;
                (tab_pid_name+i)->pid = (tab_pid_name+(i+1))->pid;

            }

            //On met à jour la taille du tableau en lui réallouant de la mémoire
            tab_pid_name = realloc(tab_pid_name, (nombrePassager)* sizeof(pid_n));

        }

        //On attend 3 secondes entre chaque envoi d'un signal
        sleep(3);
    }

    //On affiche le nombre de passagers restants
    printf("Il y a %d survivant(s) :\n", nombrePassager);

    //On tue les processus fils restant
    for(int i = 0; i<nombrePassager; i++){

        printf("    - %s (%ld)\n",(tab_pid_name+i)->nom, (long)(tab_pid_name+i)->pid);
        kill((tab_pid_name+i)->pid, SIGTERM);   

    }

    //On libère le tableau des pid
    free(tab_pid_name);
    printf( "* Dans l'espace personne ne vous entendra crier.\n" );
    return ( EXIT_SUCCESS );

}

/**
 * Fonction principale.
 * @return  code de retour
 */
int
main( void )
{
    return genererPassagers();
}
