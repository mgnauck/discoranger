
 // add.h

#ifndef __ADD__H
#define __ADD__H

void BlurBuffer(void *Buf, int Width, int Height);

//unsigned int *LoadRAW(char *Name, int xSize, int ySize);
unsigned int *LoadRAW( unsigned char *Data, int xSize, int ySize, bool palette );

bool QueryExtension(char *extName);

GLuint UploadTexture(unsigned int *Data, unsigned int w, unsigned int h);

//void Render2ActualTexture(int xStart, int yStart, int xEnd, int yEnd);

void DrawViewportQuad();

void drawQuad( int xs, int xe, int ys, int ye, float alpha, float texX, float texY );

void blurTexture( int GlowBufferDimension, int GlowTextureID, float GlowStrength, int blurPassCount, int blurStepSize );

GLuint MakeCheckerTexture(	int TextureDim, 
							unsigned int Col1, unsigned int Col2, 
							unsigned int Col3, unsigned int Col4);

int Max(int a);

void RGB2HSV(double Red, double Green, double Blue, double *Hue, double *Sat, double *Val);
void HSV2RGB(double Hue, double Sat, double Val, double *Red, double *Green, double *Blue);

#endif