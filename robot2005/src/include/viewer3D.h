/**
 * @file viewer3D.cpp
 *
 * @author Laurent Saint-Marcel
 *
 * Systeme d'affichage du terrain, du robot et des donnees dans plusieurs
 * fenetres avec OPENGL
 */

#ifndef __VIEWER_3D_H__
#define __VIEWER_3D_H__

#include <string>
#include "mthread.h"
#include "robotBase.h"
#include "robotImage.h"

#include <GL/gl.h>

#define Viewer3D Viewer3DCL::instance()

#define COLOR_WHITE_BORDER     0.9f, 0.9f,0.9f
#define COLOR_WHITE_LINE       1.0f, 1.0f,1.0f
#define COLOR_YELLOW_GRID_1    0.73f,0.67f,0.20f // mat
#define COLOR_YELLOW_GRID_2    0.76f,0.86f,0.40f // brillant
#define COLOR_SQUATCH_BALL     0.0f, 0.0f, 1.0f
#define COLOR_GRS_BALL         0.2f, 0.2f, 0.2f
#define COLOR_SKITTLE_RED      0.8f, 0.0f, 0.0f
#define COLOR_SKITTLE_GREEN    0.0f, 0.8f, 0.0f
#define COLOR_SKITTLE_REFLECT  0.9f, 0.9f, 0.9f
#define COLOR_SKITTLE_METAL    0.5f, 0.5f, 0.5f

#define COLOR_WATER            0.27f,0.48f,0.79f
#define COLOR_TEAM_RED         0.8f, 0.0f, 0.0f
#define COLOR_TEAM_GREEN       0.0f, 0.8f, 0.0f


#define VIEWER_MAX_ROBOT_NBR 4

static const double SCALE3D=0.1;

class ControlButton;

typedef struct ViewerColorST {
  float r;
  float g;
  float b;
  float a;
  ViewerColorST(double R=1, double G=1, double B=1, double A=1) {
    r=(float)R;
    g=(float)G;
    b=(float)B;
    a=(float)A;
  }
  void setGL() {
    glColor4f(r,g,b,a);
  }
} ViewerColorST;

typedef enum ViewerScreenEN {
  VIEWER_SCREEN_MAP,
  VIEWER_SCREEN_3D,
  VIEWER_SCREEN_PLAYER,
  
  VIEWER_SCREEN_NBR
} ViewerScreenEN;

typedef enum ViewerControlButtonId{
    CTRL_BTN_PREVIOUS=0,       // besoin seulement de clickCB 
    CTRL_BTN_STEP_BACKWARD,    // besoin seulement de clickCB 
    CTRL_BTN_PLAY,             // besoin seulement de clickCB 
    CTRL_BTN_STEP_FORWARD,     // besoin seulement de clickCB 
    CTRL_BTN_NEXT,             // besoin seulement de clickCB 
    CTRL_BTN_SLOWER,           // besoin seulement de clickCB 
    CTRL_BTN_FASTER,           // besoin seulement de clickCB 
    CTRL_BTN_RECORD,           // besoin seulement de clickCB 

    CTRL_BTN_BRIDGE,           // besoin seulement de clickCB 
    CTRL_BTN_SUPPORT,          // besoin seulement de clickCB 

    CTRL_BTN_ROBOTS_JACK,      // besoin de clickCB et unclickCB
    CTRL_BTN_ROBOTS_AU,        // besoin de clickCB et unclickCB

    CTRL_BTN_R0_JACK,          // besoin de clickCB et unclickCB
    CTRL_BTN_R0_AU,            // besoin de clickCB et unclickCB
    CTRL_BTN_R0_COULEUR,       // pas de CB
    CTRL_BTN_R0_YES,           // besoin de clickCB et unclickCB
    CTRL_BTN_R0_NO,            // besoin de clickCB et unclickCB
    
    CTRL_BTN_R1_JACK,          // besoin de clickCB et unclickCB
    CTRL_BTN_R1_AU,            // besoin de clickCB et unclickCB
    CTRL_BTN_R1_COULEUR,       // pas de CB
    CTRL_BTN_R1_YES,           // besoin de clickCB et unclickCB
    CTRL_BTN_R1_NO,            // besoin de clickCB et unclickCB
    
    CTRL_BTN_R2_JACK,          // besoin de clickCB et unclickCB
    CTRL_BTN_R2_AU,            // besoin de clickCB et unclickCB
    CTRL_BTN_R2_COULEUR,       // pas de CB
    CTRL_BTN_R2_YES,           // besoin de clickCB et unclickCB
    CTRL_BTN_R2_NO,            // besoin de clickCB et unclickCB
    
    CTRL_BTN_R3_JACK,          // besoin de clickCB et unclickCB
    CTRL_BTN_R3_AU,            // besoin de clickCB et unclickCB
    CTRL_BTN_R3_COULEUR,       // pas de CB
    CTRL_BTN_R3_YES,           // besoin de clickCB et unclickCB
    CTRL_BTN_R3_NO,            // besoin de clickCB et unclickCB

    CTRL_BTN_NBR
} ViewerControlButtonId;
static const int CTRL_BTN_NBR_PER_ROBOT=5;

typedef void (*clickBtnCB)(ViewerControlButtonId id);

typedef struct ViewerMovieDataST {
  bool  run;
  int   count;
  char  filename[255];
  Image img;

  ViewerMovieDataST(): run(false), count(0), img(){}
} ViewerMovieDataST;

struct ViewerRobotData {
    int              id;
    std::string      name;
    std::string      lcd;
    RobotModel       model;
    ViewerColorST    color;
    Position         pos;
    Trajectory       targetTrajectory;
    Trajectory       currentTrajectory;
    ListOfObstacles  obstacles_[OBSTACLE_TYPE_NBR];
    bool             exist;
    bool             brick;
    int              dead;
    TeamColor        teamColor;

    ViewerRobotData() :
	id(-1), name(""), lcd("Booting..."), model(ROBOT_MODEL_ATTACK), 
        color(1,0,0,1), pos(-100,0,0), exist(false), brick(false), dead(0), 
         teamColor(TEAM_RED) {}
    void set(int Id, std::string Name, RobotModel Model, bool isBrick, int isDead) {
        id    = Id;
        name  = Name;
        model = Model;
	exist = true;
        brick = isBrick;
        dead  = isDead;
    } 
    void remove() {
	exist = false;
	pos=Position(-100,0,0);
    }
    bool isBrick() {
	return brick;
    }
    void setColor(ViewerColorST const& Color) {
        color = Color;
    }
    void setTeam(TeamColor team) {
        teamColor = team;
    }
    TeamColor getTeam() {
        return teamColor;
    }
    void setLcd(int Id, const char* message) {
        lcd = message;
    }
    void setPos(Position const& Pos) {
        pos = Pos;
    }
    void setPos(Millimeter x, Millimeter y, Radian t) {
        pos.center.x  = x;
        pos.center.y  = y;
        pos.direction = t;
    }
    void setTrajectory(Trajectory const& T) {
        targetTrajectory = T;
    }
    void addPointToCurrentTrajectory(Point pt) {
        if ((pt.x >= 0) && (pt.x <= TERRAIN_X) 
            && (pt.y >= 0) && (pt.y <= TERRAIN_Y)) {
            currentTrajectory.push_back(pt);
        }
    }
    void clearTrajectory() {
        targetTrajectory.clear();
    }
    void setObstacles(ListOfObstacles const& obstacles, 
                      ObstacleType           type) {
        obstacles_[type] = obstacles;
    }
    void reset() {
        currentTrajectory.clear();
        clearTrajectory();
        setPos(-100,0,0);
        for(unsigned int i=0; i<OBSTACLE_TYPE_NBR; i++) {
            obstacles_[i].clear();
        }
    }
    
};

/** 
 * @class Viewer3DCL
 * @brief Class qui gere toute l'interface avec le systeme d'affichage du 
 * simulateur et du logViewer
 * Vue du terrain en 3D + carte du terrain + donnees
 */
class Viewer3DCL: public RobotBase
{
 public:
	virtual ~Viewer3DCL();
	static Viewer3DCL* instance();

        // ---------------------------------------------------------------
        // Initialisation
        // ---------------------------------------------------------------

        /** @brief Cree les fenetres du viewer */
	void createWindows(bool map3D=true,
			   bool logPlayer=false);
	/** @brief Met a jour le temps depuis lequel la simulation ou le log a
	    commence */
	void setTitle(const char* title);
	/** @brief Met a jour le temps depuis lequel la simulation ou le log a
	    commence */
	void setTime(Millisecond currentTime);
	/** @brief Met a jour le temps depuis lequel le match a commence */
	void setMatchTime(Millisecond currentTime);

	/** @brief Autorise ou non l'affiche du point correspondant a la 
            position du robot */
	void enableDisplayRobots(bool enable);

        /** @brief Definit la forme du robot, et son nom */
        void setRobotModel(int                  robotId,
                           std::string const&   name,
                           RobotModel           model,
                           bool                 isBrick,
                           int                  isDead=0);
        /** @brief Definit la couleur du robot */
        void setRobotColor(int                  robotId,
			   ViewerColorST const& color);
	/** 
	 * @brief Definit dans quelle equipe est le robot. Pour les robots de
	 * l'equipe verte, la position est automatiquement modifiee: 
	 * TERRAIN_X-x, TERRAIN_Y-y 
	 */
	TeamColor getRobotTeam(int robotId);
        /** @brief Definit le message affiche sur le LCD */
        void setRobotLcd(int         robotId,
			 const char* message);
        /** @brief N'affiche plus le robot robotId */
        void removeRobot(int         robotId);
        /** @brief Defini la position des balles de Squatch */
        void setBridgePosition(BridgePosition pos);
        /** @brief Defini la position des plateaux fixes supportant les 
            quilles */
	void setSupportPosition(Point const& pt1, 
                                Point const& pt2);
	/** @brief enregistre des callbacks sur les boutons du player */
	void registerBtnCallback(ViewerControlButtonId btnId,
				 clickBtnCB clickCB,
				 clickBtnCB unclickCB=NULL);
	/** @brief Pour mettre a jour l'etat des boutons (pour que le
	    simulateur puisse changer les etats de AU et jackin. Les callbacks
	    ne sont pas appeles quand on change l'etat d'un bouton avec cette
	    fonction */
	void setBtnClick(ViewerControlButtonId btnId, bool click);
	bool reset();
        // ---------------------------------------------------------------
        // Mettre a jour la position des objets sur le terrains
        // ---------------------------------------------------------------
        
        /** @brief Definit la position de la camera autour de la table 
            pour la vue 3D */ 
	void setCameraPosition(Millimeter cx, 
			       Millimeter cy, 
			       Millimeter cz);
	/** @brief Met a jour la position du robot */
	void setRobotPosition(int robotId,
                              Millimeter x, 
                              Millimeter y, 
                              Radian theta); 
	/** @brief Met a jour la position du robot */
	void setRobotPosition(int robotId,
                              Position const& pos);
	/** @brief Defini la position de toutes les quilles sur le jeu */
        void setSkittlePosition(Skittle* skittles);
        /** @brief Defini la position des balles de GRS */
        void setGRSBallsPosition(GRSBall* balls);
        /** @brief Defini la position des balles de Squatch */
        void setSquatchBallsPosition(SquatchBall* balls);
        
        // ---------------------------------------------------------------
        // Donnees de debug concernant chaque robot
        // ---------------------------------------------------------------
        
        /** @brief Trajectoire cible du robot */
	void setRobotTrajectory(int robotId,
                                Trajectory const& t);
        /** @brief Met a jour la liste d'obstacles detectes par un robot */
	void setObstacleList(int                    robotId,
                             ListOfObstacles const& obstacles,
			     ObstacleType           type);
	/** @brief Met a jour la grille de calcul de trajectoire */
	/*void setGrid(int      robotId,
                     LogGrid* grid);*/
        
        // ---------------------------------------------------------------
        // Films et screenshot
        // ---------------------------------------------------------------
        
        /** @brief Commence l'enregistrement d'une serie d'images 
	    correspondant a une fenetre */
	void movieStart(const char* filename, 
                        ViewerScreenEN screen);
	void movieStop(ViewerScreenEN screen);
	
	// ---------------------------------------------------------------
        // Callbacks des boutons du menu
        // ---------------------------------------------------------------
        
 protected:
        /** @brief constructeur accessible uniquement par la fonction 
            instance() */
        Viewer3DCL();
	/** @brief Lance le thread qui initialise les fenetres OPENGL */
	void startThread();
        /** @brief Renvoie vrai si le point est sur le terrain */
        bool isInField(Point pt);
        
        // ---------------------------------------------------------------
        // Dessin 3D
        // ---------------------------------------------------------------
        void draw3D();
	/** @brief Dessine les robots en 3D */
	void drawRobots3D();
	/** @brief Dessine un robot en 3D */
	void drawRobot3D(int robotId);
	/** @brief Dessine un robot en 3D */
	void drawRobot3DAttack(ViewerColorST& color);
	/** @brief Dessine un robot en 3D */
	void drawRobot3DDefence(ViewerColorST& color);
	/** @brief Dessine le terrain et la salle en 3D */
	void drawBG3D();
	/** @brief Dessine toutes les quilles */
	void drawSkittles3D();
	/** @brief Dessine une quille */
	void drawSkittle3D(Skittle* skittle);
	/** @brief Dessine toutes les balles */
	void drawGRSBalls3D();
	/** @brief Dessine une balle de GRS */
	void drawGRSBall3D(GRSBall* ball);
        /** @brief Dessine toutes les balles */
	void drawSquatchBalls3D();
	/** @brief Dessine une balle de Squatch */
	void drawSquatchBall3D(SquatchBall* ball);
        /** @brief Dessine tous les ponts */
	void drawBridges3D();
	/** @brief Dessine un pont */
	void drawBridge3D(Millimeter x, bool big);
        /** @brief Dessine les supports de quilles */
        void drawSupports3D();
        /** @brief Dessine un support de quilles */
        void drawSupport3D(Point pt);
        /** @brief Dessine la Legende */
        void drawLegend3D();

        // ---------------------------------------------------------------
        // Camera 3D
        // ---------------------------------------------------------------
        /** @brief Defini le robot que la camera suit 
            (-1 = camera regarde le centre du terrain) */
        void setCameraTarget3D(int robotId);
        /** @brief Defini la position de la camera */
        void setCameraPosition3D(Millimeter cx, Millimeter cy, Millimeter cz);
        /** @brief Defini le point que regarde la camera */
        void setCameraTargetPosition3D(Millimeter cx, Millimeter cy, Millimeter cz);
        /** @brief Deplace legerement la camera dans une direction */
        void cameraPositionRotate3D(int mode);
        /** @brief Place la camera dans le coin du terrain le plus proche
            du premier robot */
        void cameraNearestCorner3D();
        /** @brief La camera choisi automatiquement le meilleur angle de vue */
        void changeCameraAuto3D();
        /** @brief Active les textures dans la vue 3D */
        void changeTexture3D();

	/** @brief Renvoie la hauteur du sol au point donne (fosse=-30, normal=0) */
	Millimeter getGroundLevel(Point const& pt);

        friend void keyboardViewerMap3D(unsigned char key, int x, int y);
        friend void menuViewerMap3D(int value);
	friend void displayViewerMap3D(void);
	friend void displayViewerMap2D(void);
	friend void cameraPositionRotate3D(int mode);
	
        // ---------------------------------------------------------------
        // Dessin 2D
        // ---------------------------------------------------------------
        void draw2D();
	/** @brief Dessine les robots en 2D */
	void drawRobots2D();
	/** @brief Dessine un robot en 2D */
	void drawRobot2D(int robotId);
	/** @brief Dessine le terrain et la salle en 2D */
	void drawBG2D();
	/** @brief Dessine toutes les quilles */
	void drawSkittles2D();
	/** @brief Dessine une quille */
	void drawSkittle2D(Skittle* skittle);
	/** @brief Dessine toutes les balles */
	void drawGRSBalls2D();
	/** @brief Dessine une balle de GRS */
	void drawGRSBall2D(GRSBall* ball);
        /** @brief Dessine toutes les balles */
	void drawSquatchBalls2D();
	/** @brief Dessine une balle de Squatch */
	void drawSquatchBall2D(SquatchBall* ball);
        /** @brief Dessine tous les ponts */
	void drawBridges2D();
	/** @brief Dessine un pont */
	void drawBridge2D(Millimeter y, bool big);
        /** @brief Dessine tous les supports de quilles */
	void drawSupports2D();
	/** @brief Dessine un support de quilles */
	void drawSupport2D(Point pt);
        /** @brief Dessine tous les obstacles */
	void drawObstacles2D();
	/** @brief Dessine un obstacle */
	void drawObstacle2D(Obstacle * obstacle, int robotId);
        /** @brief Dessine tous les obstacles */
	void drawTrajectories2D();
	/** @brief Dessine une trajectoire */
	void drawTrajectory2D(Trajectory * t, bool target);
        /** @brief Dessine la legende */
	void drawLegend2D();

	friend void menuRobotMap2D(int value);
	friend void menuRobotMap3D(int value);
	friend void keyboardViewerMap2D(unsigned char key, int x, int y);

	// ---------------------------------------------------------------
        // Dessin Player control
        // ---------------------------------------------------------------
        void drawPlayer();
	/** @brief Dessine les robots en 2D */
	void drawPlayerControls();
	/** @brief Dessine un robot en 2D */
	void drawPlayerRobotInfo(int robotId);
	void mousePlayerControl(int button, int state, int x, int y);
	void setRobotTeam(int robotId, TeamColor team);

	friend void checkRobotColor(int from);
	friend void clickColorBtn(ViewerControlButtonId btnId);
	friend void unclickColorBtn(ViewerControlButtonId btnId);
	friend void mousePlayerControl(int button, int state, int x, int y);
	friend void displayPlayerControl(); 
        // ---------------------------------------------------------------
        // Films et screen shot
        // ---------------------------------------------------------------
        /** @brief Sauvegarde une image faisant partie d'un film */
	void movieScreenShot(ViewerScreenEN screen);
	/** @brief Arrete l'enregistrement d'une serie d'images */
	int  getMovieBaseName(ViewerScreenEN screen, 
                              char*filename,
                              bool incCounter=true);
        /** @brief Sauvegarde une fenetre dans un fichier image bmp */
	void screenShot(const char* filename);

 protected:
        std::string      title_;
	ViewerRobotData  robotData_[VIEWER_MAX_ROBOT_NBR];
	Millisecond      time_;
	Millisecond      matchTime_;
        Skittle*         skittles_;
        GRSBall*         grsBalls_;
        SquatchBall*     squatchBalls_;
        BridgePosition   bridge_;
        Point            support1_;
        Point            support2_;

        bool enableObjectDisplay_;
	bool useTexture_;
	bool createThread_;
	ControlButton*  btn_[CTRL_BTN_NBR];

 protected:
        // 3D options
        bool cameraModeAuto3D_;
        bool useTexture3D_;
        int  targetRobotId3D_;
	
 private:
	static Viewer3DCL* viewer_;
	MThreadId thread_;
 
 public:
	static ViewerMovieDataST movieData_[VIEWER_SCREEN_NBR];
};


// ---------------------------------------------------------------
// Viewer3DCL::isInField
// ---------------------------------------------------------------
inline bool Viewer3DCL::isInField(Point pt) {
    return ((pt.x >= 0) && (pt.x <= TERRAIN_X) 
         && (pt.y >= 0) && (pt.y <= TERRAIN_Y));
}

#endif // __VIEWER_3D_H__
