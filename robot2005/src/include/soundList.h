/**
 * @file soundList.h
 *
 * @author Laurent Saint-Marcel
 *
 * Liste des sons et musiques que le robot peut jouer
 * @see Sound
 */

#ifndef __SOUND_LIST_H__
#define __SOUND_LIST_H__

typedef enum SoundPriority {
  /* @brief le son sera joue quand les autres qui ont ete ajoutes avant 
   * lui seront termines */
  SND_PRIORITY_PUSH_BACK,
  /** @brief efface la liste des sons, stop le son courant et joue ce 
   * nouveau son immediatement */
  SND_PRIORITY_URGENT
} SoundPriority;

typedef enum SoundId {
  SOUND_0=0,
  SOUND_1,
  SOUND_2,
  SOUND_3,
  SOUND_4,
  SOUND_5,
  SOUND_6,
  SOUND_7,
  SOUND_8,
  SOUND_9,
  SOUND_10,
  SOUND_11,
  SOUND_12,
  SOUND_13,
  SOUND_14,
  SOUND_15,
  SOUND_16,

//  SOUND_ALERT_1,
//  SOUND_ALERT_2,
  SOUND_ALERT_MOTOR,
//  SOUND_ALERT_HUSTON,
//  SOUND_ALERT_RED,
  SOUND_BANG,
//  SOUND_POLICE,
  SOUND_BONJOUR,

  SOUND_GAGNE,
  SOUND_PERDU,
  SOUND_HOMOLOGUE,
  SOUND_TIMER_ALERT,

  SOUND_RELACHE_ARRET_URGENCE,
  SOUND_WAIT_JACKIN,
  SOUND_WAIT_JACKOUT,
  SOUND_WAIT_REBOOT_SWITCH,
  SOUND_GO_FOR_LAUNCH,
  SOUND_ANORMAL_EMERGENCY_STOP,
  SOUND_C_FAIT,
  
/*
  SOUND_VICTORY,
  SOUND_START_PROGRAMMATION,
  SOUND_START_INIT,
  SOUND_START_CALIBRATION,
  SOUND_SEGFAULT,
  SOUND_START_AFTER_REBOOT,
  SOUND_RELACHE_ARRET_URGENCE,
  SOUND_AUTO_CHECK_DONE,
  SOUND_AUTODESTRUCTION,
  SOUND_ON_Y_GO,
  SOUND_BATTERY_LOW,
  SOUND_ODOMETER_ALERT,
  SOUND_BUMPER,
  SOUND_COLOR,
  SOUND_ENV_DETECTOR,
  SOUND_LASER,
  SOUND_CARTE_COLOR,
  SOUND_CARTE_ENV,
  SOUND_CARTE_GONIO,
  SOUND_CARTE_LASER,
  SOUND_CARTE_LCD,
  SOUND_CARTE_SERVO, 
  SOUND_CARTE_CATAPULTE,
  SOUND_CARTE_ODOMETER,
  SOUND_CARTE_BATTERY,
  SOUND_TEST_RS,
  SOUND_CA_CACADE,
  SOUND_NE_FONCTIONNE_PAS,
  SOUND_NE_REPOND_PAS, 
  SOUND_REAR_LEFT,
  SOUND_REAR_RIGHT, 

  SOUND_ARM_LEFT_OUT,
  SOUND_ARM_LEFT_IN,
  SOUND_ARM_RIGHT_OUT,
  SOUND_ARM_RIGHT_IN,

  SOUND_ENTRY_OPENED,
  SOUND_ENTRY_CLOSED,
  SOUND_CIBLE_GAUCHE,
  SOUND_CIBLE_DROITE,
  SOUND_CIBLE_NO,
  SOUND_EAT_BALL,
  SOUND_FIRE1_BALL,
  SOUND_FIRE2_BALL,
  SOUND_FIRE3_BALL,
  SOUND_DROP_BALL,
  SOUND_COCONUT,

  SOUND_START_RIGHT,
  SOUND_START_LEFT,
  SOUND_VS_CLEANER,
  SOUND_VS_NOT_CLEANER,
  SOUND_EXPLORE_MY_ZONE_FIRST,
  SOUND_EXPLORE_SIDE_FIRST,

  SOUND_NOM_BEN,
  SOUND_NOM_CHAR,
  SOUND_NOM_JULIEN,
  SOUND_NOM_CHRISTOPHE,
  SOUND_NOM_JEFF,
  SOUND_ECOLE_ISIA,
  SOUND_ECOLE_ESINSA,
  SOUND_ECOLE_ESSI,
  SOUND_ECOLE_EK,

  SOUND_BYE,
  SOUND_BONJOUR,
  SOUND_BONJOUR_MAC,
  SOUND_BONJOUR_ARBITRE,
  SOUND_MERCI,
  SOUND_ASCENSEUR,
  SOUND_CAFE_1,
  SOUND_CAFE_2,
  SOUND_HEUREUX,
  SOUND_PERDU_STAND,
  
  SOUND_DONNE_MOI_CHOCOLAT,
  SOUND_MIAM_MIAM,
  SOUND_CADEAU,

  SOUND_BALEZE,
  SOUND_T_CON,
  SOUND_T_DIEU,
  SOUND_T_BOULET,
  SOUND_ON_S_ENCULE,
  SOUND_TELE,
  SOUND_BEING_ATTACKED,
  SOUND_ROTE,
  SOUND_UNDER_ATTACK,
  SOUND_WAIT_ORDERS,
  SOUND_WORK_COMPLETED,
  SOUND_BE_BACK,
  SOUND_BIEN_COMPRIS,
  SOUND_YES,
  SOUND_BONG,
  SOUND_BTN_YES,
  SOUND_BTN_NO,
  SOUND_DING,
  SOUND_HATE_CONFRONTATION,
  SOUND_ECRASER,
  SOUND_I_LL_BE_BACK,
  SOUND_MOUTON,
  SOUND_NEGATIVE,
  SOUND_NICELY_DONE,
  SOUND_NOBODY_IS_PERFECT,
  SOUND_NO_PROBLEMO,
  SOUND_NOTIFY,
  SOUND_ONE_MOMENT,
  SOUND_RIRE_3,
  SOUND_RIRE_2,
  SOUND_RIRE_BITCH,
  SOUND_RIRE_DIABLE,
  SOUND_RUSSE,
  SOUND_TADA,
  SOUND_TUDUU,
  SOUND_WHOOO,
  SOUND_YEHAW,
  SOUND_R2D2_1,
  SOUND_R2D2_2,
  SOUND_R2D2_3,
  SOUND_R2D2_4,
  SOUND_R2D2_5,
  SOUND_VOITURE,
  SOUND_CHIEN_MECHANT,
  SOUND_ENTER_MATRIX,
  SOUND_MATRIX_ACTIVATED,
  SOUND_MATRIX_START,
  SOUND_MATRIX_END,
  SOUND_UNAUTHORIZED,
  SOUND_TOP_LA,
  SOUND_BIEN_JOUE,
*/
  // add new sound ids before this line
  SOUND_NBR,

  SOUND_CLEAR,
  SOUND_START_MUSIC,
  SOUND_START_MUSIC_YUKULELE,
  SOUND_STOP_MUSIC,
  SOUND_SET_VOICE,
  SOUND_SET_VOLUME,
  SOUND_PLAY_MUSIC_ID

} SoundId;

#ifdef SOUND_INFO
typedef struct SoundStruct {
  SoundId id;
  const char* filename;
} SoundStruct;

static const char SOUND_DIRECTORY[] = "../sound";

SoundStruct soundList[SOUND_NBR] = {
  { SOUND_0, "0.mp3" },
  { SOUND_1, "1.mp3" },
  { SOUND_2, "2.mp3" },
  { SOUND_3, "3.mp3" },
  { SOUND_4, "4.mp3" },
  { SOUND_5, "5.mp3" },
  { SOUND_6, "6.mp3" },
  { SOUND_7, "7.mp3" },
  { SOUND_8, "8.mp3" },
  { SOUND_9, "9.mp3" },
  { SOUND_10, "10.mp3" },
  { SOUND_11, "11.mp3" },
  { SOUND_12, "12.mp3" },
  { SOUND_13, "13.mp3" },
  { SOUND_14, "14.mp3" },
  { SOUND_15, "15.mp3" },
  { SOUND_16, "16.mp3" },

//  { SOUND_ALERT_1, "alert_1.mp3" },
//  { SOUND_ALERT_2, "alert_2.mp3" },
  { SOUND_ALERT_MOTOR, "alert_incider.mp3" },
//  { SOUND_ALERT_HUSTON, "alert_problem.mp3" },
//  { SOUND_ALERT_RED, "alert_red.mp3" },
  { SOUND_BANG, "bang_ow.mp3" },
//  { SOUND_POLICE, "police.mp3" },

  { SOUND_GAGNE, "gagne.mp3" },
  { SOUND_PERDU, "garcimore.mp3" },
  { SOUND_HOMOLOGUE, "homologue.mp3" },
  { SOUND_TIMER_ALERT, "police.mp3"},
  { SOUND_BONJOUR, "bonjour.mp3" },
  

  { SOUND_RELACHE_ARRET_URGENCE, "arretdurgence.mp3" },
  { SOUND_WAIT_JACKIN, "jackin.mp3" },
  { SOUND_WAIT_JACKOUT, "jackoff0.mp3" },
  { SOUND_WAIT_REBOOT_SWITCH, "switchReboot.mp3" },
  { SOUND_GO_FOR_LAUNCH, "go_for_launch.mp3" }, 
  { SOUND_ANORMAL_EMERGENCY_STOP, "pasidiot.mp3" },
  { SOUND_C_FAIT, "c_est_fait_f1.mp3" },
  
/*  
  { SOUND_VICTORY, "victory.mp3" },
  { SOUND_START_PROGRAMMATION, "programmation.mp3" },
  { SOUND_START_INIT, "start_init.mp3" },
  { SOUND_START_CALIBRATION, "start_calibration.mp3" },
  { SOUND_SEGFAULT, "segmentationfault.mp3" },
  { SOUND_START_AFTER_REBOOT, "reboot.mp3" },
  { SOUND_RELACHE_ARRET_URGENCE, "arretdurgence.mp3" },
  { SOUND_AUTO_CHECK_DONE, "autocheck_done.mp3" },
  { SOUND_AUTODESTRUCTION, "autodestruction.mp3" },
  { SOUND_ON_Y_GO, "on_y_go.mp3" },
  { SOUND_GO_FOR_LAUNCH, "go_for_launch.mp3" },
  { SOUND_BATTERY_LOW, "batterie_faible.mp3" },
  { SOUND_ODOMETER_ALERT, "alert_red.mp3" },
  { SOUND_BUMPER, "bumper.mp3" },
  { SOUND_COLOR, "color.mp3" },
  { SOUND_ENV_DETECTOR, "detecenv.mp3" },
  { SOUND_LASER, "laser.mp3" },
  { SOUND_CARTE_COLOR, "cartecolor.mp3" },
  { SOUND_CARTE_ENV, "carteenv.mp3" },
  { SOUND_CARTE_GONIO, "cartegonio.mp3" },
  { SOUND_CARTE_LASER, "cartelaser.mp3" },
  { SOUND_CARTE_LCD, "cartelcd.mp3" },
  { SOUND_CARTE_SERVO, "carteservo.mp3" }, 
  { SOUND_CARTE_CATAPULTE, "cartemeca.mp3" },
  { SOUND_CARTE_ODOMETER, "carteodo.mp3" },
  { SOUND_CARTE_BATTERY, "cartebat.mp3" },
  { SOUND_TEST_RS, "" },
  { SOUND_CA_CACADE, "notfunctionning.mp3" },
  { SOUND_NE_FONCTIONNE_PAS, "fonctionnepas.mp3" },
  { SOUND_NE_REPOND_PAS, "repondpas.mp3" },
  { SOUND_REAR_LEFT, "arrieregauche.mp3" },
  { SOUND_REAR_RIGHT, "arrieredroit.mp3" },

  { SOUND_ARM_LEFT_OUT, "touchdown2.mp3" },
  { SOUND_ARM_LEFT_IN, "" },
  { SOUND_ARM_RIGHT_OUT, "touchdown2.mp3" },
  { SOUND_ARM_RIGHT_IN, "" },

  { SOUND_ENTRY_OPENED, "entry_free.mp3" },
  { SOUND_ENTRY_CLOSED, "entry_blocked.mp3" },
  { SOUND_CIBLE_GAUCHE, "cible_gauche.mp3" },
  { SOUND_CIBLE_DROITE, "cible_droite.mp3" },
  { SOUND_CIBLE_NO, "cible_no.mp3" },
  { SOUND_EAT_BALL, "whooo.mp3" },
  { SOUND_FIRE1_BALL, "drop1.mp3" },
  { SOUND_FIRE2_BALL, "drop2.mp3" },
  { SOUND_FIRE3_BALL, "drop3.mp3" },
  { SOUND_DROP_BALL, "touchdown4.mp3" },
  { SOUND_COCONUT, "coconut.mp3" },

  { SOUND_START_RIGHT, "" },
  { SOUND_START_LEFT, "" },
  { SOUND_VS_CLEANER, "" },
  { SOUND_VS_NOT_CLEANER, "" },
  { SOUND_EXPLORE_MY_ZONE_FIRST, "" },
  { SOUND_EXPLORE_SIDE_FIRST, "" },

  { SOUND_NOM_BEN, "nom_ben.mp3" },
  { SOUND_NOM_CHAR, "nom_charette.mp3" },
  { SOUND_NOM_JULIEN, "" },
  { SOUND_NOM_CHRISTOPHE, "" },
  { SOUND_NOM_JEFF, "" },
  { SOUND_ECOLE_ISIA, "ecole_isia.mp3" },
  { SOUND_ECOLE_ESINSA, "ecole_esinsa.mp3" },
  { SOUND_ECOLE_ESSI, "ecole_essi.mp3" },
  { SOUND_ECOLE_EK, "eucalyptus.mp3" },

  { SOUND_BYE, "a_plus_tard.mp3" },
  { SOUND_BONJOUR_MAC, "bonjour_mac.mp3" },
  { SOUND_BONJOUR_ARBITRE, "bonjour_arbitre.mp3" },
  { SOUND_MERCI, "merci.mp3" },
  { SOUND_ASCENSEUR, "ascenceur.mp3" },
  { SOUND_CAFE_1, "cafe1.mp3" },
  { SOUND_CAFE_2, "cafe2.mp3" },
  { SOUND_HEUREUX, "heureux.mp3" },
  { SOUND_PERDU_STAND, "perdu_stand.mp3" },
  { SOUND_DONNE_MOI_CHOCOLAT, "rhum.mp3" },
  { SOUND_MIAM_MIAM, "miamiam.mp3" },
  { SOUND_CADEAU, "cadeau.mp3" },

  { SOUND_BALEZE, "baleze.mp3" },
  { SOUND_T_CON, "t_es_con.mp3" },
  { SOUND_T_DIEU, "dieu.mp3" },
  { SOUND_T_BOULET, "boulet.mp3" },
  { SOUND_ON_S_ENCULE, "encule.mp3" },
  { SOUND_TELE, "tele.mp3" },
  { SOUND_BEING_ATTACKED, "be_attacked.mp3" },
  { SOUND_ROTE, "rote.mp3" },
  { SOUND_UNDER_ATTACK, "under_attack.mp3" },
  { SOUND_WAIT_ORDERS, "wait_orders.mp3" },
  { SOUND_WORK_COMPLETED, "work_completed.mp3" },
  { SOUND_BE_BACK, "be_back.mp3" },
  { SOUND_BIEN_COMPRIS, "bien_compris.mp3" },
  { SOUND_YES, "yes.mp3" },
  { SOUND_BONG, "bong.mp3" },
  { SOUND_BTN_YES, "btn_yes.mp3" },
  { SOUND_BTN_NO, "btn_no.mp3" },
  { SOUND_DING, "ding.mp3" },
  { SOUND_HATE_CONFRONTATION, "" },
  { SOUND_ECRASER, "ecraser.mp3" },
  { SOUND_I_LL_BE_BACK, "illbback.mp3" },
  { SOUND_MOUTON, "mouton.mp3" },
  { SOUND_NEGATIVE, "negative.mp3" },
  { SOUND_NICELY_DONE, "nicelydone.mp3" },
  { SOUND_NOBODY_IS_PERFECT, "nobodys_perfect.mp3" },
  { SOUND_NO_PROBLEMO, "noproblemo.mp3" },
  { SOUND_NOTIFY, "notify.mp3" },
  { SOUND_ONE_MOMENT, "one_moment.mp3" },
  { SOUND_RIRE_3, "rire_3.mp3" },
  { SOUND_RIRE_2, "rire_2.mp3" },
  { SOUND_RIRE_BITCH, "rire_bitch2.mp3" },
  { SOUND_RIRE_DIABLE, "rire_diable.mp3" },
  { SOUND_RUSSE, "russe.mp3" },
  { SOUND_TADA, "tada.mp3" },
  { SOUND_TUDUU, "tuduu.mp3" },
  { SOUND_WHOOO, "whooo.mp3" },
  { SOUND_YEHAW, "yehaw.mp3" },
  { SOUND_R2D2_1, "r2d2wst1.mp3" },
  { SOUND_R2D2_2, "r2d2wst2.mp3" },
  { SOUND_R2D2_3, "r2d2wst3.mp3" },
  { SOUND_R2D2_4, "r2d2wst4.mp3" },
  { SOUND_R2D2_5, "r2d2wst5.mp3" },
  { SOUND_VOITURE, "car_burnout.mp3"},
  { SOUND_CHIEN_MECHANT, "degage.mp3"},
  { SOUND_ENTER_MATRIX, "pillule.mp3"},
  { SOUND_MATRIX_ACTIVATED, "strange.mp3"},
  { SOUND_MATRIX_START, "presence.mp3"},
  { SOUND_MATRIX_END, "tropvieu.mp3"},
  { SOUND_UNAUTHORIZED, "cote_obscure.mp3"},
  { SOUND_TOP_LA, "topla.mp3"},
  { SOUND_BIEN_JOUE, "bienjoue.mp3"}

*/
};

#ifdef SOUND_SHM_INFO
const char* SOUND_SHM_FILENAME = "/dev/dsp";
#endif // SOUND_SHM_INFO

typedef struct SoundShmStruct {
    SoundId id;
    unsigned int  data; // voice, volume
    unsigned char newData; // class Sound writes 1 when it send new datas, 

    // Sounplayer reset it
    unsigned char isPlayingSound; // mis a jour par le soundPlayer
    int aliveCounter;
} SoundShmStruct;

#endif // SOUND_INFO

#endif // __SOUND_LIST_H__
