
 // render.cpp

#include "settings.h"

void UploadObject(Object Obj)
{
	glInterleavedArrays(GL_T2F_N3F_V3F, 0, Obj.Vertices);
}

void DrawObject(Object Obj)
{
	glDrawElements(GL_TRIANGLES, Obj.NumFaces * 3, GL_UNSIGNED_INT, Obj.Faces);
}

void RenderObject(Object Obj)
{
	glInterleavedArrays(GL_T2F_N3F_V3F, 0, Obj.Vertices);
	glDrawElements(GL_TRIANGLES, Obj.NumFaces * 3, GL_UNSIGNED_INT, Obj.Faces);
}

void RenderObjectImmediate(Object Obj)
{
	glBegin(GL_TRIANGLES);

		for(int i=0; i<Obj.NumFaces; i++)
		{
			Vertex	*V1	= &Obj.Vertices[Obj.Faces[i].a];
			Vertex	*V2	= &Obj.Vertices[Obj.Faces[i].b];
			Vertex	*V3	= &Obj.Vertices[Obj.Faces[i].c];

			glNormal3f(V1->nx, V1->ny, V1->nz);
			glTexCoord2f(V1->u, V1->v);
			glVertex3f(V1->x, V1->y, V1->z);

			glNormal3f(V2->nx, V2->ny, V2->nz);
			glTexCoord2f(V2->u, V2->v);
			glVertex3f(V2->x, V2->y, V2->z);

			glNormal3f(V3->nx, V3->ny, V3->nz);
			glTexCoord2f(V3->u, V3->v);
			glVertex3f(V3->x, V3->y, V3->z);
		}

	glEnd();
}