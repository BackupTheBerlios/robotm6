#ifndef __VIEWER_BUTTON_H__
#define __VIEWER_BUTTON_H__

#include "texture.h"
#include "viewer3D.h"
//typedef void (*clickBtnCB)(int id);

class ControlButton {
 private:
    ViewerControlButtonId id;
    int l; // largeur du bouton
    TextureId texture1;
    TextureId texturePushed;
    char name[255];
    clickBtnCB clickCB;
    clickBtnCB unClickCB; // seulement pour les boutons qui restent enfonce
    int x;
    int y;
    bool enable;
    bool clicked;
    bool maintain; // bouton qui reste enfonce

 public:
    ControlButton(ViewerControlButtonId id, 
		  TextureId textureReleased,
		  TextureId texturePressed,
		  const char* name,
		  int x, int y,
		  bool maintain=false,
		  bool enable=true,
		  bool clicked=false);

    void checkClick(int mx, int my);
    
    void checkUnclick(int mx, int my);
    void draw();
    void registerCB(clickBtnCB ClickCB,
		    clickBtnCB UnClickCB=NULL);
    void setEnable(bool Enable);
    void setClicked(bool Clicked);
    void setTexture(TextureId t1, 
		    TextureId t2);

};

#endif // __VIEWER_BUTTON_H__
