#ifndef __ECRAN_H__
#define __ECRAN_H__

#include <inttypes.h>

/*
* renvoie un pointeur sur la case mémoire correspondant aux coordonnées fournies
*/
uint16_t *ptr_mem(uint32_t lig, uint32_t col);

/*
*qui écrit le caractère c aux coordonnées spécifiées (vous pouvez aussi ajouter des paramètres pour permettre de préciser la couleur du caractère, celle du fond ou le bit de clignotement);
*/
void ecrit_car(uint32_t lig, uint32_t col, char c, uint8_t bg_color, uint8_t color);

/*
*qui doit parcourir les lignes et les colonnes de l’écran pour écrire dans chaque case un espace en blanc sur fond noir (afin d’initialiser les formats dans
la mémoire) 
*/
void efface_ecran(void);

/*
*qui place le curseur à la position donnée 
*/
void place_curseur(uint32_t lig, uint32_t col);

/*
*qui traite un caractère donné (c’est à dire qui ’affiche si c’est un caractère normal ou qui implante l’effet voulu si c’est un caractère de contrôle) ;
*/
void traite_car(char c);

/*
*qui fait remonter d’une ligne l’affichage à l’écran
*/
void defilement(void);

/*
*qui affiche une chaine de caractères à la position courante du curseur. 
*/
void console_putbytes(const char *s, int len);

#endif /* __ECRAN_H__ */
