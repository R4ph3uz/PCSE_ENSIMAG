#include <cpu.h>
#include <inttypes.h>
#include "start.h"
#include "ecran.h"
#include "tic_PIT.h"
#include <stdio.h>
#include "process.h"


// une fonction bien connue
uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1) {
        res = 1;
    } else {
        res = fact(n - 1) * n;
    }
    return res;
}


void kernel_start(void)
{
    //TP1 -> voir ecran.c
    efface_ecran();
    printf("test\nSur plusieurs ligne parcequ'on slay et en gros cette chaine et tellement longue que normalement elle déborde sur la ligne d'apres\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nEt la en gros j'ecris un truc en bas");
    defilement();
    efface_ecran();
    place_curseur(0,0);
    
    
    //INITIALISATIONS
    //initialisations table vecteur
    init_traitant_IT(32,traitant_IT_32);
    masque_IRQ(0,false);
    // definit l'horloge -> voir tic_PIT.c
    outb(0x34,0x43);
    outb((0x1234DD / 50) & 0xFF, 0x40);
    outb((((0x1234DD / 50) & 0xFF00) >> 8), 0x40); // donc à 50Hz
    //initialisation de processus
    init_table_processus();
    strcpy(proc[0]->nom,"idle");
    proc[0]->etat=ELU;
    actif=proc[0];
    listeActivable.debut=proc[0];
    int32_t pid= cree_processus(proc1,"proc1");
    pid= cree_processus(proc2,"proc2");
    pid= cree_processus(proc3,"proc3");
    listeActivable.fin=proc[pid];

    //demasquage des interruptions externes
    // sti(); // si commenté : masquage

    //demarrage idle
    idle();


    // on ne doit jamais sortir de kernel_start
    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}

