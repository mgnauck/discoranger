
 // blur.cpp

#include "settings.h"

const unsigned int	TIMEVALUE	= 4;
GLuint				BlurID;
int					BlurBufferDimX,
					BlurBufferDimY;

void InitBlurTexture(int BufferDimX, int BufferDimY)
{
	BlurBufferDimX = BufferDimX;
	BlurBufferDimY = BufferDimY;
	unsigned int *Buffer = new unsigned int [BlurBufferDimX * BlurBufferDimY];
	
	glGenTextures(1, &BlurID);
	glBindTexture(GL_TEXTURE_2D, BlurID);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, BlurBufferDimX, BlurBufferDimY, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, BlurBufferDimX, BlurBufferDimY, 0, GL_RGBA, GL_UNSIGNED_BYTE, Buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	delete [] Buffer;
}

void DrawQuad(int xStart, int yStart, int xEnd, int yEnd, double BufferDimX, double BufferDimY)
{
	glBegin(GL_QUADS);

		glTexCoord2d((double)xStart / BufferDimX, (double)yStart / BufferDimY);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		
		glTexCoord2d((double)xEnd / BufferDimX, (double)yStart / BufferDimY);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glTexCoord2d((double)xEnd / BufferDimX, (double)yEnd / BufferDimY);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2d((double)xStart / BufferDimX, (double)yEnd / BufferDimY);
		glVertex3f(-1.0f, 1.0f, 0.0f);

	glEnd();		
}

void Render2ActualTexture(int xStart, int yStart, int xEnd, int yEnd)
{
	glReadBuffer(GL_BACK);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xStart, yStart, xStart, yStart, xEnd, yEnd);
}

void RadialBlur(double Intensity, double Factor, int xStart, int yStart, int xEnd, int yEnd)
{
	static unsigned long StartTime = timeGetTime();

	if((timeGetTime() - StartTime) > TIMEVALUE)
	{
		glPushAttrib(GL_TRANSFORM_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glEnable(GL_TEXTURE_2D);    
		glBindTexture(GL_TEXTURE_2D, BlurID);

		//double	x = sin(timeGetTime() * 0.0012) * 0.03,
		//			y = cos(timeGetTime() * 0.0011) * 0.03;

		for(int i=0; i<5; i++)
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glScaled(1.0 + (double)i * 0.05, 1.0 + (double)i * 0.05, 1.0 + (double)i * 0.05);
			//glTranslatef((float)x * (float)i, (float)y * (float)i, 0.0f);
			
			double temp = Intensity - (double)i * Factor;
			glColor3d(temp, temp, temp);					
			DrawQuad(xStart, yStart, xEnd, yEnd, (double)BlurBufferDimX, BlurBufferDimY);
		}

		Render2ActualTexture(xStart, yStart, xEnd, yEnd);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glPopAttrib();

		StartTime = timeGetTime();
	}
}

void MotionBlur(double Intensity, int xStart, int yStart, int xEnd, int yEnd)
{
	static unsigned long StartTime = timeGetTime();

	if((timeGetTime() - StartTime) > TIMEVALUE)
	{
		glPushAttrib(GL_TRANSFORM_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		glLoadIdentity();
		gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();		
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);    
		glBindTexture(GL_TEXTURE_2D, BlurID);

		glColor4d(1.0, 1.0, 1.0, Intensity);
		DrawQuad(xStart, yStart, xEnd, yEnd, (double)BlurBufferDimX, BlurBufferDimY);
		Render2ActualTexture(xStart, yStart, xEnd, yEnd);

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glPopAttrib();

		StartTime = timeGetTime();
	}
}