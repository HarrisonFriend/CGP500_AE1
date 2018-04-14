//include header files
#include "text.h"
#include "renderer.h"
#include <kernel.h>
#include <user_service.h>
#include <pad.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
using namespace Solent;

//program entry point
int main()
{
	srand(time(NULL));

	//renderer class
	Renderer renderer;

	//initialize/setup
	renderer.Create();

	//bool gamePaused = false;

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

	//*****SCORE TEXT ON SCREEN*****
	Text drawScoreOnScreen();

	//simple quad
	//2    3
	//+----+
	//|\   |
	//| \  |
	//|  \ |
	//|   \|
	//+----+
	//0    1


	//PLAYER MESH CODE**********************************************************************

	Mesh* playerMesh = renderer.CreateMesh();

	//load player image
	playerMesh->LoadTextureFile("snowman.bmp");

	//						           POSITION				   COLOUR				UV
	playerMesh->AddVertex(Vertex(-0.95f, -0.05f, 0.0f,    0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	playerMesh->AddVertex(Vertex(-0.85f, -0.05f, 0.0f,    0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	playerMesh->AddVertex(Vertex(-0.95f,  0.05f, 0.0f,    0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	playerMesh->AddVertex(Vertex(-0.85f,  0.05f, 0.0f,    1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	playerMesh->AddIndex(0, 1, 2);

	//triangle 2
	playerMesh->AddIndex(1, 3, 2);

	//create buffers for renderer
	playerMesh->BuildTriangleBuffer();

	//**************************************************************************************

	//ENEMY MESH CODE***********************************************************************

	Mesh* enemyMesh = renderer.CreateMesh();

	//load enemy image
	enemyMesh->LoadTextureFile("font.bmp");

	//						 POSITION				 COLOUR				  UV
	enemyMesh->AddVertex(Vertex(-0.05f, -0.05f, 0.0f,    0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	enemyMesh->AddVertex(Vertex( 0.05f, -0.05f, 0.0f,    0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	enemyMesh->AddVertex(Vertex(-0.05f,  0.05f, 0.0f,    0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	enemyMesh->AddVertex(Vertex( 0.05f,  0.05f, 0.0f,    1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	enemyMesh->AddIndex(0, 1, 2);

	//triangle 2
	enemyMesh->AddIndex(1, 3, 2);

	//create buffers for renderer
	enemyMesh->BuildTriangleBuffer();

	//**************************************************************************************

	//WALL 1 MESH CODE**********************************************************************

	Mesh* wall1Mesh = renderer.CreateMesh();

	//load wall image
	wall1Mesh->LoadTextureFile("test.bmp");

	//						         POSITION			  COLOUR			   UV
	wall1Mesh->AddVertex(Vertex(-1.0f, 0.1f,  0.0f,    0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	wall1Mesh->AddVertex(Vertex(-0.5f, 0.1f,  0.0f,    0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	wall1Mesh->AddVertex(Vertex(-1.0f, 0.15f, 0.0f,    0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	wall1Mesh->AddVertex(Vertex(-0.5f, 0.15f, 0.0f,    1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	wall1Mesh->AddIndex(0, 1, 2);

	//triangle 2
	wall1Mesh->AddIndex(1, 3, 2);

	//create buffers for renderer
	wall1Mesh->BuildTriangleBuffer();

	//**************************************************************************************

	//WALL 2 MESH CODE**********************************************************************

	Mesh* wall2Mesh = renderer.CreateMesh();

	//load wall image
	wall2Mesh->LoadTextureFile("test.bmp");

	//						         POSITION			   COLOUR			    UV
	wall2Mesh->AddVertex(Vertex(-1.0f, -0.15f, 0.0f,   0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	wall2Mesh->AddVertex(Vertex(-0.5f, -0.15f, 0.0f,   0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	wall2Mesh->AddVertex(Vertex(-1.0f, -0.1f,  0.0f,   0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	wall2Mesh->AddVertex(Vertex(-0.5f, -0.1f,  0.0f,   1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	wall2Mesh->AddIndex(0, 1, 2);

	//triangle 2
	wall2Mesh->AddIndex(1, 3, 2);

	//create buffers for renderer
	wall2Mesh->BuildTriangleBuffer();

	//**************************************************************************************

	//WALL 3 MESH CODE**********************************************************************

	Mesh* wall3Mesh = renderer.CreateMesh();

	//load wall image
	wall3Mesh->LoadTextureFile("test.bmp");

	//						         POSITION			   COLOUR			    UV
	wall3Mesh->AddVertex(Vertex(-0.5f,  0.1f, 0.0f,    0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	wall3Mesh->AddVertex(Vertex(-0.45f, 0.1f, 0.0f,    0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	wall3Mesh->AddVertex(Vertex(-0.5f,  0.8f, 0.0f,    0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	wall3Mesh->AddVertex(Vertex(-0.45f, 0.8f, 0.0f,    1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	wall3Mesh->AddIndex(0, 1, 2);

	//triangle 2
	wall3Mesh->AddIndex(1, 3, 2);

	//create buffers for renderer
	wall3Mesh->BuildTriangleBuffer();

	//**************************************************************************************

	//WALL 4 MESH CODE**********************************************************************

	Mesh* wall4Mesh = renderer.CreateMesh();

	//load wall image
	wall4Mesh->LoadTextureFile("test.bmp");

	//						         POSITION			   COLOUR			     UV
	wall4Mesh->AddVertex(Vertex(-0.5f,  -0.8f, 0.0f,    0.7f, 0.7f, 1.0f,    0.0f, 0.0f));
	wall4Mesh->AddVertex(Vertex(-0.45f, -0.8f, 0.0f,    0.7f, 0.7f, 1.0f,    1.0f, 0.0f));
	wall4Mesh->AddVertex(Vertex(-0.5f,  -0.1f, 0.0f,    0.7f, 1.0f, 1.0f,    0.0f, 1.0f));
	wall4Mesh->AddVertex(Vertex(-0.45f, -0.1f, 0.0f,    1.0f, 0.7f, 1.0f,    1.0f, 1.0f));

	//triangle 1
	wall4Mesh->AddIndex(0, 1, 2);

	//triangle 2
	wall4Mesh->AddIndex(1, 3, 2);

	//create buffers for renderer
	wall4Mesh->BuildTriangleBuffer();

	//**************************************************************************************

	//COLLISION CODE
	float playerWidth = 0.1f;
	float playerHeight = 0.1f;

	float playerPositionX1 = playerMesh->translation.getX();
	float playerPositionY1 = playerMesh->translation.getY();

	float playerPositionX2 = playerMesh->translation.getX() + playerWidth;
	float playerPositionY2 = playerMesh->translation.getY() + playerHeight;



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
					if (playerMesh->translation.getX() > -1.0f)
					{
						playerMesh->translation.setX(playerMesh->translation.getX() - 0.003f);
					}
				}
				//move right
				if (data.buttons & SCE_PAD_BUTTON_RIGHT)
				{
					//check player doesn't go off screen
					if (playerMesh->translation.getX() < 1.0f)
					{
						playerMesh->translation.setX(playerMesh->translation.getX() + 0.003f);
					}
				}
				//move up
				if (data.buttons & SCE_PAD_BUTTON_UP)
				{
					//check player doesn't go off screen
					if (playerMesh->translation.getY() < 1.0f)
					{
						playerMesh->translation.setY(playerMesh->translation.getY() + 0.003f);
					}
				}
				//move down
				if (data.buttons & SCE_PAD_BUTTON_DOWN)
				{
					//check player doesn't go off screen
					if (playerMesh->translation.getY() > -1.0f)
					{
						playerMesh->translation.setY(playerMesh->translation.getY() - 0.003f);
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