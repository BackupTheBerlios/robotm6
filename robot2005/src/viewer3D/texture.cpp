/////////////////////////////
//  Alexis Charigon        //
//  Laurent Saint-Marcel   //
//  ISIA 2000              //
/////////////////////////////

// gestion des textures
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "texture.h"

#ifndef TEXTURE_DEFAULT_DIR
#define TEXTURE_DEFAULT_DIR "../texture"
#endif
#ifndef TEXTURE_DEFAULT_DIR_1
#define TEXTURE_DEFAULT_DIR_1 "/home/charette/soft/robot2005/texture"
#endif
#ifndef TEXTURE_DEFAULT_DIR_2
#define TEXTURE_DEFAULT_DIR_2 "/home/lsm/robot2005/texture"
#endif

typedef unsigned char BYTE;

Image VTexture::image[TEX_NBR];
VTexture* VTexture::texture_=NULL;

VTexture* VTexture::instance() 
{
    if (!texture_) texture_ = new VTexture();
    return texture_;
}

VTexture::VTexture()
{
  loadImage(TEX_LOGO,          "logo.bmp");
  loadImage(TEX_LOGO_ISIA,     "logo_isia.bmp");
  loadImage(TEX_ROBOT_SIDE_LEFT, "robot_sl.bmp");
  loadImage(TEX_ROBOT_SIDE_RIGHT,"robot_sr.bmp");
  loadImage(TEX_ROBOT_FRONT,   "robot_fr.bmp");
  loadImage(TEX_ROBOT_BACK,    "robot_bk.bmp");
  loadImage(TEX_ROBOT_TOP,     "robot_tp.bmp"); 
  loadImage(TEX_SALLE_SIDE1,   "side1.bmp");
  loadImage(TEX_SALLE_SIDE2,   "side2.bmp");
  loadImage(TEX_SALLE_SIDE3,   "side3.bmp");
  loadImage(TEX_SALLE_SIDE4,   "side4.bmp");
  loadImage(TEX_SALLE_PLAFOND, "plafond.bmp");

  loadImage(TEX_ATTACK_SIDE_R1, "a_r1.bmp");
  loadImage(TEX_ATTACK_SIDE_R2, "a_r2.bmp");
  loadImage(TEX_ATTACK_SIDE_L1, "a_l1.bmp");
  loadImage(TEX_ATTACK_SIDE_L2, "a_l2.bmp");
  loadImage(TEX_ATTACK_SIDE_F,  "a_f.bmp");
  loadImage(TEX_ATTACK_SIDE_R,  "a_r.bmp");
  loadImage(TEX_ATTACK_TOP,     "a_top.bmp");

  loadImage(TEX_DEFENCE_SIDE_R,  "d_r.bmp");
  loadImage(TEX_DEFENCE_SIDE_L,  "d_l.bmp");
  loadImage(TEX_DEFENCE_SIDE_F,  "d_f.bmp");
  loadImage(TEX_DEFENCE_SIDE_RR, "d_rr.bmp");
  loadImage(TEX_DEFENCE_TOP,     "d_top.bmp");

  loadImage(TEX_LCD, "lcd.bmp");
  loadImage(TEX_BTN_0, "btn_0.bmp");
  loadImage(TXT_BTN_1, "btn_1.bmp");
  loadImage(TXT_JACKIN, "jackin.bmp");
  loadImage(TXT_JACKOUT, "jackout.bmp");
  loadImage(TEX_AU_IN, "au_in.bmp");
  loadImage(TEX_AU_OUT, "au_out.bmp");
  loadImage(TEX_TEAM_RED, "team_red.bmp");
  loadImage(TEX_TEAM_GREEN, "team_green.bmp");

  loadImage(TEX_BRIDGE0, "bridge0.bmp");
  loadImage(TEX_BRIDGE1, "bridge1.bmp");
  loadImage(TEX_SUPPORT0, "support0.bmp");
  loadImage(TEX_SUPPORT1, "support1.bmp");

  loadImage(TEX_BTN_PREV_0, "btnprev0.bmp");
  loadImage(TEX_BTN_PREV_1, "btnprev1.bmp");
  loadImage(TEX_BTN_PLAY_0, "btnplay0.bmp");
  loadImage(TEX_BTN_PLAY_1, "btnplay1.bmp");
  loadImage(TEX_BTN_STEP_FORWARD_0,  "btnfor0.bmp");
  loadImage(TEX_BTN_STEP_FORWARD_1,  "btnfor1.bmp");
  loadImage(TEX_BTN_STEP_BACKWARD_0, "btnback0.bmp");
  loadImage(TEX_BTN_STEP_BACKWARD_1, "btnback1.bmp");
  loadImage(TEX_BTN_PAUSE_0,     "btnpause0.bmp");
  loadImage(TEX_BTN_PAUSE_1,     "btnpause1.bmp");
  loadImage(TEX_BTN_NEXT_0,      "btnnext0.bmp");
  loadImage(TEX_BTN_NEXT_1,      "btnnext1.bmp");
  loadImage(TEX_BTN_FASTER_0,    "btnfast0.bmp");
  loadImage(TEX_BTN_FASTER_1,    "btnfast1.bmp");
  loadImage(TEX_BTN_SLOWER_0,    "btnslow0.bmp");
  loadImage(TEX_BTN_SLOWER_1,    "btnslow1.bmp");
  loadImage(TEX_BTN_REC_START_0, "btnrecstart0.bmp");
  loadImage(TEX_BTN_REC_START_1, "btnrecstart1.bmp");
  loadImage(TEX_BTN_REC_STOP_0,  "btnrecstop0.bmp");
  loadImage(TEX_BTN_REC_STOP_1,  "btnrecstop1.bmp");
}

VTexture::~VTexture()
{
    texture_=NULL;
}

void VTexture::init(){
  glEnable(GL_TEXTURE_2D);
  // Texture Mapping 
  //
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  // Textures repeat.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  // Set tecturing mode to Decal or Modulate
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL /*GL_MODULATE*/);
  
   // Low quality, but faster speed.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  /*
    
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);*/
  glDisable(GL_TEXTURE_2D);
}

void VTexture::loadImageBmp(TextureId texnbr,
			   const char* filename)
{
  Image img;
  if (BmpBitmap::load(img, filename, false)) {
    image[(int)texnbr].copyAndPow2(img);
  }
}

void VTexture::loadImageJpg(TextureId texnbr,
			   const char* filename)
{

}

void VTexture::loadImageTga(TextureId texnbr,
			   const char* filename)
{
  Image img;
  TgaBitmap::load(img, filename, false);
  image[(int)texnbr].copyAndPow2(img);
}

void VTexture::loadImage(TextureId texnbr,
			const char* fileName){
    
  char filename[255];
  FILE* file=NULL;
  if (!file) {
      sprintf(filename,"%s/%s", TEXTURE_DEFAULT_DIR, 
              fileName);
      fopen(filename, "r");
  }
  if (!file) {
      sprintf(filename,"%s/%s", TEXTURE_DEFAULT_DIR_1, 
              fileName);
      file=fopen(filename, "r");
  }
  if (!file) {
      sprintf(filename,"%s/%s", TEXTURE_DEFAULT_DIR_2, 
              fileName);
      file=fopen(filename, "r");
  }
  if (!file) {
      printf("Cannot find: %s\n", fileName);
      return;
  } else {
      fclose(file);
  }

  if (strcmp("bmp", filename+strlen(filename)-3)==0) {
    loadImageBmp(texnbr, filename);
  } else if (strcmp("jpg", filename+strlen(filename)-3)==0) {
    loadImageJpg(texnbr, filename);
  } else  if (strcmp("tga", filename+strlen(filename)-3)==0) {
    loadImageTga(texnbr, filename);
  } else {
    printf("unknown format %s\n", filename);
  }
}

void VTexture::apply(TextureId texnbr){
  if (image[(int)texnbr].width == 0 
      || image[(int)texnbr].height == 0) {
    glDisable( GL_TEXTURE_2D );
    return;
  }
  glEnable( GL_TEXTURE_2D );
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 
	       0, 
	       3, // RGB textures.
	       image[(int)texnbr].width, 
               image[(int)texnbr].height,
	       0, 
	       GL_RGB,
	       GL_UNSIGNED_BYTE, // imageData is a GLubyte pointer.
	       image[(int)texnbr].data);
}

void VTexture::drawPixels(TextureId texnbr)
{
  GLint iAlign ;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &iAlign) ; 
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  glDrawPixels(image[(int)texnbr].width, image[(int)texnbr].height, 
	       GL_RGB, GL_UNSIGNED_BYTE, image[(int)texnbr].data) ;
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, iAlign);	
}

//
// TexMapScalePow2 - Scale image to power of 2 height and width
//
bool VTexture::texMapScalePow2(TextureId texnbr)
{
  GLint glMaxTexDim ;
  double xPow2, yPow2;
  int ixPow2, iyPow2;
  int xSize2, ySize2;
  
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);
  glMaxTexDim = min(128, glMaxTexDim);
  
  if (image[(int)texnbr].width <= (unsigned int)glMaxTexDim)
    xPow2 = log((double)image[(int)texnbr].width) / log(2.0);
  else
    xPow2 = log((double)glMaxTexDim) / log(2.0);
  
  if (image[(int)texnbr].height <= (unsigned int)glMaxTexDim)
    yPow2 = log((double)image[(int)texnbr].height) / log(2.0);
  else
    yPow2 = log((double)glMaxTexDim) / log(2.0);
  
  ixPow2 = (int)xPow2;
  iyPow2 = (int)yPow2;
  
  if (xPow2 != (double)ixPow2)
    ixPow2++;
  if (yPow2 != (double)iyPow2)
    iyPow2++;
  
  xSize2 = 1 << ixPow2;
  ySize2 = 1 << iyPow2;
  
  Pixel *pData = new Pixel[xSize2 * ySize2];
  if (!pData) return false;
  gluScaleImage(GL_RGB, image[(int)texnbr].width, image[(int)texnbr].height,
		GL_UNSIGNED_BYTE, image[(int)texnbr].data,
		xSize2, ySize2, GL_UNSIGNED_BYTE,
		pData);
  
  delete image[(int)texnbr].data;
  image[(int)texnbr].data   = pData; 
  image[(int)texnbr].width  = xSize2 ;
  image[(int)texnbr].height = ySize2 ;
  
  return true ;
}
