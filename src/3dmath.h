		
 // 3dmath.h

#ifndef __3DMATH__H
#define __3DMATH__H

#include "settings.h"

const double	PI	=  3.1415926535897932384626433832795;

typedef struct
{
	float	u, v;
	float	nx, ny, nz;
	float	x, y, z;
} Vertex;

typedef struct
{
	int		a, b, c;
} Face;

typedef struct
{
	int		NumVertices;
	int		NumFaces;
	int		NumLines;
	Vertex	*Vertices;
	Face	*Faces;
} Object;

// woooahh:)
void CalcNormals(Object *Obj);

#endif