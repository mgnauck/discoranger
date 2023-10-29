
 // soap.cpp
 
#include "settings.h"

const float			Depth			= 1.5f;
const unsigned int	DimensionX		= 120,	//120
					DimensionY		= 88,	//88
					TextureDim		= 256,
					Col1			= 0x489f48,
					Col2			= 0xffffff;
Object				SoapPlane1,	SoapPlane2;
unsigned int		*ShapeBuffer[2];
GLuint				SoapTextureID,
					SoapPlane1Index, SoapPlane2Index;

void InitSoap()
{
	GenPlane(	&SoapPlane1,
				DimensionX, DimensionY,
				DimensionX / 10.0f, DimensionY / 10.0f,
				6.0f, 6.0f);

	GenPlane(	&SoapPlane2,
				DimensionX, DimensionY,
				DimensionX / 12.0f, DimensionY / 12.0f,
				6.0f, 6.0f);

	ShapeBuffer[0] = new unsigned int [DimensionX * DimensionY];
	ShapeBuffer[1] = LoadRAW((unsigned char *)shape, 120, 88, true);

	Text2Buffer("be my", ShapeBuffer[0], DimensionX, DimensionY, 27, 16, true);
	Text2Buffer("disco", ShapeBuffer[0], DimensionX, DimensionY, 29, 37, false);

	// blur text
	BlurBuffer(ShapeBuffer[0], DimensionX, DimensionY);
	// blur star
	BlurBuffer(ShapeBuffer[1], DimensionX, DimensionY);
	BlurBuffer(ShapeBuffer[1], DimensionX, DimensionY);

	SoapTextureID = MakeCheckerTexture(TextureDim, Col1, Col2, Col2, Col1);

	float HeightMul = (float)Depth / 768.0f;
	for(int i=0; i<DimensionX * DimensionY; i++)
	{
		SoapPlane1.Vertices[i].z = (float)(ShapeBuffer[0][i] & 0xff) * HeightMul;
		SoapPlane2.Vertices[i].z = (float)(ShapeBuffer[1][i] & 0xff) * -HeightMul;
	}

	CalcNormals(&SoapPlane1);
	CalcNormals(&SoapPlane2);

	SoapPlane1Index = glGenLists(1);
	glNewList(SoapPlane1Index, GL_COMPILE);
		RenderObjectImmediate(SoapPlane1);
	glEndList();

	SoapPlane2Index = glGenLists(1);
	glNewList(SoapPlane2Index, GL_COMPILE);
		RenderObjectImmediate(SoapPlane2);
	glEndList();

	delete [] ShapeBuffer[1];
	delete [] ShapeBuffer[0];
}

void DoSoap(unsigned long time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat AmbientLight[]	= { 0.05f, 0.05f, 0.05f, 1.0f };
	GLfloat DiffuseLight[]	= { 0.9f, 0.8f, 0.8f, 1.0f };
	GLfloat LightPos[]		= { (float)sin(time * 0.001) * 3.0f, 
								-(float)cos(time * 0.001) * 2.0f, 
								3.6f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, DiffuseLight);
	glLightf(GL_LIGHT0, GL_SHININESS, 50.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.01f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, SoapTextureID);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(80.0, WIDTH / (double)HEIGHT, 0.1, 1000.0);
	gluLookAt(	2.8 * sin((float)time * 0.002), 
				2.8 * cos((float)time * 0.002), 
				sin(time * 0.001) * 0.7 + 2.5, 
				2.5 * sin((float)time * 0.002),
				2.5 * cos((float)time * 0.002),
				0.0, 
				sin(0.02 * time * PI / 90) * 0.4, 
				1.0, 
				cos(0.02 * time * PI / 90) * 0.4);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glCullFace(GL_FRONT);	

	double Height = sin(time * 0.0006) * 1.0 + sin(time * 0.0005) * 0.3;

	if(Height>0.0f)
	{
		glScalef(1.2f, -1.4f, (float)fabs(Height));
		glCallList(SoapPlane1Index);
	} else
	{
		glScalef(1.2f, -1.3f, (float)fabs(Height));
		glCallList(SoapPlane2Index);
	}

	glCullFace(GL_BACK);

	glPopAttrib();
}

void QuitSoap()
{
	DeleteObject(&SoapPlane1);
	DeleteObject(&SoapPlane2);
}