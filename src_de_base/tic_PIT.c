#include "ecran.h"
#include "tic_PIT.h"
#include "cpu.h"
#include <stdio.h>
#include "segment.h"
#include "process.h"

uint8_t compteur_20ms=0;
uint32_t compteur_temps_s=0;

void ecrit_temps(char* chaine, uint32_t taille){
    // ecrit en haut à droite
    for (uint32_t i = 0; i < taille; i++)
    {
        ecrit_car(0,80-taille+i,chaine[i],0,15);
    }
}

void init_traitant_IT(uint32_t num_IT,void (*traitant)(void)){
    uint32_t* addr_memoire = (uint32_t*) 0x1000+2*num_IT; 
    *addr_memoire= KERNEL_CS<<16 | ((uint32_t)traitant&0x0000FFFF);
    *(addr_memoire+1)=(uint32_t) (((uint32_t) traitant&0xFFFF0000) |0x8E00);
}

void masque_IRQ(uint32_t num_IRQ, bool masque){ 
    // :)
    unsigned char tableau=inb(0x21);
    if(masque){
        tableau|=(1<<num_IRQ);
    }
    else{
        tableau &=0xFF^1<<num_IRQ ;
    }
    outb(tableau,0x21);

}

void tic_PIT(void){
    outb(0x20,0x20);
    if (compteur_20ms==0){
        compteur_20ms++;

        compteur_temps_s++;
    }
    else{
        compteur_20ms=(compteur_20ms+1)%50;
    }
    char clock[9];
    // permet d'afficher l'horloge a chaque fois
    sprintf(clock,"%02d:%02d:%02d",compteur_temps_s/3600 %24,compteur_temps_s/60 %60,compteur_temps_s%60); 
    ecrit_temps(clock,9);
    ordonnance(); // change les processus à chaque tic d'horloge
}

uint32_t nbr_secondes(){ // utile quand les processus dorment
    return compteur_temps_s;
}

