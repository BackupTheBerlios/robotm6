#ifndef __ROBOT_2005_H__
#define __ROBOT_2005_H__

#include "robotConfig.h"


#define RobotConfig2005 RobotConfig2005CL::instance()

// ----------------------------------------------------------------------------
// class RobotConfig2005CL 
// ----------------------------------------------------------------------------
/**
 * @class RobotConfig2005CL
 * Configuration du robot d'attaque 2005
 */
class RobotConfig2005CL : public RobotConfigCL
{
 public:
    void reset() {} 
    /** @brief Renvoie true si l'un des composant du robot est simule */
    virtual bool needSimulator() const;
    static RobotConfig2005CL* instance();

 protected:
    // only the main can create the config
    RobotConfig2005CL(const char* name,
                      bool simulated);

 public:
    /** @brief True si c'est le robot d'attack, false sinon */
    bool isRobotAttack;
    /** @brief True si on utilise la catapulte en debut de match */
    bool useCatapult;

    // --------------------------------------------
    // --- E N T R E E S    /   S O R T I E S   ---
    // --------------------------------------------
    /**  @brief utilse le motorReal ou motorSimu */
    bool motorSimu;
    bool hasMotor;
      /** @brief simule le lcd ou envoie les messages sur l'uart */
    bool lcdSimu;
    bool hasLcd;
    /** @brief simule la carte odometre */
    bool odometerSimu;
    bool hasOdometer;
    /** @brief simule la carte odometre */
    bool soundSimu;
    bool hasSound;
    /** @brief simule la carte bumper */
    bool bumperSimu;
    bool hasBumper;
    /** @brief simule la carte detecteur d'environement */
    bool envSimu;
    bool hasEnv;
    /** @brief simule la carte servo moteur */
    bool servoSimu;
    bool hasServo;
    /** @brief simule la carte grue electro aimant */
    bool craneSimu;
    bool hasCrane;
    /** @brief simule la carte rateau detection de grille */
    bool skittleDetectorSimu;
    bool hasSkittleDetector;
    /** @brief simule la carte electro aimant */
    bool teslaSimu;
    bool hasTesla;
    /** @brief simule la carte alim */
    bool alimSimu;
    bool hasAlim;
};

// ----------------------------------------------------------------------------
// RobotConfig::RobotConfig
// ----------------------------------------------------------------------------
inline RobotConfig2005CL::RobotConfig2005CL(const char* Name, 
                                            bool simulated) : 
    RobotConfigCL(Name, simulated),
    isRobotAttack(true),
    useCatapult(true),

    motorSimu(simulated),
    hasMotor(true),
    lcdSimu(simulated),
    hasLcd(true),
    odometerSimu(simulated),
    hasOdometer(true),
    soundSimu(simulated),
    hasSound(true),
    bumperSimu(simulated),
    hasBumper(true),
    envSimu(simulated),
    hasEnv(true),
    servoSimu(simulated),
    hasServo(true),
    craneSimu(simulated),
    hasCrane(true),
    skittleDetectorSimu(simulated),
    hasSkittleDetector(true),
    teslaSimu(simulated),
    hasTesla(true),
    alimSimu(simulated),
    hasAlim(true)
{

}

// ----------------------------------------------------------------------------
// RobotConfig2005::needSimulator() const
// ----------------------------------------------------------------------------
inline bool RobotConfig2005CL::needSimulator() const
{
    return RobotConfigCL::needSimulator() || 
        lcdSimu || odometerSimu || motorSimu || bumperSimu || envSimu; 
    // soundSimu n'a pas besoin du simulateur !
}

// ----------------------------------------------------------------------------
// RobotConfig2005::instance
// ----------------------------------------------------------------------------
inline RobotConfig2005CL* RobotConfig2005CL::instance()
{
    if(!lastInstance_) {
        lastInstance_ = new RobotConfig2005CL("Config par defaut", false);
    }
    return (RobotConfig2005CL*)lastInstance_;
}

// ----------------------------------------------------------------------------
// class RobotConfigAttack2005CL 
// ----------------------------------------------------------------------------
/**
 * @class RobotConfigAttack2005CL
 * Configuration du robot d'attaque 2005
 */
class RobotConfigAttack2005CL : public RobotConfig2005CL
{
 public:
    void reset() {}
 private:
    // only the main can create the config
    RobotConfigAttack2005CL(bool simulated=false);
    friend int main(int, char*[]); 
};

// ----------------------------------------------------------------------------
// class RobotConfigDefence2005CL
// ----------------------------------------------------------------------------
/**
 * @class 
 * Configuration du robot de defence 2005
 */
class RobotConfigDefence2005CL : public RobotConfig2005CL
{
 public:
    void reset() {}
 private:
    // only the main can create the config
    RobotConfigDefence2005CL(bool simulated=false);
    friend int main(int, char*[]);
};

#endif // __ROBOT_2005_H__
