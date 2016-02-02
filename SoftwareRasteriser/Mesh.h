/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description: Class to represent the geometric data that makes up the meshes
we render on screen.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector4.h"
#include "Colour.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Common.h"

#include <string>
#include <fstream>

using std::ifstream;
using std::string;

enum PrimitiveType {
	PRIMITIVE_POINTS,
	PRIMITIVE_LINES,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_LINE_LOOPS
};

class Mesh	{
	friend class SoftwareRasteriser;
public:
	Mesh(void);
	~Mesh(void);

	static Mesh*	GenerateTriangle();
	static Mesh*	GenerateLine(const Vector3 &from, const Vector3 &to);

	PrimitiveType	GetType() { return type;}
	Mesh * LoadMeshFile(const string & filename);
	static Mesh* GenerateStars();
	static Mesh * GenerateShapes(const Vector3 &A1, const Vector3 &B1, const Vector3 &C1, const Vector3 &D1, const Vector3 &E1, const Vector3 &G1);

protected:
	PrimitiveType	type;

	uint			numVertices;

	Vector4*		vertices;
	Colour*			colours;
	Vector2*		textureCoords;	
};

