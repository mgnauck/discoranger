
 // text.cpp
 
#include "settings.h"

typedef struct
{
	GLuint		TexID;
	bool		Loaded;
} TexStruct;

TexStruct		TextID[17];				// Text Texture IDs
unsigned int	*Temp;

void InitText()
{
	Temp	= new unsigned int [512 * 256];

	Text2Buffer("are we", Temp, 512, 256, 0, 0, true);
	Text2Buffer("loading?", Temp, 512, 256, 0, 26, false);
	BlurBuffer(Temp, 512, 256); BlurBuffer(Temp, 512, 256);
	TextID[0].TexID = UploadTexture(Temp, 512, 256);
	
	Text2Buffer("break.", Temp, 512, 256, 0, 0, true);
	TextID[1].TexID = UploadTexture(Temp, 512, 256);

	//Text2Buffer("got the", Temp, 512, 256, 0, 0, true);
	//Text2Buffer("beauty?", Temp, 512, 256, 0, 30, false);
	Text2Buffer("end of", Temp, 512, 256, 0, 0, true);
	Text2Buffer("disco..", Temp, 512, 256, 0, 30, false);
	TextID[2].TexID = UploadTexture(Temp, 512, 256);
	
	Text2Buffer("unik..", Temp, 512, 256, 0, 0, true);
	TextID[3].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("..in the mix", Temp, 512, 256, 0, 0, true);
	TextID[4].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("discoranger", Temp, 512, 256, 0, 14, true);
	//Text2Buffer("doublemint", Temp, 512, 256, 3, 28, false);
	TextID[5].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("twinkle twinkle little pimp", Temp, 512, 256, 0, 0, true);
	TextID[6].TexID = UploadTexture(Temp, 512, 256);
   
	Text2Buffer("shake ya nuts", Temp, 512, 256, 0, 0, true);
	TextID[7].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("vector massage", Temp, 512, 256, 0, 0, true);
	TextID[8].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("electronic performers", Temp, 512, 256, 0, 0, true);
	TextID[9].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("audio", Temp, 512, 256, 0, 0, true);
	Text2Buffer(" dhg", Temp, 512, 256, 0, 26, false);
	TextID[10].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("visuals", Temp, 512, 256, 0, 0, true);
	Text2Buffer(" warp", Temp, 512, 256, 0, 24, false);
	TextID[11].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("would you?", Temp, 512, 256, 0, 0, true);
	TextID[12].TexID = UploadTexture(Temp, 512, 256);

	delete [] Temp;
	
	Temp	= new unsigned int [1024 * 128];

	Text2Buffer("bm cocoon exceed fr fs hcr inf lunix mfx nce replay", Temp, 1024, 128, 0, 20, true);
	TextID[13].TexID = UploadTexture(Temp, 1024, 128);
    
	Text2Buffer("ap cdx dcs eph excess fkt hjb kolor suburban tpolm", Temp, 1024, 128, 0, 0, true);
	TextID[14].TexID = UploadTexture(Temp, 1024, 128);

	delete [] Temp;

	Temp	= new unsigned int [512 * 256];

	Text2Buffer("the sugar tunnel", Temp, 512, 256, 0, 0, true);
	TextID[15].TexID = UploadTexture(Temp, 512, 256);

	Text2Buffer("your fav kids in town!", Temp, 512, 256, 0, 0, true);
	TextID[16].TexID = UploadTexture(Temp, 512, 256);

	for(int i=0; i<16; i++)
		TextID[i].Loaded = true;

	delete [] Temp;
}

void ShowText(	unsigned int TexID, bool BlendFlag, float Transparency,
					float ScaleX, float ScaleY,	float PosX, float PosY)
{
	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, RES_X, RES_Y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if(BlendFlag)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TexID);
	glTranslatef(PosX, PosY, 0.0f);
	glScalef(ScaleX, ScaleY, 1.0f);

	glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, Transparency);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);
	glEnd();

	glPopAttrib();
	
	glViewport(XSTART, YSTART, WIDTH, HEIGHT);
}

void DoTextLoadingScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShowText(TextID[0].TexID, false, 1.0f, 0.7f, 0.35f, 0.02f, 0.85f);
}

void DoTextBreak()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShowText(TextID[1].TexID, false, 1.0f, 1.4f, 0.7f, 0.36f, 0.45f);
}

void DoTextEnd()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShowText(TextID[2].TexID, false, 1.0f, 1.2f, 0.6f, 0.36f, 0.40f);
}

void DoTextUnik(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			x			= 0.0f, 
							y			= 0.5f,
							delay		= 1.5f;

	if(TextID[0].Loaded)
	{
		glDeleteTextures(1, &TextID[0].TexID);
		TextID[0].Loaded = false;
	}

	ShowText(TextID[3].TexID, true, 1.0f, 2.0f, 1.0f, 0.014f + x, 0.085f - y);
	ShowText(TextID[4].TexID, true, 1.0f, 1.1f, 0.55f, 0.644f - x, 0.82f + y + delay);

	if(time - TempTime > 5)
	{
		if(time > 1000)
		{
			if(y > 0.0f)
				y -= 0.005f;
			if(delay > 0.0f)
				delay -= 0.005f;
		}

		if(time > 5100)
		{
			if(x < 3.0f)
				x += 0.015f;
		}

		TempTime = time;
	}
}

void DoTextTitle(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			f			= 1.0f;

	if(TextID[3].Loaded && TextID[4].Loaded)
	{
		glDeleteTextures(1, &TextID[3].TexID);
		TextID[3].Loaded = false;
		glDeleteTextures(1, &TextID[4].TexID);
		TextID[4].Loaded = false;
	}

	if(time < 14000)
		ShowText(TextID[5].TexID, true, 1.0f - f, 2.0f, 1.0f, 0.175f, 0.38f);

	if(time - TempTime > 5)
	{
		if(time > 7000)
		{
			if(f > 0.0f)
				f -= 0.005f;
		}

		TempTime = time;
	}
}

void DoTextNubeatmachine(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			y			= 0.5f,
							x			= 0.0f,
							f			= 0.0f;

	if(TextID[5].Loaded)
	{
		glDeleteTextures(1, &TextID[5].TexID);
		TextID[5].Loaded = false;
	}

	ShowText(TextID[6].TexID, true, 1.0f, 1.1f, 0.55f, 0.01f + x, 0.115f - y);

	if(time - TempTime > 5)
	{
		if(time > 15500)
		{
			if(y > 0.0f)
				y -= 0.0075f;
		}

		if(time > 25500)
		{
			if(x < 5.0f)
				x += 0.015f;
		}

		TempTime = time;
	}
}

void DoTextPunkRock(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			x			= 0.6f,
							y			= 0.027f;

	if(TextID[6].Loaded)
	{
		glDeleteTextures(1, &TextID[6].TexID);
		TextID[6].Loaded = false;
	}

	ShowText(TextID[7].TexID, true, 1.0f, 1.1f, 0.55f, 0.74f + x, 0.775f + y);

	if(time - TempTime > 5)
	{
		if(time > 31000)
		{
			if(x > -0.175f)
				x -= 0.01f;
		}

		if(time > 40000)
		{
			if(y < 3.0f)
				y += 0.01f;
		}

		TempTime = time;
	}
}

void DoTextMilk(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			x			= 2.0f;

	if(TextID[7].Loaded && TextID[1].Loaded)
	{
		glDeleteTextures(1, &TextID[7].TexID);
		TextID[7].Loaded = false;

		glDeleteTextures(1, &TextID[1].TexID);
		TextID[1].Loaded = false;
	}

	ShowText(TextID[8].TexID, true, 1.0f, 1.1f, 0.55f, 0.041f + x, 0.81f);

	if(time - TempTime > 5)
	{
		if((time > 44000) && (time < 54000))
		{
			if(x > 0.0f)
				x -= 0.01f;
		}

		if(time > 54000)
		{
			if(x > -2.0f)
				x -= 0.01f;
		}

		TempTime = time;
	}
}

void DoTextElectronic(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			x			= 0.5f,
							x2			= 0.0f,
							y			= 0.5f;

	if(TextID[8].Loaded)
	{
		glDeleteTextures(1, &TextID[8].TexID);
		TextID[8].Loaded = false;
	}

	ShowText(TextID[9 ].TexID, true, 1.0f, 1.1f, 0.55f, 0.18f - x2, 0.115f - y);
	ShowText(TextID[10].TexID, true, 1.0f, 1.1f, 0.55f, 0.0136f - x + x2, 0.75f);
	ShowText(TextID[11].TexID, true, 1.0f, 1.1f, 0.55f, 0.78f + x + x2, 0.75f);

	if(time - TempTime > 5)
	{
		if(time > 62000)
		{
			if(x > 0.0f)
				x -= 0.01f;

			if(y > 0.0f)
				y -= 0.01f;
		}

		if(time > 79000)
		{
			if(x2 < 3.0f)
				x2 += 0.02f;
		}

		TempTime = time;
	}
}

void DoTextWould(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			x			= 0.5f;

	if(TextID[9].Loaded && TextID[10].Loaded && TextID[11].Loaded)
	{
		glDeleteTextures(1, &TextID[9].TexID);
		TextID[9].Loaded = false;
		glDeleteTextures(1, &TextID[10].TexID);
		TextID[10].Loaded = false;
		glDeleteTextures(1, &TextID[11].TexID);
		TextID[11].Loaded = false;
	}

	ShowText(TextID[12].TexID, true, 1.0f, 1.1f, 0.55f, 0.63f + x, 0.81f);

	if(time - TempTime > 5)
	{
		if((time > 82500) && (time < 91000))
		{
			if(x > 0.0f)
				x -= 0.01f;
		}

		if(time > 91000)
		{
			if(x < 0.6f)
				x += 0.01f;
		}

		TempTime = time;
	}
}

void DoTextIsotonics(unsigned long time)
{
	// grüsse
	// 3state bm cocoon exceed fr hcr inf lunix mfx moppi rpl soopadoopa tpolm
	// ap cdx dcs excess fs hjb kolor mawi mindforce nesnausk! skp suburban 

	static unsigned long	TempTime	= time;
	static float			x			= 2.7f;

	if(TextID[12].Loaded)
	{
		glDeleteTextures(1, &TextID[12].TexID);
		TextID[12].Loaded = false;
	}

	ShowText(TextID[13].TexID, true, 1.0f, 2.2f, 0.275f, -1.5f + x, 0.08f);
	ShowText(TextID[14].TexID, true, 1.0f, 2.2f, 0.275f, 1.0f - x, 0.8f);

	if(time - TempTime > 5)
	{
		if(time > 99200)
			if(x > -3.3f)
				x -= 0.008f;

		TempTime = time;
	}
}

void DoTextSugar(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			x			= 1.0f,
							y			= 0.0f;

	if(TextID[13].Loaded && TextID[14].Loaded)
	{
		glDeleteTextures(1, &TextID[13].TexID);
		TextID[13].Loaded = false;
		glDeleteTextures(1, &TextID[14].TexID);
		TextID[14].Loaded = false;
	}

	ShowText(TextID[15].TexID, true, 1.0f, 1.1f, 0.55f, 0.62f + x, 0.12f + y);

	if(time - TempTime > 5)
	{
		if((time > 114000) && (time < 126000))
		{
			if(x > -0.1f)
				x -= 0.02f;
		}

		if(time > 126000)
		{
			if(y > -1.0f)
				y -= 0.01f;
		}

		TempTime = time;
	}
}

void DoTextMom(unsigned long time)
{
	static unsigned long	TempTime	= time;
	static float			x			= 1.0f;

	if(TextID[15].Loaded)
	{
		glDeleteTextures(1, &TextID[15].TexID);
		TextID[15].Loaded = false;
	}

	ShowText(TextID[16].TexID, true, 1.0f, 1.1f, 0.55f, 0.02f + x, 0.81f);

	if(time - TempTime > 5)
	{
		// 128999
		if(time < 154000)
		{
			if(x > -2.0f)
				x -= 0.005f;
		}

		TempTime = time;
	}
}

void QuitText()
{
	// bla..
	if(TextID[16].Loaded)
	{
		glDeleteTextures(1, &TextID[16].TexID);
		TextID[16].Loaded = false;
	}
}