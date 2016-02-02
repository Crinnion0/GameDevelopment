#include "SoftwareRasteriser.h"
#include <cmath>
#include <math.h>
/*
While less 'neat' than just doing a 'new', like in the tutorials, it's usually
possible to render a bit quicker to use direct pointers to the drawing area
that the OS gives you. For a bit of a speedup, you can uncomment the define below
to switch to using this method.

For those of you new to the preprocessor, here's a quick explanation:

Preprocessor definitions like #define allow parts of a file to be selectively enabled
or disabled at compile time. This is useful for hiding parts of the codebase on a
per-platform basis: if you have support for linux and windows in your codebase, obviously
the linux platform won't have the windows platform headers available, so compilation will
fail. So instead you can hide away all the platform specific stuff:

#if PLATFORM_WINDOWS
 DoSomeWindowsStuff();
#elif PLATFORM_LINUX
 DoSomeLinuxStuff();
 #else
 #error Unsupported Platform Specified!
 #endif

 As in our usage, it also allows you to selectively compile in some different functionality
 without any 'run time' cost - if it's not enabled by the preprocessor, it won't make it to
 the compiler, so no assembly will be generated.

Also, I've implemented the Resize method for you, in a manner that'll behave itself
no matter which method you use. I kinda forgot to do that, so there was a chance you'd
get exceptions if you resized to a bigger screen area. Sorry about that.
*/
//#define USE_OS_BUFFERS

SoftwareRasteriser::SoftwareRasteriser(uint width, uint height)	: Window(width, height){
	currentTexture = NULL;
	currentDrawBuffer = 0;

#ifndef USE_OS_BUFFERS
	//Hi! In the tutorials, it's mentioned that we need to form our front + back buffer like so:
	for (int i = 0; i < 2; ++i) {
		buffers[i] = new Colour[screenWidth * screenHeight];
	}
#else
	//This works, but we can actually save a memcopy by rendering directly into the memory the 
	//windowing system gives us, which I've added to the Window class as the 'bufferData' pointers
	for (int i = 0; i < 2; ++i) {
		buffers[i] = (Colour*)bufferData[i];
	}
#endif

	depthBuffer		=	new unsigned short[screenWidth * screenHeight];

	float zScale	= (pow(2.0f,16) - 1) * 0.5f;

	Vector3 halfScreen = Vector3((screenWidth - 1) * 0.5f, (screenHeight - 1) * 0.5f, zScale);

	portMatrix = Matrix4::Translation(halfScreen) * Matrix4::Scale(halfScreen);
}

SoftwareRasteriser::~SoftwareRasteriser(void)	{
#ifndef USE_OS_BUFFERS
	for(int i = 0; i < 2; ++i) {
		delete[] buffers[i];
	}
#endif
	delete[] depthBuffer;
}

void SoftwareRasteriser::Resize() {
	Window::Resize(); 

#ifndef USE_OS_BUFFERS
	for (int i = 0; i < 2; ++i) {
		delete[] buffers[i];
		buffers[i] = new Colour[screenWidth * screenHeight];
	}
#else
	for (int i = 0; i < 2; ++i) {
		buffers[i] = (Colour*)bufferData[i];
	}
#endif

	delete[] depthBuffer;
	depthBuffer = new unsigned short[screenWidth * screenHeight];

	float zScale = (pow(2.0f, 16) - 1) * 0.5f;

	Vector3 halfScreen = Vector3((screenWidth - 1) * 0.5f, (screenHeight - 1) * 0.5f, zScale);

	portMatrix = Matrix4::Translation(halfScreen) * Matrix4::Scale(halfScreen);
}

Colour*	SoftwareRasteriser::GetCurrentBuffer() {
	return buffers[currentDrawBuffer];
}

void	SoftwareRasteriser::ClearBuffers() {
	Colour* buffer = GetCurrentBuffer();

	unsigned int clearVal = 0xFF000000;
	unsigned int depthVal = ~0;

	for(uint y = 0; y < screenHeight; ++y) {
		for(uint x = 0; x < screenWidth; ++x) {
			buffer[(y * screenWidth) + x].c  = clearVal;
			depthBuffer[(y * screenWidth) + x] = depthVal;
		}
	}
}

void	SoftwareRasteriser::SwapBuffers() {
	PresentBuffer(buffers[currentDrawBuffer]);
	currentDrawBuffer = !currentDrawBuffer;
}

void	SoftwareRasteriser::DrawObject(RenderObject*o) {
	currentTexture = o->texture;
	switch (o->GetMesh()->GetType()) {
	case PRIMITIVE_POINTS: {
							   RasterisePointsMesh(o);
	} break;
	case PRIMITIVE_LINES: {
							  RasteriseLinesMesh(o);
	} break;
	case PRIMITIVE_TRIANGLES: {
								  RasteriseTriMesh(o); 
								  } break;
	case PRIMITIVE_LINE_LOOPS: {
								   RasteriseLineLoops(o);
	}
		break;
	}

	}



void	SoftwareRasteriser::RasterisePointsMesh(RenderObject*o) {
	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();

	for (uint i = 0; i < o->GetMesh()->numVertices; ++i) {
		Vector4 vertexPos = mvp * o->GetMesh()->vertices[i];
		vertexPos.SelfDivisionByW();

		Vector4 screenPos = portMatrix * vertexPos;
		ShadePixel((uint)screenPos.x, (uint)screenPos.y, Colour::White);
	}
}

void	SoftwareRasteriser::RasteriseLinesMesh(RenderObject*o) {
	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();

	for (uint i = 0; i < o->GetMesh()->numVertices; i += 2) {
		Vector4 v0 = mvp * o->GetMesh()->vertices[i];
		Vector4 v1 = mvp * o->GetMesh()->vertices[i + 1];

		Colour c0 = o->GetMesh()->colours[i]; 
		Colour c1 = o->GetMesh()->colours[i + 1];

		v0.SelfDivisionByW();
		v1.SelfDivisionByW();

		RasteriseLine(v0, v1, c0, c1);
	}
}

void	SoftwareRasteriser::RasteriseLineLoops(RenderObject*o) {
	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();

	for (uint i = 0; i < o->GetMesh()->numVertices-1; i += 1) {

		Vector4 v0 = mvp * o->GetMesh()->vertices[i];
		Vector4 v1 = mvp * o->GetMesh()->vertices[i + 1];

		Colour c0 = o->GetMesh()->colours[i];
		Colour c1 = o->GetMesh()->colours[i + 1];

		v0.SelfDivisionByW();
		v1.SelfDivisionByW();

		
		RasteriseLine(v0, v1, c0, c1);
	}
	Vector4 v0 = mvp * o->GetMesh()->vertices[0];
	Vector4 v1 = mvp * o->GetMesh()->vertices[o->GetMesh()->numVertices -1];

	Colour c0 = o->GetMesh()->colours[0];
	Colour c1 = o->GetMesh()->colours[o->GetMesh()->numVertices -1];

	v0.SelfDivisionByW();
	v1.SelfDivisionByW();

	
	RasteriseLine(v0, v1, c0, c1);
}


void	SoftwareRasteriser::RasteriseTriMesh(RenderObject*o) {
	Matrix4 mvp = viewProjMatrix * o->GetModelMatrix();
	
		for (uint i = 0; i < o->GetMesh()->numVertices; i += 3) {
		Vector4 v0 = mvp * o->GetMesh()->vertices[i];
		Vector4 v1 = mvp * o->GetMesh()->vertices[i + 1];
		Vector4 v2 = mvp * o->GetMesh()->vertices[i + 2];

			Vector3 t0 = Vector3(
			o->GetMesh()->textureCoords[i].x,
			o->GetMesh()->textureCoords[i].y, 1.0f) / v0.w;
		
			Vector3 t1 = Vector3(
			o->GetMesh()->textureCoords[i + 1].x,
			o->GetMesh()->textureCoords[i + 1].y, 1.0f) / v1.w;
		
			Vector3 t2 = Vector3(
			o->GetMesh()->textureCoords[i + 2].x,
			o->GetMesh()->textureCoords[i + 2].y, 1.0f) / v2.w;

		v0.SelfDivisionByW(); v1.SelfDivisionByW(); v2.SelfDivisionByW();


		RasteriseTri(v0, v1, v2, 
			o->GetMesh()->colours[i], 
			o->GetMesh()->colours[i + 1], 
			o->GetMesh()->colours[i + 2],
			t0,t1,t2
			);
	}
}




void SoftwareRasteriser::RasteriseLine(
	const Vector4 &vertA, const Vector4 &vertB,
	const Colour & colA, const Colour & colB,
	const Vector2 & texA, const Vector2 & texB) {

	Vector4 v0 = portMatrix * vertA;
	Vector4 v1 = portMatrix * vertB;
	Vector4 dir = v1 - v0;

	int xDir = (dir.x < 0.0f) ? -1 : 1; 
	int yDir = (dir.y < 0.0f) ? -1 : 1; 

	int x = (int)v0.x; 
	int y = (int)v0.y; 

	int * target = NULL;
	int * scan = NULL;
	int scanVal = 0;
	int targetVal = 0;

	float slope = 0.0f;

	int range = 0; 

	
	if (abs(dir.y) > abs(dir.x)) {
		slope = (dir.x / dir.y);
		range = (int)abs(dir.y);

		target = &x; 
		scan = &y; 
		scanVal = yDir;
		targetVal = xDir;

	}
	else { 
		slope = (dir.y / dir.x);
		range = (int)abs(dir.x);

		target = &y; 
		scan = &x; 
		scanVal = xDir;
		targetVal = yDir;
	}

	float absSlope = abs(slope);
	float error = 0.0f;

	float reciprocalRange = 1.0f / range; 

	for (int i = 0; i < range; ++i) {

		float t = i*reciprocalRange;
		Colour currentCol = colB*t + colA*(1.0f - t);

		ShadePixel(x, y, currentCol);

		error += absSlope;

		if (error > 0.5f) {
			error -= 1.0f;
			(*target) += targetVal; 
		}
		(*scan) += scanVal; 
	}
}

	BoundingBox SoftwareRasteriser::CalculateBoxForTri(
		const Vector4 &a, const Vector4 &b, const Vector4 &c) {
		BoundingBox box;
		
			box.topLeft.x = a.x; 
		box.topLeft.x = min(box.topLeft.x, b.x); 
		box.topLeft.x = min(box.topLeft.x, c.x); 
		box.topLeft.x = max(box.topLeft.x, 0.0f);
		
			box.topLeft.y = a.y;
		box.topLeft.y = min(box.topLeft.y, b.y);
		box.topLeft.y = min(box.topLeft.y, c.y);
		box.topLeft.y = max(box.topLeft.y, 0.0f); 
		
			box.bottomRight.x = a.x; 
		box.bottomRight.x = max(box.bottomRight.x, b.x); 
		box.bottomRight.x = max(box.bottomRight.x, c.x); 
		box.bottomRight.x = min(box.bottomRight.x, screenWidth); 
		
			box.bottomRight.y = a.y;
		box.bottomRight.y = max(box.bottomRight.y, b.y);
		box.bottomRight.y = max(box.bottomRight.y, c.y);
		box.bottomRight.y = min(box.bottomRight.y, screenHeight); 
		
			return box;
		
	}

	float SoftwareRasteriser::ScreenAreaOfTri(
		const Vector4 &a, const Vector4 &b, const Vector4 &c) {
		
			float area = ((a.x*b.y) + (b.x*c.y) + (c.x*a.y)) -
			((b.x*a.y) + (c.x*b.y) + (a.x*c.y)); 
		return area * 0.5f;
		
	}

	void SoftwareRasteriser::RasteriseTri(
		const Vector4 &triA, const Vector4 &triB, const Vector4 &triC,
		const Colour &colA, const Colour &colB, const Colour &colC,
		const Vector3 &texA, const Vector3 &texB, const Vector3 & texC) {
		
		Vector4 v0 = portMatrix * triA;
		Vector4 v1 = portMatrix * triB;
		Vector4 v2 = portMatrix * triC;
		
			BoundingBox b = CalculateBoxForTri(v0, v1, v2);
		
			float triArea = ScreenAreaOfTri(v0, v1, v2);
			float areaRecip = 1.0f / triArea;

			float subTriArea[3];
		Vector4 screenPos(0, 0, 0, 1);
		
			for (float y = b.topLeft.y; y < b.bottomRight.y; ++y) {
				for (float x = b.topLeft.x; x < b.bottomRight.x; ++x) {
					screenPos.x = x; 
					screenPos.y = y; 

					subTriArea[0] = abs(ScreenAreaOfTri(v0, screenPos, v1));
					subTriArea[1] = abs(ScreenAreaOfTri(v1, screenPos, v2));
					subTriArea[2] = abs(ScreenAreaOfTri(v2, screenPos, v0));

					float triSum = subTriArea[0] + subTriArea[1] + subTriArea[2];

					if (triSum > (triArea + 0.9f)) {
						continue;

					}
					if (triSum < 1.0f) { 
						continue;

					}

					float alpha = subTriArea[1] * areaRecip;
					float beta = subTriArea[2] * areaRecip;
					float gamma = subTriArea[0] * areaRecip;

					if (currentTexture){
						Vector3 subTex = (texA * alpha) + (texB * beta) + (texC* gamma);

						subTex.x /= subTex.z;
						subTex.y /= subTex.z;

						ShadePixel((int)x, (int)y, currentTexture->NearestTextSample(subTex));

					}
					else{
						Colour subColour = ((colA * alpha) +
						(colB * beta) +
						(colC * gamma));

					ShadePixel((uint)x, (uint)y, subColour);
				}
			}
			
		}
		
	}