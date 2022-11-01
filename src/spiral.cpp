
 // spiral.cpp

#include "settings.h"

const float		MAX		= 0.17f;
Object			Spiral;
GLuint			SpiralIndex;

#ifdef _WIN32_
__forceinline double sqr(double x)
#endif
#ifdef _MAC_OSX_
double sqr(double x)
#endif
{
	return x * x;
}

void Tesselate(Vertex v1, Vertex v2, Vertex v3, float Max)
{
	double	d1, d2, d3,
			dmax;

	// no sqrt because (dmax>sqr(Max))
	d1 = sqr(v3.x - v1.x) + sqr(v3.y - v1.y) + sqr(v3.z - v1.z);
	d2 = sqr(v1.x - v2.x) + sqr(v1.y - v2.y) + sqr(v1.z - v2.z);
	d3 = sqr(v2.x - v3.x) + sqr(v2.y - v3.y) + sqr(v2.z - v3.z);

	// kriterium
	dmax = d1;
	if(d2>dmax)
		dmax = d2;
	if(d3>dmax)
		dmax = d3;

	if(dmax>sqr(Max))
	{
		Vertex	*n1 = new Vertex,
				*n2 = new Vertex,
				*n3 = new Vertex;
	
		n1->x	= (v1.x + v3.x) / 2.0f;
		n1->y	= (v1.y + v3.y) / 2.0f;
		n1->z	= (v1.z + v3.z) / 2.0f;
		n1->nx	= (v1.nx + v3.nx) / 2.0f;
		n1->ny	= (v1.ny + v3.ny) / 2.0f;
		n1->nz	= (v1.nz + v3.nz) / 2.0f;

		n2->x	= (v1.x + v2.x) / 2.0f;
		n2->y	= (v1.y + v2.y) / 2.0f;
		n2->z	= (v1.z + v2.z) / 2.0f;
		n2->nx	= (v1.nx + v2.nx) / 2.0f;
		n2->ny	= (v1.ny + v2.ny) / 2.0f;
		n2->nz	= (v1.nz + v2.nz) / 2.0f;

		n3->x	= (v2.x + v3.x) / 2.0f;
		n3->y	= (v2.y + v3.y) / 2.0f;
		n3->z	= (v2.z + v3.z) / 2.0f;
		n3->nx	= (v2.nx + v3.nx) / 2.0f;
		n3->ny	= (v2.ny + v3.ny) / 2.0f;
		n3->nz	= (v2.nz + v3.nz) / 2.0f;

		Tesselate(v1, *n2, *n1, Max);
		Tesselate(*n2, *n3, *n1, Max);
		Tesselate(*n2, v2, *n3, Max);
		Tesselate(*n1, *n3, v3, Max);

		delete n3;
		delete n2;
		delete n1;
	} else
	{
		glNormal3f(v1.nx, v1.ny, v1.nz);
		glVertex3f(v1.x, v1.y, v1.z);

		glNormal3f(v2.nx, v2.ny, v2.nz);
		glVertex3f(v2.x, v2.y, v2.z);

		glNormal3f(v3.nx, v3.ny, v3.nz);
		glVertex3f(v3.x, v3.y, v3.z);
	}
}

void InitSpiral()
{
	GenSpiral(&Spiral, 4);
	CalcNormals(&Spiral);

	SpiralIndex = glGenLists(1);
	glNewList(SpiralIndex, GL_COMPILE);

		bool	C	= true;

		glBegin(GL_TRIANGLES);
		for(int i=0; i<Spiral.NumFaces; i++)
		{
			if(C)
				glColor4f(0.831f, 0.505f, 0.752f, 1.0f);
			else
				glColor4f(0.635f, 0.188f, 0.486f, 1.0f);

			Tesselate(	Spiral.Vertices[Spiral.Faces[i].a], 
						Spiral.Vertices[Spiral.Faces[i].b],
						Spiral.Vertices[Spiral.Faces[i].c],
						MAX);
			
			C = !C;
		}
		glEnd();

	glEndList();
}

void DoSpiral(unsigned long time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glPushAttrib(GL_ENABLE_BIT);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glDisable( GL_TEXTURE_2D );

	GLfloat AmbientLight[]	= { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat DiffuseLight[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPos[]		= { -(float)sin(time * 0.0012) * 0.5f, 
								-(float)cos(time * 0.0012) * 0.5f,
								0.6f, 
								1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.6f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(90.0 + (float)sin(time * 0.002) * 10.0f, WIDTH / (double)HEIGHT, 0.1, 1000.0);
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);

	glScalef(2.6f, 2.6f, 1.0f);
	glRotatef((float)sin(time * 0.00012) * 360.0f, 0.0f, 0.0f, 1.0f);

	glCallList(SpiralIndex);
	
	/*
	bool	C	= true;

	glBegin(GL_TRIANGLES);
	for(int i=0; i<Spiral.NumFaces; i++)
	{
		if(C)
			glColor4f(0.831f, 0.505f, 0.752f, 1.0f);
		else
			glColor4f(0.635f, 0.188f, 0.486f, 1.0f);

		Vertex	v1 = Spiral.Vertices[Spiral.Faces[i].a],
				v2 = Spiral.Vertices[Spiral.Faces[i].b],
				v3 = Spiral.Vertices[Spiral.Faces[i].c];

		glNormal3f(v1.nx, v1.ny, v1.nz);
		glVertex3f(v1.x, v1.y, v1.z);

		glNormal3f(v2.nx, v2.ny, v2.nz);
		glVertex3f(v2.x, v2.y, v2.z);

		glNormal3f(v3.nx, v3.ny, v3.nz);
		glVertex3f(v3.x, v3.y, v3.z);
		
		C = !C;
	}
	glEnd();
	//*/

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glPopAttrib();

	glEnable( GL_TEXTURE_2D );
}

void QuitSpiral()
{
	DeleteObject(&Spiral);
}