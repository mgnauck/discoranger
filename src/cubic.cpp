
 // cubic.cpp

#include "settings.h"

GLuint	CubicTexture,
		CubicSphereTexture;
Object	Cubic,
		CubicSphere,
		CubicSphere2;

void InitCubic()
{
	unsigned int *temp;

	temp = LoadRAW((unsigned char *)quad, 256, 256, true);
	CubicTexture = UploadTexture(temp, 256, 256);
	delete [] temp;

	temp = LoadRAW((unsigned char *)chess, 256, 256, true);
	CubicSphereTexture = UploadTexture(temp, 256, 256);
	delete [] temp;

	GenCube(&Cubic, 2, 2, 12.0f, 3.0f, 3.0f);
	GenSphere(&CubicSphere, 64, 64, 2.0f, 1.0f, 1.0f);		// src CubicSphere
	GenSphere(&CubicSphere2, 64, 64, 2.0f, 6.0f, 6.0f);
}

void DoCubic(unsigned long time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushAttrib(GL_ENABLE_BIT);

		GLfloat AmbientLight[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
		GLfloat DiffuseLight[]	= { 0.8f, 0.8f, 0.9f, 1.0f };
		GLfloat LightPos[]		= { 0.0f, 5.0f, 0.0f, 1.0f };

		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.02f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(100.0 + (float)sin(time * 0.003) * 20.0f, WIDTH / (double)HEIGHT, 0.1, 1000.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
		
		double	Radius	= sin(time * 0.002) * 1.5 + 4.25;
		gluLookAt(	Radius * sin(0.05 * time * PI / 180), 
					sin(time * 0.001) * 4.0, 
					Radius * cos(0.05 * time * PI / 180), 
					0, 0, 0, 
					sin(0.002 * time * PI / 90) * 1.0, 
					1.0, 
					cos(0.002 * time * PI / 90) * 1.0);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, CubicTexture);

		glDisable(GL_LIGHTING);

		glCullFace(GL_FRONT);
		CalcNormals(&Cubic);
		RenderObject(Cubic);
		glCullFace(GL_BACK);
		
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		for(int i=0; i<CubicSphere.NumVertices; i++)
		{
			float	d =	1.32f	+ 0.33f * (float)sin(CubicSphere.Vertices[i].x * 0.58f + time * 0.003f)
								+ 0.56f * (float)sin(CubicSphere.Vertices[i].y * 0.83f + time * 0.004f)
								+ 0.13f * (float)sin(CubicSphere.Vertices[i].z * 0.38f + time * 0.005f);

			CubicSphere2.Vertices[i].x = (CubicSphere.Vertices[i].x * d) * 0.8f;
			CubicSphere2.Vertices[i].y = (CubicSphere.Vertices[i].y * d) * 0.8f;
			CubicSphere2.Vertices[i].z = (CubicSphere.Vertices[i].z * d) * 0.8f;
		}

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, CubicSphereTexture);

		CalcNormals(&CubicSphere2);

		glRotatef((float)sin(time * 0.00006) * 360.0f, 1.0f, 0.0f, 0.0f);
		glRotatef((float)sin(time * 0.00007) * 360.0f, 0.0f, 1.0f, 0.0f);
		glRotatef((float)sin(time * 0.00005) * 360.0f, 0.0f, 0.0f, 1.0f);
		RenderObject(CubicSphere2);

	glPopAttrib();
}

void QuitCubic()
{
	DeleteObject(&CubicSphere2);
	DeleteObject(&CubicSphere);
	DeleteObject(&Cubic);
}