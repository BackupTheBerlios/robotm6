#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GL/glu.h>
#include "robotImage.h"

#define Texture VTexture::instance()

typedef enum TextureId{
  TEX_LOGO=0, 
  TEX_LOGO_ISIA,
  TEX_LOGO_ESSI, 
  TEX_LOGO_ESINSA, 
  TEX_ROBOT_SIDE_LEFT,
  TEX_ROBOT_SIDE_RIGHT,
  TEX_ROBOT_FRONT, 
  TEX_ROBOT_BACK, 
  TEX_ROBOT_TOP, 

  TEX_ATTACK_SIDE_R1,
  TEX_ATTACK_SIDE_R2,
  TEX_ATTACK_SIDE_L1,
  TEX_ATTACK_SIDE_L2,
  TEX_ATTACK_SIDE_F,
  TEX_ATTACK_SIDE_R,
  TEX_ATTACK_TOP,

  TEX_DEFENCE_SIDE_R,
  TEX_DEFENCE_SIDE_L,
  TEX_DEFENCE_SIDE_F,
  TEX_DEFENCE_SIDE_RR,
  TEX_DEFENCE_TOP,

  TEX_SALLE_SIDE1,
  TEX_SALLE_SIDE2,
  TEX_SALLE_SIDE3,
  TEX_SALLE_SIDE4,
  TEX_SALLE_PLAFOND,

  TEX_LCD,
  TEX_BTN_0,
  TXT_BTN_1,
  TXT_JACKIN,
  TXT_JACKOUT,
  TEX_AU_IN,
  TEX_AU_OUT,
  TEX_TEAM_RED,
  TEX_TEAM_GREEN,

  TEX_BRIDGE0,
  TEX_BRIDGE1,
  TEX_SUPPORT0,
  TEX_SUPPORT1,

  TEX_BTN_PREV_0,
  TEX_BTN_PREV_1,
  TEX_BTN_PLAY_0,
  TEX_BTN_PLAY_1,
  TEX_BTN_STEP_FORWARD_0,
  TEX_BTN_STEP_FORWARD_1,
  TEX_BTN_STEP_BACKWARD_0,
  TEX_BTN_STEP_BACKWARD_1,
  TEX_BTN_PAUSE_0,
  TEX_BTN_PAUSE_1,
  TEX_BTN_NEXT_0,
  TEX_BTN_NEXT_1,
  TEX_BTN_FASTER_0,
  TEX_BTN_FASTER_1,
  TEX_BTN_SLOWER_0,
  TEX_BTN_SLOWER_1,
  TEX_BTN_REC_START_0,
  TEX_BTN_REC_START_1,
  TEX_BTN_REC_STOP_0,
  TEX_BTN_REC_STOP_1,

  TEX_NBR
} TextureId;

class VTexture {
 private:
  static Image image[TEX_NBR];
  static VTexture* texture_;

 private:
  /** The constructor loads all textures */
  VTexture();
  void loadImage(   TextureId texnbr, const char* filename);
  void loadImageBmp(TextureId texnbr, const char* filename);
  void loadImageJpg(TextureId texnbr, const char* filename);
  void loadImageTga(TextureId texnbr, const char* filename);
  bool texMapScalePow2(TextureId texnbr);
  void drawPixels(TextureId texnbr);

 public:
  static VTexture* instance();
  virtual ~VTexture();
  /** init Must be run when creating the openGL context */
  void init();
  /** Apply a texture, next shapes drawn will use this texture */
  void apply(TextureId texnbr);
  /** disable the current texture. Next shapes wont use the current texture */
  void disable(){ glDisable(GL_TEXTURE_2D); }

  static void screenShot(const char* filename);
};
#endif
