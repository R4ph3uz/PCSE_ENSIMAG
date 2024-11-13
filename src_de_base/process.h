#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdio.h>
#include <inttypes.h>
#include <string.h>


#define NBPROCESS 4
extern int nbProcess;

//initialisation d'un enum de cte
typedef enum {ELU,ACTIVABLE,ENDORMI,ZOMBIE} ACTIVATION;
//initialisation des structures de processus
typedef struct processus_s{
    int32_t pid;
    char nom[50];
    ACTIVATION etat;
    uint32_t zoneSauvegarde[5];
    uint32_t pileExecution[512];
    uint32_t heure_reveil;
    struct processus_s* suivant;
}  processus;

typedef struct {
    processus* debut;
    processus* fin;
} listeProcessus;

extern listeProcessus listeActivable;
extern listeProcessus listeEndormi;
extern processus* proc[NBPROCESS];
extern processus* actif;

extern void ctx_sw(uint32_t* pointeur1, uint32_t* pointeur2);

/*
* S'occupe de l'initialisation de la table (pour les malloc)
*/
void init_table_processus(void);

/*
* Crée un processus
*/
int32_t cree_processus(void (*code)(void), char *nom);

/*
* retourne le nom du processus actif
*/
char* mon_nom(void);

/*
* retourne le PID du processus actif
*/
int32_t mon_pid(void);

/*
* S'occupe de l'ordonnancement
*/
void ordonnance(void);

/*
* Insere un processus en tete de la liste Activable
*/
void insertion_tete(processus* p);

/*
* Insere un processus en queue de la liste Activable
*/
void insertion_queue(processus* p);

/*
* endors un process pdt nbr_secs sec
*/
void dors(uint32_t nbr_secs);

/*
 * fonction qui transforme en string l'enum Activation
 */
char* enumString(int i);

/**
 * mets un processus en mode Zombie
 */
void fin_processus(void);

/**
 * affiche les etats de chaque processus
 */
void affiche_etats(void);

/*
* Fct d'attente
*/
void idle(void);

/*
* Fct de calcul
*/
void proc1(void);
/*
* Fct de calcul
*/
void proc2(void);
/*
* Fct de calcul
*/
void proc3(void);
/*
* Fct de calcul
*/
void proc4(void);
/*
* Fct de calcul
*/
void proc5(void);
/*
* Fct de calcul
*/
void proc6(void);
/*
* Fct de calcul
*/
void proc7(void);

#endif /* __PROCESS_H__ */
