#include "button.h"

ControlButton::ControlButton(ViewerControlButtonId Id, 
			     TextureId textureReleased,
			     TextureId texturePressed,
			     const char* Name,
			     int X, int Y,
			     bool Maintain,
			     bool Enable,
			     bool Clicked) :
	id(Id), texture1(textureReleased), texturePushed(texturePressed),
	clickCB(NULL), unClickCB(NULL), x(X), y(Y), 
	enable(Enable), clicked(Clicked), maintain(Maintain) 
{
    strcpy(name, Name);
    l=maintain?15:31;
}

void ControlButton::checkClick(int mx, int my) {
    if (mx<x || mx>x+l+1 || my<y || my>y+l+1) return;
    if (!maintain) {
	if (enable) clicked=true;
    }
}

void ControlButton::checkUnclick(int mx, int my) {
    if (mx<x || mx>x+l+1 || my<y || my>y+l+1) return;
    if (maintain) {
	if (enable) {
	    clicked=!clicked;
	    if (!clicked && unClickCB) unClickCB(id);
	    if (clicked && clickCB) clickCB(id);
	}
    } else {
	if (enable && clickCB) clickCB(id);
	if (enable) clicked=false;
    }
}
void ControlButton::draw()
{
    if (enable) {
	glColor3f(1.0f,1.0f,1.0f);
    } else {
	glColor3f(.5f,.5f,.5f);
    }
    if (clicked) {
	Texture->apply(texturePushed);
    } else {
	Texture->apply(texture1);
    }
    glBegin(GL_QUADS);
    glTexCoord2f( 0.0, 0.0 ); glVertex2d(x+l, y);
    glTexCoord2f( 0.0, 1.0 ); glVertex2d(x+l, y+l);
    glTexCoord2f( 1.0, 1.0 ); glVertex2d(x,   y+l);	
    glTexCoord2f( 1.0, 0.0 ); glVertex2d(x,   y);	
    glEnd(); 
    glDisable(GL_TEXTURE_2D);
}

void ControlButton::registerCB(clickBtnCB ClickCB,
			       clickBtnCB UnClickCB)
{
    clickCB   = ClickCB;
    unClickCB = UnClickCB;
}

void ControlButton::setEnable(bool Enable)
{
    enable=Enable;
}

void ControlButton::setTexture(TextureId t1, 
			       TextureId t2)
{
    texture1=t1;
    texturePushed=t2;
}

void ControlButton::setClicked(bool Clicked)
{
    clicked=Clicked;
}

