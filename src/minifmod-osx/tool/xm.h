/* XM module file format structs */

typedef struct{
	unsigned int length;
	unsigned int loopstart;
	unsigned int looplen;
	unsigned char defvol;
	char finetune;
	unsigned char bytes;
	unsigned char defpan;
	char relative;
	unsigned char Reserved;
}FSOUND_SAMPLE;

typedef struct{
	unsigned char note;
	unsigned char number;
	unsigned char volume;
	unsigned char effect;
	unsigned char eparam;
}FMUSIC_NOTE;

typedef struct{
	unsigned short rows;
	unsigned short patternsize;
	FMUSIC_NOTE* data;
}FMUSIC_PATTERN;

typedef struct{
	unsigned int numsamples;
	FSOUND_SAMPLE* sample[16];
	unsigned char keymap[96];
	unsigned short VOLPoints[24];
	unsigned short PANPoints[24];
	unsigned char VOLnumpoints;
	unsigned char PANnumpoints;
	unsigned char VOLsustain;
	unsigned char VOLLoopStart;
	unsigned char VOLLoopEnd;
	unsigned char PANsustain;
	unsigned char PANLoopStart;
	unsigned char PANLoopEnd;
	unsigned char VOLtype;
	unsigned char PANtype;
	unsigned char VIBtype;
	unsigned char VIBsweep;
	unsigned char VIBdepth;
	unsigned char VIBrate;
	unsigned short VOLfade;
}FMUSIC_INSTRUMENT;

typedef struct{
	FMUSIC_PATTERN* pattern;
	FMUSIC_INSTRUMENT* instrument;
	int mixer_samplesleft;
	int globalvolume;
	int tick;
	unsigned int speed;
	int row;
	int order;
	int patterndelay;
	int nextrow;
	int nextorder;
	int time_ms;
	unsigned char globalvsl;
	char _align[11];
	int mixer_samplespertick;
	unsigned short numorders;
	short restart;
	short numchannels;
	unsigned short numpatternsmem;
	unsigned short numinsts;
	unsigned short flags;
	unsigned short defaultspeed;
	short defaultbpm;
	unsigned char orderlist[256];
}FMUSIC_MODULE;

enum FMUSIC_XMCOMMANDS{
	FMUSIC_XM_ARPEGGIO,
	FMUSIC_XM_PORTAUP,
	FMUSIC_XM_PORTADOWN,
	FMUSIC_XM_PORTATO,
	FMUSIC_XM_VIBRATO,
	FMUSIC_XM_PORTATOVOLSLIDE,
	FMUSIC_XM_VIBRATOVOLSLIDE,
	FMUSIC_XM_TREMOLO,
	FMUSIC_XM_SETPANPOSITION,
	FMUSIC_XM_SETSAMPLEOFFSET,
	FMUSIC_XM_VOLUMESLIDE,
	FMUSIC_XM_PATTERNJUMP,
	FMUSIC_XM_SETVOLUME,
	FMUSIC_XM_PATTERNBREAK,
	FMUSIC_XM_SPECIAL,
	FMUSIC_XM_SETSPEED,
	FMUSIC_XM_SETGLOBALVOLUME,
	FMUSIC_XM_GLOBALVOLSLIDE,
	FMUSIC_XM_I,
	FMUSIC_XM_J,
	FMUSIC_XM_KEYOFF,
	FMUSIC_XM_SETENVELOPEPOS,
	FMUSIC_XM_M,
	FMUSIC_XM_N,
	FMUSIC_XM_O,
	FMUSIC_XM_PANSLIDE,
	FMUSIC_XM_Q,
	FMUSIC_XM_MULTIRETRIG,
	FMUSIC_XM_S,
	FMUSIC_XM_TREMOR,
	FMUSIC_XM_U,
	FMUSIC_XM_V,
	FMUSIC_XM_W,
	FMUSIC_XM_EXTRAFINEPORTA,
	FMUSIC_XM_Y,
	FMUSIC_XM_Z
};

enum FMUSIC_XMCOMMANDSSPECIAL{
	FMUSIC_XM_SETFILTER,
	FMUSIC_XM_FINEPORTAUP,
	FMUSIC_XM_FINEPORTADOWN,
	FMUSIC_XM_SETGLISSANDO,
	FMUSIC_XM_SETVIBRATOWAVE,
	FMUSIC_XM_SETFINETUNE,
	FMUSIC_XM_PATTERNLOOP,
	FMUSIC_XM_SETTREMOLOWAVE,
	FMUSIC_XM_SETPANPOSITION16,
	FMUSIC_XM_RETRIG,
	FMUSIC_XM_FINEVOLUMESLIDEUP,
	FMUSIC_XM_FINEVOLUMESLIDEDOWN,
	FMUSIC_XM_NOTECUT,
	FMUSIC_XM_NOTEDELAY,
	FMUSIC_XM_PATTERNDELAY
};
