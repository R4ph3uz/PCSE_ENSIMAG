#ifndef __TICPIT_H__
#define __TICPIT_H__

#include <inttypes.h>
#include <stdbool.h>

/*
* qui prend en paramètre une chaine de caractères (ainsi que sa taille) et l’affiche en haut à droite de l’écran : c’est cette fonction qui sera appelée par le traitant d’interruption quand on devra mettre à jour l’affichage de l’heure ;
*/
void ecrit_temps(char* chaine, uint32_t taille);

/*
*écrire le traitant de l’interruption 32 qui affiche à l’écran le temps écoulé depuis le démarrage du système : ce traitant commence par une partie en assembleur pour sauvegarder les registres (qui est fournie dans le fichier traitant.S)
*/
void tic_PIT(void);

/*
* Mets le IRQX à masque
*/
void masque_IRQ(uint32_t num_IRQ, bool masque);

/*
* Définis la table de vecteur
*/
void init_traitant_IT(uint32_t num_IT,void (*traitant)(void));

extern void traitant_IT_32(void);

/*
* Renvois le temps en secondes depuis le démarrage
*/
uint32_t nbr_secondes();

#endif /* __TICPIT_H__ */
