#include "text.h"
#include <GL/gl.h>
#include <ctype.h>
#include <stdarg.h>
#include <string>

int textSize_=8;
ViewerColorST textColor_;

void ViewerText::setColor(ViewerColorST color)
{
  textColor_ = color;
}

void ViewerText::setSize(int size)
{
  textSize_=size;
}
 
void ViewerText::draw(int x, int y, const char* fmt,...)
{
  int i=0, j=0;
  textColor_.setGL();
  char txt[256];
  va_list argp;
  va_start(argp, fmt);
  vsprintf(txt, fmt, argp);
  va_end(argp);
  while(i<256 && txt[i] != 0) {
    if (txt[i]=='\n') {
      y+=textSize_;
      j=0;
      ++i;
      continue;
    }
    drawLetter(x+j*textSize_, y, txt[i]);
    ++i;
    ++j;
  }
}


void ViewerText::drawLetter(int x, int y, const char letter)
{
  glPushMatrix();
  glTranslatef(x,y,0);
  glScaled(textSize_*0.6,-textSize_*0.8,textSize_);
  int l = toupper(letter);
  switch(l) {
  case ' ':
    break;

    // letters
  case 'A':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0.4,1); glVertex2f(0.8,0);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.2,0.5); glVertex2f(0.6,0.5);
    glEnd();
    break;
  case 'B':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.6,0.6); glVertex2f(0.8,0.6); glVertex2f(0.8,0); 
    glVertex2f(0,0); glVertex2f(0, 1); glVertex2f(0.6,1); 
    glVertex2f(0.6,0.6); glVertex2f(0,0.6);
    glEnd();
    break;
  case 'C':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8,0); glVertex2f(0,0); glVertex2f(0,1);  glVertex2f(0.6,1); 
    glEnd();
    break;
  case 'D':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8,0); glVertex2f(0,0); glVertex2f(0,1);  glVertex2f(0.6,1);
    glVertex2f(0.8,0);
    glEnd();
    break;
  case 'E':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8,0); glVertex2f(0,0); glVertex2f(0,1);  glVertex2f(0.8,1);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0.5); glVertex2f(0.6,0.5);
    glEnd();
    break;
  case 'F':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0,1);  glVertex2f(0.8, 1);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0.5); glVertex2f(0.6,0.5);
    glEnd();
    break; 
  case 'G':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.7,0.5); glVertex2f(0.8,0.5);  glVertex2f(0.8,0);
    glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0.8, 1); glVertex2f(0.8,0.9);
    glEnd();
    break;
  case 'H':
    glBegin(GL_LINES);
    glVertex2f(0,0); glVertex2f(0,1); 
    glVertex2f(0.8,0); glVertex2f(0.8,1); 
    glVertex2f(0,0.5); glVertex2f(0.8,0.5); 
    glEnd();
    break;
  case 'I':
    glBegin(GL_LINES);
    glVertex2f(0.4,0); glVertex2f(0.4,1); 
    glVertex2f(0.3,0); glVertex2f(0.5,0); 
    glVertex2f(0.3,1); glVertex2f(0.5,1); 
    glEnd();
    break;
  case 'J':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.6,1); glVertex2f(0.6,0); 
    glVertex2f(0,0); glVertex2f(0,0.3);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.5,1); glVertex2f(0.7,1); 
    glEnd();
    break;
  case 'K':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8,1); glVertex2f(0,0.5); glVertex2f(0.8,0); 
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0,0); glVertex2f(0,1); 
    glEnd();
    break;
  case 'L':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0.8,0); 
    glEnd();
    break;
  case 'M':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0.4,0.5);
    glVertex2f(0.8,1); glVertex2f(0.8,0);
    glEnd();
    break;
  case 'N':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0.8,0);
    glVertex2f(0.8,1); 
    glEnd();
    break;
  case 'O':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0.8,1);
    glVertex2f(0.8,0); glVertex2f(0,0);
    glEnd();
    break;
  case 'P':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0.8,1);
    glVertex2f(0.8,0.5); glVertex2f(0,0.5);
    glEnd();
    break;
  case 'Q':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8,0); glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0.8,1);
    glVertex2f(0.8,0); glVertex2f(0.7,0.1);
    glEnd();
    break;
  case 'R':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0.8,1);
    glVertex2f(0.8,0.5); glVertex2f(0,0.5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.5,0.5); glVertex2f(0.8,0); 
    glEnd();
    break;
  case 'S':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0.8,0); glVertex2f(0.8,0.5);
    glVertex2f(0,0.5); glVertex2f(0,1);  glVertex2f(0.8,1);
    glEnd();
    break;
  case 'T':
    glBegin(GL_LINES);
    glVertex2f(0.4,0); glVertex2f(0.4,1); 
    glVertex2f(0,1); glVertex2f(0.8,1);
    glEnd();
    break;
  case 'U':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,1); glVertex2f(0,0); glVertex2f(0.8,0);
    glVertex2f(0.8,1); 
    glEnd();
    break;
  case 'V':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,1); glVertex2f(0.4,0); glVertex2f(0.8,1);
    glEnd();
    break;
  case 'W':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,1); glVertex2f(0.1,0); glVertex2f(0.4,0.5);
    glVertex2f(0.7,0); glVertex2f(0.8,1); 
    glEnd();
    break;
  case 'X':
    glBegin(GL_LINES);
    glVertex2f(0,1); glVertex2f(0.8,0); 
    glVertex2f(0,0); glVertex2f(0.8,1);
    glEnd();
    break;
  case 'Y':
    glBegin(GL_LINES);
    glVertex2f(0,1); glVertex2f(0.4,0.5); 
    glVertex2f(0.8,1); glVertex2f(0.4,0.5);
    glVertex2f(0.4,0); glVertex2f(0.4,0.5);
    glEnd();
    break;
  case 'Z':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,1); glVertex2f(0.8,1); glVertex2f(0,0);
    glVertex2f(0.8,0);
    glEnd();
    break;
    
    // numbers
  case '0':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0,1); glVertex2f(0.8,1);
    glVertex2f(0.8,0); glVertex2f(0,0);glVertex2f(0.8,1);
    glEnd();
    break;
  case '1':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.3,0.9); glVertex2f(0.4,1); glVertex2f(0.4,0);
    glEnd();
    break;
  case '2':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0.6); glVertex2f(0,1); glVertex2f(0.8,1);
    glVertex2f(0.8,0.5); glVertex2f(0,0); glVertex2f(0.8,0);
    glEnd();
    break;
  case '3':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0.8,0); glVertex2f(0.8,1);
    glVertex2f(0,1); 
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0.3,0.5); glVertex2f(0.8,0.5);
    glEnd();
    break;
  case '4':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.6,0); glVertex2f(0.6,1); glVertex2f(0,0.4);
    glVertex2f(0.8,0.4); 
    glEnd();
    break;
  case '5':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8,1); glVertex2f(0,1); glVertex2f(0,0.5);
    glVertex2f(0.8,0.5); glVertex2f(0.8,0); glVertex2f(0,0);
    glEnd();
    break;
  case '6':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8,1); glVertex2f(0,1); glVertex2f(0,0);
    glVertex2f(0.8,0); glVertex2f(0.8,0.5); glVertex2f(0,0.5);
    glEnd();
    break;
  case '7':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,1); glVertex2f(0.8,1); glVertex2f(0.4,0);
    glEnd();
    break;
  case '8':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0.8,0); glVertex2f(0.8,1);
    glVertex2f(0,1); glVertex2f(0,0);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(0,0.5); glVertex2f(0.8,0.5);
    glEnd();
    break;
  case '9':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0.8,0); glVertex2f(0.8,1);
    glVertex2f(0,1); glVertex2f(0,0.5); glVertex2f(0.8,0.5);
    glEnd();
    break;

    // special caracteres
  case '-':
    glBegin(GL_LINES);
    glVertex2f(0.1,0.3); glVertex2f(0.7,0.3); 
    glEnd();
    break; 
  case '+':
    glBegin(GL_LINES);
    glVertex2f(0.1,0.3); glVertex2f(0.7,0.3); 
    glVertex2f(0.4,0); glVertex2f(0.4,0.6); 
    glEnd();
    break;
  case '(':
  case '[':
  case '{':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.5,0); glVertex2f(0.4,0); glVertex2f(0.4,1);
    glVertex2f(0.5,1); 
    glEnd();
    break; 
  case ')':
  case ']':
  case '}':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.3,0); glVertex2f(0.4,0); glVertex2f(0.4,1);
    glVertex2f(0.3,1); 
    glEnd();
    break;
  case '=':
    glBegin(GL_LINES);
    glVertex2f(0.1,0); glVertex2f(0.7,0); 
    glVertex2f(0.1,0.4); glVertex2f(0.7,0.4); 
    glEnd();
    break;
  case ':':
  case ';':
    glBegin(GL_LINES);
    glVertex2f(0.4,0); glVertex2f(0.4,0.05); 
    glVertex2f(0.4,0.4); glVertex2f(0.4,0.35); 
    glEnd();
    break;
  case '.':
  case ',':
    glBegin(GL_LINES);
    glVertex2f(0.4,0); glVertex2f(0.4,0.05);  
    glEnd();
    break;
  case '_':
    glBegin(GL_LINES);
    glVertex2f(0,0); glVertex2f(0.8,0.0);  
    glEnd();
    break;
  case '<':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.8,0); glVertex2f(0,0.5); glVertex2f(0.8,1); 
    glEnd();
    break;
  case '>':
    glBegin(GL_LINE_STRIP);
    glVertex2f(0,0); glVertex2f(0.8,0.5); glVertex2f(0,1); 
    glEnd();
    break;
  case '!':
  case '?':
  case '|':
    glBegin(GL_LINES);
    glVertex2f(0.4,0); glVertex2f(0.4,1);
    glEnd();
    break;
  case '/':
    glBegin(GL_LINES);
    glVertex2f(0.1,0); glVertex2f(0.7,1);
    glEnd();
    break; 
  case '\\':
    glBegin(GL_LINES);
    glVertex2f(0.1,1); glVertex2f(0.7,0);
    glEnd();
    break;
  case '\'':
    glBegin(GL_LINES);
    glVertex2f(0.4,1); glVertex2f(0.4,0.7);
    glEnd();
    break;
  case '\"':
    glBegin(GL_LINES);
    glVertex2f(0.35,1); glVertex2f(0.35,0.7);
    glVertex2f(0.45,1); glVertex2f(0.45,0.7);
    glEnd();
    break;
  default:
    break;
  }
  glPopMatrix();
}

