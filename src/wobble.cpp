
 // wobble.cpp

#include "settings.h"

#define XSubDiv			90
#define YSubDiv			90
Object	Plane;
GLuint	Texture;

void InitWobble()
{
	unsigned int *Buffer = LoadRAW((unsigned char *)circle, 256, 256, true);
	
	for(int i=0; i<256 * 256; i++)
	{
		int		col	= Buffer[i];
		double	h, s, v,
				rd, gd, bd;
		int		r = (col >> 16) & 0xff,
				g = (col >> 8) & 0xff,
				b = col & 0xff;

		RGB2HSV((double)r, (double)g, (double)b, &h, &s, &v);
		HSV2RGB((double)h + 300.0, (double)s, (double)v - 10.0, &rd, &gd, &bd);

		r = (int)rd;
		g = (int)gd;
		b = (int)bd;
		
		Buffer[i] = (r << 16) | (g << 8) | b;
	}

	Texture	= UploadTexture(Buffer, 256, 256);
	GenPlane(&Plane, XSubDiv, YSubDiv, 140, 140, 1.2f, 1.2f);

	delete [] Buffer;
}

void DoWobble(unsigned long time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushAttrib(GL_ENABLE_BIT);

	float	xTab[XSubDiv];

	for(int k=0; k<XSubDiv; k++)
		xTab[k]	= 6.75f * (float)cos(0.0015f * (float)time + k * ((8.0f * fabs((float)sin(0.00002 * time)) + 12.0f) / XSubDiv));

	for (int i=0; i<YSubDiv; i++)
	{
		float addy = 6.5f * (float)sin(i * ((18.0f * fabs((float)sin(0.0005 * time)) + 15.0f) / YSubDiv) + 0.002 * (float)time);

		for (int j=0; j<XSubDiv; j++)
			Plane.Vertices[i * XSubDiv + j].z = -24.0f + addy + xTab[j];
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat AmbientLight[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat DiffuseLight[]	= { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat LightPos[]		= { 0.0f, 0.0f, -8.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(85.0, WIDTH / (double)HEIGHT, 0.1, 1000.0);
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	float	fac	= (float)sin((float)time * 0.000571);
	glTranslatef(12.0f * fac, 19.0f * fac, 0.0f);
	glScalef(1.2f, 1.2f, 1.0f);
	glRotatef(360.0f * (float)sin(0.000428f * (float)time) * 0.5f, 0.0f, 0.0f, 1.0f);

	CalcNormals(&Plane);
	RenderObject(Plane);

	glPopAttrib();
}

void QuitWobble()
{
	DeleteObject(&Plane);
}