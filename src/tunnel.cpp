
 // tunnel.cpp
 
#include "settings.h"

Object	Cylinder1,
		Cylinder2,
		TunSphere;

GLuint	Cylinder1Index,
		Cylinder2Index,
		TunSphereIndex,
		Cylinder1Texture,
		Cylinder2Texture,
		TunSphereTexture;

void InitTunnel()
{
	GenCylinder(&Cylinder1, 24, 24, 1.0f, 1.0f, 7.0f, 30.0f);
	GenCylinder(&Cylinder2, 24, 24, 1.0f, 1.0f, 2.0f, 30.0f);
	GenSphere(&TunSphere, 16, 16, 1.0f, 1.0f, 1.0f);

	Cylinder1Texture = MakeCheckerTexture(32, 0xaf0000, 0xffffff, 0xffffff, 0xaf0000);
	Cylinder2Texture = MakeCheckerTexture(64, 0xaf0000, 0xffffff, 0xffffff, 0xaf0000);
	TunSphereTexture = MakeCheckerTexture(64, 0x0000ff, 0x12afff, 0x12afff, 0x0000ff);

	CalcNormals(&Cylinder1);
	CalcNormals(&Cylinder2);

	Cylinder1Index = glGenLists(1);
	glNewList(Cylinder1Index, GL_COMPILE);
		RenderObjectImmediate(Cylinder1);
	glEndList();	

	Cylinder2Index = glGenLists(1);
	glNewList(Cylinder2Index, GL_COMPILE);
		RenderObjectImmediate(Cylinder2);
	glEndList();	

	TunSphereIndex = glGenLists(1);
	glNewList(TunSphereIndex, GL_COMPILE);
		RenderObjectImmediate(TunSphere);
	glEndList();	
}

void DoTunnel(unsigned long time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glPushAttrib(GL_ENABLE_BIT);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Cylinder1Texture);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat AmbientLight[]	= { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat DiffuseLight[]	= { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat LightPos[]		= { (float)sin(0.002 * time) * 3.0f, 
								(float)sin(0.002 * time) * 3.0f, 
								-(float)cos(0.002 * time) * 3.0f, 
								0.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(80.0, WIDTH / (double)HEIGHT, 0.1, 1000.0);

	double	Radius	= sin(0.001 * time) * 0.4 + 2.6,
			y		= -sin(0.0002 * time ) * 14.0;

	gluLookAt(	sin(0.03 * time * PI / 180.0) * Radius,
				y,
				cos(0.03 * time * PI / 180.0) * Radius,
				0.0, -sin(0.001 * time ) * 0.65 + y, 0.0, 
				0.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);
	glCullFace(GL_FRONT);
	glScalef(3.0f, 60.0f, 3.0f);
	glCallList(Cylinder1Index);
	glCullFace(GL_BACK);	
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, false);

	glBindTexture(GL_TEXTURE_2D, TunSphereTexture);

	for(int i=-32; i<32; i++)
	{
		glLoadIdentity();
		
		glRotatef((float)sin(time * 0.0002) * 360.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(	1.5f * (float)sin((double)i * 0.65), 
						(float)i * 0.8f, 
						1.5f * (float)cos((double)i * 0.65));
		
		glRotatef((float)sin(time * 0.0006) * 360.0f, 1.0f, 0.0f, 0.0f);
		glRotatef((float)sin(time * 0.0005) * 360.0f, 0.0f, 0.0f, 1.0f);

		glScalef(0.45f, 0.2f, 0.45f);
		
		glCallList(TunSphereIndex);
	}

	glBindTexture(GL_TEXTURE_2D, Cylinder2Texture);
	glLoadIdentity();

	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_ONE, GL_SRC_COLOR);
	glEnable(GL_BLEND);
	glScalef(0.75f, 60.0f, 0.75f);
	glCallList(Cylinder2Index);
	
	glPopAttrib();
}

void QuitTunnel()
{
	DeleteObject(&TunSphere);
	DeleteObject(&Cylinder2);
	DeleteObject(&Cylinder1);
}