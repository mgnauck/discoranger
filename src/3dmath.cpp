
 // 3dmath.cpp

#include "settings.h"

void CalcNormals(Object *Obj)
{
	int		i;

	for(i=0; i<Obj->NumVertices; i++)
	{
		Obj->Vertices[i].nx = 0;
		Obj->Vertices[i].ny = 0;
		Obj->Vertices[i].nz = 0;
	}

	for(i=0; i<Obj->NumFaces; i++)
	{
		unsigned int	i0	= Obj->Faces[i].a;
		unsigned int	i1	= Obj->Faces[i].b;
		unsigned int	i2	= Obj->Faces[i].c;

		float			vx	= Obj->Vertices[i1].x - Obj->Vertices[i0].x;
		float			vy	= Obj->Vertices[i1].y - Obj->Vertices[i0].y;
		float			vz	= Obj->Vertices[i1].z - Obj->Vertices[i0].z;
		float			ux	= Obj->Vertices[i2].x - Obj->Vertices[i1].x;
		float			uy	= Obj->Vertices[i2].y - Obj->Vertices[i1].y;
		float			uz	= Obj->Vertices[i2].z - Obj->Vertices[i1].z;

		float			nx	= vy * uz - uy * vz;
		float			ny	= vz * ux - uz * vx;
		float			nz	= vx * uy - ux * vy;

		Obj->Vertices[i0].nx += nx;
		Obj->Vertices[i0].ny += ny;
		Obj->Vertices[i0].nz += nz;
		Obj->Vertices[i1].nx += nx;
		Obj->Vertices[i1].ny += ny;
		Obj->Vertices[i1].nz += nz;
		Obj->Vertices[i2].nx += nx;
		Obj->Vertices[i2].ny += ny;
		Obj->Vertices[i2].nz += nz;
	}

	for(i=0; i<Obj->NumVertices; i++)
	{
		float	x	= Obj->Vertices[i].nx,
				y	= Obj->Vertices[i].ny,
				z	= Obj->Vertices[i].nz;

		double	l	= 1.0 / sqrt(x * x + y * y + z * z);

		Obj->Vertices[i].nx = x * (float)l;
		Obj->Vertices[i].ny = y * (float)l;
		Obj->Vertices[i].nz = z * (float)l;
	}
}

// hehe.. no more needed;)