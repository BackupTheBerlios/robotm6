/** 
 * @file robotConfig.h
 *
 * @author Laurent Saint-Marcel
 *
 * Defini la configuration du robot : capteurs utilises, mode simulé...
 * C'est util pour avoir des variables globales.
 * A la base le but etait d'avoir une configuration par strategie, mais
 * c'etait complique a gerer car comme des classes sont des singletons, 
 * il fallait toujours passer une config en parametre. Pour simplifer les
 * chose, la class RobotConfig a ete transformee en singleton ce qui evite
 * de passer 3 milliard de parametres a tous les constructeurs. Le seul 
 * probleme et qu'il faut bien mettre a jour les parametre de config avant 
 * de lancer des strategies differentes
 */

#ifndef __ROBOT_CONFIG_H__
#define __ROBOT_CONFIG_H__

#include "robotTypes.h"

#define RobotConfig RobotConfigCL::instance()

// ----------------------------------------------------------------------------
// class RobotConfig : robot en mode match
// ----------------------------------------------------------------------------
/** 
 * @class RobotConfig
 *
 * Defini la configuration du robot : capteurs utilises...
 */
class RobotConfigCL
{
 public:
    RobotConfigCL(const char* name,
                  bool simulated);
    virtual ~RobotConfigCL();
    static RobotConfigCL* instance();
    virtual void reset(){};
    
    /** @brief Renvoie true si l'un des composant du robot est simule */
    bool needSimulator() const;


 public:
    /** @brief liste des port de l'UART a scanner */
    unsigned short uartPortMask; 
    bool disableUartAnswerRequest;
    /** @brief Est ce aue ioManager alloue les classes des cartes electroniques */
    bool ioManagerAlloc;
    // --------------------------------------------
    // --- E N T R E E S    /   S O R T I E S   ---
    // --------------------------------------------
    /**  @brief utilse le motorReal ou motorSimu */
    bool motorSimu;
    /** @brief simule le lcd ou envoie les messages sur l'uart */
    bool lcdSimu;
    /** @brief simule la carte odometre */
    bool odometerSimu;
    /** @brief simule la carte odometre */
    bool soundSimu;
    
    // --------------------------------------------
    // --- description du robot                 ---
    // --------------------------------------------  
    /** nom de la config */
    char name[64];
    /** @brief True si c'est le robot d'attack, false sinon */
    bool isRobotAttack;
    /** @brief True si la carte des moteurs est sur port isa, false si
        c'est sur port serie */
    bool isMotorISA;
    /** @brief position de depart du robot */
    Position startingPos;
    /** @brief moment de la derniere action du match */
    Millisecond timeAlertBeforeEnd;
    
    // --------------------------------------------
    // --- constantes des odometres             ---
    // --------------------------------------------
    double odometerK;
    double odometerD;
    double odometerEd;
    double odometerCr;
    double odometerCl;
    double odometerSignLeft;
    double odometerSignRight;
    double getOdometerKLeft()  const { return odometerSignLeft*odometerK*odometerCl; }
    double getOdometerKRight() const { return odometerSignRight*odometerK*odometerCr; }
    double getOdometerD()      const { return odometerD; }
    
    // --------------------------------------------
    // --- constantes des motors                ---
    // --------------------------------------------
    /** @brief reset automatiquement les moteurs quand on detecte que la 
	consigne envoyée aux moteurs sature */
    bool   automaticMotorReset; 
    double motorK;
    double motorD;
    double motorEd;
    double motorCr;
    double motorCl;
    double motorSignLeft;
    double motorSignRight;
    double getMotorKLeft()  const { return motorSignLeft*motorK*motorCl; }
    double getMotorKRight() const { return motorSignRight*motorK*motorCr; }
    double getMotorD()      const { return motorD; }
    
 private:
    static RobotConfigCL* lastInstance_;
};

// ----------------------------------------------------------------------------
// RobotConfig::RobotConfig
// ----------------------------------------------------------------------------
inline RobotConfigCL::RobotConfigCL(const char* Name, 
                                    bool simulated) : 
    uartPortMask(0xFFFF),
    disableUartAnswerRequest(false),
    ioManagerAlloc(true),

    motorSimu(simulated),
    lcdSimu(simulated),
    odometerSimu(simulated),
    soundSimu(simulated),

    isRobotAttack(true),
    isMotorISA(true),
    startingPos(),
    timeAlertBeforeEnd(TIME_MATCH),

    odometerK(0.1),
    odometerD(313),
    odometerEd(1),
    odometerCr(1),
    odometerCl(1),
    odometerSignLeft(1),
    odometerSignRight(1),

    automaticMotorReset(true),
    motorK(0.004),
    motorD(340),
    motorEd(1),
    motorCr(1),
    motorCl(1),
    motorSignLeft(1),
    motorSignRight(1)
    
{
    assert(!lastInstance_);
    strcpy(name, Name);
    lastInstance_ = this;
    reset();
}

// ----------------------------------------------------------------------------
// RobotConfig::needSimulator() const
// ----------------------------------------------------------------------------
inline bool RobotConfigCL::needSimulator() const
{
    return lcdSimu || odometerSimu || motorSimu; 
    // soundSimu n'a pas besoin du simulateur !
}

// ----------------------------------------------------------------------------
// RobotConfig::~RobotConfig
// ----------------------------------------------------------------------------
inline RobotConfigCL::~RobotConfigCL()
{
    lastInstance_ = NULL;
}
// ----------------------------------------------------------------------------
// RobotConfig::getClassConfig
// ----------------------------------------------------------------------------
inline RobotConfigCL* RobotConfigCL::instance()
{
    if(!lastInstance_) {
        lastInstance_ = new RobotConfigCL("Config par defaut", false);
    }
    return lastInstance_;
}


#endif // __ROBOT_CONFIG_H__
