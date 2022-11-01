
#include "settings.h"

const unsigned int GlowSize = 256;

GLuint	BackID;
GLuint	ObjID;
GLuint	GlowID;
GLuint	ObjListIndex;
Object	FaintPlane;
Object	Obj;

void InitGreets() {

	unsigned int *buffer;

	buffer	= LoadRAW((unsigned char *)faint, 64, 64, false);
	BackID	= UploadTexture( buffer, 64, 64 );
	
	delete [] buffer;

	buffer = LoadRAW( (unsigned char *)star, 256, 256, true );
	ObjID	= UploadTexture( buffer, 256, 256 );
	
	delete [] buffer;

	buffer = new unsigned int [ GlowSize * GlowSize ];

	glGenTextures( 1, &GlowID );
	glBindTexture( GL_TEXTURE_2D, GlowID );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, GlowSize, GlowSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	delete [] buffer;

	GenPlane( &FaintPlane, 50, 50, 3.0f, 3.0f, 35.0f, 30.0f );
	GenPQKnot( &Obj, 40, 200, 1.0f, 1.0f, 3.0f, 2.0f, 0.18f );

	ObjListIndex = glGenLists(1);
	glNewList( ObjListIndex, GL_COMPILE );
		RenderObjectImmediate( Obj );
	glEndList();
}

void setupTransform( unsigned long time ) {

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0, WIDTH / (double)HEIGHT, 0.1, 10.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void setupLighting( unsigned long time ) {

	float lx = (float)( sin( time * 0.001 ) ) * 1.0f;
	float ly = (float)( cos( time * 0.003 ) ) * 0.4f;
	float lz = (float)fabs( sin( time * 0.001 ) ) * -0.5f - 2.0f;
	GLfloat amb[]	= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diff[]	= { 0.8f, 0.7f, 0.7f, 1.0f };
	GLfloat spec[]	= { 0.9f, 0.8f, 0.8f, 1.0f };
	GLfloat pos[]	= { lx, ly, lz, 1.0f };

	glLightfv( GL_LIGHT0, GL_AMBIENT, amb );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diff );
	glLightfv( GL_LIGHT0, GL_SPECULAR, spec );
	glLightfv( GL_LIGHT0, GL_POSITION, pos );	
}

void setupCamera( unsigned long time ) {

	double px = sin( time * 0.002 ) * 0.2f;
	double py = cos( time * 0.003 ) * 0.1f;
	double pz = fabs( sin( time * 0.001 ) ) * -0.1 - 2.0;
	gluLookAt( px, py, pz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
}

void renderBackPlane( unsigned long time ) {

	glDisable( GL_CULL_FACE );
	glBindTexture( GL_TEXTURE_2D, BackID );

	GLfloat amb[]	= { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diff[]	= { 0.9f, 0.7f, 0.3f, 1.0f };
	GLfloat spec[]	= { 3.8f, 0.8f, 1.8f, 1.0f };

	glMaterialfv( GL_FRONT, GL_AMBIENT, amb );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff );
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec );
	glMaterialf( GL_FRONT, GL_SHININESS, 110.0f);

	glPushMatrix();

	glScalef( 3.4f, 2.4f, 1.0f );
	RenderObject( FaintPlane );

	glPopMatrix();
	glEnable( GL_CULL_FACE );
}

void renderObj( unsigned long time ) {

	GLfloat amb[]	= { 0.2f, 0.2f, 0.0f, 1.0f };
	GLfloat diff[]	= { 0.85f, 0.0f, 0.0f, 1.0f };
	GLfloat spec[]	= { 1.8f, 1.8f, 1.8f, 1.0f };

	glMaterialfv( GL_FRONT, GL_AMBIENT, amb );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff );
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec );
	glMaterialf( GL_FRONT, GL_SHININESS, 100.0f);

	float px = (float)sin( time * 0.002 ) * 0.64f;
	float py = (float)cos( time * 0.001 ) * 0.93f;
	float pz = (float)fabs( sin( time * 0.001 ) ) * -0.4 - 0.4;

	glPushMatrix();

	glRotatef( px * 113.0f, 1.0, 0.0, 0.0 );
	glRotatef( py * 126.0f, 0.0, 1.0, 0.0 );
	glRotatef( pz * 154.0f, 0.0, 0.0, 1.0 );

	glScalef( 0.4f, 0.4f, 0.4f );

	glTranslatef( px * 2.2f, py * 2.0f, pz * 2.2f );

	glDisable( GL_TEXTURE_2D );

	//RenderObject( Obj );
	glCallList( ObjListIndex );

	glEnable( GL_TEXTURE_2D );

	glPopMatrix();
}

void renderObjBlack( unsigned long time ) {

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	glEnable( GL_COLOR_MATERIAL );
	glColor3f( 0.0f, 0.0f, 0.0f );

	renderObj( time );

	glColor3f( 1.0f, 1.0f, 1.0f );
	glDisable( GL_COLOR_MATERIAL );
	glEnable( GL_LIGHTING );
	glEnable( GL_TEXTURE_2D );
}

void renderObjReal( unsigned long time ) {

	glBindTexture(GL_TEXTURE_2D, ObjID );
	renderObj( time );
}

void DoGreets( unsigned long time ) {

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_NORMALIZE );
	glDisable( GL_COLOR_MATERIAL );
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	setupTransform( time );
	setupLighting( time );
	setupCamera( time );
	
	// texture pass
	glViewport( 0, 0, GlowSize, GlowSize );
	renderBackPlane( time );
	renderObjBlack( time );

	glBindTexture( GL_TEXTURE_2D, GlowID );
	Render2ActualTexture( 0, 0, GlowSize, GlowSize );

	blurTexture( GlowSize, GlowID, 0.32f, 7, 1 );
	glEnable( GL_LIGHTING );
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	//*/

	// real pass
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  );
	glViewport( XSTART, YSTART, WIDTH, HEIGHT );

	renderBackPlane( time );
	renderObjReal( time );

	// glow pass
	glDisable( GL_LIGHTING );

	glBlendFunc( GL_ONE, GL_ONE );
	glEnable( GL_BLEND );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glBindTexture( GL_TEXTURE_2D, GlowID );
	DrawViewportQuad();
	
	glDisable( GL_BLEND );
	//*/
	
	glDisable( GL_TEXTURE_2D );
	glEnable( GL_COLOR_MATERIAL );
}

void QuitGreets() {

	DeleteObject( &Obj );
	DeleteObject( &FaintPlane );
}
