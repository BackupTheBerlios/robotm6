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
    RobotConfigCL();
    virtual ~RobotConfigCL();
    static RobotConfigCL* instance();
    virtual void reset();

 public:
    /** @brief liste des port de l'UART a scanner */
    unsigned short uartPortMask; 

    // -------------------------------------------
    // ---          M O T E U R S              ---
    // -------------------------------------------
    /**  @brief utilse le motorReal ou motorSimu */
    bool motorSimu;
    /** @brief reset automatiquement les moteurs quand on detecte que la 
	consigne envoyée aux moteurs sature */
    bool automaticMotorReset; 

    // --------------------------------------------
    // --- E N T R E E S    /   S O R T I E S   ---
    // --------------------------------------------
    bool disableUartAnswerRequest;
    /** @brief Est ce aue ioManager alloue les classes des cartes electroniques */
    bool ioManagerAlloc;
    /** @brief simule le lcd ou envoie les messages sur l'uart */
    bool lcdSimu;
    /** @brief simule la carte odometre */
    bool odometerSimu;
     /** @brief affiche la position estimee du robot toutes les secondes */
    bool displayOdomPos;

    // --------------------------------------------
    // --- description du robot   ---
    // --------------------------------------------
    /** @brief True si c'est le robot d'attack, false sinon */
   bool isRobotAttack;
   /** @brief True si la carte des moteurs est sur port isa, false si
       c'est sur port serie */
   bool isMotorISA;

   bool needSimulator() const;

   private:
    static RobotConfigCL* lastInstance_;
};

// ----------------------------------------------------------------------------
// class RobotConfigSimu : robot en mode mtch simule
// ----------------------------------------------------------------------------
/**
 * @class RobotConfigSimu
 * Configuration du robot utilisée en mode simulé
 */
class RobotConfigSimuCL : public RobotConfigCL
{
  public:
    RobotConfigSimuCL();
    virtual void reset();
};


// ----------------------------------------------------------------------------
// RobotConfigSimuCL::reset
// ----------------------------------------------------------------------------
inline void RobotConfigCL::reset() 
{
    
}

// ----------------------------------------------------------------------------
// RobotConfig::RobotConfig
// ----------------------------------------------------------------------------
inline RobotConfigCL::RobotConfigCL() : 
    uartPortMask(0xFFFF),
   
    motorSimu(true), 
    automaticMotorReset(true), 
    disableUartAnswerRequest(false), 
    ioManagerAlloc(true),
    lcdSimu(false),
    odometerSimu(false),
    displayOdomPos(true),
    isRobotAttack(true),
    isMotorISA(true)
{
    lastInstance_ = this;
    reset();
}

// ----------------------------------------------------------------------------
// RobotConfig::needSimulator() const
// ----------------------------------------------------------------------------
inline bool RobotConfigCL::needSimulator() const
{
    return lcdSimu || odometerSimu || motorSimu;
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
    if(!lastInstance_) lastInstance_ = new RobotConfigCL();
    return lastInstance_;
}

// ----------------------------------------------------------------------------
// RobotConfigSimu::RobotConfigSimu
// ----------------------------------------------------------------------------
inline RobotConfigSimuCL::RobotConfigSimuCL() : RobotConfigCL()
{
    uartPortMask=0x00;
    motorSimu=true;
    lcdSimu=true;
    odometerSimu=true;
 
    reset();
}

// ----------------------------------------------------------------------------
// RobotConfigSimuCL::reset
// ----------------------------------------------------------------------------
inline void RobotConfigSimuCL::reset() 
{

}

#endif // __ROBOT_CONFIG_H__
