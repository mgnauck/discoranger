
 // settings.cpp

#include "settings.h"
#include <stdlib.h>

#ifdef _MAC_OSX_
	#include <sys/time.h>
	#include <time.h>
#endif

#ifdef _WIN32_
	HWND								hwnd;
	HGLRC								glRC;
	HDC									hDC;
	HINSTANCE							hInstance;
#endif

int									g_KbHit	= 0,
									g_GetCh	= 0,
									MouseX,	MouseY, MouseB;

bool								DemoEnd = false;

#ifdef _WIN32_
HGLRC inline SetupGL() 
{
    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
        1,                              // version number
        PFD_DRAW_TO_WINDOW              // support window
        |  PFD_SUPPORT_OPENGL           // support OpenGL
        |  PFD_DOUBLEBUFFER,            // double buffered
        PFD_TYPE_RGBA,                  // RGBA type
        32,                             // 32-bit color depth
        0, 0, 
		0, 0, 
		0, 0,							// color bits ignored
        1,								// alpha buffer?
        0,                              // shift bit ignored?
        0,                              // accumulation buffer?
        0, 0, 0, 0,                     // accum bits ignored
        16,                             // 16-bit z-buffer      
        1,                              // stencil buffer?
        0,                              // auxiliary buffer?
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
    };
    int  pixelFormat;
    HGLRC rv = 0;

    pixelFormat = ChoosePixelFormat(hDC, &pfd);
    if(pixelFormat) 
	{
        if(SetPixelFormat(hDC, pixelFormat, &pfd)) 
		{
            rv = wglCreateContext(hDC);
            if(rv) 
			{
                if(!wglMakeCurrent(hDC, rv)) 
				{
                    wglDeleteContext(rv);
                    rv = 0;
                }
            }
        }
    }
    return rv;
}

LONG WINAPI WndProc(	HWND    hWnd,
						UINT    uMsg,
                        WPARAM  wParam,
                        LPARAM  lParam) 
{
    LONG    lRet = 1;

    switch (uMsg) 
	{
		case WM_CREATE:
			hDC  = GetDC(hWnd);
			glRC = SetupGL();
			if ( !glRC ) 
			{
	            MessageBox(0, "Unable to Create OpenGL Rendering Context!", TITLE, MB_ICONERROR);
		        DestroyWindow(hWnd);
	        }
			break;
		
		case WM_SIZE: break;
    
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_QUIT:
        break;

		case WM_KEYDOWN:
			g_KbHit = 1;
		    g_GetCh = wParam;
        break;

		case WM_KEYUP:
			g_KbHit = 0;
			//g_GetCh = 0;
		break;

		/*
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
        case WM_MOUSEMOVE:
			MouseB = wParam;
			MouseX = LOWORD(lParam);
			MouseY = HIWORD(lParam);
		*/

    default:
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam);
        break;
    }

    return lRet;
}

int InitGLWindow(int width, int height, int bpp)
{
	WNDCLASS wc;

    wc.style         = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "ficken";


    if (!RegisterClass(&wc))
	{
        MessageBox(0, "Couldn't Register Window Class!", TITLE, MB_ICONERROR);
		return 0;
    }


#ifdef FULLSCREEN
	DEVMODE dmScreenSettings;									// Device Mode
	memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize			= sizeof(dmScreenSettings);	// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= width;
	dmScreenSettings.dmPelsHeight	= height;
	dmScreenSettings.dmBitsPerPel	= bpp;
	dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		return FALSE;
#endif

	int ew = width;
	int eh = height;

    hwnd = CreateWindow(	"ficken",
							TITLE,
							WS_POPUP | WS_VISIBLE |	WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							0,
							0,
							ew,
							eh,
							NULL,
							NULL,
							hInstance,
							NULL
						);
	if(!hwnd)
		return 0;

	SetFocus(hwnd);
	SetCursor(NULL);
	
	return 1;
}

void CloseGLWindow()
{
#ifdef FULLSCREEN
	ChangeDisplaySettings(NULL, 0);
	ShowCursor(TRUE);
#endif
	DestroyWindow(hwnd);
}

int APIENTRY WinMain(HINSTANCE hinstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	hInstance = hinstance;
	if (!InitGLWindow(RES_X, RES_Y, 32))
	{
		MessageBox(NULL, "Error opening OpenGL!", TITLE, MB_ICONERROR );
		return 1;
	}

	Init();

	while((g_GetCh!=27) && (!DemoEnd))
	{
		Display();

		glFinish();
		SwapBuffers(hDC);

		MSG msg;
		if( PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) 
		{
			GetMessage(&msg, 0, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Quit();
	CloseGLWindow();

	return 0;
}

#endif

#ifdef _MAC_OSX_
#include <stdio.h>
long timeGetTime() {
	
	/*
	timeval* tv;
	gettimeofday(tv, 0);
	return tv->tv_usec;
	//*/
	
	static long horst = 0;
	horst += 1000;
	return horst;
	//*/
}

// use glut
void Reshape (int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 27:
			Quit();
			exit(0);
		break;
	}
	glutPostRedisplay();
}

#include <assert.h>

void disp() {
	
	float r = (rand()%10) / 10.0f;
	float g = (rand()%10) / 10.0f;
	float b = (rand()%10) / 10.0f;

	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	assert(glGetError() == GL_NO_ERROR);
}

int main(int argc, char* argv[]) {
	
	// setup window & gl ..
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(RES_X, RES_Y); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	
	Init();
	//glutDisplayFunc(Display); 
	glutDisplayFunc(disp);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();

	Quit();
	
	return 0;
}

#endif