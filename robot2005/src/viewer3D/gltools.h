#ifndef __GLTOOLS_H__
#define __GLTOOLS_H__

static const int TEXTURE_NBR=2;
int loadGLTextures();
void buildFont();
void killFont();
void glPrint(int x, int y, int set, const char *fmt, ...);

#endif
