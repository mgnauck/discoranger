
 // main.cpp

#include "settings.h"
#include <assert.h>

void InitGL()
{
	glViewport(XSTART, YSTART, WIDTH, HEIGHT);

	glEnable(GL_NORMALIZE);
	
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDepthMask(true);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);

	glLineWidth(0.05f);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void Init()
{
	InitGL();
	InitFont();
	InitText();

	DoTextLoadingScreen();
	glFinish();

#ifdef _WIN32_
	SwapBuffers(hDC);
#endif

	InitFlowers();
	InitWobble();
	InitCubic();
	InitFeedback();
	InitSpiral();
	InitSoap();
	InitWeight();
	InitTunnel();
	InitGreets();
	
#ifdef _MUSIC_
	InitMusic("minifmodkotztmichtotalan");
	DoMusic();
#endif
}

void Display()
{
	static unsigned long	StartTime			= timeGetTime();
	unsigned long			Time				= ( timeGetTime() - StartTime );
	static unsigned long	TimeDelta			= 0;
	static int				EffectNumber		= 0,
							TextNumber			= 0,
							EventCount			= 0,
							FlashCount			= 0,
							TextCount			= 0,
							QuitCnt				= 0;

#define RUN_INTRO
#ifndef RUN_INTRO

	//DoGreets(Time);
	DoSoap(Time);
	//DoWeight(Time);
	//DoTextEnd();

#else

	if((Time >= EventList[EventCount].StartTime) && (Time < EventList[EventCount].EndTime))
	{
		EffectNumber = EventList[EventCount].EffectNumber;

		if(EventList[EventCount].TimeDelta)
			TimeDelta = (EventList[EventCount].StartTime + ( 7000 + ( rand() % 4000 ) ) );
		else
			TimeDelta = 0;

		if(EventCount < MaxEvents)
			EventCount++;
	}

	unsigned long	EffectTime = (Time + 11000) - TimeDelta;

	switch(EffectNumber)
	{
		case	1:	DoSpiral(EffectTime); break;
		case	2:	DoCubic(EffectTime); break;
		case	3:	DoWobble(EffectTime); break;
		case	4:	DoWeight(EffectTime); break;
		case	5:	DoFeedback(EffectTime); break;
		case	6:	DoSoap(EffectTime + 11000); break;
		case	7:	DoFlowers(EffectTime); break;
		case	8:	DoTunnel(EffectTime); break;
		case	9:	DoGreets(EffectTime); break;
		case	0:	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT ); break;
	}

	if((Time >= 29450) && (Time < 30290))
		DoTextBreak();

/*	if((Time >= 93100) && (Time < 96700))
	{
		// irgendwas geiles hier.. ??
	}*/
	
	if(Time > 150150)
		DoTextEnd();

	if(Time >= FlashEventList[FlashCount].StartTime)
	{
		InitFlash(	FlashEventList[FlashCount].r, 
					FlashEventList[FlashCount].g, 
					FlashEventList[FlashCount].b, 
					FlashEventList[FlashCount].Speed);
		
		if(FlashCount < MaxFlashEvents)
			FlashCount++;
	}
	
	UpdateFlash();
	DrawFlash();

	if(Time >= TextEventList[TextCount].StartTime)
	{
		TextNumber = TextEventList[TextCount].TextNumber;

		if(TextCount < MaxTextEvents)
			TextCount++;
	}

	switch(TextNumber)
	{
		case	1:	DoTextUnik(Time); break;
		case	2:	DoTextTitle(Time); break;
		case	3:	DoTextNubeatmachine(Time); break;
		case	4:	DoTextPunkRock(Time); break;
		case	5:	DoTextMilk(Time); break;
		case	6:	DoTextElectronic(Time); break;
		case	7:	DoTextWould(Time); break;
		case	8:	DoTextIsotonics(Time); break;
		case	9:	DoTextSugar(Time); break;
		case	10:	DoTextMom(Time); break;
		case	11: break;
	}

	if((Time > 93110) && (QuitCnt == 0))
	{
		QuitSoap();
		QuitCnt++;
	}	
	if((Time > 95010) && (QuitCnt == 1))
	{
		QuitCubic();
		QuitCnt++;
	}
	if((Time > 95510) && (QuitCnt == 2))
	{
		QuitSpiral();
		QuitCnt++;
	}
	if((Time > 95960) && (QuitCnt == 3))
	{
		QuitFeedback();
		QuitCnt++;
	}
	if((Time > 96710) && (QuitCnt == 4))
	{
		QuitWeight();
		QuitCnt++;
	}
	if((Time > 113010) && (QuitCnt == 5))
	{
		QuitWobble();
		QuitCnt++;
	}
	if((Time > 128110) && (QuitCnt == 6))
	{
		QuitTunnel();
		QuitCnt++;
	}
	
	//glFlush();

#ifdef _MUSIC_
	if(Time > 152050)
		StopMusic();
#endif

	if(Time > 158000)
		DemoEnd = true;

#endif
#undef RUN_INTRO

	// da color_material an aber nur erster+letzter effekt benutzt vertex color
	// kann man jetzt schön helligkeit der anderen effekte regeln :)
	glColor3f(1.0f, 1.0f, 1.0f);

//	assert(glGetError() == GL_NO_ERROR);
}

void Quit()
{
#ifdef _MUSIC_
	QuitMusic();
#endif
	QuitGreets();
	QuitFlowers();
	QuitText();
	QuitFont();
}