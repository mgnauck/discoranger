
 // timing.h

#ifndef __TIMING_H
#define __TIMING_H

struct Event
{
	unsigned long	StartTime;
	unsigned long	EndTime;
	int				EffectNumber;
	bool			TimeDelta;
};

struct Flash
{
	unsigned long	StartTime;
	float			r, g, b,
					Speed;
};

struct Text
{
	unsigned long	StartTime;
	int				TextNumber;
};


const char	SPIRAL						= 1;
const char	CUBIC						= 2;
const char	WOBBLE						= 3;
const char	WEIGHT						= 4;
const char	FEEDBACK					= 5;
const char	SOAP						= 6;
const char	FLOWERS						= 7;
const char	TUNNEL						= 8;
const char	GREETS						= 9;

const int	MaxEvents					= 27;
const Event	EventList[MaxEvents]		= {	

	{ 0000,		14000,	SPIRAL,		0},
	{ 14000,	21000,	WEIGHT,		0},
	{ 21000,	21350,	WEIGHT,		1},
	{ 21350,	21700,	WEIGHT,		0},
	{ 21700,	22050,	WEIGHT,		1},
	{ 22050,	28400,	WEIGHT,		0},
	{ 28400,	28700,	WEIGHT,		1},
	{ 28700,	29000,	WEIGHT,		0},
	{ 29000,	29450,	WEIGHT,		1},
	{ 29450,	30290,	0,			0},
	{ 30290,	45000,	CUBIC,		0},
	{ 45000,	59800,	WOBBLE,		1},
	{ 59800,	82080,	FEEDBACK,	0},
	{ 82080,	93100,	SOAP,		0},
	{ 93100,	95000,	FEEDBACK,	1},
	{ 95000,	95500,	SPIRAL,		1},
	{ 95500,	95950,	WOBBLE,		1},
	{ 95950,	96700,	WEIGHT,		1},
	{ 96700,	113000,	GREETS,		0},
	{ 113000,	128100,	TUNNEL,		1},
	{ 128100,	148200,	FLOWERS,	1},
	{ 148200,	148600,	FLOWERS,	1},
	{ 148600,	149000,	FLOWERS,	0},
	{ 149000,	149400,	FLOWERS,	1},
	{ 149400,	149800,	FLOWERS,	0},
	{ 149800,	150200,	FLOWERS,	1},
	{ 150200,	190000,	0,			0},

											};

const int	MaxFlashEvents					= 24;
const Flash	FlashEventList[MaxFlashEvents]	= {

	{ 21350,	0.6f, 0.6f, 0.9f,	0.07f }, 
	{ 21700,	0.6f, 0.9f, 0.6f,	0.07f },
	{ 22050,	0.9f, 0.6f, 0.6f,	0.07f },
	{ 28400,	0.6f, 0.6f, 0.9f,	0.07f },
	{ 28700,	0.6f, 0.9f, 0.6f,	0.07f },
	{ 29000,	0.9f, 0.6f, 0.6f,	0.07f },
	{ 29450,	1.0f, 1.0f, 1.0f,	0.06f },
	{ 37300,	1.0f, 1.0f, 1.0f,	0.30f },
	{ 37400,	1.0f, 1.0f, 1.0f,	0.30f },
	{ 37500,	1.0f, 1.0f, 1.0f,	0.30f },
	{ 37600,	1.0f, 1.0f, 1.0f,	0.10f },
	{ 95000,	1.0f, 1.0f, 1.0f,	0.20f },
	{ 95500,	1.0f, 1.0f, 1.0f,	0.20f },
	{ 95950,	1.0f, 1.0f, 1.0f,	0.20f },
	{ 96700,	1.0f, 1.0f, 1.0f,	0.20f },
	{ 112020,   0.0f, 0.0f, 0.0f,  -0.10f },
	{ 113000,   1.0f, 1.0f, 1.0f,   1.00f },
	
	{ 148200,	0.9f, 0.6f, 1.6f,	0.20f },
	{ 148600,	0.6f, 0.6f, 1.9f,	0.20f },
	{ 149000,	0.6f, 0.9f, 1.6f,	0.20f },
	{ 149400,	0.9f, 0.6f, 1.3f,	0.20f },
	{ 149800,	0.6f, 0.3f, 1.9f,	0.20f },

	{ 150200,	1.0f, 1.0f, 1.0f,	0.01f },
	{ 153000,	0.0f, 0.0f, 0.0f,  -0.01f }

												};

const int	MaxTextEvents	= 11;
const Text	TextEventList[MaxTextEvents]		= {

	{ 0,		1 },
	{ 6000,		2 },
	{ 14000,	3 },
	{ 30290,	4 },
	{ 45000,	5 },
	{ 59890,	6 },
	{ 82080,	7 },
	{ 96700,	8 },
	{ 113000,	9 },
	{ 129000,	10},
	{ 189000,	11},

												};

#endif