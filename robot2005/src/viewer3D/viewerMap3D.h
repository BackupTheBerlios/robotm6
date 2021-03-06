#ifndef __VIEWER_MAP_3D_H__
#define __VIEWER_MAP_3D_H__

// --------------------------------------------------------------------
// initViewerMap3D
// --------------------------------------------------------------------
void initViewerMap3D(int winId);

// --------------------------------------------------------------------
// resetViewerMap3D
// --------------------------------------------------------------------
void resetViewerMap3D();

// --------------------------------------------------------------------
// displayViewerMap3D
// --------------------------------------------------------------------
void displayViewerMap3D(void);

// --------------------------------------------------------------------
// keyboardViewerMap3D
// --------------------------------------------------------------------
void keyboardViewerMap3D (unsigned char key, int x, int y);

// --------------------------------------------------------------------
// menuViewerMap3D
// --------------------------------------------------------------------
void menuViewerMap3D (int value);

// --------------------------------------------------------------------
// reshapeViewerMap3D
// --------------------------------------------------------------------
void reshapeViewerMap3D(int w, int h);

// --------------------------------------------------------------------
// idleViewerMap3D
// --------------------------------------------------------------------
void idleViewerMap3D(void);

// --------------------------------------------------------------------
// timerViewerMap3D
// --------------------------------------------------------------------
void timerViewerMap3D(int v);


#endif /* __VIEWER_MAP_3D_H__ */
