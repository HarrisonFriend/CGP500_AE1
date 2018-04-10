//include header files for our PS4 renderer class
#include "renderer.h"
using namespace Solent;

//program entry point
int main()
{
	//renderer class
	Renderer renderer;

	//initialize/setup
	renderer.Create();

	//simple quad
	//2    3
	//+----+
	//|\   |
	//| \  |
	//|  \ |
	//|   \|
	//+----+
	//0    1

	Mesh* m = renderer.CreateMesh();

	//load texture for triangles
	m->LoadTextureFile("snowman.bmp"); //try different test images, font.bmp, test.bmp

	//						POSITION				COLOUR				UV
	m->AddVertex(Vertex(-0.5f, -0.5f, 0.0f,   0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	m->AddVertex(Vertex( 0.5f, -0.5f, 0.0f,   0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	m->AddVertex(Vertex(-0.5f,  0.5f, 0.0f,   0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	m->AddVertex(Vertex( 0.5f,  0.5f, 0.0f,   1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	m->AddIndex(0, 1, 2);

	//triangle 2
	m->AddIndex(1, 3, 2);

	//create buffers for renderer
	m->BuildTriangleBuffer();

	//start drawing the triangles for 1000 frames then exit
	for (uint32_t frameIndex = 0; frameIndex < 1000; ++frameIndex)
	{
		renderer.RenderLoop();
	}

	//tidy up before exiting
	renderer.Release();
}