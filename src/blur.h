
 // blur.h

#ifndef __BLUR_H
#define __BLUR_H

void InitBlurTexture(int BufferDimX, int BufferDimY);
void Render2ActualTexture(int xStart, int yStart, int xEnd, int yEnd);
void RadialBlur(double Intensity, double Factor, int xStart, int yStart, int xEnd, int yEnd);
void MotionBlur(double Intensity, int xStart, int yStart, int xEnd, int yEnd);

#endif