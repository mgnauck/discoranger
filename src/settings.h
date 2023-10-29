
 // settings.h

#ifndef __SETTINGS__H
#define __SETTINGS__H

#define _MAC_OSX_
//#define _MUSIC_

#ifdef _WIN32_
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
#endif

#ifdef _MAC_OSX_
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#endif

#include <math.h>
#include "3dmath.h"
#include "render.h"
#include "add.h"
#include "font.h"
#include "flash.h"
#include "text.h"
#include "generator.h"
#include "wobble.h"
#include "flowers.h"
#include "cubic.h"
#include "feedback.h"
#include "spiral.h"
#include "soap.h"
#include "weight.h"
#include "greets.h"
#include "tunnel.h"

#ifdef _MUSIC_
	#include "music.h"
#endif
	
#include "timing.h"

// datafiles

#ifdef _MUSIC_
	#include "data/d2.h"
#endif

#include "data/chess.h"
#include "data/circle.h"
#include "data/font.h"
#include "data/octagon.h"
#include "data/quad.h"
#include "data/shape.h"
#include "data/star.h"
#include "data/faint.h"

#define	TITLE						" _69.housemeizter"
#define	FULLSCREEN
#define GL_GENERATE_MIPMAP_SGIS		0x8191

#define	RES_X						800
#define	RES_Y						600

const GLfloat white[4]				= {1.0f, 1.0f, 1.0f, 1.0f};

#define WIDTH						RES_X
#define HEIGHT						(RES_Y * 11 / 16)

#define XSTART						0
#define YSTART						((RES_Y - HEIGHT) / 2)
#define XEND						RES_X
#define YEND						(RES_Y - ((RES_Y - HEIGHT) / 2))

#ifdef _WIN32_
	extern	HDC							hDC;
	extern	HWND						hwnd;
#endif

extern 	int							g_KbHit;
extern	int							g_GetCh;
extern	int							MouseB, MouseX, MouseY;	
extern	bool						DemoEnd;

#ifdef _MAC_OSX_
	long timeGetTime();
#endif

extern void Init();
extern void Display();
extern void Quit();

#endif