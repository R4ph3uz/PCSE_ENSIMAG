#include "process.h"
#include <tinyalloc.h>
#include "cpu.h"
#include "tic_PIT.h"
#include "ecran.h"

processus* proc[NBPROCESS];
processus* actif;
int nbProcess=0;
listeProcessus listeActivable;
listeProcessus listeEndormi;
listeProcessus listeMort;

void init_table_processus(void){
    listeActivable.debut=NULL;
    listeActivable.fin=NULL;
    for (int i = 0; i < NBPROCESS; i++){ // malloc les choses communes à chaque processus
        proc[i]=malloc(sizeof(processus));
        proc[i]->pid=i;
        proc[i]->etat=ACTIVABLE;
        proc[i]->zoneSauvegarde[1]=(uint32_t)&(proc[i]->pileExecution[510]);
        if(i!=0){
            proc[i-1]->suivant=proc[i];
        }
    }

}

char* mon_nom(void){
    return actif->nom;
}

int32_t mon_pid(void){
    return actif->pid;
}


int32_t cree_processus(void (*code)(void), char *nom){ // pour creer un processus
    if (listeMort.debut == NULL) // personne n'est mort
    {
        int nvPID=nbProcess+1;
        if (nbProcess==NBPROCESS){
            return -1;
        }
        strcpy(proc[nvPID]->nom , nom); // on lui met son nom
        proc[nvPID]->pileExecution[510]=(uint32_t) code; // normalement c'est 511 pour qu'il execute son code mais pour la derniere partie il faut que ça soit 510
        proc[nvPID]->pileExecution[511]=(uint32_t) fin_processus; // pour qu'on sache comment il s'arrête
        nbProcess++;
        return nvPID;
    }
    else{ // on recycle
        int nvPID = listeMort.debut->pid; // on recupere celui de la liste zombie
        listeMort.debut=listeMort.debut->suivant;
        strcpy(proc[nvPID]->nom , nom); // on lui met son nom
        proc[nvPID]->pileExecution[510]=(uint32_t) code; // normalement c'est 511 pour qu'il execute son code mais pour la derniere partie il faut que ça soit 510
        proc[nvPID]->pileExecution[511]=(uint32_t) fin_processus; // pour qu'on sache comment il s'arrête
        return nvPID;
    }
}

void insertion_tete(processus* p){
    if(listeActivable.debut == NULL){
        listeActivable.debut=p;
        listeActivable.fin=p;
        return;
    }
    p->suivant=listeActivable.debut;
    listeActivable.debut=p;
}

processus* extraction_tete(){
    processus* p = listeActivable.debut;
    listeActivable.debut=p->suivant;
    return p;
}

void insertion_queue(processus* p){
    if(listeActivable.debut == NULL){
        listeActivable.debut=p;
        listeActivable.fin=p;
        return;
    }
    p->suivant=NULL;
    listeActivable.fin->suivant=p;
    listeActivable.fin=listeActivable.fin->suivant;
}
//ancien ordonnance
// void ordonnance(void){
//     int pidEnCours= mon_pid();
//     int prochainPID=(pidEnCours+1)%NBPROCESS;
//     actif->etat=ACTIVABLE;
//     actif=proc[prochainPID];
//     actif->etat=ELU;
//     ctx_sw(proc[pidEnCours]->zoneSauvegarde,proc[prochainPID]->zoneSauvegarde);
// }

char* enumString(int i){ // fonction qui transforme en string l'enum Activation
    switch (i)
    {
    case 0:
        return "ELU";
        break;
    case 1:
        return "ACTIVABLE";
        break;
    case 2:
        return "ENDORMI";
        break;
    case 3:
        return "ZOMBIE";
        break;
    default:
        return "";
        break;
    }
}

void affiche_etats(void){ // affiche les etats de chaque processus
    for (int i=1; i < NBPROCESS+1; i++)
    {
        processus* p=proc[i];
        for (uint32_t j = 0; j < 50; j++)
        {
            ecrit_car(i+3,80-30+j,p->nom[j],0,15);
        }
        for (uint16_t k = 0; k < 8; k++)
        {
            ecrit_car(i+3,80-8+k,enumString(p->etat)[k],0,15);
        }
    }
}

void ordonnance(){
    processus* p = extraction_tete();
    // affiche_etats();
    // on saute le zombie il sort des activable
    if (p->etat == ZOMBIE)
    {
        ordonnance();
    }
    p->etat=ACTIVABLE;
    insertion_queue(p);
    
    //check le reveil
    processus* reveil=listeEndormi.debut;
    processus* ancient=NULL;
    while (reveil!=NULL)
    {
        
        if (reveil->heure_reveil <= nbr_secondes())
        {
            if(ancient==NULL){
                listeEndormi.debut=reveil->suivant;
            }
            else{
                ancient->suivant=reveil->suivant;
            }
            insertion_tete(reveil);
            reveil = (ancient == NULL) ? listeEndormi.debut : ancient->suivant;
        }
        else{
            ancient=reveil;
            reveil=reveil->suivant;
        }
    }
    
    actif=listeActivable.debut;
    actif->etat=ELU;
    ctx_sw(p->zoneSauvegarde,actif->zoneSauvegarde);
}

void dors(uint32_t nbr_secs){
    processus* aDormir = extraction_tete();
    aDormir->etat=ENDORMI;
    aDormir->heure_reveil=nbr_secondes()+nbr_secs; // mets un temps s'il dort
    aDormir->suivant=listeEndormi.debut;
    listeEndormi.debut=aDormir;

    actif=listeActivable.debut;
    actif->etat=ELU;
    ctx_sw(aDormir->zoneSauvegarde,actif->zoneSauvegarde);
}

void fin_processus(){
    processus* mort = extraction_tete();
    mort->etat= ZOMBIE;
    mort->suivant= listeMort.debut;
    listeMort.debut = mort;
    // fait mourir le processus

    actif = listeActivable.debut;
    actif->etat =ELU;
    ctx_sw(mort->zoneSauvegarde, actif->zoneSauvegarde);
}

// donne la main à proc1 et s'arrête

// void idle(void)
// {
// printf("[idle] je tente de passer la main a proc1...\n");
// ctx_sw(proc[0].zoneSauvegarde,proc[1].zoneSauvegarde);
// }
// void proc1(void)
// {
// printf("[proc1] idle m’a donne la main\n");
// printf("[proc1] j’arrete le systeme\n");
// hlt();
// }

// aller retour 3 fois entre idle et proc1

// void idle(void){
//     for (int i = 0; i < 3; i++) {
//         printf("[idle] je tente de passer la main a proc1...\n");
//         ctx_sw(proc[0].zoneSauvegarde,proc[1].zoneSauvegarde);
//         printf("[idle] proc1 m'a redonne la main\n");
//     }
//     printf("[idle] je bloque le systeme\n");
//     hlt();
// }
// void proc1(void){
//     for (;;) {
//         printf("[proc1] idle m’a donne la main\n");
//         printf("[proc1] je tente de lui la redonner...\n");
//         ctx_sw(proc[1].zoneSauvegarde,proc[0].zoneSauvegarde);
//     }
// }


// pour cette partie du tp, chaque processus donne la main à son prochain indefiniment.

// void idle(void)
// {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }
// void proc1(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }
// void proc2(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }
// void proc3(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }
// void proc4(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }
// void proc5(void) {For help, type "help".

//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }
// void proc6(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }
// void proc7(void) {
//     for (;;) {
//         printf("[%s] pid = %i\n", mon_nom(), mon_pid());
//         ordonnance();
//     }
// }

void idle(void){
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),mon_nom(), mon_pid());
        sti();
        hlt();
        cli();
    }
}

void proc1(void)
{
    for (int32_t i = 0; i < 2; i++) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
        mon_nom(), mon_pid());
        dors(2); // mets en place le fait qu'il dorme pdt X secondes
    }
    // fin_processus() // si doit etre terminé à la main.
}
void proc2(void)
{
    for (int32_t i = 0; i < 2; i++) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
        mon_nom(), mon_pid());
        dors(3);
    }
}
void proc3(void)
{
    for (;;) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(),
        mon_nom(), mon_pid());
        // sti();
        // hlt();
        // cli();
        dors(5);
    }
}
void proc4(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();
        hlt();
        cli();
    }
}
void proc5(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();
        hlt();
        cli();
    }
}
void proc6(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();
        hlt();
        cli();
    }
}
void proc7(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();
        hlt();
        cli();
    }
}
