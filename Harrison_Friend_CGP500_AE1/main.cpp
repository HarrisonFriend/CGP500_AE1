//include header files for our PS4 renderer class
#include "renderer.h"
#include <kernel.h>
#include <user_service.h>
#include <pad.h>
using namespace Solent;

//program entry point
int main()
{
	//renderer class
	Renderer renderer;

	//initialize/setup
	renderer.Create();

	int32_t ret = sceUserServiceInitialize(NULL);
	if (ret != 0) printf("sceUserServiceInitialize failed\n");

	SceUserServiceUserId userId;
	ret = sceUserServiceGetInitialUser(&userId);
	if (ret != 0) printf("sceUserServiceGetInitialiser failed\n");

	ret = scePadInit();
	if (ret != 0) printf("scePadInit failed\n");

	//example that specifies SCE_PAD_PORT_TYPE_STANDARD controller type
	int32_t handle = scePadOpen(userId, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);
	if (handle < 0) printf("scePadOpen failed\n");

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
	m->LoadTextureFile("snowman.bmp");

	//						POSITION				COLOUR				UV
	m->AddVertex(Vertex(-0.1f, -0.1f, 0.0f,    0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	m->AddVertex(Vertex( 0.1f, -0.1f, 0.0f,    0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	m->AddVertex(Vertex(-0.1f,  0.1f, 0.0f,    0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	m->AddVertex(Vertex( 0.1f,  0.1f, 0.0f,    1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	m->AddIndex(0, 1, 2);

	//triangle 2
	m->AddIndex(1, 3, 2);

	//create buffers for renderer
	m->BuildTriangleBuffer();

	//start drawing the triangles for 1000 frames then exit
	for (uint32_t frameIndex = 0; frameIndex < 1000; ++frameIndex)
	{
		while (true)
		{
			// Get the data for an individual controller
			ScePadData data;
			//obtain state of controller data
			ret = scePadReadState(handle, &data);

			//if data is obtained from the controller
			if (ret == 0)
			{
				//move left
				if (data.buttons & SCE_PAD_BUTTON_LEFT)
				{
					m->translation.setX(m->translation.getX() - 0.005f);
				}
				//move right
				if (data.buttons & SCE_PAD_BUTTON_RIGHT)
				{
					m->translation.setX(m->translation.getX() + 0.005f);
				}
				//move up
				if (data.buttons & SCE_PAD_BUTTON_UP)
				{
					m->translation.setY(m->translation.getY() + 0.005f);
				}
				//move down
				if (data.buttons & SCE_PAD_BUTTON_DOWN)
				{
					m->translation.setY(m->translation.getY() - 0.005f);
				}
				//pause game
				if (data.buttons & SCE_PAD_BUTTON_START)
				{
					
				}
				//exit game
				if (data.buttons & SCE_PAD_BUTTON_CIRCLE)
				{
					break;
				}
			}
			//small delay
			sceKernelUsleep(100); //wait here for 100 us
			renderer.RenderLoop();
		}

		//tidy up and release handle
		scePadClose(handle);

		//tidy up before exiting
		renderer.Release();
	}
	return 0;
}