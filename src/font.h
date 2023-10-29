
 // font.h

#ifndef __FONT_H
#define __FONT_H

void InitFont();
int Char2Buffer(char C, unsigned int *Buffer, int SizeX, int SizeY, int x, int y);
int Text2Buffer(char *Text, unsigned int *Buffer, int SizeX, int SizeY, int x, int y, bool NewFlag);
void QuitFont();

#endif