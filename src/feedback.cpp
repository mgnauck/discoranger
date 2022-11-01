
 // feedback.cpp

#include "settings.h"

#define	FeedbackBufferDimension		128

Object	FeedbackSphere,
		FeedbackCube,
		FeedbackSkyBox;

GLuint	FeedbackSkyBoxTexture,
		FeedbackSphereTexture,
		FeedbackTexture,
		FeedbackSphereIndex;

void InitFeedback()
{
	unsigned int *Buffer = new unsigned int [FeedbackBufferDimension * FeedbackBufferDimension];
	
	glGenTextures(1, &FeedbackTexture);
	glBindTexture(GL_TEXTURE_2D, FeedbackTexture);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, FeedbackBufferDimension, FeedbackBufferDimension, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FeedbackBufferDimension, FeedbackBufferDimension, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	delete [] Buffer;

	// MakeCheckerTexture(256, 0xd4c081, 0xa27c30, 0xa27c30, 0xd4c081);
	Buffer = LoadRAW((unsigned char *)chess, 256, 256, true);
				
	for(int i=0; i<256 * 256; i++)
	{
		int		col	= Buffer[i];
		double	h, s, v,
				rd, gd, bd;
		int		r = (col >> 16) & 0xff,
				g = (col >> 8) & 0xff,
				b = col & 0xff;

		r = (255 - r);
		g = (255 - g);
		b = (255 - b);

		RGB2HSV((double)r, (double)g, (double)b, &h, &s, &v);
		HSV2RGB((double)h - 6.0, (double)s + 0.05, (double)v - 30.0, &rd, &gd, &bd);

		r = (int)rd;
		g = (int)gd;
		b = (int)bd;
		
		Buffer[i] = (r << 16) | (g << 8) | b;
	}

	FeedbackSkyBoxTexture = UploadTexture(Buffer, 256, 256);
	delete [] Buffer;
	
	Buffer = LoadRAW((unsigned char *)star, 256, 256, true);
	FeedbackSphereTexture = UploadTexture(Buffer, 256, 256);
	delete [] Buffer;

	GenSphere(&FeedbackSphere, 20, 20, 0.5f, 4.0f, 4.0f);
	GenCube(&FeedbackSkyBox, 10, 10, 7.0f, 4.0f, 4.0f);
	GenCube(&FeedbackCube, 5, 5, 1.5f, 1.0f, 1.0f);

	FeedbackSphereIndex = glGenLists(1);
	glNewList(FeedbackSphereIndex, GL_COMPILE);
		RenderObjectImmediate(FeedbackSphere);
	glEndList();
}

void RenderRoomAndSpheres(unsigned long time) {

	GLfloat	FogCol[]	= { 0.2f, 0.2f, 0.2f, 1.0f };

	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.5f);
	glFogf(GL_FOG_START, 0.0f);
	glFogf(GL_FOG_END, 1.0f);
	glFogfv(GL_FOG_COLOR, FogCol);
	glEnable(GL_FOG);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, FeedbackSkyBoxTexture);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat AmbientLight[]	= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat DiffuseLight[]	= { 0.8f, 0.8f, 0.9f, 1.0f };
	GLfloat LightPos[]		= { 0.0f, 3.25f, 0.0f, 1.0f };	

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.02f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(80.0, WIDTH / (double)HEIGHT, 0.1, 1000.0);

	gluLookAt(	sin(0.03 * time * PI / 180.0) * 1.5,
				fabs(sin(time * 0.0003)) * 2.5 + 1.0,
				-cos(0.03 * time * PI / 180.0) * 1.5,
				0.0, 0.0, 0.0, 
				fabs( sin(0.02 * time * PI / 180.0) ) * 1.0, 0.0, fabs( cos(0.02 * time * PI / 180.0) ) * 1.0);
	
	// fog transformation to have some kind of ground fog
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	//glTranslatef(0.0f, -15.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// inverse fog transformation
	//glTranslatef(0.0f, +15.0f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);

	glCullFace(GL_FRONT);
	RenderObject(FeedbackSkyBox);
	glCullFace(GL_BACK);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, false);

	glBindTexture(GL_TEXTURE_2D, FeedbackSphereTexture);
	for(int i=0; i<360; i += 30)
	{
		glLoadIdentity();
		
		// inverse fog transformation
		//glTranslatef(0.0f, +15.0f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

		glRotatef((float)sin(time * 0.00012) * 360.0f, 0.0f, 1.0f, 0.0f);

		glTranslatef(	(float)sin(i * PI / 180) * 2.0f,
						0.25f,
						(float)cos(i * PI / 180) * 2.0f);

		glRotatef((float)sin(0.01 * i + time * 0.0003) * 360.0f, 1.0f, 0.0f, 0.0f);
		glRotatef((float)sin(0.04 * i + time * 0.0002) * 360.0f, 0.0f, 1.0f, 0.0f);
		glRotatef((float)sin(0.03 * i + time * 0.0001) * 360.0f, 0.0f, 0.0f, 1.0f);

		glCallList(FeedbackSphereIndex);
	}

	glLoadIdentity();

	glDisable(GL_FOG);
}

void DoFeedback(unsigned long time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glPushAttrib(GL_ENABLE_BIT);

	// texture pass
	glViewport(0, 0, FeedbackBufferDimension, FeedbackBufferDimension );

	RenderRoomAndSpheres( time );
	glBindTexture(GL_TEXTURE_2D, FeedbackTexture);
	Render2ActualTexture(0, 0, FeedbackBufferDimension, FeedbackBufferDimension);		// scene without cube

	RenderRoomAndSpheres( time );
	
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.3f, 0.0f);
	glRotatef((float)sin(time * 0.0003) * 360.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((float)sin(time * 0.0002) * 360.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)sin(time * 0.0001) * 360.0f, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, FeedbackTexture);
	RenderObject(FeedbackCube);

	Render2ActualTexture(0, 0, FeedbackBufferDimension, FeedbackBufferDimension);		// scene with cube

	RenderRoomAndSpheres( time );
	
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.3f, 0.0f);
	glRotatef((float)sin(time * 0.0003) * 360.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((float)sin(time * 0.0002) * 360.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)sin(time * 0.0001) * 360.0f, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, FeedbackTexture);
	RenderObject(FeedbackCube);

	Render2ActualTexture(0, 0, FeedbackBufferDimension, FeedbackBufferDimension);		// scene with cube in cube
//*/
	// real pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glViewport(XSTART, YSTART, WIDTH, HEIGHT);

	RenderRoomAndSpheres( time );
	
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, 0.3f, 0.0f);
	glRotatef((float)sin(time * 0.0003) * 360.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((float)sin(time * 0.0002) * 360.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)sin(time * 0.0001) * 360.0f, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, FeedbackTexture);
	RenderObject(FeedbackCube);

	glPopAttrib();
}

void QuitFeedback()
{
	DeleteObject(&FeedbackCube);
	DeleteObject(&FeedbackSkyBox);
	DeleteObject(&FeedbackSphere);
}