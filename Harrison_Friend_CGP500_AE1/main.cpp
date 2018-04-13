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

	bool gamePaused = false;

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

	//load player image
	m->LoadTextureFile("snowman.bmp");

	//						 POSITION				 COLOUR				  UV
	m->AddVertex(Vertex(-0.95f, -0.05f, 0.0f,    0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	m->AddVertex(Vertex(-0.85f, -0.05f, 0.0f,    0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	m->AddVertex(Vertex(-0.95f,  0.05f, 0.0f,    0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	m->AddVertex(Vertex(-0.85f,  0.05f, 0.0f,    1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	m->AddIndex(0, 1, 2);

	//triangle 2
	m->AddIndex(1, 3, 2);

	//create buffers for renderer
	m->BuildTriangleBuffer();

	//collision code
	float playerWidth = 0.1f;
	float playerHeight = 0.1f;

	float playerPositionX1 = m->translation.getX();
	float playerPositionY1 = m->translation.getY();

	float playerPositionX2 = m->translation.getX() + playerWidth;
	float playerPositionY2 = m->translation.getY() + playerHeight;

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
					//check player doesn't go off screen
					if (m->translation.getX() > -1.0f)
					{
						m->translation.setX(m->translation.getX() - 0.003f);
					}
				}
				//move right
				if (data.buttons & SCE_PAD_BUTTON_RIGHT)
				{
					//check player doesn't go off screen
					if (m->translation.getX() < 1.0f)
					{
						m->translation.setX(m->translation.getX() + 0.003f);
					}
				}
				//move up
				if (data.buttons & SCE_PAD_BUTTON_UP)
				{
					//check player doesn't go off screen
					if (m->translation.getY() < 1.0f)
					{
						m->translation.setY(m->translation.getY() + 0.003f);
					}
				}
				//move down
				if (data.buttons & SCE_PAD_BUTTON_DOWN)
				{
					//check player doesn't go off screen
					if (m->translation.getY() > -1.0f)
					{
						m->translation.setY(m->translation.getY() - 0.003f);
					}
				}
				//pause game
				if (data.buttons & SCE_PAD_BUTTON_START)
				{
					/*gamePaused = true;
					while (gamePaused == true)
					{

					}*/
				}
				//exit game
				if (data.buttons & SCE_PAD_BUTTON_CIRCLE)
				{
					return 0;
				}
			}
			//small delay
			//sceKernelUsleep(100); //wait here for 100 us
			renderer.RenderLoop();
		}

		//tidy up and release handle
		scePadClose(handle);

		//tidy up before exiting
		renderer.Release();
	}
	return 0;
}