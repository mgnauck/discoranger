
 // font.cpp

#include "settings.h"
#include <stdio.h>
#include <string.h>

// a - z . ! ?
const unsigned int	FontWidth		= 604,
					FontHeight		= 36,
					CharOffset[29]	= {	3, 26, 53, 75, 99, 119, 139, 161, 182, 198, 214, 235, 248,
										277, 297, 319, 346, 369, 388, 405, 420, 440, 461, 490, 516,
										537, 557, 571, 584 },
					CharWidth[29]	= {	20, 43, 67, 93, 114, 130, 156, 177, 190, 208, 229, 242, 271,
										292, 314, 336, 364, 379, 400, 415, 435, 455, 484, 505, 532,
										552, 565, 579, 600 };
unsigned int		*Font;

// font format is: 8bit -> ACCCCCCC
unsigned int *LoadFont(unsigned char *Data, int xSize, int ySize)
{
	unsigned int	*Buf		= new unsigned int[xSize * ySize],
					*AlphaBuf	= new unsigned int[xSize * ySize];
	unsigned char	a, c;

	for(int i=0; i<xSize * ySize; i++)
	{
		c = Data[i];
		
		// alpha == 0 || alpha == 255
		a = ((c >> 7) & 0x1) * 255;

		// since the blur is 24bit we temporary need 24bit alphabuffer
		AlphaBuf[i] = (a << 16) | (a << 8) | a;

		c = (c << 1) | 0x1;
		Buf[i] = (0 << 24) | (c << 16) | (c << 8) | c;	
	}

	BlurBuffer(AlphaBuf, xSize, ySize);
	BlurBuffer(AlphaBuf, xSize, ySize);
	BlurBuffer(AlphaBuf, xSize, ySize);

	for(int i=0; i<xSize * ySize; i++)
		Buf[i] |= ((AlphaBuf[i] & 0xff) << 24);
	
	delete [] AlphaBuf;
	
	return Buf;
}

void InitFont()
{
//	Font = LoadTextureAlpha("data/pump.raw", "data/pumpalpha.raw", FontWidth, FontHeight);
	Font = LoadFont((unsigned char *)font, FontWidth, FontHeight);
}

int Char2Buffer(char C, unsigned int *Buffer, int SizeX, int SizeY, int x, int y)
{
	unsigned int	i, j, s, d, a, r, g, b, num;
	unsigned char	as, rs, gs, bs, ad, rd, gd, bd;

	switch(C)
	{
		case '.':	num = 26; break;
		case '!':	num = 27; break;
		case '?':	num = 28; break;
		default:	num = (int)C - 97; break;
	}

	for(j=0; j<FontHeight; j++) for(i=0; i<(CharWidth[num] - CharOffset[num] + 4); i++)
	{
		// y flip font
		s = Font[FontWidth * j + (CharOffset[num] - 2 + i)];
		d = Buffer[SizeX * (y + j) + (x + i)];

		// make it additive
		as = (s >> 24) & 0xff;
		rs = (s >> 16) & 0xff;
		gs = (s >> 8) & 0xff;
		bs = s & 0xff;

		ad = (d >> 24) & 0xff;
		rd = (d >> 16) & 0xff;
		gd = (d >> 8) & 0xff;
		bd = d & 0xff;

		a = Max(ad + as);
		r = Max(rd + rs);
		g = Max(gd + gs);
		b = Max(bd + bs);	

		Buffer[SizeX * (y + j) + (x + i)] = (a << 24) | (b << 16) | (g << 8) | r;
	}

	return 1 + CharWidth[num] - CharOffset[num];
}

int Text2Buffer(char *Text, unsigned int *Buffer, int SizeX, int SizeY, int x, int y, bool NewFlag)
{
	int ofs = 0;

	if(NewFlag)
	{
		//Buffer = new unsigned int [SizeX * SizeY];
		memset(Buffer, 0, SizeX * SizeY * 4);
	}

	for(unsigned int i=0; i<strlen(Text); i++)
		if(Text[i] != ' ')
			ofs += Char2Buffer(Text[i], Buffer, SizeX, SizeY, x + ofs, y);
		else
			ofs += 10;

	return ofs;
}

void QuitFont()
{
	delete [] Font;
}