#ifndef __STRATEGY_ATTACK_H__
#define __STRATEGY_ATTACK_H__
 
#include "strategy2005.h"

/** @class StrategyAttackCL
 * Strategy du robot d'attaque
 */
class StrategyAttackCL : public Strategy2005CL
{
 public :
    StrategyAttackCL(RobotMainCL* main);
    void run(int argc, char*argv[]);
 private:
    // le boolean renvoye par les fonctions indique si c'est la fin de la 
    // strategie ou non. Pour verifier si le robot a bien fait son travail
    // il faut regarder sa position 


    // ================ Tire des catapultes ==================
    /** @brief Tire des balles avec les catapultes */
    void fireCatapults();

    // ================ Detection et traversee du pont ==================

    /** @brief va a l'endroit ou on detecte les pont par capteurs sharps */
    bool gotoBridgeDetection();
    /** @brief met a jour la variable bridge_ en fonction de ce que disent les 
        sharps */
    bool getBridgePosBySharp();
    /** @brief se place en face du pont ou on peut utiliser les bumpers pour 
        savoir si le pont est bien la. La variable bridge doit deja etre mise 
        a jour */
    bool gotoBridgeEntry();
    /** @brief verifie que le pont est bien la ! */
    bool getBridgePosByBumper();
    /** @brief traverse le pont : detection de collisions...*/
    bool crossBridge();
    /** @brief va jusq'a l'aute pont et change l'etat de la variable
        useLeftBridge_ */
    bool gotoOtherBridge();

    // ================ Exploration de la zone ennemie ==================

    /** @brief explore juste les zone ou il peut y avoir des quilles 
        d'apres le reglement */
    bool basicSkittleExploration();
    /** @brief se promene sur le terrain adverse en allant voir partout: sur
        les bords et chez l'adversaire */
    bool extendedSkittleExploration();
    /** @brief va faire tomber les quilles du milieu du terrain 
        (la pile de 3) */
    bool killCenterSkittles();

 private:
    /** mask qui dit ou le peut etre (4 possibilites a tester) quand il n'y est
        pas on passe le bit a 0 */
    unsigned char  bridgeAvailibility_;
    BridgePosition bridge_;
    /** utilise le pont de gauche ou de droite pour traverser */
    bool           useLeftBridge_;  
};

#endif // __STRATEGY_ATTACK_H__
