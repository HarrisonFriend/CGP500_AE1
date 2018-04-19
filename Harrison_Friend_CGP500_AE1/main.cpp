//include header files
#include "text.h"
#include "renderer.h"
#include <kernel.h>
#include <user_service.h>
#include <pad.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <thread>
//#include "playmp3.h"
using namespace Solent;

////number of threads
//static const int num_threads = 10;
//
////this function will be called from a thread
//void call_from_thread(int tid)
//{
//	while (true)
//	{
//		printf("Launched by thread 0x%x \n", tid);
//		::sceKernelSleep(1);
//	}
//}

////#define FILENAME_MP3 "/app0/jungle2ch128kbps.mp3
//#define FILENAME_MP3 "/app0/allegro.mp3"

//program entry point
int main()
{
	////play .mp3 music file
	//DecodeAndPlayMp3(FILENAME_MP3);

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



	int playerScore = 0;

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

	//collision booleans
	bool xCollision = false;
	bool yCollision = false;
	
	//player mesh width and height
	float playerWidth = 0.1f;
	float playerHeight = 0.1f;

	//player x-coordinate for the left of the hitbox
	float playerPositionX1 = playerMesh->translation.getX();
	//player y-coordinate for the bottom of the hitbox
	float playerPositionY1 = playerMesh->translation.getY();
	//player x-coordinate for the right of the hitbox
	float playerPositionX2 = playerMesh->translation.getX() + playerWidth;
	//player y-coordinate for the top of the hitbox
	float playerPositionY2 = playerMesh->translation.getY() + playerHeight;

	//enemy mesh width and height
	float enemyWidth = 0.1f;
	float enemyHeight = 0.1f;

	//enemy x-coordinate for the left of the hitbox
	float enemyPositionX1 = enemyMesh->translation.getX();
	//enemy y-coordinate for the bottom of the hitbox
	float enemyPositionY1 = enemyMesh->translation.getY();
	//enemy x-coordinate for the right of the hitbox
	float enemyPositionX2 = enemyMesh->translation.getX() + enemyWidth;
	//enemy y-coordinate for the top of the hitbox
	float enemyPositionY2 = enemyMesh->translation.getY() + enemyHeight;

	float playerSpeed = 0.003f;
	float enemySpeed = 0.0025f;
	int enemyMoveDirection; //8 directions, 1 = up, 2 = up and right, 3 = right etc.

	//start drawing the triangles for 1000 frames then exit
	for (uint32_t frameIndex = 0; frameIndex < 1000; ++frameIndex)
	{
		while (true)
		{
			srand(time(0));
			enemyMoveDirection = (rand() % 8) + 1; //randomise enemy movement direction

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
						playerMesh->translation.setX(playerMesh->translation.getX() - playerSpeed);
					}
				}
				//move right
				if (data.buttons & SCE_PAD_BUTTON_RIGHT)
				{
					//check player doesn't go off screen
					if (playerMesh->translation.getX() < 1.0f)
					{
						playerMesh->translation.setX(playerMesh->translation.getX() + playerSpeed);
					}
				}
				//move up
				if (data.buttons & SCE_PAD_BUTTON_UP)
				{
					//check player doesn't go off screen
					if (playerMesh->translation.getY() < 1.0f)
					{
						playerMesh->translation.setY(playerMesh->translation.getY() + playerSpeed);
					}
				}
				//move down
				if (data.buttons & SCE_PAD_BUTTON_DOWN)
				{
					//check player doesn't go off screen
					if (playerMesh->translation.getY() > -1.0f)
					{
						playerMesh->translation.setY(playerMesh->translation.getY() - playerSpeed);
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

			//ENEMY MOVE CODE******************************************************************

			enemyMoveDirection = (rand() % 8) + 1; //randomise enemy movement direction
			if (enemyMoveDirection == 1) //up
			{
				enemyMesh->translation.setY(enemyMesh->translation.getY() + enemySpeed);
			}
			else if (enemyMoveDirection == 2) //up-right
			{
				enemyMesh->translation.setY(enemyMesh->translation.getY() + enemySpeed);
				enemyMesh->translation.setX(enemyMesh->translation.getX() + enemySpeed);
			}
			else if (enemyMoveDirection == 3) //right
			{
				enemyMesh->translation.setX(enemyMesh->translation.getX() + enemySpeed);
			}
			else if (enemyMoveDirection == 4) //right-down
			{
				enemyMesh->translation.setY(enemyMesh->translation.getY() - enemySpeed);
				enemyMesh->translation.setX(enemyMesh->translation.getX() + enemySpeed);
			}
			else if (enemyMoveDirection == 5) //down
			{
				enemyMesh->translation.setY(enemyMesh->translation.getY() - enemySpeed);
			}
			else if (enemyMoveDirection == 6) //down-left
			{
				enemyMesh->translation.setY(enemyMesh->translation.getY() - enemySpeed);
				enemyMesh->translation.setX(enemyMesh->translation.getX() - enemySpeed);
			}
			else if (enemyMoveDirection == 7) //left
			{
				enemyMesh->translation.setX(enemyMesh->translation.getX() - enemySpeed);
			}
			else if (enemyMoveDirection == 8) //left-up
			{
				enemyMesh->translation.setY(enemyMesh->translation.getY() + enemySpeed);
				enemyMesh->translation.setX(enemyMesh->translation.getX() - enemySpeed);
			}

			//player is to the left of the enemy
			if (enemyPositionX1 - playerPositionX2 < 0.2)
			{
				enemyMoveDirection == 3; //right
			}
			//player is to the right of the enemy
			else if (playerPositionX1 - enemyPositionX2 < 0.2)
			{
				enemyMoveDirection == 7; //left
			}
			//player is above the enemy
			else if (playerPositionY1 - enemyPositionY2 < 0.2)
			{
				enemyMoveDirection == 5; //down
			}
			//player is below the enemy
			else if (enemyPositionY1 - playerPositionY2 < 0.2)
			{
				enemyMoveDirection == 1; //up
			}

			//CHECK FOR COLLISION************************************************************************************

			//check if player collided with enemy, x-axis
			if ((playerPositionX2 >= enemyPositionX1) && (playerPositionX1 <= enemyPositionX2))
			{
				xCollision = true;
			}
			//check if player collided with enemy, x-axis part 2
			else if ((playerPositionX1 <= enemyPositionX2) && (playerPositionX2 >= enemyPositionX1))
			{
				xCollision = true;
			}
			//check if player collided with enemy, y-axis
			if ((playerPositionY1 <= enemyPositionY2) && (playerPositionY2 >= enemyPositionY1))
			{
				yCollision = true;
			}
			//check if player collided with enemy, y-axis part 2
			else if ((playerPositionY2 >= enemyPositionY1) && (playerPositionY1 <= enemyPositionY2))
			{
				yCollision = true;
			}
			//collide and add points to player's score
			if ((xCollision == true) && (yCollision == true))
			{
				playerScore += 50;
				xCollision = false;
				yCollision = false;
			}

			//small delay
			//sceKernelUsleep(100); //wait here for 100 us
			renderer.RenderLoop();
		}

		//THREADS*******************************************

		////create array of thread instances
		//thread t[num_threads];

		////launch a group of threads
		//for (int i = 0; i < num_threads; ++i)
		//{
		//	t[i] = std::thread(call_from_thread, i);
		//}

		//printf("Launched from the main\n");

		////join the threads with the main thread
		//for (int i = 0; i < num_threads; ++i)
		//{
		//	t[i].join();
		//}

		////wait here for 10 seconds
		//::sceKernelSleep(10);

		//tidy up and release handle
		scePadClose(handle);

		//tidy up before exiting
		renderer.Release();
	}
	return 0;
}