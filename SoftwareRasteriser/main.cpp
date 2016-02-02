#include "SoftwareRasteriser.h"

#include "Mesh.h"
#include "Texture.h"

int main() {
	
	//This is my repo test


	SoftwareRasteriser r(1200, 738);
	

	Mesh * TestPoints = Mesh::GenerateStars();
	RenderObject *o1 = new RenderObject();
	o1->mesh = TestPoints;
	o1->modelMatrix = Matrix4::Translation(Vector3(0, 0, 0));

	RenderObject *q1 = new RenderObject();
	q1->mesh = Mesh::GenerateTriangle();
	q1->texture = Texture::TextureFromTGA("../brick.tga");
	q1->modelMatrix = Matrix4::Translation(Vector3(0, 0, -10));
	
	
	RenderObject *q2 = new RenderObject();
	q2->mesh = Mesh::GenerateTriangle();
	q2->texture = Texture::TextureFromTGA("../brick.tga");
	q2->modelMatrix = Matrix4::Translation(Vector3(0, 0, -10));
	q2->modelMatrix = q2->modelMatrix * Matrix4::Rotation(180, Vector3(0, 1, 0));


	RenderObject *A1 = new RenderObject();
	A1->mesh = Mesh::GenerateShapes(Vector3(-3, 0, -10), Vector3(-1, -1, -10), Vector3(2, -3, -10), Vector3(2, 1, -10), Vector3(0, 2, -10), Vector3(-1,3, -10));
	A1->modelMatrix = Matrix4::Translation(Vector3(-10, 0, -15));

	RenderObject *A2 = new RenderObject();
	A2->mesh = Mesh::GenerateShapes(Vector3(-2, 0, -20), Vector3(-2, -2, -20), Vector3(4, -3, -20), Vector3(3, 1, -20), Vector3(0, 1, -20), Vector3(-1, 4, -20));
	A2->modelMatrix = Matrix4::Translation(Vector3(10, 11, -20));

	float aspect =1200.0f / 738.0f;
	


	Matrix4 viewMatrix;
	float yaw = 0.0f;
	float yawY = 0.0f;

	r.SetProjectionMatrix(Matrix4::Perspective(1.0f, 45.0f, aspect, 45.0f));
	


	while(r.UpdateWindow()) {
		r.ClearBuffers();
		r.DrawObject(o1);		
		r.DrawObject(q1);
		r.DrawObject(q2);
		r.DrawObject(A1);
		r.DrawObject(A2);
		r.SwapBuffers();
		
		yaw = Mouse::GetRelativePosition().x;
		yawY = Mouse::GetRelativePosition().y;

		A1->modelMatrix = A1->modelMatrix * Matrix4::Rotation(0.3f, Vector3(0, 0, 1));
		A2->modelMatrix = A2->modelMatrix * Matrix4::Rotation(0.3f, Vector3(0, 0, -1));

	

		if (Mouse::ButtonDown(MOUSE_LEFT)) {
			o1 -> modelMatrix = o1->modelMatrix * Matrix4::Rotation(yaw, Vector3(0, 1, 0));
			o1 -> modelMatrix = o1->modelMatrix * Matrix4::Rotation(yawY, Vector3(1, 0, 0));
		}
		if (Keyboard::KeyDown(KEY_Q)){

			q1->modelMatrix = q1->modelMatrix *Matrix4::Rotation(0.3f, Vector3(0, 0, 0.5f));
			q2->modelMatrix = q2->modelMatrix *Matrix4::Rotation(-0.3f, Vector3(0, 0, 0.5f));
			o1->modelMatrix = o1->modelMatrix * Matrix4::Rotation(0.1f, Vector3(0, 0, 0.5f));

			

		}if (Keyboard::KeyDown(KEY_E)){
			q1->modelMatrix = q1->modelMatrix *Matrix4::Rotation(0.3f, Vector3(0, 0, -0.5f));
			q2->modelMatrix = q2->modelMatrix *Matrix4::Rotation(-0.3f, Vector3(0, 0, -0.5f));
			o1->modelMatrix = o1->modelMatrix * Matrix4::Rotation(0.1f, Vector3(0, 0, -0.5f));
		
			
		
		}
		
		if (Keyboard::KeyDown(KEY_SPACE)){
			
			q1->modelMatrix = Matrix4::Translation(Vector3(0, 0, -10));
			q2->modelMatrix = Matrix4::Translation(Vector3(0, 0, -10));
			q2->modelMatrix = q2->modelMatrix * Matrix4::Rotation(180, Vector3(0, 1, 0));
		}
		
		if (Keyboard::KeyDown(KEY_A)) {
			q1->modelMatrix = q1->modelMatrix * Matrix4::Translation(Vector3(-0.01f, 0, 0));
			q2->modelMatrix = q2->modelMatrix * Matrix4::Translation(Vector3(0.01f, 0, 0));
		
			o1->modelMatrix = o1->modelMatrix * Matrix4::Rotation(0.1f, Vector3(0, -0.01f, 0));


			
		}
		if (Keyboard::KeyDown(KEY_D)) {
			q1->modelMatrix = q1->modelMatrix * Matrix4::Translation(Vector3(0.01f, 0, 0));
			q2->modelMatrix = q2->modelMatrix * Matrix4::Translation(Vector3(-0.01f, 0, 0));
	
			o1->modelMatrix = o1->modelMatrix * Matrix4::Rotation(0.1f, Vector3(0, 0.01f, 0));

			
		}
		if (Keyboard::KeyDown(KEY_W)) {
			q1->modelMatrix = q1->modelMatrix * Matrix4::Translation(Vector3(0, 0.01f, 0));
			q2->modelMatrix = q2->modelMatrix * Matrix4::Translation(Vector3(0, 0.01f, 0));
	
			o1->modelMatrix = o1->modelMatrix * Matrix4::Rotation(0.1f, Vector3(-0.01f, 0, 0));
		
		}
		if (Keyboard::KeyDown(KEY_S)) {
			q1->modelMatrix = q1->modelMatrix * Matrix4::Translation(Vector3(0, -0.01f, 0));
			q2->modelMatrix = q2->modelMatrix * Matrix4::Translation(Vector3(0, -0.01f, 0));
			o1->modelMatrix = o1->modelMatrix * Matrix4::Rotation(0.1f, Vector3(0.01f, 0, 0));
	
			
		}if (Keyboard::KeyDown(KEY_UP)){
			q1->modelMatrix = q1->modelMatrix * Matrix4::Rotation(0.2f, Vector3(-0.01f, 0, 0));
			q2->modelMatrix = q2->modelMatrix * Matrix4::Rotation(-0.2f, Vector3(-0.01f, 0, 0));
		}if (Keyboard::KeyDown(KEY_DOWN)){
			q1->modelMatrix = q1->modelMatrix * Matrix4::Rotation(0.2f, Vector3(0.01f, 0, 0));
			q2->modelMatrix = q2->modelMatrix * Matrix4::Rotation(-0.2f, Vector3(0.01f, 0, 0));
		}



		r.SetViewMatrix(viewMatrix);
		
		
	}
	delete o1->mesh;
	delete o1;
	delete q1->mesh;
	delete q1;
	return 0;
}