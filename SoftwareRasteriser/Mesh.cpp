#include "Mesh.h"

Mesh::Mesh(void)	{
	type			= PRIMITIVE_POINTS;

	numVertices		= 0;

	vertices		= NULL;
	colours			= NULL;
	textureCoords	= NULL;
}

Mesh::~Mesh(void)	{
	delete[] vertices;
	delete[] colours;
	delete[] textureCoords;
}

Mesh * Mesh::GenerateLine(const Vector3 &from, const Vector3 &to) {
	Mesh * m = new Mesh();
	
		m->numVertices = 2;
		m->vertices = new Vector4[m->numVertices];
		m->colours = new Colour[m->numVertices];
	
		m->vertices[0] = Vector4(from.x, from.y, from.z, 1.0f);
		m->vertices[1] = Vector4(to.x, to.y, to.z, 1.0f);
	

		m->colours[0] = Colour(255, 0, 0, 255);
		m->colours[1] = Colour(0, 0, 255, 255);

		m->type = PRIMITIVE_LINES;
	
		return m;
	
}


Mesh * Mesh::GenerateTriangle() {
	Mesh * m = new Mesh();
	m->type = PRIMITIVE_TRIANGLES;
	
	m->numVertices = 3;
	


	m->vertices = new Vector4[m->numVertices];
	m->vertices[0] = Vector4(0.5f, -0.5f, 0, 1.0f);
	m->vertices[1] = Vector4(0.0f, 0.5f, 0, 1.0f);
	m->vertices[2] = Vector4(-0.5f, -0.5f, 0, 1.0f);
	
	m->colours = new Colour[m->numVertices];
	m->colours[0] = Colour(255, 0, 0, 255); 
	m->colours[1] = Colour(255, 255, 0, 255);
	m->colours[2] = Colour(255, 0, 0, 255); 

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.0f, 0.0f);
	m->textureCoords[1] = Vector2(0.5f, 1.0f);
	m->textureCoords[2] = Vector2(1.0f, 0.0f);

		return m;
	
}

Mesh* Mesh::GenerateStars() {

	Mesh* m = new Mesh();
	m->type = PRIMITIVE_POINTS;
	m->numVertices = 4000;
	m->vertices = new Vector4[m->numVertices];
	m->colours = new Colour[m->numVertices];

	for (int i = 0; i < 2000; ++i) {
		float x = (rand() / (float)(RAND_MAX)* (1.0f - -1.0f) + -1.0f);
		float y = (rand() / (float)(RAND_MAX)* (1.0f - -1.0f) + -1.0f);
		float z = (rand() / (float)(RAND_MAX)* (1.0f - -1.0f) + -1.0f);
		m->vertices[i] = Vector4(x, y, z, 1.0f);
		m->colours[i] = Colour(255, 255, 255, 255);

	}


	return m;

}

Mesh * Mesh::GenerateShapes(const Vector3 &A1, const Vector3 &B1, const Vector3 &C1, const Vector3 &D1, const Vector3 &E1, const Vector3 &G1) {
	Mesh * m = new Mesh();

	m->numVertices = 6;
	m->vertices = new Vector4[m->numVertices];
	m->colours = new Colour[m->numVertices];

	m->vertices[0] = Vector4(A1.x, A1.y, A1.z, 1.0f);
	m->vertices[1] = Vector4(B1.x, B1.y, B1.z, 1.0f);
	m->vertices[2] = Vector4(C1.x, C1.y, C1.z, 1.0f);
	m->vertices[3] = Vector4(D1.x, D1.y, D1.z, 1.0f);
	m->vertices[4] = Vector4(E1.x, E1.y, E1.z, 1.0f);
	m->vertices[5] = Vector4(G1.x, G1.y, G1.z, 1.0f);

	m->colours[0] = Colour(139, 90, 0, 255);
	m->colours[1] = Colour(139, 90, 255, 255);
	m->colours[2] = Colour(139, 90, 0, 255);
	m->colours[3] = Colour(139, 90, 255, 255);
	m->colours[4] = Colour(139, 90, 0, 255);
	m->colours[5] = Colour(139, 90, 255, 255);

	m->type = PRIMITIVE_LINE_LOOPS;

	return m;

}

Mesh * Mesh::LoadMeshFile(const string & filename) {
	ifstream f(filename);
	
		if (!f) {
		return NULL;
		
	}
	
		Mesh *m = new Mesh();
	m->type = PRIMITIVE_TRIANGLES;
	f >> m->numVertices;
	
		int hasTex = 0;
	int hasColour = 0;
	
		f >> hasTex; 
	f >> hasColour;
	
	 m->vertices = new Vector4[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Colour[m->numVertices];
	
		for (int i = 0; i < m->numVertices; ++i) {
		f >> m->vertices[i].x;
		f >> m->vertices[i].y;
		f >> m->vertices[i].z;
		
	}
	if (hasColour) {
		for (int i = 0; i < m->numVertices; ++i) {
		 f >> m->colours[i].r; f >> m->colours[i].g;
			f >> m->colours[i].b; f >> m->colours[i].a;
			
		}
		
	}
	return m;
	
}