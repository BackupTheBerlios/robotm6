
#ifndef __VIEWER_TEXT_H__
#define __VIEWER_TEXT_H__

#include "viewer3D.h"

namespace ViewerText {
  void setColor(ViewerColorST);
  void setSize(int size);
  void draw(int x, int y, const char* txt,...);

  // protected
  void drawLetter(int x, int y, const char letter);
};

#endif
