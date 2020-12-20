#include <windows.h>				//Header File For Windows
#include "gl/glew.h"				//Window creation header file
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")
#include "MyIncludes\glut\glut.h"

#define STB_IMAGE_IMPLEMENTATION
#include "MyIncludes\stb_image.h"

#include "shaders/Shader.h"			//Include shader header file, this is not part of OpenGL

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "Images/nvImage.h"	//Image loader

#include <iostream>

#include "Smoke/DiscEmitter.h"

#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"
#include "Models.h"
#include "CollisionDetection.h"

//Bounding box switch
bool isDrawBB = false;

//Shader objects
Shader* lightingShader;
Shader* skySphereShader;
Shader* smokeShader;
Shader* depthShader;
Shader* depthCubemapShader;

//Variables for day and night cycle
float angleIncrement = 0.001744444444f / 10.0f;	//1 degree per frame (in radians)
float angle = 0;						//Current angle (in radians)

GLuint skyTexName2 = 0;	//Second sky sphere texture
float mixStr = 0.0f;	//Texture mixing strength
float mixStrIncrement = 0.0025f / 10.0f;

GLuint smokeTexName = 0;

//MODEL LOADING
ThreeDModel droneModel, houseModel, light1Model, crateModel, landModel, AMBaseModel, skySphereModel;
OBJLoader objLoader;
Models modelLoader;
CollisionDetection CD;
ThreeDModel TDM;
///END MODEL LOADING

DiscEmitter DE;
const GLuint nrParticles = 1000; //6000;
std::vector<Particle> particleList;
//GLfloat particlePos[nrParticles * 3];
glm::vec3 particlePos[nrParticles * 3];

//Screen size variables
int screenWidth = 1920, screenHeight = 1080;

int halfWidth = screenWidth / 2;
int halfHeight = screenHeight / 2;
///End of screen size variables

const float PI = 3.1415926535897932384626433f;

//Drone camera variables
int camNum = 1;	//Current camera
glm::vec3 droneCam[] = {
	glm::vec3(-1.0f, -5.0f,  12.0f),	//Bottom camera
	glm::vec3(-1.0f,  5.0f,  11.0f),	//Top camera
	glm::vec3(-1.0f,  10.0f, 50.0f),	//Third person camera
	glm::vec3(-1.0f, -5.0f,  -12.0f)	//Rear facing camera
};

glm::vec3 controllerPos = glm::vec3(-600.0f, 40.0f, 200.0f);
glm::vec3 cameraPosBirdsEye = glm::vec3(-1500.0f, 500.0f, 2000.0f);
///End of camera variables

//Speed variables
float speedLimit = 15.0f;
float acceleration = 0.2f;
float backwards = 0.2f;
///End of speed variables

//Hover animation variables
float hoverSpeed = 0.1f;
float hoverAnimation = 0.0f;
bool isHoverUp = false;
bool isHover = true;
///End of hover animation variables

//Mouse camera variables
glm::vec3 cameraPos = glm::vec3(-1500.0f, 100.0f, 2000.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float fov = 60.0f;
float xoffset = 0;
float yoffset = 0;

float pitch = 0.0f;
float yaw = 270.0f;
float cursorPosX = halfWidth;
float cursorPosY = halfHeight;
float lastX = 0;
float lastY = 0;
bool isSpotlight = true;
bool isFPressed = false;
bool isXPressed = false;
bool isDrawSmoke = true;
bool isCPressed = false;
bool isDayNightCycle = true;
bool isVPressed = false;
bool isCollisions = true;
bool isBPressed = false;
///End of mouse camera variables

//Movement variables
float speedZ = 0.0f;
float speedX = 0.0f;
float speedY = 0.0f;
float strafeRotationZ = 0.0f;

bool isEngine = true;	//Allows for takeoff after landing, controlls disables when false

bool LMBPressed = false;
bool keys[256];
float spin = 180;
float speed = 0;
///End of movement variables

//Delta time variables
float deltaTimeVar = 0.0f;
float lastFrame = 0.0f;
///End of delta time variables


//Matrix creation
glm::mat4 DroneModelMatrix(1.0f), HouseModelMatrix(1.0f), Light1ModelMatrix(1.0f), CrateModelMatrix(1.0f), LandModelMatrix(1.0f), AMBaseModelMatrix(1.0f), SkyModelMatrix(1.0f);
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix; //Matrix for the orthographic projection
///End of matrix creation


//Lighting positions
glm::vec3 dirLight1 = glm::vec3(-5000.0f, -1000.0f, 12000.0f); //glm::vec3(-3000.0f, -950.0f, 3000.0f);
float dirLight1Specular = 0.5f;
glm::vec3 pointLightPositions[] = {
	glm::vec3(-275.0f, 20.0f, -50.0f),		//House second floor front
	glm::vec3(-177.0f, -130.0f, 180.0f),	//House ground floor front
	glm::vec3(-300.0f, -125.0f, -399.0f)	//House rear
};

bool isLight1Broken = false;	//If true, the lights are disabled


//Model position attributes
glm::vec3 houseModelPos = glm::vec3(-150.0f, 50.0f, -70.0f);
glm::vec3 lightModelPos[] = {
	glm::vec3(-275.0f, 20.0f, -70.0f),		//House second floor front
	glm::vec3(-197.0f, -130.0f, 180.0f),	//House ground floor front (Unused)
	glm::vec3(-300.0f, -125.0f, -379.0f)	//House rear	(Unused)
};
glm::vec3 AMBasePos = glm::vec3(900.0f, 580.0f, -7450.0f);
glm::vec3 crateModelPos = glm::vec3(400.0f, -140.0f, -125.0f);
glm::vec3 landModelPos = glm::vec3(500.0f, -800.0f, 0.0f);
glm::vec3 skySphereModelPos = glm::vec3(0.0f, 0.0f, 0.0f);


//Smoke attributes
//unsigned int particleVBO, particleVAO;
unsigned int smokeVBO, smokeVAO;
unsigned int smokeDiffuseMap;

float cubeVertNormTex[] = {
	// positions          // normals           // texture coords
	 10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,	//Rear
	 10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	-10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	-10.0f, -10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	-10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	 10.0f,  10.0f, -10.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	
	-10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,	//Front
	-10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-10.0f,  10.0f, -10.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,	//Left
	-10.0f, -10.0f, -10.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-10.0f, -10.0f,  10.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-10.0f, -10.0f,  10.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-10.0f,  10.0f,  10.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-10.0f,  10.0f, -10.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 10.0f,  10.0f,  10.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,	//Right
	 10.0f, -10.0f,  10.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 10.0f, -10.0f, -10.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 10.0f, -10.0f, -10.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 10.0f,  10.0f, -10.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 10.0f,  10.0f,  10.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,

	-10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,	//Bottom
	-10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,	//Top
	-10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

glm::vec3 smokePos = glm::vec3(-435.0f, 210.0f, -185.0f);
glm::mat4 ParticleModelMatrix = glm::translate(glm::mat4(1.0f), smokePos);
glm::vec3 gravity = glm::vec3(0.0f, 90.0f, 0.0f);
glm::vec3 SmokeBB[] = {
	glm::vec3(-450.0f, 270.0f, -150.0f),	//BFL (Bottom Front Left)
	glm::vec3(-410.0f, 270.0f, -150.0f),	//BFR (Bottom Front Right)
	glm::vec3(-410.0f, 270.0f, -210.0f),	//BBR (Bottom Back Right)
	glm::vec3(-450.0f, 270.0f, -210.0f),	//BBL (Bottom Back Left)
	glm::vec3(-450.0f, 290.0f, -150.0f),	//TFL (Top Front Left)
	glm::vec3(-410.0f, 290.0f, -150.0f),	//TFR (Top Front Right)
	glm::vec3(-410.0f, 290.0f, -210.0f),	//TBR (Top Back Right)
	glm::vec3(-450.0f, 290.0f, -210.0f)		//TBL (Top Back Left)
};

//Shadow map attributes
GLuint depthMapFBODirectional = 0;
GLuint depthMapTexDir;

GLuint depthMapFBOPoint = 0;

GLuint depthMapFBOSpotlight = 0;
GLuint depthMapTexSpotlight;

int SHADOW_WIDTH = 2048;
int SHADOW_HEIGHT = 2048;

//Shadow map - point lights
unsigned int depthCubemap;
std::vector<glm::mat4> shadowTransforms;
float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
float nearPlane = 0.5f;
float farPlane = 1000.0f;


//OPENGL FUNCTION PROTOTYPES
void display();				//Called in winmain to draw everything to the screen
void dayNightCycle(float, float, float);	//Orbits a directional light around a point
void reshape(int width, int height, float yoffset);	//Called when the window is resized
void playerMove();	//Updates camera position as player moves
void droneShutdownBalance(float &pitch);	//Resets the pitch and yaw when drone is shutdown
void init();		//Called in winmain when the program starts.
void processKeys();	//Called in winmain to process keyboard input
void FPSLock();	//Locks the frame rate
void displayFPS();	//Displays the frame rate
void droneCD();		//Checks for drone collisions
void generalCollisionFront();
void generalCollisionRear();
void generalCollisionLeft();
void generalCollisionRight();
void drawModels(Shader *shader);
void drawShadowMapModels(Shader *shader);
void drawShadowMapModelsNoDrone(Shader *shader);
void drawSkySphere();
void drawSmoke();
unsigned int loadTexture(const char *path);

/*************    START OF OPENGL FUNCTIONS   ****************/
void init() {
	ShowCursor(false);
	SetCursorPos(halfWidth, halfHeight);

	glClearColor(1.0, 1.0, 1.0, 0.0);		//Sets the clear colour to white
											//glClear(GL_COLOR_BUFFER_BIT) in the display function will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//LIGHTING SHADER
	lightingShader = new Shader;
	if (!lightingShader->load("LightingShader", "MyShaders/lightingShader.vs", "MyShaders/lightingShader.fs")) {
		cout << "failed to load shader" << endl;
	}

	//SKY SPHERE SHADER
	skySphereShader = new Shader;
	if (!skySphereShader->load("skySphereShader", "MyShaders/skySphereShader.vs", "MyShaders/skySphereShader.fs")) {
		cout << "failed to load shader" << endl;
	}

	//SMOKE SHADER
	smokeShader = new Shader;
	if (!smokeShader->load("smokeShader", "Smoke/SmokeShader.vs", "Smoke/SmokeShader.fs")) {
		cout << "failed to load shader" << endl;
	}


	//DEPTH SHADER - DIRECTIONAL
	depthShader = new Shader;
	if (!depthShader->load("depthShader", "MyShaders/depthShader.vs", "MyShaders/depthShader.fs")) {
		cout << "failed to load shader" << endl;
	}

	//DEPTH SHADER - POINT LIGHTS
	depthCubemapShader = new Shader;
	if (!depthCubemapShader->load3("depthCubemapShader", "MyShaders/depthCubemapShader.vs", "MyShaders/depthCubemapShader.fs", "MyShaders/depthCubemapShader.ge")) {
		cout << "failed to load shader" << endl;
	}

	//MODEL LOADING
	glEnable(GL_TEXTURE_2D);
	//Load drone model
	cout << " loading model " << endl;
	if (objLoader.loadModel("Models/HighTechDrone/Drone.obj", droneModel)) {	//Returns true if the model is loaded, puts the model in the model parameter
		droneModel.scale(10);
		cout << " model loaded " << endl;

		droneModel.BVPoints[0] = glm::vec3(0.0f, 0.0f, -10.0f);
		droneModel.BVRadius[0] = 5.0f;
		droneModel.BVPoints[1] = glm::vec3(0.0f, 0.0f, 10.0f);
		droneModel.BVRadius[1] = 5.0f;
		droneModel.BVPoints[2] = glm::vec3(-20.0f, 0.0f, 0.0f);
		droneModel.BVRadius[2] = 5.0f;
		droneModel.BVPoints[3] = glm::vec3(20.0f, 0.0f, 0.0f);
		droneModel.BVRadius[3] = 5.0f;

		droneModel.calcCentrePoint();
		droneModel.centreOnZero();

		droneModel.calcVertNormalsUsingOctree();  //The method will construct the octree if it hasn't already been created.

		droneModel.initDrawElements();
		droneModel.initVBO(lightingShader);
		droneModel.deleteVertexFaceData();
	}
	else {
		cout << " model failed to load " << endl;
	}

	//Load house model
	cout << " loading model " << endl;
	if (objLoader.loadModel("Models/bigHouse/house.obj", houseModel)) {	//Returns true if the model is loaded, puts the model in the model parameter
		cout << " model loaded " << endl;
		houseModel.calcCentrePoint();
		houseModel.centreOnZero();

		houseModel.calcVertNormalsUsingOctree();  //The method will construct the octree if it hasn't already been created.

		houseModel.initDrawElements();
		houseModel.initVBO(lightingShader);
		houseModel.deleteVertexFaceData();
	}
	else {
		cout << " model failed to load " << endl;
	}

	//Load light1 model
	cout << " loading model " << endl;
	if (objLoader.loadModel("Models/Lantern/Lantern.obj", light1Model)) {	//Returns true if the model is loaded, puts the model in the model parameter
		cout << " model loaded " << endl;
		light1Model.scale(80.0f);

		light1Model.calcCentrePoint();
		light1Model.centreOnZero();

		light1Model.calcVertNormalsUsingOctree();  //The method will construct the octree if it hasn't already been created.

		light1Model.initDrawElements();
		light1Model.initVBO(lightingShader);
		light1Model.deleteVertexFaceData();
	}
	else {
		cout << " model failed to load " << endl;
	}


	//Load crates model
	cout << " loading model " << endl;
	if (objLoader.loadModel("Models/Crates/Crates.obj", crateModel)) {	//Returns true if the model is loaded, puts the model in the model parameter
		cout << " model loaded " << endl;
		crateModel.scale(20.0f);

		crateModel.calcCentrePoint();
		crateModel.centreOnZero();

		crateModel.calcVertNormalsUsingOctree();  //The method will construct the octree if it hasn't already been created.

		crateModel.initDrawElements();
		crateModel.initVBO(lightingShader);
		crateModel.deleteVertexFaceData();
	}
	else {
		cout << " model failed to load " << endl;
	}


	//Load land model
	if (objLoader.loadModel("Models/Landscape/Land.obj", landModel)) {	//Returns true if the model is loaded, puts the model in the model parameter
		cout << " model loaded " << endl;
		landModel.calcCentrePoint();
		landModel.centreOnZero();

		landModel.calcVertNormalsUsingOctree();  //The method will construct the octree if it hasn't already been created.

		landModel.initDrawElements();
		landModel.initVBO(lightingShader);
		landModel.deleteVertexFaceData();
	}
	else {
		cout << " model failed to load" << endl;
	}


	//Load base model
	if (objLoader.loadModel("Models/AbandonedMilitaryBase/Base.obj", AMBaseModel)) {	//Returns true if the model is loaded, puts the model in the model parameter
		cout << " model loaded " << endl;
		AMBaseModel.calcCentrePoint();
		AMBaseModel.centreOnZero();

		AMBaseModel.calcVertNormalsUsingOctree();  //The method will construct the octree if it hasn't already been created.

		AMBaseModel.initDrawElements();
		AMBaseModel.initVBO(lightingShader);
		AMBaseModel.deleteVertexFaceData();
	}
	else {
		cout << " model failed to load " << endl;
	}


	//Load sky sphere model
	cout << " loading model " << endl;
	if (objLoader.loadModel("Models/SkySphere/SkySphere.obj", skySphereModel)) {	//Returns true if the model is loaded, puts the model in the model parameter
		cout << " model loaded " << endl;
		skySphereModel.scale(400.0f);
		skySphereModel.calcCentrePoint();
		skySphereModel.centreOnZero();

		skySphereModel.calcVertNormalsUsingOctree();  //The method will construct the octree if it hasn't already been created.

		skySphereModel.initDrawElements();
		skySphereModel.initVBO(skySphereShader);
		skySphereModel.deleteVertexFaceData();
	}
	else {
		cout << " model failed to load " << endl;
	}
	///END OF MODEL LOADING


	//LOAD SECOND SKYSPHERE TEXTURE
	nv::Image skyImg2;
	glActiveTexture(GL_TEXTURE1);
	if (skyImg2.loadImageFromFile("Models/SkySphere/SkyDay.png")) {
		glGenTextures(1, &skyTexName2);
		glBindTexture(GL_TEXTURE_2D, skyTexName2);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(GL_TEXTURE_2D, 0, skyImg2.getInternalFormat(), skyImg2.getWidth(), skyImg2.getHeight(), 0, skyImg2.getFormat(), skyImg2.getType(), skyImg2.getLevel(0));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		MessageBox(NULL, "Failed to load texture", "End of the world", MB_OK | MB_ICONINFORMATION);


	////LOAD SMOKE TEXTURE SPRITE
	//nv::Image smokeTex;
	//glActiveTexture(GL_TEXTURE2);
	//if (smokeTex.loadImageFromFile("Smoke/smoke3.png")) {
	//	glGenTextures(1, &smokeTexName);
	//	glBindTexture(GL_TEXTURE_2D, smokeTexName);
	//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	//	glTexImage2D(GL_TEXTURE_2D, 0, smokeTex.getInternalFormat(), smokeTex.getWidth(), smokeTex.getHeight(), 0, smokeTex.getFormat(), smokeTex.getType(), smokeTex.getLevel(0));
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//	MessageBox(NULL, "Failed to load texture", "End of the world", MB_OK | MB_ICONINFORMATION);
	/////END OF TEXTURE LOADING


	const char path[] = "Smoke/smoke3.png";
	smokeDiffuseMap = loadTexture(path);
	glUseProgram(smokeShader->handle());	//Use the smoke shader
	smokeShader->setInt("material.diffuse", 0);



	glUseProgram(lightingShader->handle());  // Use the lighting shader
	lightingShader->setInt("material.diffuse", 0);
	lightingShader->setInt("material.specular", 1);


	//SMOKE INITIALISATION
	//Fills the list with particles and randomly sets their values using DiscEmitter
	/*for (size_t i = 0; i < nrParticles; i++) {
		Particle p = Particle();
		DE.getParticle(p);
		particleList.push_back(p);
		particleList[i].age = particleList[i].lifetime - particleList[i].delay * 2;
		particlePos[i * 3] = p.initPos.x;
		particlePos[i * 3 + 1] = p.initPos.y;
		particlePos[i * 3 + 2] = p.initPos.z;
	}*/

	for (size_t i = 0; i < nrParticles; i++) {
		Particle p = Particle();
		DE.getParticle(p);
		particleList.push_back(p);
		particleList[i].age = particleList[i].lifetime - particleList[i].delay * 2;
		particlePos[i] = p.initPos;
	}

	glGenVertexArrays(1, &smokeVAO);
	glGenBuffers(1, &smokeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, smokeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertNormTex), cubeVertNormTex, GL_STATIC_DRAW);

	glBindVertexArray(smokeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);





	glUseProgram(smokeShader->handle());
	smokeShader->setVec3("gravity", gravity);
	smokeShader->setVec3("colour", glm::vec3(1.0f, 1.0f, 1.0f));


	//SMOKE ALPHA BLENDING
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);


	//SHADOW BUFFER - DIRECTIONAL LIGHT
	glGenFramebuffers(1, &depthMapFBODirectional);
	glGenTextures(1, &depthMapTexDir);
	glBindTexture(GL_TEXTURE_2D, depthMapTexDir);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
					GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBODirectional);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexDir, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//SHADOW BUFFER - POINT LIGHT
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenFramebuffers(1, &depthMapFBOPoint);
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPoint);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::mat4 shadowProj = glm::perspective(90.0f, aspect, nearPlane, farPlane);

	//Cubemap transformation matrices
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPositions[0], pointLightPositions[0] + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));		//Right
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPositions[0], pointLightPositions[0] + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));	//Left
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPositions[0], pointLightPositions[0] + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));		//Top
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPositions[0], pointLightPositions[0] + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));	//Bottom
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPositions[0], pointLightPositions[0] + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));		//Near
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPositions[0], pointLightPositions[0] + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));	//Far

	//SHADOW BUFFER - SPOTLIGHT
	glGenFramebuffers(1, &depthMapFBOSpotlight);
	glGenTextures(1, &depthMapTexSpotlight);
	glBindTexture(GL_TEXTURE_2D, depthMapTexSpotlight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOSpotlight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexSpotlight, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP);
}


void display() {
	//FPSLock();	//Locks the frame rate
	displayFPS();	//(Debug) Displays the frames per second on the command prompt

	ViewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//SHADOW MAP - DIRECTIONAL
	glm::mat4 depthProjectionPerspective = glm::perspective(60.0f, aspect, 0.5f, 30000.0f);
	glm::mat4 depthView = glm::lookAt(-dirLight1, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 depthMVPDir = depthProjectionPerspective * depthView;
	//Render scene from light's point of view
	glUseProgram(depthShader->handle());
	depthShader->setMat4("LightSpaceMatrix", depthMVPDir);
	//Render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBODirectional);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawShadowMapModels(depthShader);	//Also provides model matrix for shader
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Reset
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//SHADOW MAP - POINT LIGHT
	glUseProgram(depthCubemapShader->handle());
	//Render to depth cubemap
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOPoint);
	glClear(GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < 6; i++) {
		depthCubemapShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}
	depthCubemapShader->setFloat("far_plane", farPlane);
	depthCubemapShader->setVec3("lightPos", pointLightPositions[0]);
	drawShadowMapModels(depthCubemapShader);	//Also provides model matrix for shader
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//Reset viewport
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//SHADOW MAP - SPOTLIGHT
	glm::mat4 depthMVPSpotlight = depthProjectionPerspective * ViewMatrix;
	//Render scene from light's point of view
	glUseProgram(depthShader->handle());
	depthShader->setMat4("LightSpaceMatrix", depthMVPSpotlight);
	//1. Render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBOSpotlight);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawShadowMapModelsNoDrone(depthShader);	//Also provides model matrix for shader
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Reset
	glViewport(0, 0, screenWidth, screenHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	//LIGHTING
	glUseProgram(lightingShader->handle());
	lightingShader->setVec3("view", cameraPos);
	lightingShader->setFloat("material.shininess", 64.0f);

	//Directional light with day and night cycle
	if (isDayNightCycle) {
		dayNightCycle(3000, 500, angleIncrement);	//Orbits a directional light around a point on x,y axis
	}
	
	lightingShader->setVec3("dirLight.direction", dirLight1);
	lightingShader->setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
	lightingShader->setVec3("dirLight.diffuse", 2.0f, 2.0f, 2.0f);
	if (dirLight1.y > 0) {	//Turn off drone specular if night time
		if (dirLight1Specular > 0.0f) {
			dirLight1Specular -= 0.001f;
		}
		lightingShader->setVec3("dirLight.specular", dirLight1Specular, dirLight1Specular, dirLight1Specular);
	}
	else {	//Else turn on drone specular if day time
		if (dirLight1Specular < 0.2f) {
			dirLight1Specular += 0.001f;
		}
		lightingShader->setVec3("dirLight.specular", dirLight1Specular, dirLight1Specular, dirLight1Specular);
	}


	//Point light 1 (light1)
	lightingShader->setVec3("pointLights[0].position", pointLightPositions[0]);
	if (!isLight1Broken) {
		lightingShader->setVec3("pointLights[0].ambient", 1.0f, 1.0f, 1.0f);
		lightingShader->setVec3("pointLights[0].diffuse", 4.0f, 4.0f, 4.0f);
		lightingShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	}
	else {	//Turn off light1 when drone collides
		lightingShader->setVec3("pointLights[0].ambient", 0.0f, 0.0f, 0.0f);
		lightingShader->setVec3("pointLights[0].diffuse", 0.0f, 0.0f, 0.0f);
		lightingShader->setVec3("pointLights[0].specular", 0.0f, 0.0f, 0.0f);
	}
	lightingShader->setFloat("pointLights[0].constant", 1.0f);
	lightingShader->setFloat("pointLights[0].linear", 0.00005);
	lightingShader->setFloat("pointLights[0].quadratic", 0.00010);


	//SpotLight
	lightingShader->setVec3("spotLight.position", cameraPos);
	lightingShader->setVec3("spotLight.direction", cameraFront);
	lightingShader->setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
	lightingShader->setFloat("spotLight.constant", 0.5f);
	lightingShader->setFloat("spotLight.linear", 0.000000018f);	//0.0000009f
	lightingShader->setFloat("spotLight.quadratic", 0.00000016f);	//0.00000032f
	lightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(17.5f)));
	lightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));

	lightingShader->setMat4("projection", ProjectionMatrix);


	//Fog variable for distance calculation
	lightingShader->setVec3("C", cameraPos);


	//SHADOW MAP - DIRECTIONAL
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMapTexDir);
	GLuint tex1Location = glGetUniformLocation(lightingShader->handle(), "ShadowMapDir");	//Binds depthMap texture
	glUniform1i(tex1Location, 1);
	lightingShader->setMat4("LightSpaceMatrixDir", depthMVPDir);

	//SHADOW MAP - POINT LIGHT
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);	//Shadow map for point lights
	GLuint tex2Location = glGetUniformLocation(lightingShader->handle(), "ShadowCubeMap");	//Binds depthMap texture
	glUniform1i(tex2Location, 2);

	//SHADOW MAP - SPOTLIGHT
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMapTexSpotlight);
	GLuint tex3Location = glGetUniformLocation(lightingShader->handle(), "ShadowMapSpotlight");	//Binds depthMap texture
	glUniform1i(tex3Location, 3);
	lightingShader->setMat4("LightSpaceMatrixSpotlight", depthMVPSpotlight);

	lightingShader->setFloat("far_plane", farPlane);

	glActiveTexture(GL_TEXTURE0);
	drawModels(lightingShader);
	if (isCollisions) {
		droneCD();
	}

	drawSkySphere();

	if (isDrawSmoke) {
		drawSmoke();
	}

	playerMove();	//Updates camera position as player moves

	glFlush();
}


void drawModels(Shader *shader) {
	glCullFace(GL_BACK);

	//Draw drone
	modelLoader.drawDrone(DroneModelMatrix, cameraPos, hoverSpeed, hoverAnimation, isHoverUp, isHover, yaw, pitch, strafeRotationZ, camNum, droneCam, controllerPos, cameraFront, cameraUp, shader, ViewMatrix, droneModel, isSpotlight, false, false);
	//Draw house
	//modelLoader.drawStaticModel(HouseModelMatrix, -dirLight1, 270.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, ViewMatrix, houseModel, false, isDrawBB);	//Debug code - Directional light position tracker
	modelLoader.drawStaticModel(HouseModelMatrix, houseModelPos, 270.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, ViewMatrix, houseModel, false, isDrawBB);
	//Draw light1
	modelLoader.drawStaticModel(Light1ModelMatrix, lightModelPos[0], 180.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, ViewMatrix, light1Model, false, isDrawBB);
	//Draw crates
	modelLoader.drawStaticModel(CrateModelMatrix, crateModelPos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, ViewMatrix, crateModel, false, isDrawBB);
	//Draw land
	modelLoader.drawStaticModel(LandModelMatrix, landModelPos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, ViewMatrix, landModel, false, isDrawBB);
	//Draw abandoned military base
	modelLoader.drawStaticModel(AMBaseModelMatrix, AMBasePos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, ViewMatrix, AMBaseModel, false, isDrawBB);

	glCullFace(GL_FRONT);
}


void drawShadowMapModels(Shader *shader) {
	glCullFace(GL_BACK);
	//Draw drone
	modelLoader.drawDroneShadowMapModel(DroneModelMatrix, cameraPos, yaw, pitch, strafeRotationZ, shader, droneModel);
	//Draw house
	modelLoader.drawStaticShadowMapModel(HouseModelMatrix, houseModelPos, 270.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, houseModel);
	//Draw crates
	modelLoader.drawStaticShadowMapModel(CrateModelMatrix, crateModelPos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, crateModel);
	//Draw land
	modelLoader.drawStaticShadowMapModel(LandModelMatrix, landModelPos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, landModel);
	//Draw abandoned military base
	modelLoader.drawStaticShadowMapModel(AMBaseModelMatrix, AMBasePos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, AMBaseModel);
	
	glCullFace(GL_FRONT);
}


void drawShadowMapModelsNoDrone(Shader *shader) {
	glCullFace(GL_BACK);

	//Draw house
	modelLoader.drawStaticShadowMapModel(HouseModelMatrix, houseModelPos, 270.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, houseModel);
	//Draw crates
	modelLoader.drawStaticShadowMapModel(CrateModelMatrix, crateModelPos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, crateModel);
	//Draw land
	modelLoader.drawStaticShadowMapModel(LandModelMatrix, landModelPos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, landModel);
	//Draw abandoned military base
	modelLoader.drawStaticShadowMapModel(AMBaseModelMatrix, AMBasePos, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), shader, AMBaseModel);

	glCullFace(GL_FRONT);
}


void drawSkySphere() {
	glUseProgram(skySphereShader->handle());  // Use the sky sphere shader
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, skyTexName2);
	glUniform1i(glGetUniformLocation(skySphereShader->handle(), "DiffuseMap2"), 1);
	skySphereShader->setFloat("mixStr", mixStr);
	skySphereShader->setMat4("projection", ProjectionMatrix);

	//Fog distance attenuation
	skySphereShader->setVec3("C", cameraPos);

	modelLoader.drawStaticModel(SkyModelMatrix, skySphereModelPos, angle * 15 / PI, glm::vec3(0.0f, 1.0f, 0.0f), skySphereShader, ViewMatrix, skySphereModel, false, false);
}


//void drawSmoke() {
//	glEnable(GL_BLEND);
//	glDepthMask(GL_FALSE);
//	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
//
//	glUseProgram(smokeShader->handle());
//
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, smokeTexName);
//	glUniform1i(glGetUniformLocation(smokeShader->handle(), "smoke_texture"), 2);
//
//	glGenVertexArrays(1, &particleVAO);
//	glBindVertexArray(particleVAO);
//
//	glGenBuffers(1, &particleVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(particlePos), particlePos, GL_DYNAMIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//	glEnableVertexAttribArray(0);
//
//	
//
//	smokeShader->setMat4("mvp", ProjectionMatrix * ViewMatrix * ParticleModelMatrix);
//	smokeShader->setVec3("cameraPos", cameraPos);
//
//
//	//The age is checked and reset if more than particle lifetime. Then particles are checked for collisions, the shader values are set and each particle is drawn.
//	for (size_t i = 0; i < nrParticles; i++) {
//		particleList[i].age += 0.005f;
//		if (particleList[i].age > particleList[i].lifetime) {
//			particleList[i].age = 0.0f;
//			particleList[i].posShift = glm::vec3(0.0f, 0.0f, 0.0f);
//		}
//
//		glm::vec3 smokeWorldPosUpdate = glm::vec3(ParticleModelMatrix * glm::vec4(particleList[i].initPos + particleList[i].posShift + particleList[i].initVel * (particleList[i].age - particleList[i].delay) + 0.5f * gravity * (particleList[i].age - particleList[i].delay) * (particleList[i].age - particleList[i].delay), 1.0f));
//		if (CD.pointToAABBCollision(smokeWorldPosUpdate, SmokeBB)) {
//			particleList[i].posShift += glm::vec3(1.5f, -5.0f, 0.0f);
//		}
//
//		smokeShader->setVec3("Position", particleList[i].initPos + particleList[i].posShift);
//		smokeShader->setVec3("Velocity", particleList[i].initVel);
//		smokeShader->setVec3("ParticleTimes", glm::vec3(particleList[i].delay, particleList[i].lifetime, particleList[i].age));
//
//		//glDrawArrays(GL_POINTS, i, 1);
//		glDrawArrays(GL_TRIANGLES, i, 3);
//	}
//
//	glDisableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
//	glDepthMask(GL_TRUE);
//	glDisable(GL_BLEND);
//}




void drawSmoke() {
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	glUseProgram(smokeShader->handle());

	//Applying smoke texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, smokeDiffuseMap);

	glBindVertexArray(smokeVAO);

	smokeShader->setMat4("model", ParticleModelMatrix);
	smokeShader->setMat4("view", ViewMatrix);
	smokeShader->setMat4("projection", ProjectionMatrix);
	smokeShader->setVec3("cameraPos", cameraPos);


	//The age is checked and reset if more than particle lifetime. Then particles are checked for collisions, the shader values are set and each particle is drawn.
	for (size_t i = 0; i < nrParticles; i++) {
		particleList[i].age += 0.005f;
		if (particleList[i].age > particleList[i].lifetime) {
			particleList[i].age = 0.0f;
			particleList[i].posShift = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		////Collision detection
		//glm::vec3 smokeWorldPosUpdate = glm::vec3(ParticleModelMatrix * glm::vec4(particleList[i].initPos + particleList[i].posShift + particleList[i].initVel * (particleList[i].age - particleList[i].delay) + 0.5f * gravity * (particleList[i].age - particleList[i].delay) * (particleList[i].age - particleList[i].delay), 1.0f));
		//if (CD.pointToAABBCollision(smokeWorldPosUpdate, SmokeBB)) {
		//	particleList[i].posShift += glm::vec3(0.3f, -1.0f, 0.0f);
		//}

		smokeShader->setVec3("Position", particleList[i].initPos + particleList[i].posShift);
		smokeShader->setVec3("Velocity", particleList[i].initVel);
		smokeShader->setVec3("ParticleTimes", glm::vec3(particleList[i].delay, particleList[i].lifetime, particleList[i].age));

		//glPointSize(20.0f);
		//glDrawArrays(GL_POINTS, 0, 1);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}




//Orbits a directional light around a point
void dayNightCycle(float radiusX, float radiusY, float angleIncrement) {
	angle += angleIncrement;

	if (angle > 6.28) {	//Reset if angle exceeds 360 degrees
		angle -= 6.28;
	}

	if (angle < 0.6977777776f) {	//If angle is less than 40 degrees, increment the mixStr
		mixStr += mixStrIncrement;
	}
	else if (angle > 2.4422222216f && angle < 3.1399999992f) {	//If angle is more than 140 degrees and less than 180 degrees
		mixStr -= mixStrIncrement;
	}

	float s = sin(angleIncrement);
	float c = cos(angleIncrement);

	dirLight1.x -= radiusX;	//Move to original position
	dirLight1.y -= radiusY;

	float newX = dirLight1.x * c - dirLight1.y * s;	//Calculate rotation
	float newY = dirLight1.x * s + dirLight1.y * c;

	dirLight1.x = newX + radiusX;	//Restore position with new coordinates
	dirLight1.y = newY + radiusY;
}

//COLLISION DETECTION
void droneCD() {
	//Drone-House
	if (CD.modelAABBCollision(droneModel, DroneModelMatrix, houseModel, HouseModelMatrix, 0.0f, 0.0f, 50.0f)) {	//If AABB collision, check octree collision
		if (droneModel.isOctreeCollision(houseModel.octree, HouseModelMatrix, droneModel.updatedBVPoints[0])) {	//Drone front collision detection
			generalCollisionFront();
		}
		else if (droneModel.isOctreeCollision(houseModel.octree, HouseModelMatrix, droneModel.updatedBVPoints[1])) {	//Drone rear collision detection
			generalCollisionRear();
		}
		else if (droneModel.isOctreeCollision(houseModel.octree, HouseModelMatrix, droneModel.updatedBVPoints[2])) {	//Drone left collision detection
			generalCollisionLeft();
		}
		else if (droneModel.isOctreeCollision(houseModel.octree, HouseModelMatrix, droneModel.updatedBVPoints[3])) {	//Drone right collision detection
			generalCollisionRight();
		}
	}


	//Drone-Light1
	if (CD.modelAABBCollision(droneModel, DroneModelMatrix, light1Model, Light1ModelMatrix, 0.0f, 0.0f, 0.0f)) {	//If AABB collision, check octree collision
		if (droneModel.isOctreeCollision(light1Model.octree, Light1ModelMatrix, droneModel.updatedBVPoints[0])) {	//Drone front collision detection
			isLight1Broken = true;
			generalCollisionFront();
		}
		else if (droneModel.isOctreeCollision(light1Model.octree, Light1ModelMatrix, droneModel.updatedBVPoints[1])) {	//Drone rear collision detection
			isLight1Broken = true;
			generalCollisionRear();
		}
		else if (droneModel.isOctreeCollision(light1Model.octree, Light1ModelMatrix, droneModel.updatedBVPoints[2])) {	//Drone left collision detection
			isLight1Broken = true;
			generalCollisionLeft();
		}
		else if (droneModel.isOctreeCollision(light1Model.octree, Light1ModelMatrix, droneModel.updatedBVPoints[3])) {	//Drone right collision detection
			isLight1Broken = true;
			generalCollisionRight();
		}
	}


	//Drone-Crates
	if (CD.modelAABBCollision(droneModel, DroneModelMatrix, crateModel, CrateModelMatrix, 0.0f, 0.0f, 0.0f)) {	//If AABB collision, check octree collision
		if (droneModel.isOctreeCollision(crateModel.octree, CrateModelMatrix, droneModel.updatedBVPoints[0])) {	//Drone front collision detection
			generalCollisionFront();
		}
		else if (droneModel.isOctreeCollision(crateModel.octree, CrateModelMatrix, droneModel.updatedBVPoints[1])) {	//Drone rear collision detection
			generalCollisionRear();
		}
		else if (droneModel.isOctreeCollision(crateModel.octree, CrateModelMatrix, droneModel.updatedBVPoints[2])) {	//Drone right collision detection
			generalCollisionLeft();
		}
		else if (droneModel.isOctreeCollision(crateModel.octree, CrateModelMatrix, droneModel.updatedBVPoints[3])) {	//Drone right collision detection
			generalCollisionRight();
		}
	}

	//Drone-AMBase
	if (CD.modelAABBCollision(droneModel, DroneModelMatrix, AMBaseModel, AMBaseModelMatrix, 0.0f, 0.0f, 0.0f)) {	//If AABB collision, check octree collision
		if (droneModel.isOctreeCollision(AMBaseModel.octree, AMBaseModelMatrix, droneModel.updatedBVPoints[0])) {	//Drone front collision detection
			generalCollisionFront();
		}
		else if (droneModel.isOctreeCollision(AMBaseModel.octree, AMBaseModelMatrix, droneModel.updatedBVPoints[1])) {	//Drone rear collision detection
			generalCollisionRear();
		}
		else if (droneModel.isOctreeCollision(AMBaseModel.octree, AMBaseModelMatrix, droneModel.updatedBVPoints[2])) {	//Drone right collision detection
			generalCollisionLeft();
		}
		else if (droneModel.isOctreeCollision(AMBaseModel.octree, AMBaseModelMatrix, droneModel.updatedBVPoints[3])) {	//Drone right collision detection
			generalCollisionRight();
		}
	}

	//Drone-Land
	if (CD.modelAABBCollision(droneModel, DroneModelMatrix, landModel, LandModelMatrix, 0.0f, 0.0f, 0.0f)) {
		if (isEngine) {
			if (droneModel.isOctreeCollision(landModel.octree, LandModelMatrix, droneModel.updatedBVPoints[0]) || droneModel.isOctreeCollision(landModel.octree, LandModelMatrix, droneModel.updatedBVPoints[1]) ||
				droneModel.isOctreeCollision(landModel.octree, LandModelMatrix, droneModel.updatedBVPoints[2]) || droneModel.isOctreeCollision(landModel.octree, LandModelMatrix, droneModel.updatedBVPoints[3])) {
				if (keys['G']) {
					isEngine = false;
					isHover = false;
					speedY = 0.0f;
				}
				else {
					if (keys[VK_SPACE]) {
						speedZ = -2.0f;
						speedY = 2.0f;
					}
					else {
						speedZ /= 2;
						speedY = 2.0f;
					}
				}
			}
		}
		else {
			//Resets the pitch and yaw when drone is shutdown
			droneShutdownBalance(pitch);
		}
	}


	//Drone altitude limit
	if (cameraPos.y > 1600.0f && speedY > -20.0f) {
		speedY -= 0.8f;
	}


	//Boundary invisible wall collision detection
	float adjustedMinZ = CD.getMinZ(landModel, LandModelMatrix, -5500.0f);
	float adjustedMaxZ = CD.getMaxZ(landModel, LandModelMatrix, -50.0f);
	float adjustedMinX = CD.getMinX(landModel, LandModelMatrix, 960.0f);
	float adjustedMaxX = CD.getMaxX(landModel, LandModelMatrix, -500.0f);
	if (droneModel.updatedBVPoints[0].z < adjustedMinZ || droneModel.updatedBVPoints[0].z > adjustedMaxZ ||
		droneModel.updatedBVPoints[0].x < adjustedMinX || droneModel.updatedBVPoints[0].x > adjustedMaxX) {
		TDM.collisionFront(speedY, speedZ);
	}
	if (droneModel.updatedBVPoints[1].z < adjustedMinZ || droneModel.updatedBVPoints[1].z > adjustedMaxZ ||
		droneModel.updatedBVPoints[1].x < adjustedMinX || droneModel.updatedBVPoints[1].x > adjustedMaxX) {
		TDM.collisionRear(speedY, speedZ);
	}
	if (droneModel.updatedBVPoints[2].z < adjustedMinZ || droneModel.updatedBVPoints[2].z > adjustedMaxZ ||
		droneModel.updatedBVPoints[2].x < adjustedMinX || droneModel.updatedBVPoints[2].x > adjustedMaxX) {
		TDM.collisionLeft(speedX, speedY);
	}
	if (droneModel.updatedBVPoints[3].z < adjustedMinZ || droneModel.updatedBVPoints[3].z > adjustedMaxZ ||
		droneModel.updatedBVPoints[3].x < adjustedMinX || droneModel.updatedBVPoints[3].x > adjustedMaxX) {
		TDM.collisionRight(speedX, speedY);
	}
	///END OF COLLISION DETECTION
}

void generalCollisionFront() {
	if (isEngine) {
		TDM.collisionFront(speedY, speedZ);
		if (keys['G']) {
			isEngine = false;
			isHover = false;
			speedY = 0.0f;
		}
		else {
			if (keys[VK_SPACE]) {
				speedY = -2.0f;
			}
			if (keys[VK_CONTROL]) {
				speedY = 2.0f;
			}
		}
	}
	else {
		droneShutdownBalance(pitch);	//Resets the pitch and yaw when drone is shutdown
	}
}

void generalCollisionRear() {
	if (isEngine) {
		TDM.collisionRear(speedY, speedZ);
		if (keys['G']) {
			isEngine = false;
			isHover = false;
			speedY = 0.0f;
		}
		else {
			if (keys[VK_SPACE]) {
				speedY = -2.0f;
			}
			if (keys[VK_CONTROL]) {
				speedY = 2.0f;
			}
		}
	}
	else {
		droneShutdownBalance(pitch);	//Resets the pitch and yaw when drone is shutdown
	}
}

void generalCollisionLeft() {
	if (isEngine) {
		TDM.collisionLeft(speedX, speedY);
		if (keys['G']) {
			isEngine = false;
			isHover = false;
			speedY = 0.0f;
		}
		else {
			if (keys[VK_SPACE]) {
				speedY = -2.0f;
			}
			if (keys[VK_CONTROL]) {
				speedY = 2.0f;
			}
		}
	}
	else {
		droneShutdownBalance(pitch);	//Resets the pitch and yaw when drone is shutdown
	}
}

void generalCollisionRight() {
	if (isEngine) {
		TDM.collisionRight(speedX, speedY);
		if (keys['G']) {
			isEngine = false;
			isHover = false;
			speedY = 0.0f;
		}
		else {
			if (keys[VK_SPACE]) {
				speedY = -2.0f;
			}
			if (keys[VK_CONTROL]) {
				speedY = 2.0f;
			}
		}
	}
	else {
		droneShutdownBalance(pitch);	//Resets the pitch and yaw when drone is shutdown
	}
}

//Resets the pitch and yaw when drone is shutdown
void droneShutdownBalance(float &pitch) {
	if (pitch < 0.5f && pitch > -0.5) {
		pitch = 0.0f;

		glm::vec3 front; //Calculated yaw and pitch
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw + 45));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw + 45));
		cameraFront = glm::normalize(front);
	}
	else {
		if (pitch > 0.0f) {
			pitch -= 0.5f;
		}
		else if (pitch < 0.0f) {
			pitch += 0.5f;
		}

		glm::vec3 front; //Calculated yaw and pitch
		front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw + 45));
		front.y = sin(glm::radians(pitch));
		front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw + 45));
		cameraFront = glm::normalize(front);
	}
}


// Resize the OpenGL window to ensure the mouse coordinates match and to set the coordinate system
void reshape(int width, int height, float yoffset) {
	screenWidth = width; screenHeight = height;			
	glViewport(0, 0, width, height);	// Reset the current Viewport

	if (fov >= 10.0f && fov <= 60.0f) {
		fov -= yoffset;
	}
	if (fov < 10.0f) {
		fov = 10.0f;
	}
	if (fov > 60.0f) {
		fov = 60.0f;
	}

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(fov, (GLfloat)screenWidth / (GLfloat)screenHeight, 0.5f, 50000.0f);
}


void playerMove() {	//Updates camera position as player moves
	//Movement
	if (speedZ > 0.2f) {	//Accelerate
		cameraPos += speedZ * cameraFront;
		speedZ -= 0.2f;
	}
	else if (speedZ < -0.1f) {	//Reverse
		cameraPos += speedZ * cameraFront;
		speedZ += 0.1f;
	}
	else {
		speedZ = 0.0f;
	}
	if (speedX < -0.2f) {	//Strafe left
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speedX;
		speedX += 0.2f;
	}
	else if (speedX > 0.2f) {	//Strafe right
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speedX;
		speedX -= 0.2f;
	}
	else {
		speedX = 0.0f;
	}
	if (speedY > 0.3f) {	//Ascend
		cameraPos += glm::vec3(0.0f, speedY, 0.0f);
		speedY -= 0.3f;
	}
	else if (speedY < -0.25f) {	//Descend
		cameraPos += glm::vec3(0.0f, speedY, 0.0f);
		speedY += 0.25f;
	}
	else {
		speedY = 0.0f;
	}

	//Strafe rotations
	if (strafeRotationZ > 0.5f && !keys['A']) { //Strafe left tilt
		strafeRotationZ -= 0.5f;
	}
	if (strafeRotationZ < 0.5f && !keys['D']) { //Strafe right tilt
		strafeRotationZ += 0.5f;
	}
}


void processKeys() {
	if (isEngine) {
		//Accelerate
		if (keys['W'] && speedZ < speedLimit) {
			speedZ += acceleration;
		}
		//Reverse
		if (keys['S'] && speedZ > -10.0f) {
			speedZ -= backwards;
		}

		//Strafe left
		if (keys['A'] && speedX > -20.0f) {
			speedX -= 0.4f;
			strafeRotationZ += 0.5f;
			if (strafeRotationZ > 5.0f) {
				strafeRotationZ = 5.0f;
			}
		}
		//Strafe right
		if (keys['D'] && speedX < 20.0f) {
			speedX += 0.4f;
			strafeRotationZ -= 0.5f;
			if (strafeRotationZ < -5.0f) {
				strafeRotationZ = -5.0f;
			}
		}

		//Ascend
		if (keys[VK_SPACE] && speedY < 20.0f) {		
			speedY += 0.4f;
		}
		//Descend
		if (keys[VK_CONTROL] && speedY > -20.0f) {
			speedY -= 0.4f;
		}

		//Speed boost
		if (keys[VK_SHIFT]) {
			speedLimit = 60.0f;
			acceleration = 1.2f;
		}
		else if (!keys[VK_SHIFT]) {
			speedLimit = 30.0f;
			acceleration = 0.4f;
		}

		//Landing
		if (keys['G'] && speedY > -4.0f) {
			speedY -= 0.8;
			if (speedZ < 1.0f && speedZ > 0.0f) {
				speedZ = 0.0f;
			}
			else if (speedZ > 1.0f) {
				speedZ -= 0.8f;
			}
			else if (speedZ < 1.0f) {
				speedZ += 0.8f;
			}

			if (speedX < 1.0f && speedX > 0.0f) {
				speedZ = 0.0f;
			}
			else if (speedX > 1.0f) {
				speedX -= 0.8f;
			}
			else if (speedX < 1.0f) {
				speedX += 0.8f;
			}
		}
	}

	//Camera selection
	if (keys['1']) {		//Bottom camera camera
		camNum = 1;
	}
	else if (keys['2']) {	//Top camera camera
		camNum = 2;
	}
	else if (keys['3']) {	//Third person camera
		camNum = 3;
	}
	else if (keys['4']) {	//Controller camera
		camNum = 4;
	}
	else if (keys['5']) {	//Top-down camera
		camNum = 5;
	}
	else if (keys['6']) {	//Rear facing camera
		camNum = 6;
	}

	if (keys[VK_SUBTRACT]) {							//Camera Zoom out
		fov += 1.0f;
		reshape(screenWidth, screenHeight, yoffset);
	}

	if (keys[VK_ADD]) {									//Camera Zoom in
		fov -= 1.0f;
		reshape(screenWidth, screenHeight, yoffset);
	}

	if (keys['F'] && !isFPressed) {						//Spotlight switch
		if (isSpotlight) {
			isSpotlight = false;
		}
		else {
			isSpotlight = true;
		}
		isFPressed = true;
	}
	else if (!keys['F']) {
		isFPressed = false;
	}

	if (keys['E']) {									//Start the engine to take off
		if (!isEngine) {
			speedY = 4.0f;
		}
		isEngine = true;
		isHover = true;
	}

	if (keys['C'] && !isCPressed) {						//Day and night cycle toggle
		if (isDayNightCycle) {
			isDayNightCycle = false;
		}
		else {
			isDayNightCycle = true;
		}
		isCPressed = true;
	}
	else if (!keys['C']) {
		isCPressed = false;
	}

	if (keys['X'] && !isXPressed) {						//Day and night cycle toggle
		if (isDrawSmoke) {
			isDrawSmoke = false;
		}
		else {
			isDrawSmoke = true;
		}
		isXPressed = true;
	}
	else if (!keys['X']) {
		isXPressed = false;
	}

	if (keys['V'] && !isVPressed) {						//Collision detection toggle
		if (isCollisions) {
			isCollisions = false;
		}
		else {
			isCollisions = true;
		}
		isVPressed = true;
	}
	else if (!keys['V']) {
		isVPressed = false;
	}

	if (keys['B'] && !isBPressed) {						//Switch to draw octree bounding boxes
		if (isDrawBB) {
			isDrawBB = false;
		}
		else {
			isDrawBB = true;
		}
		isBPressed = true;
	}
	else if (!keys['B']) {
		isBPressed = false;
	}
}


void displayFPS() {
	static float FPS = 0.0f;	//Stores FPS
	static float lastTime = 0.0f;	//Holds time from last frame
	float currentTime = GetTickCount() * 0.001f;
	++FPS;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		cout << "FPS: " << (int) FPS << endl;
		FPS = 0;
	}
}


//Locks the frame rate
void FPSLock() {
	const int FRAMES_PER_SECOND = 60;
	const int SKIP_TICKS = 1000 / FRAMES_PER_SECOND;
	DWORD next_game_tick = GetTickCount();			//GetTickCount() returns the current number of milliseconds 
													//that have elapsed since the system was started
	int sleep_time = 0;
	next_game_tick += SKIP_TICKS;
	sleep_time = next_game_tick - GetTickCount();
	if (sleep_time >= 0) {
		Sleep(sleep_time);
	}
}


// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
/**************** END OPENGL FUNCTIONS *************************/

//WIN32 functions
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
void KillGLWindow();									// releases and destroys the window
bool CreateGLWindow(char* title, int width, int height); //creates the window
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);  // Win32 main function

//win32 global variabless
HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;	// Holds Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application


/******************* WIN32 FUNCTIONS ***************************/
int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	bool	done = false;								// Bool Variable To Exit Loop

	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);

	// Create Our OpenGL Window
	if (!CreateGLWindow("3D simulation of drone and collision detection", screenWidth, screenHeight)) {
		return 0;									// Quit If Window Was Not Created
	}

	while (!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message == WM_QUIT)				// Have We Received A Quit Message?
			{
				done = true;							// If So done=TRUE
			}
			else {									// If Not, Deal With Window Messages
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else {										// If There Are No Messages

			if (keys[VK_ESCAPE])
				done = true;

			processKeys();			//process keyboard

			display();					// Draw The Scene
			SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (int)(msg.wParam);						// Exit The Program
}

//WIN32 Processes function - useful for responding to user inputs or other events.
LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		//Mouse controlled camera
	case WM_MOUSEMOVE:
	{
		if (isEngine) {
			lastX = cursorPosX;
			lastY = cursorPosY;

			POINT p;
			GetCursorPos(&p);
			cursorPosX = p.x;
			cursorPosY = p.y;

			xoffset = cursorPosX - halfWidth;
			yoffset = halfHeight - cursorPosY; // reversed since y-coordinates range from bottom to top

			float sensitivity = 0.1f;
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 45.0f)
				pitch = 45.0f;
			if (pitch < -45.0f)
				pitch = -45.0f;

			glm::vec3 front; //Calculated yaw and pitch
			front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw + 45));
			front.y = sin(glm::radians(pitch));
			front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw + 45));
			cameraFront = glm::normalize(front);
		}

		SetCursorPos(halfWidth, halfHeight);
		return 0;
	}
	break;

	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(0);						// Send A Quit Message
		return 0;								// Jump Back
	}
	break;

	case WM_SIZE:								// Resize The OpenGL Window
	{
		reshape(LOWORD(lParam), HIWORD(lParam), yoffset);  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}
	break;

	case WM_LBUTTONDOWN:
	{
		//mouseX = LOWORD(lParam);          
		//mouseY = screenHeight - HIWORD(lParam);
		LMBPressed = true;
	}
	break;

	case WM_LBUTTONUP:
	{
		LMBPressed = false;
	}
	break;

	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = true;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}
	break;
	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = false;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}
	break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void KillGLWindow()								// Properly Kill The Window
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*/

bool CreateGLWindow(char* title, int width, int height)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		"OpenGL",							// Class Name
		title,								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);

	glewInit();

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1) {
		hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hDC, hRC);
	}
	else {	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		hRC = tempContext;
		cout << " not possible to make context " << endl;
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	cout << GLVersionString << endl;

	//OpenGL 3.2 way of checking the version
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	reshape(width, height, yoffset);				// Set Up Our Perspective GL Screen

	init();

	return true;									// Success
}