#ifndef HCT_H
#define HCT_H

/* module definissant les routines permettant de controler le HCTL   */
/*  dans tous les modes possibles :                                  */
/*      - commande de position                                       */
/*      - commande proportionnelle de vitesse                        */
/*      - commande integrale de vitesse (acceleration imposee)       */
/*      - commande profil trapezoidale :                             */
/*          position finale, vitesse max et acceleration imposees    */
/*  en connaissant les valeurs de :                                  */
/*      - la position reelle                                         */
/*      - la vitesse reelle                                          */




#ifdef __cplusplus
extern "C" {
#endif

  


/* types utilises pour le passage des parametres                     */

typedef unsigned char byte;

struct trois_reg {
  byte dummy_alig_octet;  /* => size 4 => can be converted to unsigned integers .. */
  byte octet1; /* msb= most significant bits = poids forts*/
  byte octet2;
  byte octet3; /* lsb= last significant bits = poids faibles */
};
  
struct deux_reg {
  /* size 2 => can be converted to unsigned short */
  byte octet1; /* msb */
  byte octet2; /*lsb */
};




/*
typedef byte TS_hctl;
*/


/* routines disponibles                                              */

  int lecture_position_1(void);
  int lecture_position_2(void);

void reinitialisation_materielle();
  /* Pour reinitialiser (en hard) les deux HCTL */
void reinitialisation_logiciel(int hctl);
 /* Pour mettre le HCTL en mode reinitialisation                     */

void initialisation_repos(int hctl);
 /* Pour mettre le HCTL en mode initialisation repos                 */

void alignement(int hctl);
 /* Pour mettre le HCTL en mode alignement                           */

void mode_commande(int hctl);
 /* Pour mettre le HCTL en mode de commande                          */

void com_position(int hctl);
 /* Pour mettre le HCTL en commande de position                    */
 /* Le HCTL doit etre en mode de commande                          */

void com_prop_vit(int hctl);
 /* Pour mettre le HCTL en commande proportionnelle de vitesse     */
 /* Le HCTL doit etre en mode de commande                          */

void com_int_vit(int hctl);
 /* Pour mettre le HCTL en commande integrale de vitesse           */
 /* Le HCTL doit etre en mode de commande                          */

void com_prof_trap(int hctl);
 /* Pour mettre le HCTL en commande de profil trapezoidal           */
 /* Le HCTL doit etre en mode de commande                           */

void pos_com(int hctl,byte pos_fort,byte pos,byte pos_faible);
 /* Ecriture de la position commande en mode commande de position   */
 /* valeurs en complement a 2                                       */

void pos_reelle(int hctl, struct trois_reg * pos );
 /* Pour lire la position reelle en mode commande de position        */
 /* valeur en complement a  2 dans un article de trois champs        */

void vit_reelle(int hctl, struct deux_reg *vit );
 /* Pour lire la vitesse reelle en mode commande proportionnelle de  */
 /* vitesse valeur en complement a 2 dans un article de deux champs  */

void vit_com(int hctl,byte pos_fort,byte pos_faible);
 /* Ecriture de la vitesse commande en mode commande prop de vitesse */
 /* valeurs en complement a 2                                        */

void vit_com1(byte pos_fort, byte pos_faible,int signe1);
void vit_com2(byte pos_fort, byte pos_faible, int signe2);

void vit_com_int(int hctl,byte vit);
 /* Ecriture de la vitesse commande en mode commande integrale       */
 /* de vitesse, valeurs en complement a 2                            */

void acc_com_int(int hctl,byte acc_com_fort,byte acc_com_faible);
 /* Ecriture de l'acceleration commande en mode commande integrale    */
 /* de vitesse, valeurs en scalaire limite a 00..FF                   */

void pos_fin(int hctl,byte pos_fin_fort,byte pos_fin,byte pos_fin_faible);
 /* Ecriture de la position finale en mode commande profil trap‚zoidal*/
 /* valeurs en complement a 2                                         */

void acc_trap(int hctl,byte acc_fort,byte acc_faible);
 /* Ecriture de l'acceleration en mode commande profil trapezoidal    */
 /* valeurs en scalaire limite a 00..FF                               */

void vit_max_trap(int hctl,byte vit);
 /* Ecriture de la vitesse max en mode commande profil trapezoidal    */
 /* valeurs en scalaire limite a 00..FF                               */

void raz_dist(int hctl);
 /* remise a zero du registre position du hctl                        */

void filter(int hctl,byte A,byte B,byte K);
 /* programmation des valeurs du filtre correcteur                    */
 /* A = pole , B = zero , K = gain                                    */

void timer(int hctl,byte T);
 /* Ecriture de la valeur du temps d'echantillonnage T = 0 .. ff      */
 /* Les valeurs extremes sont en fonction du mode de commande         */
 /* Pour plus de detail voir la notice du hctl                        */

void pwm(int hctl, byte offset);
 /* Ecriture de la valeur du PWM motor command port                   */

void config_out(int hctl, byte code);
 /* Ecriture de la configuration des sorties du HCTL                  */
 /* Codage sur les 4 premiers bits                                    */
 /* bit 0 : 1 un creneau de puissance est saute lors du changement de */
 /*         sens pour le mode PWM                                     */
 /* bit 1 : configuration du commutateur de phase (0,1 -> 3,4 phases) */
 /* bit 2 : comptage du commutateur (0,1 -> quadrature,full )         */
 /* bit 3 : toujours a zero                                           */


void read_pwm(int hctl, byte *vit );


#ifdef __cplusplus
}
#endif

#endif /* HCT_H */
