
 // generator.cpp

#include "settings.h"

void Triangulate(Object *Obj, int xSub, int ySub)
{
	int num	= 0;
	
	Obj->NumFaces	= (xSub - 1) * (ySub - 1) * 2;
	Obj->Faces		= new Face [Obj->NumFaces];

	for(int j=0; j<ySub - 1; j++)
	{
		unsigned int	ofs = j * xSub;
		for(int i=0; i<xSub - 1; i++)
		{	
			Obj->Faces[num].a = ofs + i;
			Obj->Faces[num].b = ofs + i + xSub + 1;
			Obj->Faces[num].c = ofs + i + xSub;
			num++;

			Obj->Faces[num].a = ofs + i;
			Obj->Faces[num].b = ofs + i + 1;
			Obj->Faces[num].c = ofs + i + xSub + 1;
			num++;
		}
	}
}

void GenPlane(Object *Obj, int xSub, int ySub, float Width, float Height, float xTexMul, float yTexMul)
{
	int		num	= 0;

	float	dx	= Width / (float)(xSub - 1),
			dy	= Height / (float)(ySub - 1),
			du	= 1.0f / (float)(xSub - 1),
			dv	= 1.0f / (float)(ySub - 1);

	float	x,
			y	= -Height / 2.0f,
			u,
			v	= 0.0f;
	
	Obj->NumVertices	= xSub * ySub;
	Obj->Vertices		= new Vertex [Obj->NumVertices];

	for(int j=0; j<ySub; j++)
	{
		x = -(float)Width / 2.0f;
		u = 0.0f;
		for(int i=0; i<xSub; i++)	
		{
			Obj->Vertices[num].x = x;
			Obj->Vertices[num].y = y;
			Obj->Vertices[num].z = 0.0f;

			Obj->Vertices[num].nx = 0.0f;
			Obj->Vertices[num].ny = 0.0f;
			Obj->Vertices[num].nz = 1.0f;

			Obj->Vertices[num].u = u * xTexMul;
			Obj->Vertices[num].v = v * yTexMul;
			
			x += dx;
			u += du;
			num++;
		}
		y += dy;
		v += dv;
	}

	Triangulate(Obj, xSub, ySub);
}

void GenSphere(Object *Obj, int xSub, int ySub, float Radius, float xTexMul, float yTexMul)
{
	int		num		= 0;
	float	dtheta	= 2.0f * (float)(PI / (double)(xSub - 1)),
			drho	= (float)(PI / (double)(ySub - 1)),
			rho, 
			theta;
	
	Obj->NumVertices	= xSub * ySub;
	Obj->Vertices		= new Vertex [Obj->NumVertices];

	for (int j=0; j<ySub; j++)
	{
		rho = j * drho;
		for(int i=0; i<xSub; i++) 
		{		
			theta = i * dtheta;

			float	x = (float)sin(theta) * (float)sin(rho),
					y = (float)cos(theta) * (float)sin(rho),
					z = (float)cos(rho);
		
			Obj->Vertices[num].x = Radius * x;
			Obj->Vertices[num].y = Radius * y;
			Obj->Vertices[num].z = Radius * z;

			Obj->Vertices[num].nx = 1.0f * x;
			Obj->Vertices[num].ny = 1.0f * y;
			Obj->Vertices[num].nz = 1.0f * z;

			Obj->Vertices[num].u = ((float)i / (float)(xSub - 1)) * xTexMul;
			Obj->Vertices[num].v = ((float)j / (float)(ySub - 1)) * yTexMul;

			num++;
		}
	}

	Triangulate(Obj, xSub, ySub);
}

void GenPQKnot( Object *Obj, int xSub, int ySub, float xTexMul, float yTexMul, float p, float q, float radius ) {

	float	drund	= 2.0f * ( float )PI / ( float )( xSub - 1 );
	float	dlaengs	= 2.0f * ( float )PI / ( float )( ySub - 1 );
	float	laengs	= 0.0f;

	int		num			= 0;
	Obj->NumVertices	= xSub * ySub;
	Obj->Vertices		= new Vertex [Obj->NumVertices];

	for( int j=0; j<ySub; j++ ) {

		float	r		= 0.5f * ( float )( 2.0 + sin( q * laengs ) );
		
		float	x1		= r * ( float )cos( p * laengs );								// p1
		float	y1		= r * ( float )cos( q * laengs );
		float	z1		= r * ( float )sin( p * laengs );

		float	x2		= r * ( float )cos( p * ( laengs + 0.01 ) );						// p2
		float	y2		= r * ( float )cos( q * ( laengs + 0.01 ) );
		float	z2		= r * ( float )sin( p * ( laengs + 0.01 ) );

		float	tx		= x2 - x1;												// t = p2 - p1
		float	ty		= y2 - y1;
		float	tz		= z2 - z1;

		float	nx		= x2 + x1;												// n = p2 + p1
		float	ny		= y2 + y1;
		float	nz		= z2 + z1;

		float	bx		= ( ty * nz ) - ( tz * ny );							// b = t x n
		float	by		= -( ( tx * nz ) - ( tz * nx ) );
		float	bz		= ( tx * ny ) - ( ty * nx );

		nx	= ( by * tz ) - ( bz * ty );										// n = b x t (recalc)
		ny	= -( ( bx * tz ) - ( bz * tx ) );
		nz	= ( bx * ty ) - ( by * tx );

		float length = 1.0f / ( float )sqrt( nx * nx + ny * ny + nz * nz );		// n normalisieren

		nx *= length;
		ny *= length;
		nz *= length;

		length = 1.0f / ( float )sqrt( bx * bx + by * by + bz * bz );			// b normalisieren

		bx *= length;
		by *= length;
		bz *= length;

		float	rund = 0.0f;
		for( int i=0; i<xSub; i++ ) {

			float	rad		= radius * sin( laengs * 10.0 ) + radius + 0.03f;
			float	shapex	= rad * cos( rund );
			float	shapey	= rad * sin( rund );

			Obj->Vertices[num].x = x1 + ( nx * shapex ) + ( bx * shapey );
			Obj->Vertices[num].y = y1 + ( ny * shapex ) + ( by * shapey );
			Obj->Vertices[num].z = z1 + ( nz * shapex ) + ( bz * shapey );

			rund += drund;
			num++;
		}

		laengs += dlaengs;
	}

	Triangulate( Obj, xSub, ySub );
	CalcNormals( Obj );
}

void GenCube(Object *Obj, int xSub, int ySub, float Size, float xTexMul, float yTexMul)
{
	int		num = 0;

	float	dx	= Size / (float)(xSub - 1),
			dy	= Size / (float)(ySub - 1),
			du	= 1.0f / (float)(xSub - 1),
			dv	= 1.0f / (float)(ySub - 1);

	float	x,
			y	= -Size / 2.0f,
			u,
			v	= 0.0f;

	Obj->NumVertices	= xSub * ySub * 6;
	Obj->Vertices		= new Vertex [Obj->NumVertices];

	// vorn
	for(int j=0; j<ySub; j++)
	{
		x = -Size / 2.0f;
		u = 0.0f;
		for(int i=0; i<xSub; i++)	
		{
			Obj->Vertices[num].x = x;
			Obj->Vertices[num].y = y;
			Obj->Vertices[num].z = Size / 2.0f;

			Obj->Vertices[num].nx = 0.0f;
			Obj->Vertices[num].ny = 0.0f;
			Obj->Vertices[num].nz = 1.0f;

			Obj->Vertices[num].u = u * xTexMul;
			Obj->Vertices[num].v = v * yTexMul;
			
			x += dx;
			u += du;
			num++;
		}
		y += dy;
		v += dv;
	}

	// hinten
	y = -Size / 2.0f;
	v = 0.0f;
	for(int j=0; j<ySub; j++)
	{
		x = -Size / 2.0f;
		u = 0.0f;
		for(int i=0; i<xSub; i++)	
		{
			Obj->Vertices[num].x = x;
			Obj->Vertices[num].y = y;
			Obj->Vertices[num].z = -Size / 2.0f;

			Obj->Vertices[num].nx = 0.0f;
			Obj->Vertices[num].ny = 0.0f;
			Obj->Vertices[num].nz = -1.0f;

			Obj->Vertices[num].u = u * xTexMul;
			Obj->Vertices[num].v = v * yTexMul;
			
			x += dx;
			u += du;
			num++;
		}
		y += dy;
		v += dv;
	}

	// links
	y = -Size / 2.0f;
	v = 0.0f;
	for(int j=0; j<ySub; j++)
	{
		x = -Size / 2.0f;
		u = 0.0f;
		for(int i=0; i<xSub; i++)	
		{
			Obj->Vertices[num].x = -Size / 2.0f;
			Obj->Vertices[num].y = y;
			Obj->Vertices[num].z = x;

			Obj->Vertices[num].nx = -1.0f;
			Obj->Vertices[num].ny = 0.0f;
			Obj->Vertices[num].nz = 0.0f;

			Obj->Vertices[num].u = u * xTexMul;
			Obj->Vertices[num].v = v * yTexMul;
			
			x += dx;
			u += du;
			num++;
		}
		y += dy;
		v += dv;
	}

	// rechts
	y = -Size / 2.0f;
	v = 0.0f;
	for(int j=0; j<ySub; j++)
	{
		x = -(float)Size / 2.0f;
		u = 0.0f;
		for(int i=0; i<xSub; i++)	
		{
			Obj->Vertices[num].x = Size / 2.0f;
			Obj->Vertices[num].y = y;
			Obj->Vertices[num].z = x;

			Obj->Vertices[num].nx = 1.0f;
			Obj->Vertices[num].ny = 0.0f;
			Obj->Vertices[num].nz = 0.0f;

			Obj->Vertices[num].u = u * xTexMul;
			Obj->Vertices[num].v = v * yTexMul;
			
			x += dx;
			u += du;
			num++;
		}
		y += dy;
		v += dv;
	}

	// oben
	y = -Size / 2.0f;
	v = 0.0f;
	for(int j=0; j<ySub; j++)
	{
		x = -(float)Size / 2.0f;
		u = 1.0f;
		for(int i=0; i<xSub; i++)	
		{
			Obj->Vertices[num].x = y;
			Obj->Vertices[num].y = Size / 2.0f;
			Obj->Vertices[num].z = x;

			Obj->Vertices[num].nx = 0.0f;
			Obj->Vertices[num].ny = 1.0f;
			Obj->Vertices[num].nz = 0.0f;

			Obj->Vertices[num].u = v * xTexMul;
			Obj->Vertices[num].v = u * yTexMul;
			
			x += dx;
			u -= du;
			num++;
		}
		y += dy;
		v += dv;
	}

	// unten
	y = -Size / 2.0f;
	v = 0.0f;
	for(int j=0; j<ySub; j++)
	{
		x = -Size / 2.0f;
		u = 0.0f;
		for(int i=0; i<xSub; i++)	
		{
			Obj->Vertices[num].x = y;
			Obj->Vertices[num].y = -Size / 2.0f;
			Obj->Vertices[num].z = x;

			Obj->Vertices[num].nx = 0.0f;
			Obj->Vertices[num].ny = -1.0f;
			Obj->Vertices[num].nz = 0.0f;

			Obj->Vertices[num].u = v * xTexMul;
			Obj->Vertices[num].v = u * yTexMul;
			
			x += dx;
			u += du;
			num++;
		}
		y += dy;
		v += dv;
	}

	Obj->NumFaces = (xSub - 1) * (ySub - 1) * 2 * 6;
	Obj->Faces = new Face [Obj->NumFaces];

	num = 0;

	unsigned int	ofs1 = 0,
					ofs2 = 0;
	for(int k=0; k<3; k++)
	{
		for(int j=0; j<ySub - 1; j++)
		{
			ofs2 = ofs1 + j * xSub;
			for(int i=0; i<xSub - 1; i++)
			{	
				Obj->Faces[num].a = ofs2 + i;
				Obj->Faces[num].b = ofs2 + i + xSub + 1;
				Obj->Faces[num].c = ofs2 + i + xSub;
				num++;

				Obj->Faces[num].a = ofs2 + i;
				Obj->Faces[num].b = ofs2 + i + 1;
				Obj->Faces[num].c = ofs2 + i + xSub + 1;
				num++;
			}
		}
		ofs1 += xSub * ySub;

		for(int j=0; j<ySub - 1; j++)
		{
			ofs2 = ofs1 + j * xSub;
			for(int i=0; i<xSub - 1; i++)
			{	
				Obj->Faces[num].a = ofs2 + i;
				Obj->Faces[num].b = ofs2 + i + xSub;
				Obj->Faces[num].c = ofs2 + i + xSub + 1;
				num++;

				Obj->Faces[num].a = ofs2 + i;
				Obj->Faces[num].b = ofs2 + i + xSub + 1;
				Obj->Faces[num].c = ofs2 + i + 1;
				num++;
			}
		}
		ofs1 += xSub * ySub;
	}
}

void GenSpiral(Object *Obj, int Slices)
{
	Obj->NumVertices	= 1 + (Slices + 1) * 4;
	Obj->NumFaces		= Slices * 4;
	Obj->NumLines		= Obj->NumVertices - 1;

	Obj->Vertices		= new Vertex [Obj->NumVertices];
	Obj->Faces			= new Face [Obj->NumFaces];

	float	ValueDX		= 2.0f / (float)Slices,
			Value		= -1.0f;

	int		i;

	Obj->Vertices[0].x	= 0.0f;
	Obj->Vertices[0].y	= 0.0f;
	Obj->Vertices[0].z	= 0.0f;

	Value = -1.0f;
	for(i=0; i<Slices; i++)
	{
		int ofs = 1 + i;
		Obj->Vertices[ofs].x = Value;
		Obj->Vertices[ofs].y = 1.0f;
		Obj->Vertices[ofs].z = 0.0f;

		ofs = 1 + Slices + i;
		Obj->Vertices[ofs].x = 1.0f;
		Obj->Vertices[ofs].y = -Value;
		Obj->Vertices[ofs].z = 0.0f;

		ofs = 1 + (2 * Slices) + i;
		Obj->Vertices[ofs].x = -Value;
		Obj->Vertices[ofs].y = -1.0f;
		Obj->Vertices[ofs].z = 0.0f;

		ofs = 1 + (3 * Slices) + i;
		Obj->Vertices[ofs].x = -1.0f;
		Obj->Vertices[ofs].y = Value;
		Obj->Vertices[ofs].z = 0.0f;

		Value += ValueDX;
	}

	for(i=0; i<Obj->NumVertices; i++)
	{
		Obj->Vertices[i].nx = 0.0f;
		Obj->Vertices[i].ny = 0.0f;
		Obj->Vertices[i].nz = 1.0f;
	}

	for(i=0; i<Obj->NumFaces - 1; i++)
	{
		Obj->Faces[i].a = 1 + i;
		Obj->Faces[i].b = 0;
		Obj->Faces[i].c = 2 + i;
	}

	// last connects with first
	Obj->Faces[Obj->NumFaces - 1].a = 4 * Slices;
	Obj->Faces[Obj->NumFaces - 1].b = 0;
	Obj->Faces[Obj->NumFaces - 1].c = 1;
}

void GenCylinder(	Object *Obj, int xSub, int ySub, float Radius, float Height,
					float xTexMul, float yTexMul)
{
	int		i, j, num = 0;

	float	y, u, v;

	float	dy	= Height / (float)(ySub - 1),
			d	= 2.0f * (float)PI / (float)(xSub - 1);

	float	du	= 1.0f / (float)(xSub - 1),
			dv	= 1.0f / (float)(ySub - 1);

	Obj->NumVertices	= xSub * ySub;
	Obj->Vertices		= new Vertex [Obj->NumVertices];

	v = 1.0f;
	y = -(float)(Height / 2);

	for(j=0; j<ySub; j++)
	{
		u = 0.0f;	
		for(i=0; i<xSub; i++)
		{
			Obj->Vertices[num].x	= Radius * (float)sin(d * i);
			Obj->Vertices[num].y	= y;
			Obj->Vertices[num].z	= Radius * (float)cos(d * i);

			Obj->Vertices[num].u	= u * xTexMul;
			Obj->Vertices[num].v	= v * yTexMul;

			u += du;
			num++;
		}
		y += dy;
		v -= dv;
	}
	
	Triangulate(Obj, xSub, ySub);
}

void DeleteObject(Object *Obj)
{
	delete [] Obj->Faces;
	delete [] Obj->Vertices;
}