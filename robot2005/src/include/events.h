/**
 * @file events.h
 *
 * @author Laurent Saint-Marcel lstmarcel@yahoo.fr
 *
 * Gestion des evenements: messages qui vont des couchent basses
 * vers le haut niveau et la strategie. Par exemple : collision detectee, 
 * arret d'urgence presse.
 * Les capteurs (bumpers, roues bloquees), la necessite de changer de 
 * strategie la fin d'un deplacement declenchent des evenements. Le partie
 * principale du programme qui gere le robot se met en attente d'une 
 * selection d'evenements et quand ils arrive, elle met a jour sa strategie
 * est ses consignes de deplacement.
 *
 * Pour plus de simplicite, les evenements appartiennent a un groupe qui
 * est lui meme un evenement, ce qui permet de se mettre en attente d'un 
 * ensemble d'evenements assez facilement. Par exemple, Tous les bumpers
 * declenchent l'evenement collision!
 */

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "robotBase.h"

// ============================================================================
// ===============================  typedef   =================================
// ============================================================================

// TODO: I renamed the Events - enum into EventsEnum (so it's easy to find and
// replace to give it a better name. [flo]
#define Events         EventsManagerCL::instance()
#define EVENTS_MANAGER_DEFAULT EventsManagerActiveCL

typedef enum EventsEnum {
    EVENTS_GROUP_NONE, 
    EVENTS_GROUP_BUMPER,
    EVENTS_GROUP_BRIDGE,
    EVENTS_GROUP_USER,
    EVENTS_GROUP_ENV_DETECTOR,
    EVENTS_GROUP_MOTOR,
    EVENTS_GROUP_MATCH,
    
    EVENTS_MOVE_END,       // fin de trajectoire
    EVENTS_PWM_ALERT,      // moteurs bloques
  
    EVENTS_JACKIN,         //jack de depart
    EVENTS_EMERGENCY_STOP, // arret d'urgence
    EVENTS_USER_ABORT,     // CTRL+C
    EVENTS_BUTTON_YES,
    EVENTS_BUTTON_NO,
    EVENTS_SWITCH_MATCH,

    EVENTS_GAME_OVER,      // timer fin du match
    EVENTS_TIMER_ALERT,    // timer, proche de la fin du match

    EVENTS_BRIDGE_BUMP_LEFT,
    EVENTS_BRIDGE_BUMP_RIGHT,

    EVENTS_ENV_TOP_FRONT,
    EVENTS_ENV_TOP_RIGHT,
    EVENTS_ENV_TOP_LEFT,
    EVENTS_ENV_MIDDLE_LEFT,
    EVENTS_ENV_MIDDLE_RIGHT,
    EVENTS_ENV_BOTTOM_BACK,
    EVENTS_ENV_MIDDLE_FRONT_LEFT,
    EVENTS_ENV_MIDDLE_FRONT_RIGHT,
    EVENTS_ENV_MIDDLE_BACK_LEFT,
    EVENTS_ENV_MIDDLE_BACK_RIGHT,
    EVENTS_ENV_BOTTOM_LEFT_1,
    EVENTS_ENV_BOTTOM_LEFT_2,
    EVENTS_ENV_BOTTOM_RIGHT_1,
    EVENTS_ENV_BOTTOM_RIGHT_2,
    EVENTS_ENV_BOTTOM_BACK_LEFT,
    EVENTS_ENV_BOTTOM_BACK_RIGHT,
    EVENTS_ENV_MIDDLE_FRONT_CENTER,
    
    EVENTS_BUMPER_FC,  // front center
    EVENTS_BUMPER_FR,  // front right
    EVENTS_BUMPER_FL,  // front left
    EVENTS_BUMPER_SLF, // side left front
    EVENTS_BUMPER_SLR, // side left rear
    EVENTS_BUMPER_SRF, // side right front
    EVENTS_BUMPER_SRR, // side right rear
    EVENTS_BUMPER_RR, // rear right
    EVENTS_BUMPER_RL, // rear left
    EVENTS_BUMPER_RC, // rear center
    EVENTS_BUMPER_BORDURE_RR, // bordure rear right (z=15mm)
    EVENTS_BUMPER_BORDURE_RL, // bordure rear left
    EVENTS_BUMPER_TOP_F, // top front   (z=390mm) => detection barre d'enbut
    EVENTS_BUMPER_TOP_R, // top rear 
   
     EVENTS_NBR
} EventsEnum;

typedef struct EventsInfoStruct {
    EventsEnum evt;
    char* name;
    EventsEnum groupEvt;
} EventsInfoStruct;

typedef bool (*EventsFn)(bool[]);

#ifdef EVENTS_INFO
static EventsInfoStruct EventsInfo[EVENTS_NBR] = {  
    { EVENTS_GROUP_NONE,   "EVT_GROUP_NONE",  EVENTS_GROUP_NONE},
    { EVENTS_GROUP_BUMPER, "EVT_GROUP_BUMPER",EVENTS_GROUP_NONE},
    { EVENTS_GROUP_BRIDGE, "EVT_GROUP_BRIDGE",EVENTS_GROUP_NONE},
    { EVENTS_GROUP_USER,   "EVT_GROUP_USER",  EVENTS_GROUP_NONE},
    { EVENTS_GROUP_ENV_DETECTOR, "EVT_GROUP_ENV_DETECTOR", EVENTS_GROUP_NONE},
    { EVENTS_GROUP_MOTOR,  "EVT_GROUP_MOTOR", EVENTS_GROUP_NONE},
    { EVENTS_GROUP_MATCH,  "EVT_GROUP_MATCH", EVENTS_GROUP_NONE},
   
    { EVENTS_MOVE_END,       "EVT_MOVE_END",  EVENTS_GROUP_MOTOR },  
    { EVENTS_PWM_ALERT,      "EVT_PWM_ALERT", EVENTS_GROUP_MOTOR  },

    { EVENTS_JACKIN,         "EVT_JACKIN",          EVENTS_GROUP_USER },    
    { EVENTS_EMERGENCY_STOP, "EVT_EMERGENCY_STOP",  EVENTS_GROUP_USER  },
    { EVENTS_USER_ABORT,     "EVT_USER_ABORT",      EVENTS_GROUP_USER  },  
    { EVENTS_BUTTON_YES,     "EVT_BUTTON_YES",      EVENTS_GROUP_USER  },  
    { EVENTS_BUTTON_NO,      "EVT_BUTTON_NO",       EVENTS_GROUP_USER  },   
    { EVENTS_SWITCH_MATCH,   "EVT_SWITCH_MATCH",    EVENTS_GROUP_USER  },

    { EVENTS_GAME_OVER,      "EVT_GAME_OVER",       EVENTS_GROUP_MATCH },   
    { EVENTS_TIMER_ALERT,    "EVT_TIMER_ALERT",     EVENTS_GROUP_MATCH }, 

    { EVENTS_BRIDGE_BUMP_LEFT, "EVT_BRIDGE_BUMP_LEFT", EVENTS_GROUP_BRIDGE }, 
    { EVENTS_BRIDGE_BUMP_RIGHT,"EVT_BRIDGE_BUMP_RIGHT",EVENTS_GROUP_BRIDGE }, 
   
    { EVENTS_ENV_TOP_FRONT, "EVT_ENV_TOP_FRONT",    EVENTS_GROUP_ENV_DETECTOR},
    { EVENTS_ENV_TOP_RIGHT, "EVT_ENV_TOP_RIGHT",    EVENTS_GROUP_ENV_DETECTOR},
    { EVENTS_ENV_TOP_LEFT,  "EVT_ENV_TOP_LEFT",     EVENTS_GROUP_ENV_DETECTOR},
    { EVENTS_ENV_MIDDLE_LEFT,"EVT_ENV_MIDDLE_LEFT", EVENTS_GROUP_ENV_DETECTOR},
    {EVENTS_ENV_MIDDLE_RIGHT,"EVT_ENV_MIDDLE_RIGHT",EVENTS_GROUP_ENV_DETECTOR},
    { EVENTS_ENV_BOTTOM_BACK,"EVT_ENV_BOTTOM_BACK", EVENTS_GROUP_ENV_DETECTOR},
    { EVENTS_ENV_MIDDLE_FRONT_LEFT, "EVT_ENV_MIDDLE_FRONT_LEFT", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_MIDDLE_FRONT_RIGHT,"EVT_ENV_MIDDLE_FRONT_RIGHT",
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_MIDDLE_BACK_LEFT,  "EVT_ENV_MIDDLE_BACK_LEFT", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_MIDDLE_BACK_RIGHT, "EVT_ENV_MIDDLE_BACK_RIGHT", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_BOTTOM_LEFT_1,     "EVT_ENV_BOTTOM_LEFT_1", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_BOTTOM_LEFT_2,     "EVT_ENV_BOTTOM_LEFT_2", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_BOTTOM_RIGHT_1,    "EVT_ENV_BOTTOM_RIGHT_1", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_BOTTOM_RIGHT_2,    "EVT_ENV_BOTTOM_RIGHT_2", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_BOTTOM_BACK_LEFT,     "EVT_ENV_BOTTOM_BACK_LEFT", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_BOTTOM_BACK_RIGHT,    "EVT_ENV_BOTTOM_BACK_RIGHT", 
        EVENTS_GROUP_ENV_DETECTOR },
    { EVENTS_ENV_MIDDLE_FRONT_CENTER,    "EVT_ENV_MIDDLE_FRONT_CENTER", 
        EVENTS_GROUP_ENV_DETECTOR },  

    { EVENTS_BUMPER_FC, "EVT_BUMP_FRONT_CENTER", EVENTS_GROUP_BUMPER }, 
    { EVENTS_BUMPER_FR, "EVT_BUMP_FRONT_RIGHT", EVENTS_GROUP_BUMPER }, 
    { EVENTS_BUMPER_FL, "EVT_BUMP_FRONT_LEFT", EVENTS_GROUP_BUMPER }, 
    { EVENTS_BUMPER_SLF,"EVT_BUMP_SIDE_LEFT_FRONT", EVENTS_GROUP_BUMPER },
    { EVENTS_BUMPER_SLR,"EVT_BUMP_SIDE_LEFT_REAR", EVENTS_GROUP_BUMPER }, 
    { EVENTS_BUMPER_SRF,"EVT_BUMP_SIDE_RIGHT_FRONT", EVENTS_GROUP_BUMPER },
    { EVENTS_BUMPER_SRR,"EVT_BUMP_SIDE_RIGHT_REAR", EVENTS_GROUP_BUMPER },
    { EVENTS_BUMPER_RR, "EVT_BUMP_REAR_RIGHT", EVENTS_GROUP_BUMPER },
    { EVENTS_BUMPER_RL, "EVT_BUMP_REAR_LEFT", EVENTS_GROUP_BUMPER },
    { EVENTS_BUMPER_RC, "EVT_BUMP_REAR_CENTER", EVENTS_GROUP_BUMPER },
    { EVENTS_BUMPER_BORDURE_RR,"EVT_BUMP_BORDURE_RIGHT",EVENTS_GROUP_BUMPER },
    { EVENTS_BUMPER_BORDURE_RL,"EVT_BUMP_BORDURE_LEFT", EVENTS_GROUP_BUMPER },
    { EVENTS_BUMPER_TOP_F, "EVT_BUMP_TOP_F", EVENTS_GROUP_BUMPER }, 
    { EVENTS_BUMPER_TOP_R, "EVT_BUMP_TOP_R", EVENTS_GROUP_BUMPER }, 
};
#endif // EVENTS_INFO

// ============================================================================
// =============================  class EventsManager   =======================
// ============================================================================

typedef void (*EventsFunctionPtr)(void*, EventsEnum); 
typedef struct EventsCallBackStruct {
  EventsCallBackStruct():function(NULL), userData(NULL) {name[0]=0;}

  EventsFunctionPtr function;
  void*             userData;
  char              name[32];
} EventsCallBackStruct;

/** 
 * @class EventsManager
 * Gestion des evenements: messages qui vont des couchent bassent bassent
 * vers le haut niveau et la strategie. Par exemple : collision detectee, 
 * arret d'urgence pressé
 */
class EventsManagerCL : public RobotComponent 
{
 public:
    virtual ~EventsManagerCL();
    /** Retourne true si le module est valide*/
    bool validate();
    /** Unraise all events and clear disable filter */
    bool reset();
    void print(bool displayTrue=true,
	       bool displayFalse=false);

    /** @brief Retourne un pointeur vers l'instance unique d'eventsManager */
    static EventsManagerCL* instance();

    /** @brief Déclenche un évenement et l'evenement correspondant au group */
    virtual void raise(EventsEnum evt)=0;
    /** @brief Annule un évenement */
    virtual void unraise(EventsEnum evt)=0;
    /** @brief unraise tous les EventsGroups, cette fonction est appelee
     * par les fonctions wait
     */
    void unraiseEventsGroups();
    void unraiseEventsBumpers();

    /** @brief Active un évenement: les callbacks seront appelés et 
     * l'évenement pourra declencher les fonctions wait. Par défaut un 
     * évenement est enablé 
     */
    void enable(EventsEnum evt);
    /** @brief Désactive un évenement: les callbacks ne seront pas appelés et 
     * l'évenement ne pourra pas declencher les fonctions wait. Par défaut un 
     * évenement est enablé 
     */
    void disable(EventsEnum evt);
    /** @brief Enregistre un callback exécuté par le thread qui fait un move 
     * (permet de thrower des exceptions dans le callback quand un evenement est raisé
     */
    void registerCallback(EventsEnum evt,
			  void* userData,
                          EventsFunctionPtr evtCallback,
			  const char* callBackName);
    /** @brief Enregistre un callback exécuté par la fonction raise */
    void registerImmediatCallback(EventsEnum evt,
				  void* userData,
                                  EventsFunctionPtr evtCallback,
				  const char* callBackName);
    /** @brief Enregistre un callback exécuté par le thread qui fait un move 
     * (permet de thrower des exceptions dans le callback quand un evenement est unraisé
     */
    void registerNotCallback(EventsEnum evt,
			     void* userData,
			     EventsFunctionPtr evtCallback,
			     const char* callBackName);
    /** @brief Enregistre un callback exécuté par la fonction unraise */
    void registerNotImmediatCallback(EventsEnum evt,
				     void* userData,
				     EventsFunctionPtr evtCallback,
				     const char* callBackName);


    /** @brief Function qui ne retourne que quand l'évenement evt est raisé */
    virtual void wait(EventsEnum evt)=0;
    /** @brief Fonction qui attend que la fonction evtFn retourne vrai */
    virtual void wait(EventsFn evtFn)=0;
    /** @brief Function qui ne retourne que quand l'évenement evt est unraisé */
    virtual void waitNot(EventsEnum evt)=0;
    /** @brief Fonction qui attend que la fonction evtFn retourne faux */
    virtual void waitNot(EventsFn evtFn)=0;

    /** @brief Retourne TRUE si l'evenement a permis de sortir du dernier wait
	ou waitNot. Utilie pour savoir quels evenement on declenché 
	l'EventsFunction. A utiliser de preference au check */
    bool isInWaitResult(EventsEnum evt);

    /** @brief Retourne TRUE si la fonction evtFn retourne true */
    bool check(EventsFn evtFn);
    /** @brief Retourne TRUE si l'évenement est raisé */
    bool check(EventsEnum evt);

    /** @brief Retourne le nom de l'évenement correspondant */
    const char* evtName(EventsEnum evt);

    bool isEnabled(EventsEnum evt);

 protected:
    static EventsManagerCL* evtMgr_;
    bool evtStatus_[EVENTS_NBR];
    bool evtWaitStatus_[EVENTS_NBR]; // copie de evtStatus quand on sort de wait
    bool evtEnables_[EVENTS_NBR];
    EventsCallBackStruct evtImmediatCallbacks_[EVENTS_NBR];
    EventsCallBackStruct evtCallbacks_[EVENTS_NBR];
    bool evtCallbackMustBeRun_[EVENTS_NBR];
    EventsCallBackStruct evtNotImmediatCallbacks_[EVENTS_NBR];
    EventsCallBackStruct evtNotCallbacks_[EVENTS_NBR];
    bool evtNotCallbackMustBeRun_[EVENTS_NBR];

 public:
    EventsManagerCL();
 protected:
    void runCallbacks();
};

/** 
 * @class EventsManagerPassive
 * Attente des evenements de maniere passive avec des attentes de threads. 
 * Efficace et on est reveille tout le temps tout de suite, mais risque de 
 * deadlock...
 */
class EventsManagerPassiveCL : public EventsManagerCL
{
 public:
    EventsManagerPassiveCL();
    virtual ~EventsManagerPassiveCL();
    
    /** @brief Déclenche un évenement et l'evenement correspondant au group */
    void raise(EventsEnum evt);
    /** @brief Annule un évenement */
    void unraise(EventsEnum evt);

    /** @brief Function qui ne retourne que quand l'évenement evt est raisé */
    void wait(EventsEnum evt);
    /** @brief Fonction qui attend que la fonction evtFn retourne vrai */
    void wait(EventsFn evtFn);
    /** @brief Function qui ne retourne que quand l'évenement evt est unraisé */
    void waitNot(EventsEnum evt);
    /** @brief Fonction qui attend que la fonction evtFn retourne faux */
    void waitNot(EventsFn evtFn);
};

/** 
 * @class EventsManagerActive
 * Attente des evenements de maniere active: on va verifier les evenements
 * periodiquement.
 * Probleme: risque de manquer un evenement qui est raise et unraise trop vite
 * Avantage: pas de risque de deadlock
 */
class EventsManagerActiveCL : public EventsManagerCL
{
 public:
    EventsManagerActiveCL();
    virtual ~EventsManagerActiveCL();
    
    /** @brief Déclenche un évenement et l'evenement correspondant au group */
    void raise(EventsEnum evt);
    /** @brief Annule un évenement */
    void unraise(EventsEnum evt);

    /** @brief Function qui ne retourne que quand l'évenement evt est raisé */
    void wait(EventsEnum evt);
    /** @brief Fonction qui attend que la fonction evtFn retourne vrai */
    void wait(EventsFn evtFn);
    /** @brief Function qui ne retourne que quand l'évenement evt est unraisé */
    void waitNot(EventsEnum evt);
    /** @brief Fonction qui attend que la fonction evtFn retourne faux */
    void waitNot(EventsFn evtFn);
};

// ===========================================================================
// inline functions
// ===========================================================================

// ----------------------------------------------------------------------------
// EventsManagerCL::instance
// ----------------------------------------------------------------------------
inline EventsManagerCL* EventsManagerCL::instance()
{
    assert(evtMgr_);
    return evtMgr_;
}

// ----------------------------------------------------------------------------
// EventsManagerCL::isEnabled
// ----------------------------------------------------------------------------
inline bool EventsManagerCL::isEnabled(EventsEnum evt)
{
    return evtEnables_[(int)evt];
}

// ----------------------------------------------------------------------------
// evtExceptions
// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre les evenements lancant
// des exceptions
// ----------------------------------------------------------------------------
inline bool evtExceptions(bool evt[])
{
    return 
      evt[EVENTS_GAME_OVER] || 
      evt[EVENTS_EMERGENCY_STOP] || 
      evt[EVENTS_TIMER_ALERT];
}

// ----------------------------------------------------------------------------
// evtEndMove
// ----------------------------------------------------------------------------
// Cette fonction est un EventsFn qui permet d'attendre que la fin d'un 
// mouvement
// ----------------------------------------------------------------------------
inline bool evtEndMoveAndExceptions(bool evt[])
{
    return evt[EVENTS_MOVE_END] || evtExceptions(evt);
}

#endif /* __EVENTS_H__ */
