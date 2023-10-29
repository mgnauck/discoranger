
 // flash.cpp
 
#include "settings.h"

const unsigned int	TIMEVALUE	= 3;

static float	flash_r;
static float	flash_g;
static float	flash_b;
static float	flash_fade;
static float	flash_speed;

void InitFlash(float r, float g, float b, float speed)
{
	flash_r		= r;
	flash_g		= g;
	flash_b		= b;

	flash_fade	= 1.0f;
	flash_speed	= speed;
}

void UpdateFlash()
{
	static unsigned long StartTime = timeGetTime();
	
	if((timeGetTime() - StartTime) > TIMEVALUE)
	{
		flash_fade -= (float)fabs(flash_speed);

		if(flash_fade < 0.0f)
			flash_fade = 0.0f;

		StartTime = timeGetTime();
	}
}

void DrawFlash()
{
	float f;

	if(flash_speed < 0.0f)
		f = 1.0f - flash_fade;
	else 
		f = flash_fade;

	if(f >= 0.01f)
	{
		glPushAttrib(GL_ENABLE_BIT);
		
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		glEnable(GL_BLEND);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glColor4f(flash_r, flash_g, flash_b, f);
		glRectf(1.0f, 1.0f, -1.0f, -1.0f);

		glPopMatrix();
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glPopAttrib();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}