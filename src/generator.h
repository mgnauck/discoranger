
 // generator.h

#ifndef __GENERATOR__H
#define __GENERATOR__H

void GenPlane(Object *Obj, int xSub, int ySub, float Width, float Height, float xTexMul, float yTexMul);
void GenSphere(Object *Obj, int xSub, int ySub, float Radius, float xTexMul, float yTexMul);
void GenPQKnot( Object *Obj, int xSub, int ySub, float xTexMul, float yTexMul, float p, float q, float radius );
void GenCube(Object *Obj, int xSub, int ySub, float Size, float xTexMul, float yTexMul);
void GenSpiral(Object *Obj, int Slices);
void GenCylinder(	Object *Obj, int xSub, int ySub, float Radius, float Height,
					float xTexMul, float yTexMul);
void DeleteObject(Object *Obj);

#endif