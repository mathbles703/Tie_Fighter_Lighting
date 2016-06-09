#include "Utils.h"
#define WINDOW_TITLE_PREFIX "Week 5, Day 1 (ligthing)"

#include <iostream>
#include <sstream>

// To load the ply file
#include <string>
#include <fstream>
#include <vector>
#include "cVertex.h"
#include "cTriangle.h"
#include <sstream>		// Why isn't it stringstream.... really?
#include <iomanip>

//void LoadPlyFile(std::string filename, 
//	             std::vector< cVertex > &vecVerts,
//	             std::vector< cTriangle > &vecTris, 
//	             int &numberOfVerts, 
//	             int &numberOfTriangle );

//std::vector< cVertex >   g_vecVerts;
//std::vector< cTriangle > g_vecTris;
//int g_numberOfVerts = 0;
//int g_numberOfTriangles = 0;

#include "cGameObject.h"
#include "cMeshManager.h"
#include "CShaderManager/CGLShaderManager.h"	// Note: it's "C" here

#include <sstream>

cMeshManager* g_pTheMeshManager = 0;

IGLShaderManager* g_pTheShaderManager;

cGameObject* g_pDebugBall = 0;

bool g_bDebugLights = true;


int
  CurrentWidth = 800,
  CurrentHeight = 600,
  WindowHandle = 0;

unsigned FrameCount = 0;

GLint  ProjectionMatrixUniformLocation = 0;
GLint  ViewMatrixUniformLocation = 0;
GLint  ModelMatrixUniformLocation = 0;
GLint  ModelMatrixRotationOnlyUniformLocation = 0;

std::vector<cLightDesc> g_vecLights;

std::vector< cGameObject* > g_vec_pBullets;


// Fragment shader uniforms...
// For directly setting the ambient and diffuse (if NOT using textures)
GLint UniLoc_MaterialAmbient_RGB = 0;
GLint UniLoc_MaterialDiffuse_RGB = 0;
GLint UniLoc_MaterialSpecular = 0;
GLint UniLoc_MaterialShininess = 0;
GLint UniLoc_eye = 0;
GLint UniLoc_debugColour = 0;
GLint UniLoc_bUseDebugColour = 0;
GLint UniLoc_bUseVertexRGBAColours = 0;
GLint UniLoc_bUseTextureMaterials = 0;	
//
//GLint UniLoc_Light_0_position = 0;
//GLint UniLoc_Light_0_ambient = 0;
//GLint UniLoc_Light_0_diffuse = 0;
//GLint UniLoc_Light_0_specular = 0;
//GLint UniLoc_Light_0_attenConst = 0;
//GLint UniLoc_Light_0_attenLinear = 0;
//GLint UniLoc_Light_0_attenQuad = 0;
//
//GLint UniLoc_Light_1_position = 0;
//GLint UniLoc_Light_1_ambient = 0;
//GLint UniLoc_Light_1_diffuse = 0;
//GLint UniLoc_Light_1_specular = 0;
//GLint UniLoc_Light_1_attenConst = 0;
//GLint UniLoc_Light_1_attenLinear = 0;
//GLint UniLoc_Light_1_attenQuad = 0;

//GLuint  BufferIds[3] = { 0 };
        // BufferIds[0] = VAO (or VBO)
        // BufferIds[1] = vertex buffer ID
		// BufferIds[2] = index buffer ID
//GLuint  ShaderIds[3] = { 0 };

//Matrix
//  ProjectionMatrix,
//  ViewMatrix,
//  ModelMatrix;

glm::mat4 matProjection;
glm::mat4 matView;			// "camera"
glm::mat4 matWorld;			// "model"

float CubeRotation = 0;
clock_t LastTime = 0;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);

void SetupShader(void);

//void CreateCube(void);
//bool LoadMeshIntoVBO(std::string fileToLoad, unsigned int &VBO);

void DestroyCube(void);
// Add keyboard and mouse callbacks
void myKeyboardCallback( unsigned char keyCode,
	                     int x, int y );
// for arrow, PigUp, PigDown, F1, etc.
void mySpecialKeyboardCallback(int key, int x, int y);

void myMouseCallback();		// Look into this if you like

//void DrawCube(void);
void DrawObject(cGameObject* pGO);

void CreateTheObjects(void);

int g_selectedLightIndex = 0;

int main(int argc, char* argv[])
{
	std::cout << "Preparing OpenGL..." << std::endl;
  Initialize(argc, argv);

  std::cout << "Loading objects..." << std::endl;
  CreateTheObjects();

  ::g_cam_at = glm::vec3( 0.0f, 0.0f, 0.0f );
  ::g_cam_eye = glm::vec3( 0.0f, 0.0f, 10.0f );

  g_AniTimer.Reset( );
  g_AniTimer.Start( );

  std::cout << "Boom!" << std::endl;
  glutMainLoop();

  exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
  GLenum GlewInitResult;

  InitWindow(argc, argv);
  
  glewExperimental = GL_TRUE;
  GlewInitResult = glewInit();

  if (GLEW_OK != GlewInitResult) {
    fprintf(
      stderr,
      "ERROR: %s\n",
      glewGetErrorString(GlewInitResult)
    );
    exit(EXIT_FAILURE);
  }
  
  fprintf(
    stdout,
    "INFO: OpenGL Version: %s\n",
    glGetString(GL_VERSION)
  );

  glGetError();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  ExitOnGLError("ERROR: Could not set OpenGL depth testing options");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  ExitOnGLError("ERROR: Could not set OpenGL culling options");

//  ModelMatrix = IDENTITY_MATRIX;
//  ProjectionMatrix = IDENTITY_MATRIX;
//  ViewMatrix = IDENTITY_MATRIX;
//  TranslateMatrix(&ViewMatrix, 0, 0, -2);


  SetupShader();

  ::g_pTheMeshManager = new cMeshManager();

//  CreateCube();
  //unsigned int VBO_ID = 0;
  //if ( !::g_pTheMeshManager->LoadMeshIntoVBO("tie_Unit_BBox.ply", VBO_ID) )
  //{
	 // std::cout << "Oh no." << std::endl;
	 // exit(-1);
  //}
  //// Assume we are loading these just fine, thank you very much...
  //::g_pTheMeshManager->LoadMeshIntoVBO("bun_zipper_res3_1UnitBB.ply", VBO_ID);
  //::g_pTheMeshManager->LoadMeshIntoVBO("mig29_xyz.ply", VBO_ID);
  //::g_pTheMeshManager->LoadMeshIntoVBO("Seafloor2.ply", VBO_ID);
  //// Do more magic


	// Moved to "CreateTheObjects()"

  return;
}

void InitWindow(int argc, char* argv[])
{
  glutInit(&argc, argv);
  
  glutInitContextVersion(4, 0);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  glutSetOption(
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_ACTION_GLUTMAINLOOP_RETURNS
  );
  
  glutInitWindowSize(CurrentWidth, CurrentHeight);

  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

  WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

  if(WindowHandle < 1) {
    fprintf(
      stderr,
      "ERROR: Could not create a new rendering window.\n"
    );
    exit(EXIT_FAILURE);
  }
  
  glutReshapeFunc(ResizeFunction);
  glutDisplayFunc(RenderFunction);
  glutIdleFunc(IdleFunction);
  glutTimerFunc(0, TimerFunction, 0);
  glutCloseFunc(DestroyCube);
  // Register the keyboard callback
  glutKeyboardFunc(myKeyboardCallback);
  //glutKeyboardUpFunc()
  glutSpecialFunc(mySpecialKeyboardCallback);
}

void ResizeFunction(int Width, int Height)
{
  CurrentWidth = Width;
  CurrentHeight = Height;
  glViewport(0, 0, CurrentWidth, CurrentHeight);
  //ProjectionMatrix =
  //  CreateProjectionMatrix(
  //    60,
  //    (float)CurrentWidth / CurrentHeight,
  //    1.0f,
  //    100.0f
  //  );
  matProjection = glm::perspective(
	              // 60.0f,				// <-- not this
	              glm::radians(60.0f),  // <-- this instead
                  (float)CurrentWidth / CurrentHeight,
                  0.1f,		// Near 
                  10000.0f    // Far
   );	              

//  glUseProgram(ShaderIds[0]);
  ::g_pTheShaderManager->UseShaderProgram("basicShader");

//  glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, 
//	  ProjectionMatrix.m);
  glUniformMatrix4fv(ProjectionMatrixUniformLocation, 
	                 1, GL_FALSE, 
	                 glm::value_ptr(matProjection) );
//  glUseProgram(0);
  ::g_pTheShaderManager->UseShaderProgram(0);
}

// Will move this later... 
//cGameObject* pTheOneGO = 0;

void SetLightUniforms(void)
{
	  	for ( int index = 0; index != NUMBEROFLIGHTS; index++ )
	{
		glUniform4f( ::g_vecLights[index].UniLoc_position, 
					 ::g_vecLights[index].position.x, 
					 ::g_vecLights[index].position.y, 
					 ::g_vecLights[index].position.z, 
					 ::g_vecLights[index].position.w );
		glUniform4f( ::g_vecLights[index].UniLoc_diffuse, 
					 ::g_vecLights[index].diffuse.x, 
					 ::g_vecLights[index].diffuse.y, 
					 ::g_vecLights[index].diffuse.z, 
					 ::g_vecLights[index].diffuse.w );
		glUniform4f( ::g_vecLights[index].UniLoc_ambient, 
					 ::g_vecLights[index].ambient.x, 
					 ::g_vecLights[index].ambient.y, 
					 ::g_vecLights[index].ambient.z, 
					 ::g_vecLights[index].ambient.w );
		glUniform4f( ::g_vecLights[index].UniLoc_specular, 
					 ::g_vecLights[index].specular.x, 
					 ::g_vecLights[index].specular.y, 
					 ::g_vecLights[index].specular.z, 
					 ::g_vecLights[index].specular.w );
		glUniform1f( ::g_vecLights[index].UniLoc_attenConst, ::g_vecLights[index].attenConst);
		glUniform1f( ::g_vecLights[index].UniLoc_attenLinear, ::g_vecLights[index].attenLinear);
		glUniform1f( ::g_vecLights[index].UniLoc_attenQuad, ::g_vecLights[index].attenQuad);
		// ... and so on.. 
	}

	return;
}

void RenderFunction(void)
{
  ++FrameCount;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up a camera... 

  matView = glm::mat4(1.0f);

  // Look at the bunny
  ::g_cam_at = ::g_vec_pGOs[0]->position;

  //matView = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f),  // "eye"
	 //                   glm::vec3(0.0f, 0.0f, 0.0f),   // "At"
	 //                   glm::vec3(0.0f, 1.0f, 0.0f));  // up
  matView = glm::lookAt( ::g_cam_eye,  // "eye"
	                     ::g_cam_at,   // "At"
	                    glm::vec3(0.0f, 1.0f, 0.0f));  // up

  ::g_pTheShaderManager->UseShaderProgram("basicShader");

  glUniform3f( UniLoc_eye, ::g_cam_eye.x, ::g_cam_eye.y, ::g_cam_eye.z );

	SetLightUniforms();


	//glUniform4f( UniLoc_Light_0_position, 0.0f, 3.0f, 0.0f, 1.0f );	// "theLights[0].position"); 
	//glUniform4f( UniLoc_Light_0_ambient, 0.2f, 0.0f, 0.02f, 1.0f );	// "theLights[0].ambient");
	//glUniform4f( UniLoc_Light_0_diffuse, 1.0f, 0.0f, 0.0f, 1.0f );	// "theLights[0].diffuse");  
	//glUniform4f( UniLoc_Light_0_specular, 1.0f, 1.0f, 1.0f, 1.0f );	// "theLights[0].specular");  
	//glUniform1f( UniLoc_Light_0_attenConst, 0.1f );	// "theLights[0].attenConst");  
	//glUniform1f( UniLoc_Light_0_attenLinear, 0.1f );	// "theLights[0].attenLinear");  
	//glUniform1f( UniLoc_Light_0_attenQuad, 0.1f );	// "theLights[0].attenQuad");  

	//glUniform4f( UniLoc_Light_1_position, 3.0f, 3.0f, 0.0f, 1.0f );	// "theLights[0].position"); 
	//glUniform4f( UniLoc_Light_1_ambient, 0.0f, 0.2f, 0.0f, 1.0f );	// "theLights[0].ambient");
	//glUniform4f( UniLoc_Light_1_diffuse, 0.0f, 1.0f, 0.0f, 1.0f );	// "theLights[0].diffuse");  
	//glUniform4f( UniLoc_Light_1_specular, 1.0f, 1.0f, 1.0f, 1.0f );	// "theLights[0].specular");  
	//glUniform1f( UniLoc_Light_1_attenConst, 0.1f );	// "theLights[0].attenConst");  
	//glUniform1f( UniLoc_Light_1_attenLinear, 0.1f );	// "theLights[0].attenLinear");  
	//glUniform1f( UniLoc_Light_1_attenQuad, 0.1f );	// "theLights[0].attenQuad");  

	// Put light where tie fighter is...
	
	//engine lights
	::g_vecLights[0].position.x = ::g_vec_pGOs[0]->position.x - 0.9f;
	::g_vecLights[0].position.y = ::g_vec_pGOs[0]->position.y;
	::g_vecLights[0].position.z = ::g_vec_pGOs[0]->position.z - 0.55f;

	::g_vecLights[1].position.x = ::g_vec_pGOs[0]->position.x - 0.9f;
	::g_vecLights[1].position.y = ::g_vec_pGOs[0]->position.y;
	::g_vecLights[1].position.z = ::g_vec_pGOs[0]->position.z + 0.55f;

	//shooter thingy
	::g_vecLights[2].position.x = ::g_vec_pGOs[0]->position.x + 1.2f;
	::g_vecLights[2].position.y = ::g_vec_pGOs[0]->position.y;
	::g_vecLights[2].position.z = ::g_vec_pGOs[0]->position.z - 0.55f;

	::g_vecLights[3].position.x = ::g_vec_pGOs[0]->position.x + 1.2f;
	::g_vecLights[3].position.y = ::g_vec_pGOs[0]->position.y;
	::g_vecLights[3].position.z = ::g_vec_pGOs[0]->position.z + 0.55f;


	//testing for shooting
	//::g_vec_pBullets[0]->position = glm::vec3(::g_vecLights[2].position.x,
	//	::g_vecLights[2].position.y,
	//	::g_vecLights[2].position.z);
	//
	//DrawObject(::g_vec_pBullets[0]);

  for (std::vector< cGameObject* >::iterator itGO = ::g_vec_pGOs.begin();
	  itGO != ::g_vec_pGOs.end(); itGO++)
  {
	  cGameObject* pCurGO = *itGO;
	  DrawObject(pCurGO);
  }

  if ( g_bDebugLights )
  {
	// White, tiny ball at position
	::g_pDebugBall->position = glm::vec3(::g_vecLights[g_selectedLightIndex].position.x,
											::g_vecLights[g_selectedLightIndex].position.y, 
											::g_vecLights[g_selectedLightIndex].position.z);
	::g_pDebugBall->debugColour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	::g_pDebugBall->bUseDebugColour = true;
	::g_pDebugBall->scale = 0.1f;
	DrawObject(::g_pDebugBall);

	// At 75% brightness
	::g_pDebugBall->debugColour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	::g_pDebugBall->bUseDebugColour = true;
	::g_pDebugBall->scale 
		= ::g_vecLights[g_selectedLightIndex].calcDistanceAtBrightness(0.75f );
	DrawObject(::g_pDebugBall);

	// at 50% brightness
	::g_pDebugBall->debugColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	::g_pDebugBall->scale 
		= ::g_vecLights[g_selectedLightIndex].calcDistanceAtBrightness(0.50f);
	DrawObject(::g_pDebugBall);

	// at 25% 
	::g_pDebugBall->scale = 1.0f/ 0.01f;		// quad atten
	::g_pDebugBall->debugColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	::g_pDebugBall->scale 
		= ::g_vecLights[g_selectedLightIndex].calcDistanceAtBrightness(0.25f);
	DrawObject(::g_pDebugBall);
  }
  
  glutSwapBuffers();
}

void HandleIO(void)
{
	// Super Meat Boy...
	::g_vec_pGOs[0]->velocity = glm::vec3(0.0f);
	//::g_vec_pBullets[0]->velocity = glm::vec3(0.0f);
	::g_vecLights[0].attenQuad = 20.0f;
	::g_vecLights[1].attenQuad = 20.0f;

	bool bBunnyMoved = false;
	
	// Thruster bunny (Dead Space 2??)
	//::g_vecLights[0]->accel = glm::vec3(0.0f);
	//::g_vecLights[0]->velocity *= 0.99f;	

	if ( (GetAsyncKeyState('1') & 0x8000) != 0 )
	{	// Increase attenuation (making it darker)
		::g_vecLights[::g_selectedLightIndex].attenLinear *= 1.01f;
	}
	if ( (GetAsyncKeyState('2') & 0x8000) != 0 )
	{	// Decrease attenuation (making it darker)
		::g_vecLights[::g_selectedLightIndex].attenLinear *= 0.99f;
	}
	if ( (GetAsyncKeyState('3') & 0x8000) != 0 )
	{	// Increase attenuation (making it darker)
		::g_vecLights[::g_selectedLightIndex].attenQuad *= 1.001f;
	}
	if ( (GetAsyncKeyState('4') & 0x8000) != 0 )
	{	// Decrease attenuation (making it darker)
		::g_vecLights[::g_selectedLightIndex].attenQuad *= 0.999f;
	}

	// Ctrl pressed, move selected light, otherwise, move object zero(0)
	bool bCrtlPressed = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_RCONTROL) & 0x8000);

	const float lightMoveSpeed = 0.1f;

	if ( bCrtlPressed ) 
	{
		if ( (GetAsyncKeyState('A') & 0x8000) != 0 )
		{ 
			::g_vecLights[::g_selectedLightIndex].position.x -= lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('D') & 0x8000) != 0 )
		{ 
			::g_vecLights[::g_selectedLightIndex].position.x += lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('W') & 0x8000) != 0 ) 
		{ 
			::g_vecLights[::g_selectedLightIndex].position.z += lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('S') & 0x8000) != 0 ) 
		{ 
			::g_vecLights[::g_selectedLightIndex].position.z -= lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('Q') & 0x8000) != 0 ) 
		{ 
			::g_vecLights[::g_selectedLightIndex].position.y += lightMoveSpeed;
		}
		if ( (GetAsyncKeyState('E') & 0x8000) != 0 ) 
		{ 
			::g_vecLights[::g_selectedLightIndex].position.y -= lightMoveSpeed;
		}
	}
	else
	{	// Move object zero
		// TODO: Sexy code...
		if ( (GetAsyncKeyState('A') & 0x8000) != 0)
		{	// Move Bunny Left
			::g_vec_pGOs[0]->velocity.x = -2.0f;	bBunnyMoved = true;
			if (g_selectedLightIndex <= 1)
			{
				::g_vecLights[0].attenQuad = 3.5f;
				::g_vecLights[1].attenQuad = 3.5f;
			}
		}
		if ( (GetAsyncKeyState('D') & 0x8000) != 0 )
		{	// Move Bunny Right
			::g_vec_pGOs[0]->velocity.x = +2.0f;	bBunnyMoved = true;
			if (g_selectedLightIndex <= 1)
			{
				::g_vecLights[0].attenQuad = 3.5f;
				::g_vecLights[1].attenQuad = 3.5f;
			}
		}
		if ( (GetAsyncKeyState('W') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->velocity.z = +2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('S') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->velocity.z = -2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('Q') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->velocity.y = +2.0f;	bBunnyMoved = true;
		}
		if ( (GetAsyncKeyState('E') & 0x8000) != 0 )
		{
			::g_vec_pGOs[0]->velocity.y = -2.0f;	bBunnyMoved = true;
		}
		if ((GetAsyncKeyState('M') & 0x8000) != 0)
		{
			::g_vec_pBullets[0]->velocity.x = +2.0f;
		}
		if ((GetAsyncKeyState('N') & 0x8000) != 0)
		{
			::g_vecLights[0].attenQuad = 6.0f;
		}
		if ( bBunnyMoved )
		{
			std::cout << ::g_vec_pGOs[0]->position.x << ", " 
				<< ::g_vec_pGOs[0]->position.y << ", " 
				<< ::g_vec_pGOs[0]->position.z << std::endl;
		}
	}  // if ( bCrtlPressed ) 


	return;
}

void IdleFunction(void)
{
	//// Look at the bunny
	//::g_cam_at = ::g_vec_pGOs[0]->position;
	//SHORT keyState_A = GetAsyncKeyState( 'A' );	
//
	//if ( ( keyState_A & 0x8000) != 0 )
	//{	// 'A' Key is down, yo
	//	std::cout << "A is down" << std::endl;
	//}
	//else
	//{
	//	std::cout << "A is up" << std::endl;
	//}

	HandleIO();

	// Get elapsed secs since last idle (likely 0.1 ms)
	float deltaTime = g_AniTimer.GetElapsedSeconds( true );

	cGameObject* pBullet = 0;
	for (std::vector< cGameObject* >::iterator itB = ::g_vec_pBullets.begin();
		itB != ::g_vec_pBullets.end(); itB++)
	{
		pBullet = *itB;

		pBullet->velocity += pBullet->accel * deltaTime;
		pBullet->position += pBullet->velocity * deltaTime;
	}


	// This is technically "Explicit forward Euler integration"
	cGameObject* pCurGO = 0;
	for ( std::vector< cGameObject* >::iterator itGO = ::g_vec_pGOs.begin();
		  itGO != ::g_vec_pGOs.end(); itGO++ )
	{
		pCurGO = *itGO;

		// Doing x, y, and z at the same time
		pCurGO->velocity += pCurGO->accel * deltaTime;		// 1.0;

		pCurGO->position += pCurGO->velocity * deltaTime; 

		//pCurGO->velocity.x += pCurGO->accel.x * deltaTime;
		//pCurGO->velocity.y += pCurGO->accel.y * deltaTime;
		//pCurGO->velocity.z += pCurGO->accel.z * deltaTime;
	}



	glutPostRedisplay();
}

void TimerFunction(int Value)
{
  //if (0 != Value) {
  //  char* TempString = (char*)
  //    malloc(512 + strlen(WINDOW_TITLE_PREFIX));
//
  //  sprintf(
  //    TempString,
  //    "%s: %d Frames Per Second @ %d x %d",
  //    WINDOW_TITLE_PREFIX,
  //    FrameCount * 4,
  //    CurrentWidth,
  //    CurrentHeight
  //  );

	std::stringstream ssTitle;
	ssTitle << std::fixed << std::setprecision(3) 
		<< "Light# (" << ::g_selectedLightIndex << "): "
		<< ::g_vecLights[::g_selectedLightIndex].position.x 
		<< ", " << ::g_vecLights[::g_selectedLightIndex].position.y
		<< ", " << ::g_vecLights[::g_selectedLightIndex].position.z
		<< "; "
		<< ::g_vecLights[::g_selectedLightIndex].attenLinear 
		<< "; "
		<< ::g_vecLights[::g_selectedLightIndex].attenQuad;

    glutSetWindowTitle(ssTitle.str().c_str());

    //glutSetWindowTitle(TempString);
    //free(TempString);

  //FrameCount = 0;
  glutTimerFunc(100, TimerFunction, 1);
}

//std::vector< cVertex >	 g_vecVerts;
//std::vector< cTriangle >   g_vecTris;
//int g_numberOfVerts = 0;
//int g_numberOfIndices = 0;
//void LoadPlyFile(std::string filename,
//	             std::vector< cVertex > &vecVerts,
//	             std::vector< cTriangle > &vecTris,
//	             int &numberOfVerts,
//	             int &numberOfTriangles)
// **** MOVED INTO THE cMeshManager... 

// Formerly known as void CreateCube(void)
//bool LoadMeshIntoVBO( std::string fileToLoad, 
//	                  unsigned int &VBO )
//{
//	//LoadPlyFile("tie_Unit_BBox.ply");
//
//	std::vector< cVertex > vecVerts;
//	std::vector< cTriangle > vecTriangles;
//	int numberOfVertices = 0;
//	int numberOfTriangles = 0;
//
//	LoadPlyFile( fileToLoad, vecVerts, vecTriangles, 
//		         numberOfVertices, numberOfTriangles );
//
//
//	//const Vertex VERTICES[8] =
//	//  { { -0.5f, -.5f,  .5f, 1 }, { 1, 1, 1, 1 } },
//
//	//const Vertex VERTICES[8] =  
//	//{		                         // GLUT_RGBA
//		  //	// 1 0 1 (1 = alpha)
//	//  { { -0.5f, -.5f,  .5f, 1 }, { 1, 1, 1, 1 } },
//	//  { { -.5f,  .5f,  .5f, 1 },  { 1, 1, 1, 1 } },
//	//  { {  0.5f,  .5f,  .5f, 1 }, { 1, 1, 1, 1 } },
//	//  { {  .5f, -.5f,  .5f, 1 },  { 1, 1, 1, 1 } },
//	//  { { -.5f, -.5f, -.5f, 1 },  { 1, 1, 1, 1 } },
//	//  { { -.5f,  .5f, -.5f, 1 },  { 1, 1, 1, 1 } },
//	//  { {  .5f,  .5f, -.5f, 1 },  { 1, 1, 1, 1 } },
//	//  { {  .5f, -.5f, -.5f, 1 },  { 1, 1, 1, 1 } }
//	//};
//	Vertex* pVerts = new Vertex[numberOfVertices];
//	// Copy the vertices from the nice vector to the 
//	// raw, evil, array
//	for (int index = 0; index != numberOfVertices; index++)
//	{
//		pVerts[index].Position[0] = vecVerts[index].x;
//		pVerts[index].Position[1] = vecVerts[index].y;
//		pVerts[index].Position[2] = vecVerts[index].z;
//		pVerts[index].Position[3] = 1.0f;		// w coordinate
//
//		pVerts[index].Color[0] = 1.0f;	// red
//		pVerts[index].Color[1] = 1.0f;	// green
//		pVerts[index].Color[2] = 1.0f;	// green
//		pVerts[index].Color[3] = 1.0f;	// alpha (1.0 unless...)
//
//	}
//
////  const GLuint INDICES[36] = 0,2,1,  0,3,2,  and so on... 
//
//  //{
//  //  0,2,1,  0,3,2,
//  //  4,3,0,  4,7,3,
//  //  4,1,5,  4,0,1,
//  //  3,6,2,  3,7,6,
//  //  1,6,5,  1,2,6,
//  //  7,5,6,  7,4,5
//  //};
//	unsigned int numIndices = numberOfTriangles * 3;
//	GLuint* pIndices = new GLuint[numIndices];
//
//	int triIndex = 0;
//	for (; triIndex != numberOfTriangles;
//		triIndex++)
//	{
//		cTriangle tempTri = vecTriangles[triIndex];
//
//		pIndices[triIndex * 3 + 0] = tempTri.v0;
//		pIndices[triIndex * 3 + 1] = tempTri.v1;
//		pIndices[triIndex * 3 + 2] = tempTri.v2;
//	}
//
//
//  ShaderIds[0] = glCreateProgram();
//  ExitOnGLError("ERROR: Could not create the shader program");
//  {
//    ShaderIds[1] = LoadShader("SimpleShader.fragment.glsl", GL_FRAGMENT_SHADER);
//    ShaderIds[2] = LoadShader("SimpleShader.vertex.glsl", GL_VERTEX_SHADER);
//    glAttachShader(ShaderIds[0], ShaderIds[1]);
//    glAttachShader(ShaderIds[0], ShaderIds[2]);
//  }
//  glLinkProgram(ShaderIds[0]);
//  ExitOnGLError("ERROR: Could not link the shader program");
//
//  ModelMatrixUniformLocation 
//	  = glGetUniformLocation(ShaderIds[0], "ModelMatrix");
//
//  ViewMatrixUniformLocation 
//	  = glGetUniformLocation(ShaderIds[0], "ViewMatrix");
//
//  ProjectionMatrixUniformLocation 
//	  = glGetUniformLocation(ShaderIds[0], "ProjectionMatrix");
//
//  ExitOnGLError("ERROR: Could not get shader uniform locations");
//
//  glGenVertexArrays(1, &BufferIds[0]);
//  ExitOnGLError("ERROR: Could not generate the VAO");	// AKA VBO
//  glBindVertexArray(BufferIds[0]);
//  ExitOnGLError("ERROR: Could not bind the VAO");
//
//  glEnableVertexAttribArray(0);
//  glEnableVertexAttribArray(1);
//  ExitOnGLError("ERROR: Could not enable vertex attributes");
//
//  glGenBuffers(2, &BufferIds[1]);
//  ExitOnGLError("ERROR: Could not generate the buffer objects");
//
//  glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
//  glBufferData(GL_ARRAY_BUFFER, 
//			   sizeof(Vertex) * numberOfVertices,	// sizeof(VERTICES), 
//	           pVerts,								// VERTICES,
//	           GL_STATIC_DRAW);
//  ExitOnGLError("ERROR: Could not bind the VBO to the VAO");
//
//  Vertex tempVert;
//
//  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
//						sizeof(tempVert),	//  sizeof(VERTICES[0]),
//	                    (GLvoid*)0);	// At start of each vertex
//
//  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
//	                    sizeof(tempVert),	// sizeof(VERTICES[0]), 
//	                    (GLvoid*)sizeof(tempVert.Position) );	// (GLvoid*)sizeof(VERTICES[0].Position));
//
//  ExitOnGLError("ERROR: Could not set VAO attributes");
//
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
//
//  unsigned int sizeOfIndexArray =
//	  sizeof(GLuint) * numberOfTriangles * 3;
//
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
//				sizeOfIndexArray,	// sizeof(INDICES),
//				(GLvoid*)pIndices,			// INDICES,
//	            GL_STATIC_DRAW);
//
//  ExitOnGLError("ERROR: Could not bind the IBO to the VAO");
//
//  glBindVertexArray(0);
//
//  // Clean up
//  delete [] pVerts;			// note odd syntax
//  delete [] pIndices;		// note odd syntax
//
//  // Assume we are good to go...
//  VBO = BufferIds[0]; // "return" the VBO (VAO) ID
//  return true;
//}

bool bCheckShaderCompileStatus(GLuint shaderID, std::string errors)
{
	GLint statusOK; 
	::glGetObjectParameterivARB( shaderID , GL_OBJECT_COMPILE_STATUS_ARB, &statusOK );
	if ( !statusOK )
	{
		// This gets the 'last' error message for that shader (if there was one)
		GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
		glGetInfoLogARB( shaderID , GL_INFO_LOG_LENGTH, NULL, infoLog );
		std::stringstream ss;
		ss << infoLog << std::endl;
		errors = ss.str();
		return false;
	}
	// No errors
	return true;
}

bool bCheckShaderLinkStatus(GLuint shaderID, std::string errors)
{
	GLint statusOK; 
	::glGetObjectParameterivARB( shaderID , GL_LINK_STATUS, &statusOK );
	if ( !statusOK )
	{
		// This gets the 'last' error message for that shader (if there was one)
		GLcharARB infoLog[ GL_INFO_LOG_LENGTH ];	// defined in glext.h
		glGetInfoLogARB( shaderID , GL_INFO_LOG_LENGTH, NULL, infoLog );
		std::stringstream ss;
		ss << infoLog << std::endl;
		errors = ss.str();
		return false;
	}
	// No errors
	return true;
}

void SetupShader(void)
{
//	std::string error;
//	
	::g_pTheShaderManager = new CGLShaderManager();
	
	CShaderDescription vertShader;
//	vertShader.filename = "assets/shaders/SimpleShader.vertex.glsl";
	vertShader.filename = "assets/shaders/MultiLightsTextures.vertex.glsl";
	vertShader.name = "basicVert";
	vertShader.type = GLSHADERTYPES::VERTEX_SHADER;
	
	CShaderDescription fragShader;
//	fragShader.filename = "assets/shaders/SimpleShader.fragment.glsl";
	fragShader.filename = "assets/shaders/MultiLightsTextures.fragment.glsl";
	fragShader.name = "basicFrag";
	fragShader.type = GLSHADERTYPES::FRAGMENT_SHADER;
	
	CShaderProgramDescription basicShaderProg;
	basicShaderProg.name = "basicShader";
	basicShaderProg.vShader = vertShader;
	basicShaderProg.fShader = fragShader;
	
	if ( ! ::g_pTheShaderManager->CreateShaderProgramFromFile(basicShaderProg) )
	{	// Oh no, Mr. Bill!
		if ( ! basicShaderProg.bIsOK )
		{
			std::cout << "Error compiling the shader program" << std::endl;
			// Insert sexy error handling, logging code here... 
			for (std::vector<std::string>::iterator itError 
				 = basicShaderProg.vecErrors.begin();
				 itError != basicShaderProg.vecErrors.end(); 
				 itError++ )
			{
				std::cout << *itError << std::endl;
			}
			for (std::vector<std::string>::iterator itError 
				 = basicShaderProg.vShader.vecShaderErrors.begin();
				 itError != basicShaderProg.vShader.vecShaderErrors.end(); 
				 itError++ )
			{
				std::cout << *itError << std::endl;
			}	
			for (std::vector<std::string>::iterator itError 
				 = basicShaderProg.fShader.vecShaderErrors.begin();
				 itError != basicShaderProg.fShader.vecShaderErrors.end(); 
				 itError++ )
			{
				std::cout << *itError << std::endl;
			}		
		}
	}// if ( ! ::g_pTheShaderManager->...
	
	// Assume we are good to go...
	::g_pTheShaderManager->UseShaderProgram("basicShader");

	//ShaderIds[0] = glCreateProgram();
	//ExitOnGLError("ERROR: Could not create the shader program");
	//{
	//	ShaderIds[1] = LoadShader("assets/shaders/SimpleShader.fragment.glsl", GL_FRAGMENT_SHADER);
	//	ShaderIds[2] = LoadShader("assets/shaders/SimpleShader.vertex.glsl", GL_VERTEX_SHADER);
	//	glAttachShader(ShaderIds[0], ShaderIds[1]);
	//	glAttachShader(ShaderIds[0], ShaderIds[2]);
	//}
	//glLinkProgram(ShaderIds[0]);
	//ExitOnGLError("ERROR: Could not link the shader program");

	GLuint shaderID = ::g_pTheShaderManager->GetShaderIDFromName("basicShader");

	ModelMatrixUniformLocation
		= glGetUniformLocation(shaderID /*ShaderIds[0]*/, "ModelMatrix");

	ViewMatrixUniformLocation
		= glGetUniformLocation(shaderID /*ShaderIds[0]*/, "ViewMatrix");

	ProjectionMatrixUniformLocation
		= glGetUniformLocation(shaderID /*ShaderIds[0]*/, "ProjectionMatrix");

	ModelMatrixRotationOnlyUniformLocation
		= glGetUniformLocation(shaderID, "ModelMatrixRotOnly");


	UniLoc_MaterialAmbient_RGB = glGetUniformLocation(shaderID, "myMaterialAmbient_RGB");
	UniLoc_MaterialDiffuse_RGB = glGetUniformLocation(shaderID, "myMaterialDiffuse_RGB");
	UniLoc_MaterialSpecular = glGetUniformLocation(shaderID, "myMaterialSpecular"); 
	UniLoc_MaterialShininess = glGetUniformLocation(shaderID, "myMaterialShininess"); 

	UniLoc_eye = glGetUniformLocation(shaderID, "eye"); 

	UniLoc_debugColour = glGetUniformLocation(shaderID,  "debugColour");
	UniLoc_bUseDebugColour = glGetUniformLocation(shaderID,  "bUseDebugColour");
	UniLoc_bUseVertexRGBAColours = glGetUniformLocation(shaderID,  "bUseVertexRGBAColours");
	UniLoc_bUseTextureMaterials = glGetUniformLocation(shaderID,  "bUseTextureMaterials");

	for ( int index = 0; index != NUMBEROFLIGHTS; index++ )
	{
		cLightDesc curLight;
		
		{
			std::stringstream ssPosition;
			ssPosition << "theLights[" << index << "].position";
			//UniLoc_Light_0_position = glGetUniformLocation(shaderID, "theLights[0].position"); 
			curLight.UniLoc_position = glGetUniformLocation(shaderID, ssPosition.str().c_str() );
		}
		{
			std::stringstream ssAmbient;		
			ssAmbient << "theLights[" << index << "].ambient";
			curLight.UniLoc_ambient = glGetUniformLocation(shaderID, ssAmbient.str().c_str() );
		}
		{
			std::stringstream ssDiffuse;		
			ssDiffuse << "theLights[" << index << "].diffuse";
			curLight.UniLoc_diffuse = glGetUniformLocation(shaderID, ssDiffuse.str().c_str() );
		}
		{
			std::stringstream ssSpecular;		
			ssSpecular << "theLights[" << index << "].specular";
			curLight.UniLoc_specular = glGetUniformLocation(shaderID, ssSpecular.str().c_str() );
		}
		{
			std::stringstream ssAttenConst;		
			ssAttenConst << "theLights[" << index << "].attenConst";
			curLight.UniLoc_attenConst = glGetUniformLocation(shaderID, ssAttenConst.str().c_str() );
		}
		{
			std::stringstream ssAttenLinear;		
			ssAttenLinear << "theLights[" << index << "].attenLinear";
			curLight.UniLoc_attenLinear = glGetUniformLocation(shaderID, ssAttenLinear.str().c_str() );
		}
		{
			std::stringstream ssAttenQuad;		
			ssAttenQuad << "theLights[" << index << "].attenQuad";
			curLight.UniLoc_attenQuad = glGetUniformLocation(shaderID, ssAttenQuad.str().c_str() );
		}


		::g_vecLights.push_back( curLight );
	}


 /*

	UniLoc_Light_0_attenQuad = glGetUniformLocation(shaderID, "theLights[0].attenQuad");  

	UniLoc_Light_1_position = glGetUniformLocation(shaderID, "theLights[1].position"); 
	UniLoc_Light_1_ambient = glGetUniformLocation(shaderID, "theLights[1].ambient");
	UniLoc_Light_1_diffuse = glGetUniformLocation(shaderID, "theLights[1].diffuse");  
	UniLoc_Light_1_specular = glGetUniformLocation(shaderID, "theLights[1].specular");  
	UniLoc_Light_1_attenConst = glGetUniformLocation(shaderID, "theLights[1].attenConst");  
	UniLoc_Light_1_attenLinear = glGetUniformLocation(shaderID, "theLights[1].attenLinear");  
	UniLoc_Light_1_attenQuad = glGetUniformLocation(shaderID, "theLights[1].attenQuad");  */


	ExitOnGLError("ERROR: Could not get shader uniform locations");

	return;
}

void DestroyCube()
{
 //glDetachShader(ShaderIds[0], ShaderIds[1]);
 //glDetachShader(ShaderIds[0], ShaderIds[2]);
 //glDeleteShader(ShaderIds[1]);
 //glDeleteShader(ShaderIds[2]);
 //glDeleteProgram(ShaderIds[0]);
 //ExitOnGLError("ERROR: Could not destroy the shaders");

  ::g_pTheShaderManager->ShutDown();


//  glDeleteBuffers(2, &BufferIds[1]);
//  glDeleteVertexArrays(1, &BufferIds[0]);

  delete ::g_pTheMeshManager;
//  delete ::g_pTheShaderManager;
  // Aslo will delete the VAOs... 

  ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

//void DrawCube(void)
void DrawObject( cGameObject* pGO )
{
  float CubeAngle;
  clock_t Now = clock();

  if (LastTime == 0)
    LastTime = Now;

  CubeRotation += 45.0f * ((float)(Now - LastTime) / CLOCKS_PER_SEC);
  CubeAngle = DegreesToRadians(CubeRotation);
  LastTime = Now;

//  ModelMatrix = IDENTITY_MATRIX;
//  RotateAboutY(&ModelMatrix, CubeAngle);
//  RotateAboutX(&ModelMatrix, CubeAngle);
  //matView = glm::mat4(1.0f);

  matWorld = glm::mat4(1.0f);		// identity matrix

  glm::mat4 matWorldRotOnly(1.0f);		// For later (lighting)

  // STARTOF: From the Guts file... 
  // Rotation (post)
  matWorld = glm::rotate(matWorld, pGO->postRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  matWorld = glm::rotate(matWorld, pGO->postRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  matWorld = glm::rotate(matWorld, pGO->postRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->postRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->postRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->postRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  // Translation 
  matWorld = glm::translate(matWorld, 
	                        glm::vec3(pGO->position.x,
								      pGO->position.y,
								      pGO->position.z));

  // Rotation (pre)
  matWorld = glm::rotate(matWorld, pGO->preRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  matWorld = glm::rotate(matWorld, pGO->preRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  matWorld = glm::rotate(matWorld, pGO->preRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->preRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->preRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  //matWorldRotOnly = glm::rotate(matWorldRotOnly, pGO->preRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

  // Or you can use an inverse transpose of the model matrix.
  // Strips off translation and scaling
  matWorldRotOnly = glm::inverse(glm::transpose(matWorld));

  // Scale 
  matWorld = glm::scale(matWorld, glm::vec3(pGO->scale, 
	                                        pGO->scale,
	                                        pGO->scale));
  // ENDOF: From the Guts file

  //glUseProgram(ShaderIds[0]);
  //ExitOnGLError("ERROR: Could not use the shader program");

  ::g_pTheShaderManager->UseShaderProgram("basicShader");


  glUniformMatrix4fv(ModelMatrixUniformLocation, 1, GL_FALSE, 
	                 glm::value_ptr(matWorld) );

  glUniformMatrix4fv(ViewMatrixUniformLocation, 1, GL_FALSE, 
	                 glm::value_ptr(matView) );

  glUniformMatrix4fv(ModelMatrixRotationOnlyUniformLocation, 1, GL_FALSE, 
	                 glm::value_ptr(matWorldRotOnly) );

	if ( pGO->bUseDebugColour )
	{
		glUniform4f( UniLoc_debugColour, pGO->debugColour.r, pGO->debugColour.g, 
					                     pGO->debugColour.b, pGO->debugColour.a );
		// Boolean 
		glUniform1f( UniLoc_bUseDebugColour, 1.0f /*TRUE*/ );	// Non zero
		//glUniform1i( UniLoc_bUseDebugColour, 1 /*TRUE*/ );	// Non zero)
	}
	else
	{	// Set the unform material stuff for the object
		glUniform1f( UniLoc_bUseDebugColour, 0.0f /*FALSE*/ );	// Non zero

		glUniform3f( UniLoc_MaterialAmbient_RGB, pGO->ambient.r, pGO->ambient.g, pGO->ambient.b );
		glUniform3f( UniLoc_MaterialDiffuse_RGB, pGO->diffuse.r, pGO->diffuse.g, pGO->diffuse.b );
		glUniform3f( UniLoc_MaterialSpecular, pGO->specular.r, pGO->specular.g, pGO->specular.b );
		glUniform1f( UniLoc_MaterialShininess, pGO->shininess );
	}
 
	glUniform1f( UniLoc_bUseVertexRGBAColours, 0.0f /*FALSE*/ );	// Non zero
	glUniform1f( UniLoc_bUseTextureMaterials, 0.0f /*FALSE*/ );		// Non zero

	ExitOnGLError("ERROR: Could not set the shader uniforms");

	std::string modelToDraw = pGO->modelName;

	cVBOInfo curVBO;
	if (!::g_pTheMeshManager->LookUpVBOInfoFromModelName(modelToDraw, curVBO))
	{ // Didn't find it.
		return;
	}

//  glBindVertexArray(BufferIds[0]);
  glBindVertexArray(curVBO.VBO_ID);


  ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

  // Make everything lines ("wireframe")
  if ( pGO->bIsWireframe )
  {
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  glDisable(GL_CULL_FACE);	// Enable "backface culling
  }
  else
  {  
	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	  glEnable(GL_CULL_FACE);	// Enable "backface culling
  }
	  
  unsigned int numberOfIndicesToDraw = curVBO.numberOfTriangles * 3;

  glDrawElements(GL_TRIANGLES, numberOfIndicesToDraw,	// 36,
	             GL_UNSIGNED_INT, 
	             (GLvoid*)0);
  ExitOnGLError("ERROR: Could not draw the cube");

  glBindVertexArray(0);
//  glUseProgram(0);
  ::g_pTheShaderManager->UseShaderProgram(0);

  return;
}

// GLUT keyboard callback handler
void myKeyboardCallback(unsigned char keyCode, int x, int y)
{
	// Get # seconds, then reset
	//float deltaTime = g_AniTimer.GetElapsedSeconds( true );

	//std::cout << "delta Time = " << deltaTime << std::endl;

	//const float speedPerSecond = 1.0f;
	//const float rotSpeedPerSecond = 2.0f;

	//float speed = speedPerSecond * deltaTime;
	//float rotSpeed = rotSpeedPerSecond * deltaTime;


	switch (keyCode)
	{	// WASD  QE    vs arrows

		// Change selected light
	case '+':
		::g_selectedLightIndex++;
		if ( ::g_selectedLightIndex >= NUMBEROFLIGHTS ) 
		{ ::g_selectedLightIndex = 0; }
		break;
	case '-':
		::g_selectedLightIndex--;
		if ( ::g_selectedLightIndex < 0 ) 
		{ ::g_selectedLightIndex = NUMBEROFLIGHTS - 1; }
		break;
	case '(':	// Turn on light debugging
		::g_bDebugLights = true;
		break;
	case ')':
		::g_bDebugLights = false;
		break;


	case 'l': case 'L':
		//glUniform4f( lightPosition, x, y, z);

		//::g_vecLights[0].position.x += 1.0f;

//		::g_vec_pGOs[0]->colour = glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f );
		break;

	case 'o': case 'O':		// Note radians
//		::g_vec_pGOs[0]->preRotation.y += glm::radians(rotSpeed);
//		::g_vec_pGOs[0]->postRotation.y += glm::radians(rotSpeed);
		break;

	case 'w': case 'W':		// Up
//		::g_vec_pGOs[0]->position.y += speed;
		break;
	case 's': case 'S':
//		::g_vec_pGOs[0]->position.y -= speed;
		break;

	case 'a': case 'A':		// "Left"
//		::g_vec_pGOs[0]->position.x -= speed;
		break;
	
	case 'd': case 'D':		// "Right"
//		::g_vec_pGOs[0]->position.x += speed;
		break;

	case 'q': case 'Q':		// "Away???"
//		::g_vec_pGOs[0]->position.z -= speed;
		break;
	
	case 'e': case 'E':		// "Towards???"
//		::g_vec_pGOs[0]->position.z += speed;
		break;

	default:
		// Because it's just the right thing to do...
		break;
	}

	return;
}

void mySpecialKeyboardCallback(int key, int x, int y)
{
	float camSpeed = 0.1f;

	switch (key)
	{
	case GLUT_KEY_UP:		// "Away??"
		::g_cam_eye.z += camSpeed;  
		break;
	case GLUT_KEY_DOWN:
		::g_cam_eye.z -= camSpeed;	
		break;

	case GLUT_KEY_LEFT:		// "Away??"
		::g_cam_eye.x -= camSpeed;  
		break;
	case GLUT_KEY_RIGHT:
		::g_cam_eye.x += camSpeed;	
		break;

	case GLUT_KEY_PAGE_UP:
		::g_cam_eye.y += camSpeed;
		break;
	case GLUT_KEY_PAGE_DOWN:
		::g_cam_eye.y -= camSpeed;
		break;

	case GLUT_KEY_F1:
		std::cout << "You pressed F1" << std::endl;
		break;

	}

	return;
}

void CreateTheObjects(void)
{
	// Now with more ply...
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/tie_Unit_BBox.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/bun_zipper_hiRez.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/mig29_xyz.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/Seafloor2.ply");
	::g_pTheMeshManager->LoadPlyIntoVBO("assets/models/Isoshphere.ply");


	g_pDebugBall = new cGameObject();
	g_pDebugBall->modelName = "assets/models/Isoshphere.ply";
	g_pDebugBall->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	g_pDebugBall->bUseDebugColour = true;
	g_pDebugBall->bIsWireframe = true;
	g_pDebugBall->scale = 1.0f;
	g_pDebugBall->bIsADebugObject = true;

	cGameObject* pBullet1 = new cGameObject();
	pBullet1->modelName = "assets/models/Isoshphere.ply";
	pBullet1->scale = 0.4f;
	pBullet1->bUseDebugColour = true;
	pBullet1->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pBullet1->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	cGameObject* pBullet2 = new cGameObject();
	pBullet2->modelName = "assets/models/Isoshphere.ply";
	pBullet2->scale = 0.4f;
	pBullet2->bUseDebugColour = true;
	pBullet2->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pBullet2->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	::g_vec_pBullets.push_back(pBullet1);
	::g_vec_pBullets.push_back(pBullet2);

	// Tie is object zero 
	cGameObject* pTieFighter = new cGameObject();
	pTieFighter->modelName = "assets/models/tie_Unit_BBox.ply";
	pTieFighter->debugColour = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	pTieFighter->diffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	pTieFighter->ambient = glm::vec4(0.3f, 0.0f, 0.3f, 1.0f);
	pTieFighter->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pTieFighter->scale = 2.0f;
	pTieFighter->preRotation.x = glm::radians( 90.0f );
	pTieFighter->preRotation.z = glm::radians( 90.0f );
	pTieFighter->shininess = 1.0f;


	cGameObject* pBunny = new cGameObject();
	pBunny->modelName = "assets/models/bun_zipper_hiRez.ply";
	pBunny->debugColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pBunny->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	pBunny->ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	pBunny->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pBunny->shininess = 1000.0f;			// 1.0 to 100,000 
	pBunny->scale = 50.0f;
	//pBunny->position.x = -3.0f;
	pBunny->position = glm::vec3(-1.96708f, -3.57167f, -2.23921f);


	cGameObject* pMig = new cGameObject();
	pMig->modelName = "assets/models/mig29_xyz.ply";
	pMig->debugColour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	pMig->diffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	pMig->ambient = glm::vec4(0.0f, 0.2f, 0.0f, 1.0f);
	pMig->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pMig->shininess = 1.0f;
	pMig->position.x = 3.0f;

	cGameObject* pGround = new cGameObject();
	pGround->modelName = "assets/models/Seafloor2.ply";
	pGround->debugColour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	pGround->diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	pGround->ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	pGround->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pGround->shininess = 1.0f;
	pGround->scale = 0.1f;
	pGround->position.y = -3.0f;


	::g_vec_pGOs.push_back( pTieFighter );
	::g_vec_pGOs.push_back( pBunny );
	::g_vec_pGOs.push_back( pMig );
	::g_vec_pGOs.push_back( pGround );

	// Assume the lights are loaded into the g_vecLights;
	::g_vecLights[0].position = glm::vec4( 0.0f, 3.0f, 0.0f, 1.0f );
	::g_vecLights[0].diffuse =glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f );
	::g_vecLights[0].ambient =glm::vec4( 0.0f, 0.2f, 0.0f, 1.0f );
	::g_vecLights[0].specular =glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	::g_vecLights[0].attenConst = 0.0f;
	::g_vecLights[0].attenLinear = 0.0f;
	::g_vecLights[0].attenQuad = 20.0f;

	// Assume the lights are loaded into the g_vecLights;
	::g_vecLights[1].position = glm::vec4( 0.0f, 3.0f, 0.0f, 1.0f );
	::g_vecLights[1].diffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	::g_vecLights[1].ambient =glm::vec4( 0.0f, 0.2f, 0.0f, 1.0f );
	::g_vecLights[1].specular =glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	::g_vecLights[1].attenConst = 0.0f;
	::g_vecLights[1].attenLinear = 0.0f;
	::g_vecLights[1].attenQuad = 20.0f;

	::g_vecLights[2].position = glm::vec4(0.0f, 3.0f, 0.0f, 1.0f);
	::g_vecLights[2].diffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	::g_vecLights[2].ambient = glm::vec4(0.0f, 0.0f, 0.2f, 1.0f);
	::g_vecLights[2].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_vecLights[2].attenConst = 0.0f;
	::g_vecLights[2].attenLinear = 0.0f;
	::g_vecLights[2].attenQuad = 50.0f;

	::g_vecLights[3].position = glm::vec4(0.0f, 3.0f, 0.0f, 1.0f);
	::g_vecLights[3].diffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	::g_vecLights[3].ambient = glm::vec4(0.0f, 0.0f, 0.2f, 1.0f);
	::g_vecLights[3].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_vecLights[3].attenConst = 0.0f;
	::g_vecLights[3].attenLinear = 0.0f;
	::g_vecLights[3].attenQuad = 50.0f;

	return;
}


//