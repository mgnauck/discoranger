 
 // flowers.cpp

#include "settings.h"

#define	FlowerBufferDimension		256

Object	FlowerSkyBox,
		FlowerFbBox,
		FlowerSphere;

GLuint	SkyBoxTexture,
		FlowerFeedbackTexture,
		FlowerSphereTexture,
		SphereIndex,
		SkyBoxIndex;

void InitFlowers()
{
	unsigned int	*Buffer = new unsigned int [FlowerBufferDimension * FlowerBufferDimension];
	
	glGenTextures(1, &FlowerFeedbackTexture);
	glBindTexture(GL_TEXTURE_2D, FlowerFeedbackTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FlowerBufferDimension, FlowerBufferDimension, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	delete [] Buffer;

	Buffer = LoadRAW((unsigned char *)octagon, 256, 256, true);
	SkyBoxTexture = UploadTexture(Buffer, 256, 256);
	delete [] Buffer;

	Buffer = LoadRAW((unsigned char *)circle, 256, 256, true);
	FlowerSphereTexture = UploadTexture(Buffer, 256, 256);
	delete [] Buffer;

	GenCube(&FlowerSkyBox, 20, 20, 8.0f, 1.5f, 1.5f);
	GenCube(&FlowerFbBox, 10, 10, 7.9f, 1.0f, 1.0f);
	GenSphere(&FlowerSphere, 24, 24, 0.6f, 1.0f, 1.0f);

	SphereIndex = glGenLists(1);
	glNewList(SphereIndex, GL_COMPILE);
		RenderObjectImmediate(FlowerSphere);
	glEndList();

	SkyBoxIndex = glGenLists(1);
	glNewList(SkyBoxIndex, GL_COMPILE);
		RenderObjectImmediate(FlowerSkyBox);
	glEndList();
}

void RenderFlowers(unsigned long time)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat AmbientLight[]	= { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat DiffuseLight[]	= { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat LightPos[]		= { (float)sin(time * 0.003) * 4.0f, 
								(float)sin(time * 0.00003) * 4.0f, 
								(float)cos(time * 0.003) * 4.0f,
								1.0f };	

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.015f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, SkyBoxTexture);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double	radius	= sin(0.03 * time * PI / 180.0) * 1.7 + 4.0,
			x		= sin(0.03 * time * PI / 180.0) * radius,
			y		= sin(0.04 * time * PI / 180.0) * 3.6,
			z		= cos(0.02 * time * PI / 180.0) * radius + 0.3,
			xl		= cos(0.05 * time * PI / 180.0) * 3.4,
			yl		= sin(0.06 * time * PI / 180.0) * 4.0,
			zl		= cos(0.06 * time * PI / 180.0) * 2.2;

	gluPerspective(90.0, WIDTH / (double)HEIGHT, 0.1, 1000.0);
	gluLookAt(x, y, z, xl, yl, zl, 0.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
	
	// render skybox
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);
	glCullFace(GL_FRONT);
	glScalef(2.0f, 1.5f, 2.0f);
	glCallList(SkyBoxIndex);
	glCullFace(GL_BACK);
	
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, false);

	glBindTexture(GL_TEXTURE_2D, FlowerSphereTexture);
	glLoadIdentity();

	for(int k=-4; k<4; k++)
	{
		glLoadIdentity();

		glRotatef((float)sin(time * 0.0002) * 360.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(	1.3f * (float)sin(k * 0.65 + time * 0.003), 
						(float)k * 1.0f + 0.5f, 
						1.15f * (float)sin(k * 0.65 + time * 0.003));
		float	t = (1.4f - (float)fabs(k) * 0.3f) * 0.75f;
		glScalef(t, t, t);
		
		glRotatef((float)sin(time * 0.0004) * 360.0f, 1.0f, 0.0f, 0.0f);
		glRotatef((float)sin(time * 0.0003) * 360.0f, 0.0f, 1.0f, 0.0f);
		glRotatef((float)sin(time * 0.0004) * 360.0f, 0.0f, 0.0f, 1.0f);

		glCallList(SphereIndex);
	}

	glBindTexture(GL_TEXTURE_2D, FlowerFeedbackTexture);
	glLoadIdentity();

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);

	// render feedback
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	
	glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	
	glCullFace(GL_FRONT);
	glScalef(2.0f, 1.5f, 2.0f);
	RenderObject(FlowerFbBox);
	glCullFace(GL_BACK);
	
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, false);

	glDisable(GL_BLEND);
}

void DoFlowers(unsigned long time) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	glPushAttrib(GL_ENABLE_BIT);

	glViewport(0, 0, FlowerBufferDimension, FlowerBufferDimension);

	RenderFlowers(time);
	Render2ActualTexture(0, 0, FlowerBufferDimension, FlowerBufferDimension);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glViewport(XSTART, YSTART, WIDTH, HEIGHT);
	RenderFlowers(time);

	glPopAttrib();
}

void QuitFlowers()
{
	DeleteObject(&FlowerSphere);
	DeleteObject(&FlowerFbBox);
	DeleteObject(&FlowerSkyBox);
}