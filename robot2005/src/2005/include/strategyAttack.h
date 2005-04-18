#ifndef __STRATEGY_ATTACK_H__
#define __STRATEGY_ATTACK_H__
 
#include "strategy2005.h"
#include "devices/bumper.h"
#include "events.h"

enum AttackExplorationDirection {
    ATTACK_EXPLORE_ROW=0,
    ATTACK_EXPLORE_COL
};

enum AttackPhase {
    ATTACK_WAIT_START=0,
    ATTACK_FIRE_CATAPULT,     // tire les balles a la catapulte
    ATTACK_CROSS_BRIDGE,      // va au pont et le traverse
    ATTACK_PREDEFINED_TRAJEC, // trajectoire d'exploration predefinie
    ATTACK_ONLY_SKITTLES,     // ne va que la ou il y a des quilles en theorie
    ATTACK_GRAND_MENAGE       // se promene partout sur le terrain adverse,
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

    void periodicTask(Millisecond time);
    void envDetectorCallBack(EventsEnum evt); 
 protected:
    bool autoCheck();
    bool testBridgeCaptors();
 private:
    // le boolean renvoye par les fonctions indique si la command a
    // terminee normalement ou non. Si elle n'a pas terminee normalement
    // il faut verifier les events pour voir si ce n'est pas la fin du match


    // ================ Tire des catapultes ==================
    /** @brief Tire des balles avec les catapultes */
    void fireCatapults();
    /** @brief met les servos des catapultes dans la position de depart */
    bool prepareCatapults();

    // ================ Detection et traversee du pont ==================
    /** @brief va a l'endroit ou on detecte les pont par capteurs sharps */
    bool gotoBridgeDetection();
    /** @brief cherche le pont et le traverse. quitte la fonction que si on 
        a reussi a traverser ou que c'est la fn du match */
    bool findAndCrossBridge();
    
    // ================ Exploration de la zone ennemie ==================
    /** @brief utilise une trajectoire d'exploration predefinie. le robot
        doit etre juste en sortie de pont */
    bool preDefinedSkittleExploration();
    /** @brief explore juste les zone ou il peut y avoir des quilles 
        d'apres le reglement */
    bool basicSkittleExploration();
    /** @brief va faire tomber les quilles du milieu du terrain 
        (la pile de 3) */
    bool killCenterSkittles();


 private:
    ///////////////////////////// PONT ////////////////////////////
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

    //////////////////////////   EXPLORATION    ////////////////////////////
    bool preDefinedSkittleExploration1();
    bool preDefinedSkittleExploration2();

    void basicSkittleExplorationCol(GridPoint gpts[3]);
    void basicSkittleExplorationRow(GridPoint gpts[3]);
    bool exploreGrid(GridPoint gpts[3]); 
    bool gotoGtps1(GridPoint gpts[3]);
    bool rotateOnGtps1(GridPoint gpts[3]);
    bool backBeforeGpts2(GridPoint gpts[3]);
    bool gotoGpts2(GridPoint gpts[3],
                   bool &needMoveBackward);
    bool goBackToGpts2(GridPoint gpts[3]);
    void updateGrid();

    ////////////////////////// MISC ////////////////////////////////
    void setAttackPhase(AttackPhase);
    
 private:
    /** mask qui dit ou le peut etre (4 possibilites a tester) quand il n'y est
        pas on passe le bit a 0 */
    unsigned char  bridgeAvailibility_;
    BridgePosition bridge_;
    /** utilise le pont de gauche ou de droite pour traverser */
    bool           useLeftBridge_; 
    /** utilise le pont du mileu pour essayer de traverser */
    bool           bridgeDetectionByCenter_;
    bool           useSharpToDetectBridge_;
    
    /** grille qui permet de savoir ou on est deja passe et ou il faut aller */
    GridAttack*    grid_;
    /** derniere direction d'exploration utilisee, la prochaine fois 
        on prendra l'autre type */
    AttackExplorationDirection lastExplorationDir_;
    /* Dans quel partie du match on est */
    AttackPhase    attackPhase_;
    /* est ce qu'on a deja fait tomber les quilles du milieu ? */
    volatile bool  skittleMiddleProcessed_;
    /* est ce qu'on est en train d'aller faire tomber les quilles du milieu? 
       si oui on active les detecteurs de ponts quand x<2400 */
    volatile bool  isProcessingMiddleSkittles_;
    
};

// abscisse sure, sur laquelle on s'alligne pour aller d'une position de pont a l'autre
static const Millimeter BRIDGE_ENTRY_NAVIGATE_X = 1200;
// abscisse ou on peut utiliser les sharps pour detecter le pont
static const Millimeter BRIDGE_DETECT_SHARP_X = 1400; // on s'arrete toujours un peu avant
static const Millimeter BRIDGE_DETECT_BUMP_X  = 1400;
// abscisse de fin de traversee de pont
static const Millimeter BRIDGE_CROSS_BRIDGE_X  = 2300;
static const Millimeter BRIDGE_CROSS_BRIDGE_X_MARGIN  = 50;

static const Millimeter BRIDGE_ENTRY_SIOUX_Y          = 1300; // passe par le pont du milieu
static const Millimeter BRIDGE_ENTRY_CENTER_Y         = 1425; // centre du pont
static const Millimeter BRIDGE_ENTRY_MIDDLE_CENTER_Y  = 1575; // centre du pont
static const Millimeter BRIDGE_ENTRY_MIDDLE_BORDURE_Y = 1725; // centre du pont
static const Millimeter BRIDGE_ENTRY_BORDURE_Y        = 1875; // centre du pont
// si on est a plus que ca du centre du point on recule pour se realigner
static const Millimeter BRIDGE_ENTRY_MARGIN           = 50;   



// temps d'attente entre le demarage des servo et le debut du deplacement
static const Millisecond CATAPULT_AWAIT_DELAY         = 500;
// au boutde ce temps, si on force le passage en mode j'explore le
// terrain tout entier
static const Millisecond ATTACK_TIME_EXPLORE_ALL      = 45000;

// distance de laquelle on depalce le point cible pour etre sur qu'un mouvement se terminera au centre d'une case
static const Millimeter ATTACK_CHANGE_TARGET_POINT_DIST = 50;
// si y > EXPLORE_METHOD_Y on utilise la trajectoire 1, sinon on utilise la 2
static const Millimeter ATTACK_EXPLORE_METHOD_Y = 1570;

#endif // __STRATEGY_ATTACK_H__
