
 // add.cpp

#include "settings.h"
#include <stdio.h>
#include "blur.h"

void BlurBuffer(void *Buf, int Width, int Height)
{
	unsigned int	ofs, 
					v1, v2, v3, v4, v5, 
					r, g, b;

	for(int j=1; j<Height-1; j++) for(int i=1; i<Width-1; i++)
	{
		ofs	=	Width * j + i;

		v1	=	((unsigned int *)Buf)[ofs];
		v2	=	((unsigned int *)Buf)[ofs - 1];
		v3	=	((unsigned int *)Buf)[ofs + 1];
		v4	=	((unsigned int *)Buf)[ofs - Width];
		v5	=	((unsigned int *)Buf)[ofs + Width];

		r	=	(((v1 >> 16) & 0xff) + ((v2 >> 16) & 0xff) + ((v3 >> 16) & 0xff) +
				((v4 >> 16) & 0xff) + ((v5 >> 16) & 0xff)) / 5;
		
		g	=	(((v1 >> 8) & 0xff) + ((v2 >> 8) & 0xff) + ((v3 >> 8) & 0xff) +
				((v4 >> 8) & 0xff) + ((v5 >> 8) & 0xff)) / 5;

		b	=	((v1 & 0xff) + (v2 & 0xff) + (v3 & 0xff) + 
				(v4 & 0xff) + (v5 & 0xff)) / 5;
		
		((unsigned int *)Buf)[ofs] = (r << 16) | (g << 8) | b;
	}
}

unsigned int *LoadRAW( unsigned char *Data, int xSize, int ySize, bool palette )		// how low can you go?
{
	unsigned char	*Pal = new unsigned char [768],
					*Src = new unsigned char [xSize * ySize];
	unsigned int	*Dst = new unsigned int [xSize * ySize];
	
	if( palette ) {

		for(int i=0; i<768; i++)
			Pal[i] = Data[i];

		for(int i=0; i<xSize * ySize; i++)
			Src[i] = Data[768 + i];

	} else {		// grayscale

		for(int i=0; i<768; i++)
			Pal[i] = i / 3;

		for(int i=0; i<xSize * ySize; i++)
			Src[i] = Data[i];
	}

	for(int i=0; i<xSize * ySize; i++)
	{
		unsigned char c = (Src[i] & 0xff);
		Dst[i] =	(0xff << 24) |
					(Pal[c * 3 + 2] << 16) |
					(Pal[c * 3 + 1] << 8) |
					(Pal[c * 3 + 0]);
	}

	delete [] Src;
	delete [] Pal;

	return Dst;
}

GLuint UploadTexture(unsigned int *Data, unsigned int w, unsigned int h)
{
	unsigned int	TextureID;

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, Data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	return TextureID;
}
/*
void Render2ActualTexture(int xStart, int yStart, int xEnd, int yEnd)
{
	glReadBuffer(GL_BACK);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xStart, yStart, xStart, yStart, xEnd, yEnd);
}*/

void DrawViewportQuad()
{
	glBegin(GL_QUADS);

		glTexCoord2d(0.0, 0.0);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		
		glTexCoord2d(1.0, 0.0);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glTexCoord2d(1.0, 1.0);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2d(0.0, 1.0);
		glVertex3f(-1.0f, 1.0f, 0.0f);

	glEnd();		
}

void drawQuad( int xs, int xe, int ys, int ye, float alpha, float texX, float texY )
{
	glColor4f( alpha, alpha, alpha, 1.0f );
	//glColor4f( 1.0f, 1.0f, 1.0f, alpha);
	glBegin( GL_QUADS );

		glTexCoord2d( 0.0 * texX, 0.0 * texY );
		glVertex3i( xs, ys, 0 );
		
		glTexCoord2d( 1.0 * texX, 0.0 * texY );
		glVertex3i( xe, ys, 0 );

		glTexCoord2d( 1.0 * texX, 1.0 * texY );
		glVertex3i( xe, ye, 0 );

		glTexCoord2d( 0.0 * texX, 1.0 * texY );
		glVertex3i( xs, ye, 0 );

	glEnd();		
}


void blurTexture( int GlowBufferDimension, int GlowTextureID, float GlowStrength, int blurPassCount, int blurStepSize ) {

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, GlowBufferDimension, 0, GlowBufferDimension, -1, 1 );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvi( GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glBindTexture(GL_TEXTURE_2D, GlowTextureID);

	glBlendFunc( GL_ONE, GL_ONE );
	glEnable( GL_BLEND );

	glScissor( 0, 0, GlowBufferDimension, GlowBufferDimension );
	glEnable( GL_SCISSOR_TEST );

	int t = blurStepSize;
	for(int i=0; i<blurPassCount; i++) {
	
		glClear( GL_COLOR_BUFFER_BIT );

		drawQuad( -t, GlowBufferDimension - t, 0, GlowBufferDimension, GlowStrength, 1.0f, 1.0f );
		//Render2ActualTexture(0, 0, GlowBufferDimension, GlowBufferDimension);

		drawQuad( 0, GlowBufferDimension, -t, GlowBufferDimension - t, GlowStrength, 1.0f, 1.0f );
		//Render2ActualTexture(0, 0, GlowBufferDimension, GlowBufferDimension);

		drawQuad( t, GlowBufferDimension + t, 0, GlowBufferDimension, GlowStrength, 1.0f, 1.0f );
		//Render2ActualTexture(0, 0, GlowBufferDimension, GlowBufferDimension);
		
		drawQuad( 0, GlowBufferDimension, t, GlowBufferDimension + t, GlowStrength, 1.0f, 1.0f );
		Render2ActualTexture(0, 0, GlowBufferDimension, GlowBufferDimension);
	}

	//glScissor( 0, 0, RES_X, RES_Y );
	glDisable( GL_SCISSOR_TEST );

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glDisable(GL_BLEND);

	glColor3f( 1.0f, 1.0f, 1.0f );
}

GLuint MakeCheckerTexture(	int TextureDim, 
							unsigned int Col1, unsigned int Col2, 
							unsigned int Col3, unsigned int Col4)
{
	unsigned int *Texture	= new unsigned int [TextureDim * TextureDim];

	for(int y=0; y<2; y++) for(int x=0; x<2; x++)
	{
		for(int j=0; j<TextureDim / 2; j++) for(int i=0; i<TextureDim / 2; i++)
		{
			if(y == 0)
				if(x == 0)
					Texture[TextureDim * ((y * (TextureDim / 2)) + j) + ((x * (TextureDim / 2)) + i)] = (0xff << 24) | Col1;
				else
					Texture[TextureDim * ((y * (TextureDim / 2)) + j) + ((x * (TextureDim / 2)) + i)] = (0xff << 24) | Col2;
			else
				if(x == 0)
					Texture[TextureDim * ((y * (TextureDim / 2)) + j) + ((x * (TextureDim / 2)) + i)] = (0xff << 24) | Col3;
				else
					Texture[TextureDim * ((y * (TextureDim / 2)) + j) + ((x * (TextureDim / 2)) + i)] = (0xff << 24) | Col4;
		}
	}

	GLuint	TextureID = UploadTexture(Texture, TextureDim, TextureDim);

	delete [] Texture;

	return TextureID;
}

int Max(int a)
{
	if(a>255)
		return 255;
	else
		return a;
}

// rgb2hsv + hsv2rgb
// 3d computer graphics (alan watt, 2nd ed)
double MaxRGB(double r, double g, double b)
{
	double	max;

	if(r>g)
		max = r;
	else
		max = g;

	if(b>max)
		max = b;

	return max;
}

double MinRGB(double r, double g, double b)
{
	double	min;

	if(r<g)
		min = r;
	else
		min = g;

	if(b<min)
		min = b;

	return min;
}

void RGB2HSV(double Red, double Green, double Blue, double *Hue, double *Sat, double *Val)
{
	const double	undef = 99999.9;
	double			MaxVal, MinVal, Diff,
					RedDist, GreenDist, BlueDist;

	MaxVal	= MaxRGB(Red, Green, Blue);
	MinVal	= MinRGB(Red, Green, Blue);
	Diff	= MaxVal - MinVal;

	*Val = MaxVal;

	if(MaxVal!=0.0)
		*Sat = Diff / MaxVal;
	else
		*Sat = 0.0;

	if(*Sat==0.0)
		*Hue = undef;
	else
	{
		RedDist		= (MaxVal - Red) / Diff;
		GreenDist	= (MaxVal - Green) / Diff;
		BlueDist	= (MaxVal - Blue) / Diff;

		if(Red == MaxVal)
			*Hue = BlueDist - GreenDist;
		else
			if(Green == MaxVal)
				*Hue = 2.0 + RedDist - BlueDist;
			else
				if(Blue == MaxVal)
					*Hue = 4.0 + GreenDist - RedDist;

		*Hue *= 60.0;

		while(*Hue<0.0)
			*Hue += 360.0;
	}
}

void HSV2RGB(double Hue, double Sat, double Val, double *Red, double *Green, double *Blue)
{
	double	f, p, q, t;
	int		i;

	if(!Sat)
		*Red = *Green = *Blue = Val;
	else
	{
		if(Hue == 360.0)
			Hue = 0.0;
		
		Hue /= 60.0;
		i = (int)floor(Hue);
		f = Hue - (double)i;
		p = Val * (1.0 - Sat);
		q = Val * (1.0 - (Sat * f));
		t = Val * (1.0 - (Sat * (1.0 - f)));
		
		switch(i)
		{
			case 0: {	
						*Red	= Val;
						*Green	= t;
						*Blue	= p;
						break;
					}
			case 1: { 
						*Red	= q;
						*Green	= Val;
						*Blue	= p;
						break;
					}
			case 2: {
						*Red	= p;
						*Green	= Val;
						*Blue	= t;
						break;
					}
			case 3: { 
						*Red	= p;
						*Green	= q;
						*Blue	= Val;
						break;
					}
			case 4: {
						*Red	= t;
						*Green	= p;
						*Blue	= Val;
						break;
					}
			case 5: {
						*Red	= Val;
						*Green	= p;
						*Blue	= q;
						break;
					}

			if(*Red > 255.0) 
				*Red = 255.0;

			if(*Green > 255.0)
				*Green = 255.0;

			if(*Blue > 255.0)
				*Blue = 255.0;
		}
	}
}