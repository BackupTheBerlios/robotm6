#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GL/glu.h>
#include "robotImage.h"
#include "viewer3D.h"

#define Texture VTexture::instance()

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
