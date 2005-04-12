#ifndef __STRATEGY_ATTACK_H__
#define __STRATEGY_ATTACK_H__
 
#include "strategy2005.h"
#include "devices/bumper.h"

enum AttackExplorationDirection {
    ATTACK_EXPLORE_ROW=0,
    ATTACK_EXPLORE_COL
};

class GridAttack;

/** @class StrategyAttackCL
 * Strategy du robot d'attaque
 */
class StrategyAttackCL : public Strategy2005CL
{
 public :
    StrategyAttackCL(RobotMainCL* main);
    virtual ~StrategyAttackCL();

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
    /** @brief cherche le pont et le traverse. quitte la fonction que si on 
        a reussi a traverser ou que c'est la fn du match */
    bool findAndCrossBridge();
    
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
    /** @brief met a jour la variable bridge_ en fonction de ce que disent les 
        sharps */
    bool getBridgePosBySharp();
    /** @brief se place en face du pont ou on peut utiliser les bumpers pour 
        savoir si le pont est bien la. La variable bridge doit deja etre mise 
        a jour */

    bool gotoBridgeEntry();
    /** @brief verifie que le pont est bien la ! renvoie false si les 
        bumpers ne repondent pas */
    bool getBridgePosByBumper(bool& bridgeInFront);
    /** @brief traverse le pont : detection de collisions...*/
    bool crossBridge();
    unsigned char getPosBit();
    void getNearestBridgeEntry();
    bool gotoBridgeEntry(Millimeter y, 
			 bool rotateLeft=false, 
			 bool rotateRight=false);
    bool getBridgePosBySharpFromLeft(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR]);
    bool getBridgePosBySharpFromCenter(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR]);
    bool getBridgeCaptors(BridgeCaptorStatus captors[BRIDGE_CAPTORS_NBR],
                          bool checkSharps);
    void noBridgeHere();

    /////////////////////////////////////////////////////////////////////////
    bool basicSkittleExplorationCol();
    bool basicSkittleExplorationRow();
    bool basicSkittleExplorationGrid(GridPoint pt[3]);
       
 private:
    /** mask qui dit ou le peut etre (4 possibilites a tester) quand il n'y est
        pas on passe le bit a 0 */
    unsigned char  bridgeAvailibility_;
    BridgePosition bridge_;
    /** utilise le pont de gauche ou de droite pour traverser */
    bool           useLeftBridge_; 
    /** utilise le pont du mileu pour essayer de traverser */
    bool           bridgeDetectionByCenter_;
    
    // grille qui permet de savoir ou on est deja passe et ou il faut aller
    GridAttack* grid_;
    // derniere direction d'exploration utilisee, la prochaine fois 
    // on prendra l'autre type
    AttackExplorationDirection lastExplorationDir_;

    
};
static const Millimeter BRIDGE_ENTRY_NAVIGATE_X = 1200;
static const Millimeter BRIDGE_DETECT_SHARP_X = 1400; // on s'arrete toujours un peu avant
static const Millimeter BRIDGE_DETECT_BUMP_X  = 1400;
static const Millimeter BRIDGE_CROSS_BRIDGE_X  = 2300;
static const Millimeter BRIDGE_CROSS_BRIDGE_X_MARGIN  = 50;

static const Millimeter BRIDGE_ENTRY_SIOUX_Y          = 1300; // passe par le pont du milieu
static const Millimeter BRIDGE_ENTRY_CENTER_Y         = 1425;
static const Millimeter BRIDGE_ENTRY_MIDDLE_CENTER_Y  = 1575;
static const Millimeter BRIDGE_ENTRY_MIDDLE_BORDURE_Y = 1725;
static const Millimeter BRIDGE_ENTRY_BORDURE_Y        = 1875;
static const Millimeter BRIDGE_ENTRY_MARGIN           = 50;

// temps d'attente entre le demarage des servo et le debut du deplacement
static const Millisecond CATAPULT_AWAIT_DELAY         = 500;
    
#endif // __STRATEGY_ATTACK_H__
