
 // weight.cpp
 
#include "settings.h"

#define	GlowBufferDimension			128

const unsigned int	TextureDim		= 32,
					Col3			= 0x999999,
					Col4			= 0xaaaaaa;
GLuint				WeightTextureID,
					BackTextureID,
					GlowTextureID;

Object				BackPlane,
					DeformObject1,
					DeformObject2,
					DeformObject3;

float				def1CenterX;
float				def1CenterY;
float				def1CenterZ;
float				def2CenterX;
float				def2CenterY;
float				def2CenterZ;


void InitWeight()
{
	unsigned int	*Buffer = new unsigned int [GlowBufferDimension * GlowBufferDimension];
	
	glGenTextures(1, &GlowTextureID);
	glBindTexture(GL_TEXTURE_2D, GlowTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GlowBufferDimension, GlowBufferDimension, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	delete [] Buffer;

	Buffer = LoadRAW((unsigned char *)chess, 256, 256, true);

	for(int i=0; i<256 * 256; i++)
	{
		int		col	= Buffer[i];
		double	h, s, v,
				rd, gd, bd;
		int		r = (col >> 16) & 0xff,
				g = (col >> 8) & 0xff,
				b = col & 0xff;

		RGB2HSV((double)r, (double)g, (double)b, &h, &s, &v);
		//HSV2RGB((double)h + 200.0, (double)s + 0.1, (double)v - 60.0, &rd, &gd, &bd);
		HSV2RGB((double)h + 70.0, (double)s + 0.1, (double)v - 40.0, &rd, &gd, &bd);

		r = (int)rd;
		g = (int)gd;
		b = (int)bd;
		
		Buffer[i] = (r << 16) | (g << 8) | b;
	}

	WeightTextureID		= UploadTexture( Buffer, 256, 256 );
	BackTextureID		= MakeCheckerTexture( TextureDim, Col3, Col3, Col4, Col4 );

	GenSphere(&BackPlane, 30, 30, 0.9f, 15.0f, 15.0f);

	GenCube( &DeformObject1, 20, 20, 0.4f, 2.0f, 2.0f );
	GenCube( &DeformObject2, 20, 20, 0.4f, 2.0f, 2.0f );
	GenCube( &DeformObject3, 20, 20, 0.4f, 2.0f, 2.0f );

	delete [] Buffer;
}

void SetupTnL(unsigned long time) {

	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	float radius	= (float)fabs( sin(time * 0.00009 ) ) * 0.2f  + 0.5f;
	float a			= (float)sin(time * 0.000303) * 360.0f;
	float b			= (float)cos(time * 0.000402) * 360.0f;
	float mx		= radius * (float)(cos( a * PI / 180.0 ) * sin( b * PI / 180.0 ));
	float my		= radius * (float)(sin( a * PI / 180.0 ) * sin( b * PI / 180.0 ));
	float mz		= radius * (float)cos( b * PI / 180.0 );

	GLfloat AmbientLight[]	= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat DiffuseLight[]	= { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat SpecularLight[]	= { 2.8f, 1.8f, 0.8f, 1.0f };
	GLfloat LightPos[]		= { mx, my, mz,	1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularLight);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, WIDTH / (double)HEIGHT, 0.1, 3000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);

	radius	= (float)fabs(sin(time * 0.00202)) * 0.3f + 0.4f;
	a		= (float)sin(time * 0.000302) * 360.0f;
	b		= (float)cos(time * 0.000401) * 360.0f;
	mx		= radius * (float)(cos( a * PI / 180.0 ) * sin( b * PI / 180.0 ));
	my		= radius * (float)(sin( a * PI / 180.0 ) * sin( b * PI / 180.0 ));
	mz		= radius * (float)cos( b * PI / 180.0 );

	gluLookAt(	mx, 
				my, 
				mz, 
				0.0, 0.0, 0.0, 
				sin(0.02 * time * PI / 90) * 0.9, 
				1.0, 
				cos(0.02 * time * PI / 90) * 0.9);
}

void Deform( float mx, float my, float mz, float radius, Object* realObj, Object* srcObj, Object* dstObj ) {
	
	for( int i=0; i<srcObj->NumVertices; i++ ) {

		float x = srcObj->Vertices[i].x;
		float y = srcObj->Vertices[i].y;
		float z = srcObj->Vertices[i].z;

		float t = ( x - mx ) * ( x - mx ) + ( y - my ) * ( y - my ) + ( z - mz ) * ( z - mz );

		// vertex innerhalb kugel -> deform
		if( t <= ( radius * radius ) ) {
			float tx	= ( x - mx );
			float ty	= ( y - my );
			float tz	= ( z - mz );
			float l		=  1.0f / sqrt( tx * tx + ty * ty + tz * tz );
			tx *= l;
			ty *= l;
			tz *= l;
			tx *= radius;
			ty *= radius;
			tz *= radius;

			dstObj->Vertices[i].x = mx + tx;
			dstObj->Vertices[i].y = my + ty;
			dstObj->Vertices[i].z = mz + tz;

			if( realObj->Vertices[i].x < dstObj->Vertices[i].x )
				dstObj->Vertices[i].x = realObj->Vertices[i].x;
			if( realObj->Vertices[i].y < dstObj->Vertices[i].y )
				dstObj->Vertices[i].y = realObj->Vertices[i].y;
			if( realObj->Vertices[i].z < dstObj->Vertices[i].z )
				dstObj->Vertices[i].z = realObj->Vertices[i].z;

		} else {

			dstObj->Vertices[i].x = x;
			dstObj->Vertices[i].y = y;
			dstObj->Vertices[i].z = z;
		}

	}
}

void CalcObject( unsigned long time ) {

	float r			= 0.35f;

	float radius	= 0.4f;
	float a			= (float)sin(time * 0.0002) * 360.0f;
	float b			= (float)cos(time * 0.0003) * 360.0f;
	float mx		= radius * (float)(cos( a * PI / 180.0 ) * sin( b * PI / 180.0 ));
	float my		= radius * (float)(sin( a * PI / 180.0 ) * sin( b * PI / 180.0 ));
	float mz		= radius * (float)cos( b * PI / 180.0 );

	def1CenterX = mx;
	def1CenterY = my;
	def1CenterZ = mz;

	Deform( mx, my, mz, r, &DeformObject1, &DeformObject1, &DeformObject2 );


	a			= (float)cos(time * 0.0003) * 360.0f;
	b			= (float)sin(time * 0.0002) * 360.0f;
	mx			= radius * (float)(cos( a * PI / 180.0 ) * sin( b * PI / 180.0 ));
	my			= radius * (float)(sin( a * PI / 180.0 ) * sin( b * PI / 180.0 ));
	mz			= radius * (float)cos( b * PI / 180.0 );

	def2CenterX = mx;
	def2CenterY = my;
	def2CenterZ = mz;

	Deform( mx, my, mz, r, &DeformObject1, &DeformObject2, &DeformObject3 );

	CalcNormals( &DeformObject3 );
}

void RenderBackground(unsigned long time) {

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, BackTextureID);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);

	glCullFace(GL_FRONT);

	GLfloat AmbientMat[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat DiffuseMat[]	= { 1.2f, 1.2f, 1.2f, 1.0f };
	GLfloat SpecularMat[]	= { 3.8f, 3.8f, 3.8f, 1.0f };

	glMaterialfv( GL_BACK, GL_AMBIENT, AmbientMat );
	glMaterialfv( GL_BACK, GL_DIFFUSE, DiffuseMat );
	glMaterialfv( GL_BACK, GL_SPECULAR, SpecularMat );
	glMaterialf( GL_BACK, GL_SHININESS, 20.0f);
	
	glRotatef((float)sin(time * 0.0001) * 360.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((float)sin(time * 0.0002) * 360.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)sin(time * 0.0001) * 360.0f, 0.0f, 0.0f, 1.0f);
	RenderObject(BackPlane);
	
	glCullFace(GL_BACK);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, false);

	glPopMatrix();
}

void renderDeformCube() {

	GLfloat AmbientMat[]	= { 0.2f, 0.1f, 0.1f, 1.0f };
	GLfloat DiffuseMat[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat SpecularMat[]	= { 3.0f, 2.8f, 2.8f, 1.0f };

	glMaterialfv( GL_FRONT, GL_AMBIENT, AmbientMat );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, DiffuseMat );
	glMaterialfv( GL_FRONT, GL_SPECULAR, SpecularMat );
	glMaterialf( GL_FRONT, GL_SHININESS, 90.0f);

	glBindTexture(GL_TEXTURE_2D, WeightTextureID);
	RenderObject(DeformObject3);
}

void DoWeight(unsigned long time) {

	glColor3f( 1.0f, 1.0f, 1.0f );
	CalcObject( time );

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushAttrib(GL_ENABLE_BIT);

	SetupTnL(time);

	// texture pass
	glViewport(0, 0, GlowBufferDimension, GlowBufferDimension);

	//glBindTexture(GL_TEXTURE_2D, WeightTextureID);
	//RenderObject(DeformObject3);
	renderDeformCube();

	glBindTexture(GL_TEXTURE_2D, GlowTextureID);
	Render2ActualTexture(0, 0, GlowBufferDimension, GlowBufferDimension);

	// blur texture
	blurTexture( GlowBufferDimension, GlowTextureID, 0.34f, 4, 1 );
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

//*/	
	// render pass
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(XSTART, YSTART, WIDTH, HEIGHT);

	//glBindTexture(GL_TEXTURE_2D, WeightTextureID);
	//RenderObject(DeformObject3);
	renderDeformCube();

	RenderBackground(time);
//*/

	// render glow
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, GlowTextureID);
	DrawViewportQuad();
	
	glDisable(GL_BLEND);
//*/
	glPopAttrib();
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	GLfloat SpecularMat[]	= { 0.0f, 0.0f, 0.0f, 1.0f };
	glMaterialfv( GL_FRONT, GL_SPECULAR, SpecularMat );
}

void QuitWeight()
{
	DeleteObject(&DeformObject3);
	DeleteObject(&DeformObject2);
	DeleteObject(&DeformObject1);
	DeleteObject(&BackPlane);
}