/**
 * @file : lcdCom_04.h
 *
 * Description :
 * Protocole de communication avec la carte LCD 2004 
 * @date    30/11/03
 * @author  Laurent Saint-Marcel
 */

#ifndef __LCD_COM_04_H__ 
#define __LCD_COM_04_H__

#define LCD_REQ_PING             0xAA
#define LCD_UART_ID              0xB2

// requetes
#define LCD_REQ_PRINTF           0x01
#define LCD_REQ_PRINTF_EOF       0x00
#define LCD_REQ_CLEAR            0x02
#define LCD_REQ_BACKLIGHT_ON     0x03
#define LCD_REQ_BACKLIGHT_OFF    0x04
#define LCD_REQ_AUTOMATIC_BL_ON  0x05
#define LCD_REQ_AUTOMATIC_BL_OFF 0x06
#define LCD_REQ_POLICE           0x07
#define LCD_REQ_LED_ON           0x20
#define LCD_REQ_LED_OFF          0x30
#define LCD_REQ_LED_BLINK        0x40

// reponses

#define LCD_EVENTS_BTN1_PUSHED   0xC0
#define LCD_EVENTS_BTN1_RELEASED 0xC1
#define LCD_EVENTS_BTN2_PUSHED   0xC2
#define LCD_EVENTS_BTN2_RELEASED 0xC3

/*
=== Protocole de communication avec la carte lcd 2004 ===

Ping:
-----
Le PC envoie LCD_REQ_PING
La carte repond LCD_UART_ID

Effacer l'ecran:
----------------
Le PC envoie LCD_REQ_CLEAR
La carte efface les 2 lignes et renvoie LCD_REQ_CLEAR

Ecrire du texte:
----------------
Le PC envoie l'octet LCD_REQ_PRINTF
La carte efface l'ecran et positionne son curseur ligne0, caractere 0.
Le PC envoie les 2 lignes de texte. Le caractere '\n' indique le passage a la 2e ligne. Le caractere LCD_REQ_PRINTF_EOF indique la fin du texte (attention le pc peut eventuellement envoyer qu'une seule ligne de texte ou des lignes de plus de 16 caractere chacunes)
La carte affiche le texte et renvoie LCD_REQ_PRINTF
Le retro eclairage est allume pour 10secondes si on est en mode AUTOMATIC_BACKLIGHT et pour une duree indeterminee dans le ca contraire

Passage en mode retro-eclairage automatique:
--------------------------------------------
Le PC envoie LCD_REQ_AUTOMATIC_BL_ON,
La carte change de mode (mais n'allume pas le retroeclairage) puis renvoie LCD_REQ_AUTOMATIC_BL_ON
Si le retro-eclairage est deja active, il sera coupe au bout de 10 secondes
En mode AUTOMATIC_BACKLIGHT, quand un message est affiche ou quand la requete LCD_REQ_BACKLIGHT_ON arrive, le retro-eclairage est active pendant 10 second seulement

Passage en mode retro-eclairage manuel:
---------------------------------------
Le PC envoie LCD_REQ_AUTOMATIC_BL_OFF,
La carte change de mode (mais n'allume pas le retroeclairage) puis renvoie LCD_REQ_AUTOMATIC_BL_OFF
En mode NON AUTOMATIC_BACKLIGHT, quand un message est affiche ou quand la requete LCD_REQ_BACKLIGHT_ON arrive, le retro-eclairage est active pour une duree indeterminee et ce jusqu'a ce que la requete LCD_REQ_BACKLIGHT_OFF arrive

Allumer le retro-eclairage:
---------------------------
Le PC envoie LCD_REQ_BACKLIGHT_ON, la carte allume le retro eclairage (voir retro-eclairage automatique) et renvoie LCD_REQ_BACKLIGHT_ON

Couper le retro-eclairage:
---------------------------
Le PC envoie LCD_REQ_BACKLIGHT_OFF, la carte etteind le retro eclairage  et renvoie LCD_REQ_BACKLIGHT_OFF

Allumer les leds auxilieres:
---------------------------
Le PC envoie LCD_REQ_LED_ON + le numero de la led a allumer sur 4 bits, la carte allume la led et renvoie LCD_REQ_LED_ON

Etteindre les leds auxilieres:
------------------------------
Le PC envoie LCD_REQ_LED_OFF + le numero de la led a etteindre sur 4 bits, la carte allume la led et renvoie LCD_REQ_LED_OFF


Gestion des boutons:
--------------------
La carte gere les boutons que sur des changements d'etats:
Quand le bouton 1 passe de l'etat non appuie a enfonce, la carte envoie LCD_EVENTS_BTN1_PUSHED
Quand le bouton 1 passe de l'etat enfonce a relache, la carte envoie LCD_EVENTS_BTN1_RELEASED

Quand le bouton 2 passe de l'etat non appuie a enfonce, la carte envoie LCD_EVENTS_BTN2_PUSHED
Quand le bouton 2 passe de l'etat enfonce a relache, la carte envoie LCD_EVENTS_BTN2_RELEASED


*/

#endif /* __LCD_COM_04_H__ */
