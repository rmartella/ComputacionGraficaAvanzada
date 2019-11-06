#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"

#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"
//Texture includes
#include "Headers/Texture.h"
//Model loader include
#include "Headers/Model.h"
#include "Headers/AnimationUtils.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 4.0;

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int screenWidth;
int screenHeight;

float rotDartHead = 0.0, rotDartBody = 0.0, advanceDartBody = 0.0, rotDartLeftArm = 0.0,
		rotDartLeftHand = 0.0, rotDartRightArm = 0.0, rotDartRightHand = 0.0, rotDartLeftLeg = 0.0, rotDartRightLeg = 0.0;
float roty = 0.0;
float advancez = 0.0;
int modelSelected = 0;
bool enableCountSelected = true;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;
std::ofstream myfile;
std::string fileName = "";
bool record = false;

// Joints interpolations Dart Lego
std::vector<std::vector<float>> keyFramesJoints;
std::vector<std::vector<glm::mat4>> keyFramesDart;
int indexFrameJoints = 0;
int indexFrameJointsNext = 1;
float interpolationJoints = 0.0;
int maxNumPasosJoints = 20;
int numPasosJoints = 0;

int indexBob = 1;
int indexNextBob = 1;
float interpolationBob = 0.0;
int maxNumPasosBob = 150;
int numPasosBob = 0;

float r1 = 0.0, r2 = 0.0, r3 = 0.0, r4 = 0.0;

GLFWwindow * window;

Shader shaderColor;
Shader shaderTexture;
Shader shaderIluminacion;
Shader shaderDirectional;
Shader shaderPoint;
Shader shaderSpot;
Shader multipleLights;
Shader shaderSkybox;

Model modelRock;
Model modelRail;
Model modelAirCraft;
Model arturito;
Model modelTrain;

// Dart lego
Model modelDartLegoBody;
Model modelDartLegoHead;
Model modelDartLegoMask;
Model modelDartLegoLeftArm;
Model modelDartLegoRightArm;
Model modelDartLegoLeftHand;
Model modelDartLegoRightHand;
Model modelDartLegoLeftLeg;
Model modelDartLegoRightLeg;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/mp_bloodvalley/blood-valley_ft.tga",
	"../Textures/mp_bloodvalley/blood-valley_bk.tga",
	"../Textures/mp_bloodvalley/blood-valley_up.tga",
	"../Textures/mp_bloodvalley/blood-valley_dn.tga",
	"../Textures/mp_bloodvalley/blood-valley_rt.tga",
	"../Textures/mp_bloodvalley/blood-valley_lf.tga" };

Sphere sphere1(20, 20);
Sphere sphereLamp(20, 20);
Sphere skyboxSphere(20, 20);
Cylinder cylinder1(4, 4, 0.5, 0.3);
Box box1;
Box box2;
Box boxWater;
Box boxCesped;
Cylinder cylinder2(20, 20, 0.5, 0.5);
Cylinder cylinder3(30, 30, 0.5, 0.5);
Box boxCollider;
Sphere sphereCollider(20, 20);

// Descomentar
GLuint textureID1, textureID2, textureID3, textureCespedID, textureWaterID;
GLuint skyboxTextureID;

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

double deltaTime;
double currTime, lastTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);

bool raySphereIntersect(glm::vec3 orig, glm::vec3 dest, glm::vec3 dir,
	glm::vec3 vSphereCenter, float sphereRadius, float &t) {
	// Vector del Origen del rayo al centro de la esfera.
	glm::vec3 vDirToSphere = vSphereCenter - orig;

	// Distancia del origen al destino del rayo.
	float fLineLength = glm::distance(orig, dest);

	// Proyección escalar de vDirToSphere sobre la direccion del rayo.
	t = glm::dot(vDirToSphere, dir);

	glm::vec3 vClosestPoint;
	// Si la distancia proyectada del origen es menor o igual que cero
	// Significa que el punto mas cercano al centro es el origen.
	if (t <= 0.0f)
		vClosestPoint = orig;
	// Si la proyección escalar del origen es mayor a distancia del origen
	// al destino, el punto mas cercano es el destino.
	else if (t >= fLineLength)
		vClosestPoint = dest;
	// En caso contrario de calcula el punto sobre la linea usando t.
	else
		vClosestPoint = orig + dir * (t);

	// Se pureba si el punto mas cercao esta contenido en el radio de la esfera.
	return glm::distance(vSphereCenter, vClosestPoint) <= sphereRadius;
}

bool testSphereSphereIntersection(AbstractModel::SBB sbb1, AbstractModel::SBB sbb2) {
	float d = glm::distance(sbb1.c, sbb2.c);
	/*float d = sqrt(
	pow(sbb1.center.x - sbb2.center.x, 2)
	+ pow(sbb1.center.y - sbb2.center.y, 2)
	+ pow(sbb1.center.y - sbb2.center.y, 2));
	std::cout << "d:" << d << std::endl;
	std::cout << "sum:" << sbb1.ratio + sbb2.ratio << std::endl;*/
	if (d <= (sbb1.ratio + sbb2.ratio))
		return true;
	return false;
}

bool testSphereOBox(AbstractModel::SBB sbb, AbstractModel::OBB obb){
	float d = 0;
	glm::quat qinv = glm::inverse(obb.orientation);
	sbb.c = qinv * glm::vec4(sbb.c, 1.0);
	obb.c = qinv * glm::vec4(obb.c, 1.0);
	AbstractModel::AABB aabb;
	aabb.mins = obb.c - obb.dims / 2.0f;
	aabb.maxs = obb.c + obb.dims / 2.0f;
	if (sbb.c[0] >= aabb.mins[0] && sbb.c[0] <= aabb.maxs[0]
			&& sbb.c[1] >= aabb.mins[1] && sbb.c[1] <= aabb.maxs[1]
			&& sbb.c[2] >= aabb.mins[2] && sbb.c[2] <= aabb.maxs[2])
		return true;
	for (int i = 0; i < 3; i++){
		if(sbb.c[i] < aabb.mins[i])
			d += (sbb.c[i] - aabb.mins[i]) * (sbb.c[i] - aabb.mins[i]);
		else if(sbb.c[i] > aabb.maxs[i])
			d += (sbb.c[i] - aabb.maxs[i]) * (sbb.c[i] - aabb.maxs[i]);
	}
	if(d <= sbb.ratio * sbb.ratio)
		return true;
	return false;
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {
	
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	shaderColor.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");
	shaderIluminacion.initialize("../Shaders/iluminacion_textura.vs",
			"../Shaders/iluminacion_textura.fs");
	shaderDirectional.initialize("../Shaders/iluminacion_textura.vs",
			"../Shaders/directionalLight.fs");
	shaderPoint.initialize("../Shaders/iluminacion_textura.vs",
			"../Shaders/pointLight.fs");
	shaderSpot.initialize("../Shaders/iluminacion_textura.vs",
			"../Shaders/spotLight.fs");
	multipleLights.initialize("../Shaders/iluminacion_textura.vs",
		"../Shaders/multipleLights.fs");
	shaderSkybox.initialize("../Shaders/cubeTexture.vs",
		"../Shaders/cubeTexture.fs");

	sphere1.init();
	sphere1.setShader(&multipleLights);

	sphereLamp.init();
	sphereLamp.setShader(&shaderColor);
	sphereLamp.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	cylinder1.init();
	cylinder1.setShader(&multipleLights);

	cylinder3.init();
	cylinder3.setShader(&multipleLights);

	box1.init();
	box1.setShader(&multipleLights);

	box2.init();
	box2.setShader(&multipleLights);

	cylinder2.init();
	//cylinder2.setShader(&shaderDirectional);
	//cylinder2.setShader(&shaderPoint);
	cylinder2.setShader(&multipleLights);

	boxWater.init();
	boxWater.setShader(&multipleLights);

	boxCesped.init();
	boxCesped.setShader(&multipleLights);

	modelRock.loadModel("../objects/rock/rock.obj");
	modelRock.setShader(&multipleLights);

	modelRail.loadModel("../objects/railroad/railroad_track.obj");
	modelRail.setShader(&multipleLights);

	modelAirCraft.loadModel("../objects/Aircraft_obj/E 45 Aircraft_obj.obj");
	modelAirCraft.setShader(&multipleLights);

	// Dart Lego
	modelDartLegoBody.loadModel("../objects/LegoDart/LeoDart_body.obj");
	modelDartLegoBody.setShader(&multipleLights);
	modelDartLegoMask.loadModel("../objects/LegoDart/LeoDart_mask.obj");
	modelDartLegoMask.setShader(&multipleLights);
	modelDartLegoHead.loadModel("../objects/LegoDart/LeoDart_head.obj");
	modelDartLegoHead.setShader(&multipleLights);
	modelDartLegoLeftArm.loadModel("../objects/LegoDart/LeoDart_left_arm.obj");
	modelDartLegoLeftArm.setShader(&multipleLights);
	modelDartLegoRightArm.loadModel("../objects/LegoDart/LeoDart_right_arm.obj");
	modelDartLegoRightArm.setShader(&multipleLights);
	modelDartLegoLeftHand.loadModel("../objects/LegoDart/LeoDart_left_hand.obj");
	modelDartLegoLeftHand.setShader(&multipleLights);
	modelDartLegoRightHand.loadModel("../objects/LegoDart/LeoDart_right_hand.obj");
	modelDartLegoRightHand.setShader(&multipleLights);
	modelDartLegoLeftLeg.loadModel("../objects/LegoDart/LeoDart_left_leg.obj");
	modelDartLegoLeftLeg.setShader(&multipleLights);
	modelDartLegoRightLeg.loadModel("../objects/LegoDart/LeoDart_right_leg.obj");
	modelDartLegoRightLeg.setShader(&multipleLights);

	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	boxCollider.init();
	boxCollider.setShader(&shaderColor);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	sphereCollider.init();
	sphereCollider.setShader(&shaderColor);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	camera->setPosition(glm::vec3(0.0, 0.0, 6.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(0.1);

	// Texturas
	int imageWidth, imageHeight;
	Texture texture1("../Textures/sponge.jpg");
	FIBITMAP *bitmap = texture1.loadImage();
	unsigned char *data = texture1.convertToData(bitmap, imageWidth,
			imageHeight);
	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
				GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	texture1.freeImage(bitmap);

	Texture texture2("../Textures/texturaLadrillos.jpg");
	bitmap = texture2.loadImage();
	data = texture2.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID2);
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture2.freeImage(bitmap);

	Texture texture3("../Textures/bluewater.png");
	bitmap = texture3.loadImage();
	data = texture3.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID3);
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture3.freeImage(bitmap);

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth,
				imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

	Texture texture4 = Texture("../Textures/cesped.jpg");
	bitmap = texture4.loadImage(false);
	data = texture4.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureCespedID);
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture4.freeImage(bitmap);

	Texture texture5 = Texture("../Textures/water2.jpg");
	bitmap = texture5.loadImage(false);
	data = texture5.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureWaterID);
	glBindTexture(GL_TEXTURE_2D, textureWaterID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture5.freeImage(bitmap);
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	sphere1.destroy();
	cylinder1.destroy();
	box1.destroy();
	box2.destroy();

	shaderColor.destroy();
	shaderTexture.destroy();
	shaderIluminacion.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication){
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	/*if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);*/

	// Seleccionar modelo
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS){
		enableCountSelected = false;
		modelSelected++;
		if(modelSelected > 2)
			modelSelected = 0;
		if(modelSelected == 1)
			fileName = "../animaciones/animation_dart_joints.txt";
		if (modelSelected == 2)
			fileName = "../animaciones/animation_dart.txt";
		std::cout << "modelSelected:" << modelSelected << std::endl;
	}
	else if(glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		enableCountSelected = true;

	// Guardar key frames
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		record = true;
		if(myfile.is_open())
			myfile.close();
		myfile.open(fileName);
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE
			&& glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		record = false;
		myfile.close();
		if(modelSelected == 1)
			keyFramesJoints = getKeyRotFrames(fileName);
		if (modelSelected == 2)
			keyFramesDart = getKeyFrames(fileName);
	}
	if(availableSave && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
		saveFrame = true;
		availableSave = false;
	}if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
		availableSave = true;

	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		r1 += 0.001;
	else if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		r1 -= 0.001;
	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		r2 += 0.001;
	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		r2 -= 0.001;
	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		r3 += 0.001;
	if(modelSelected && glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		r3 -= 0.001;
	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		r4 += 0.001;
	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		r4 -= 0.001;

	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		advancez = 0.001;
	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		advancez = -0.001;
	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		roty = 0.001;
	if(modelSelected == 0 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		roty = -0.001;

	// Dart Lego model movements
	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rotDartHead += 0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rotDartHead -= 0.02;

	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rotDartLeftArm += 0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rotDartLeftArm -= 0.02;

	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rotDartRightArm += 0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rotDartRightArm -= 0.02;

	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rotDartLeftHand += 0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rotDartLeftHand -= 0.02;

	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		rotDartRightHand += 0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		rotDartRightHand -= 0.02;

	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		rotDartLeftLeg += 0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		rotDartLeftLeg -= 0.02;

	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		rotDartRightLeg += 0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		rotDartRightLeg -= 0.02;

	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rotDartBody = 0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rotDartBody = -0.02;
	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		advanceDartBody = -0.02;
	else if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		advanceDartBody = 0.02;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	float angle = 0.0;
	float ratio = 5.0;
	glm::vec2 off = glm::vec2(0.0, 0.0);

	glm::mat4 matrixModelAirCraft = glm::mat4(1.0f);
	matrixModelAirCraft = glm::translate(matrixModelAirCraft,
			glm::vec3(10.0, 0.2, 20.0));
	int state = 0;
	float offsetAircraft = 0.0;

	glm::mat4 modelMatrixDart = glm::mat4(1.0f);
	modelMatrixDart = glm::translate(modelMatrixDart, glm::vec3(3.0, -1.7, 20.0));

	// Variables to interpolation key frames
	fileName = "../animaciones/animation_dart_joints.txt";
	keyFramesJoints = getKeyRotFrames(fileName);
	keyFramesDart = getKeyFrames("../animaciones/animation_dart.txt");

	lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		// Variables to animations keyframes
		std::vector<float> matrixDartJoints;

		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;

		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		//glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 100.0f);
		glm::vec3 axis = glm::axis(glm::quat_cast(modelMatrixDart));
		float angleTarget = glm::angle(glm::quat_cast(modelMatrixDart));
		if(isnanf(angleTarget))
			angleTarget = 0.0;
		if(axis.y < 0)
			angleTarget = -angleTarget;
		camera->setCameraTarget(modelMatrixDart * glm::vec4(0, 0, 0, 1) + glm::vec4(0, 1.7, 0.0, 0.0));
		camera->setAngleTarget(angleTarget - glm::radians(90.0f));
		glm::mat4 view = camera->getViewMatrix();

		shaderColor.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderColor.setMatrix4("view", 1, false, glm::value_ptr(view));

		shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(view));

		shaderIluminacion.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderIluminacion.setMatrix4("view", 1, false, glm::value_ptr(view));

		shaderPoint.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderPoint.setMatrix4("view", 1, false, glm::value_ptr(view));

		shaderSpot.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderSpot.setMatrix4("view", 1, false, glm::value_ptr(view));

		shaderDirectional.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderDirectional.setMatrix4("view", 1, false, glm::value_ptr(view));

		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));

		multipleLights.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		multipleLights.setMatrix4("view", 1, false, glm::value_ptr(view));

		shaderDirectional.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderDirectional.setVectorFloat3("light.direction", glm::value_ptr(glm::vec3(0.0, 0.0, 1.0)));
		shaderDirectional.setVectorFloat3("light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderDirectional.setVectorFloat3("light.diffuse", glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderDirectional.setVectorFloat3("light.specular", glm::value_ptr(glm::vec3(0.0, 0.6, 0)));

		shaderPoint.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderPoint.setVectorFloat3("light.position",
				glm::value_ptr(glm::vec3(0.0, 0.0, -3.0)));
		shaderPoint.setVectorFloat3("light.ambient",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderPoint.setVectorFloat3("light.diffuse",
				glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderPoint.setVectorFloat3("light.specular",
				glm::value_ptr(glm::vec3(0.0, 0.6, 0)));

		shaderPoint.setFloat("light.constant", 1.0);
		shaderPoint.setFloat("light.linear", 0.014);
		shaderPoint.setFloat("light.quadratic", 0.044);

		shaderSpot.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderSpot.setVectorFloat3("light.position",
				glm::value_ptr(camera->getPosition()));
		/*shaderSpot.setVectorFloat3("light.direction",
				glm::value_ptr(camera->getFront()));*/
		shaderSpot.setVectorFloat3("light.direction",
				glm::value_ptr(camera->getFront()));
		shaderSpot.setFloat("light.cutOff", cos(glm::radians(5.5)));
		shaderSpot.setFloat("light.outerCutOff", cos(glm::radians(7.0)));
		shaderSpot.setVectorFloat3("light.ambient",
				glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderSpot.setVectorFloat3("light.diffuse",
				glm::value_ptr(glm::vec3(0.6, 0.0, 0.6)));
		shaderSpot.setVectorFloat3("light.specular",
				glm::value_ptr(glm::vec3(0.0, 0.6, 0)));

		shaderSpot.setFloat("light.constant", 1.0);
		shaderSpot.setFloat("light.linear", 0.014);
		shaderSpot.setFloat("light.quadratic", 0.044);

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.0001;


		multipleLights.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		multipleLights.setFloat("pointLightCount", 2.0);
		multipleLights.setFloat("spotLightCount", 1.0);
		multipleLights.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		multipleLights.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));
		multipleLights.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
		multipleLights.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.0, 0.8, 0.8)));
		multipleLights.setVectorFloat3("pointLights[0].light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		multipleLights.setVectorFloat3("pointLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		multipleLights.setVectorFloat3("pointLights[0].light.specular", glm::value_ptr(glm::vec3(0.8, 0.0, 0.0)));
		multipleLights.setFloat("pointLights[0].constant",1);
		multipleLights.setFloat("pointLights[0].linear", 0.09);
		multipleLights.setFloat("pointLights[0].quadratic", 0.032);

		multipleLights.setVectorFloat3("pointLights[1].light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		multipleLights.setVectorFloat3("pointLights[1].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		multipleLights.setVectorFloat3("pointLights[1].light.specular", glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
		multipleLights.setFloat("pointLights[1].constant", 1);
		multipleLights.setFloat("pointLights[1].linear", 0.14);
		multipleLights.setFloat("pointLights[1].quadratic", 0.07);

		multipleLights.setVectorFloat3("spotLights[0].position", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
		multipleLights.setVectorFloat3("spotLights[0].direction", glm::value_ptr(camera->getFront()));
		multipleLights.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		multipleLights.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		multipleLights.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
		multipleLights.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5)));
		multipleLights.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.5)));
		multipleLights.setFloat("spotLights[0].constant", 1);
		multipleLights.setFloat("spotLights[0].linear", 0.03);
		multipleLights.setFloat("spotLights[0].quadratic", 0.01);

		glm::mat4 lightModelmatrix = glm::rotate(glm::mat4(1.0f), angle,
				glm::vec3(1.0f, 0.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix,
				glm::vec3(0.0f, 0.0f, -ratio));
		sphereLamp.render(lightModelmatrix);
		//se coloca la direccion de la trayectoria de la luz
		multipleLights.setVectorFloat3("pointLights[0].position",
			glm::value_ptr(
				glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));

		multipleLights.setVectorFloat3("pointLights[1].position",
			glm::value_ptr(
				glm::vec3(-3.0,-2.0, 0.0)));

		shaderIluminacion.setVectorFloat3("light.position",
				glm::value_ptr(
						glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		shaderIluminacion.setVectorFloat3("light.ambient", glm::value_ptr(glm::vec3(0.0, 0.5, 0.0)));
		shaderIluminacion.setVectorFloat3("light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.0, 0.0)));
		shaderIluminacion.setVectorFloat3("light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.0)));
		shaderIluminacion.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));

		// Renderizado del cylindro con luz direccional
		glm::mat4 modelCylinder = glm::mat4(1.0);
		modelCylinder = glm::translate(modelCylinder,
				glm::vec3(3.0, 0.0, -4.0));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder2.render(modelCylinder);
		glBindTexture(GL_TEXTURE_2D, 0);

		modelCylinder = glm::translate(glm::mat4(1.0),
				glm::vec3(-2.0, 0.0, -3.0));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder2.render(modelCylinder);
		glBindTexture(GL_TEXTURE_2D, 0);

		modelCylinder = glm::translate(glm::mat4(1.0),
				glm::vec3(-2.0, 0.0, -10.0));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder2.render(modelCylinder);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 modelocilindro3 = glm::mat4(1.0);
		modelocilindro3 = glm::translate(modelocilindro3, glm::vec3(1.0, 2.0, -5.0));
		glBindTexture(GL_TEXTURE_2D, textureID1);
		cylinder3.render(modelocilindro3);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		boxCesped.setPosition(glm::vec3(0.0, -1.7, 0.0));
		boxCesped.setScale(glm::vec3(100.0, 0.001, 100.0));
		multipleLights.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40.0, 40.0)));
		boxCesped.render();
		multipleLights.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0.0, 0.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindTexture(GL_TEXTURE_2D, textureWaterID);
		boxWater.setPosition(glm::vec3(3.0, 2.0, -5.0));
		boxWater.setScale(glm::vec3(10.0, 0.001, 10.0));
		multipleLights.setVectorFloat2("offset", glm::value_ptr(off));
		boxWater.render();
		multipleLights.setVectorFloat2("offset", glm::value_ptr(glm::vec2(0.0, 0.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		modelRock.setPosition(glm::vec3(5.0, 3.0, -20.0));
		modelRock.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelRock.render();
		glActiveTexture(GL_TEXTURE0);

		modelRail.setPosition(glm::vec3(-10.0, -1.7, 25.0));
		modelRail.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelRail.render();
		glActiveTexture(GL_TEXTURE0);

		modelAirCraft.setScale(glm::vec3(1.0, 1.0, 1.0));
		//modelAirCraft.setPosition(glm::vec3(10.0, 2.0, 15.0));
		modelAirCraft.render(matrixModelAirCraft);
		glActiveTexture(GL_TEXTURE0);

		// Dart lego
		// Se deshabilita el cull faces IMPORTANTE para la capa
		glDisable(GL_CULL_FACE);
		modelMatrixDart = glm::translate(modelMatrixDart, glm::vec3(advanceDartBody, 0.0, 0.0));
		modelMatrixDart = glm::rotate(modelMatrixDart, rotDartBody, glm::vec3(0, 1, 0));
		glm::mat4 modelMatrixDartBody = glm::mat4(modelMatrixDart);
		modelMatrixDartBody = glm::scale(modelMatrixDartBody, glm::vec3(0.5, 0.5, 0.5));
		modelDartLegoBody.render(modelMatrixDartBody);
		glm::mat4 modelMatrixDartHead = glm::mat4(modelMatrixDartBody);
		modelMatrixDartHead = glm::rotate(modelMatrixDartHead, rotDartHead, glm::vec3(0, 1, 0));
		modelDartLegoHead.render(modelMatrixDartHead);
		modelDartLegoMask.render(modelMatrixDartHead);
		glm::mat4 modelMatrixDartLeftArm = glm::mat4(modelMatrixDartBody);
		modelMatrixDartLeftArm = glm::translate(modelMatrixDartLeftArm, glm::vec3(-0.023515, 2.43607, 0.446066));
		modelMatrixDartLeftArm = glm::rotate(modelMatrixDartLeftArm, glm::radians(-5.0f), glm::vec3(1, 0, 0));
		modelMatrixDartLeftArm = glm::rotate(modelMatrixDartLeftArm, rotDartLeftArm, glm::vec3(0, 0, 1));
		modelMatrixDartLeftArm = glm::rotate(modelMatrixDartLeftArm, glm::radians(5.0f), glm::vec3(1, 0, 0));
		modelMatrixDartLeftArm = glm::translate(modelMatrixDartLeftArm, glm::vec3(0.023515, -2.43607, -0.446066));
		modelDartLegoLeftArm.render(modelMatrixDartLeftArm);
		glm::mat4 modelMatrixDartLeftHand = glm::mat4(modelMatrixDartLeftArm);
		modelMatrixDartLeftHand = glm::translate(modelMatrixDartLeftHand, glm::vec3(0.201343, 1.68317, 0.99774));
		modelMatrixDartLeftHand = glm::rotate(modelMatrixDartLeftHand, glm::radians(-5.0f), glm::vec3(1, 0, 0));
		modelMatrixDartLeftHand = glm::rotate(modelMatrixDartLeftHand, rotDartLeftHand, glm::vec3(0, 1, 0));
		modelMatrixDartLeftHand = glm::rotate(modelMatrixDartLeftHand, glm::radians(5.0f), glm::vec3(1, 0, 0));
		modelMatrixDartLeftHand = glm::translate(modelMatrixDartLeftHand, glm::vec3(-0.201343, -1.68317, -0.99774));
		modelDartLegoLeftHand.render(modelMatrixDartLeftHand);
		glm::mat4 modelMatrixDartRightArm = glm::mat4(modelMatrixDartBody);
		modelMatrixDartRightArm = glm::translate(modelMatrixDartRightArm, glm::vec3(-0.023515, 2.43607, -0.446066));
		modelMatrixDartRightArm = glm::rotate(modelMatrixDartRightArm, glm::radians(5.0f), glm::vec3(1, 0, 0));
		modelMatrixDartRightArm = glm::rotate(modelMatrixDartRightArm, rotDartRightArm, glm::vec3(0, 0, 1));
		modelMatrixDartRightArm = glm::rotate(modelMatrixDartRightArm, glm::radians(-5.0f), glm::vec3(1, 0, 0));
		modelMatrixDartRightArm = glm::translate(modelMatrixDartRightArm, glm::vec3(0.023515, -2.43607, 0.446066));
		modelDartLegoRightArm.render(modelMatrixDartRightArm);
		glm::mat4 modelMatrixDartRightHand = glm::mat4(modelMatrixDartRightArm);
		modelMatrixDartRightHand = glm::translate(modelMatrixDartRightHand, glm::vec3(0.201343, 1.68317, -0.99774));
		modelMatrixDartRightHand = glm::rotate(modelMatrixDartRightHand, glm::radians(5.0f), glm::vec3(1, 0, 0));
		modelMatrixDartRightHand = glm::rotate(modelMatrixDartRightHand, rotDartRightHand, glm::vec3(0, 1, 0));
		modelMatrixDartRightHand = glm::rotate(modelMatrixDartRightHand, glm::radians(-5.0f), glm::vec3(1, 0, 0));
		modelMatrixDartRightHand = glm::translate(modelMatrixDartRightHand, glm::vec3(-0.201343, -1.68317, 0.99774));
		modelDartLegoRightHand.render(modelMatrixDartRightHand);
		glm::mat4 modelMatrixDartLeftLeg = glm::mat4(modelMatrixDartBody);
		modelMatrixDartLeftLeg = glm::translate(modelMatrixDartLeftLeg, glm::vec3(0, 1.12632, 0.423349));
		modelMatrixDartLeftLeg = glm::rotate(modelMatrixDartLeftLeg, rotDartLeftLeg, glm::vec3(0, 0, 1));
		modelMatrixDartLeftLeg = glm::translate(modelMatrixDartLeftLeg, glm::vec3(0, -1.12632, -0.423349));
		modelDartLegoLeftLeg.render(modelMatrixDartLeftLeg);
		glm::mat4 modelMatrixDartRightLeg = glm::mat4(modelMatrixDartBody);
		modelMatrixDartRightLeg = glm::translate(modelMatrixDartRightLeg, glm::vec3(0, 1.12632, -0.423349));
		modelMatrixDartRightLeg = glm::rotate(modelMatrixDartRightLeg, rotDartRightLeg, glm::vec3(0, 0, 1));
		modelMatrixDartRightLeg = glm::translate(modelMatrixDartRightLeg, glm::vec3(0, -1.12632, 0.423349));
		modelDartLegoRightLeg.render(modelMatrixDartRightLeg);
		glActiveTexture(GL_TEXTURE0); // IMPORTANTE regresar a la textura 0
		// Se regresa el cull faces IMPORTANTE para la capa
		glEnable(GL_CULL_FACE);

		// Para salvar el frame
		if(record && modelSelected == 1){
			matrixDartJoints.push_back(rotDartHead);
			matrixDartJoints.push_back(rotDartLeftArm);
			matrixDartJoints.push_back(rotDartLeftHand);
			matrixDartJoints.push_back(rotDartRightArm);
			matrixDartJoints.push_back(rotDartRightHand);
			matrixDartJoints.push_back(rotDartLeftLeg);
			matrixDartJoints.push_back(rotDartRightLeg);
			if (saveFrame) {
				appendFrame(myfile, matrixDartJoints);
				saveFrame = false;
			}
		}
		else if(keyFramesJoints.size() > 0){
			// Para reproducir el frame
			interpolationJoints = numPasosJoints / (float) maxNumPasosJoints;
			numPasosJoints++;
			if (interpolationJoints > 1.0) {
				numPasosJoints = 0;
				interpolationJoints = 0;
				indexFrameJoints = indexFrameJointsNext;
				indexFrameJointsNext++;
			}
			if (indexFrameJointsNext > keyFramesJoints.size() - 1)
				indexFrameJointsNext = 0;

			rotDartHead = interpolate(keyFramesJoints, indexFrameJoints, indexFrameJointsNext, 0, interpolationJoints);
			rotDartLeftArm = interpolate(keyFramesJoints, indexFrameJoints, indexFrameJointsNext, 1, interpolationJoints);
			rotDartLeftHand = interpolate(keyFramesJoints, indexFrameJoints, indexFrameJointsNext, 2, interpolationJoints);
			rotDartRightArm = interpolate(keyFramesJoints, indexFrameJoints, indexFrameJointsNext, 3, interpolationJoints);
			rotDartRightHand = interpolate(keyFramesJoints, indexFrameJoints, indexFrameJointsNext, 4, interpolationJoints);
			rotDartLeftLeg = interpolate(keyFramesJoints, indexFrameJoints, indexFrameJointsNext, 5, interpolationJoints);
			rotDartRightLeg = interpolate(keyFramesJoints, indexFrameJoints, indexFrameJointsNext, 6, interpolationJoints);
		}

		// render de colliders
		// Collider del dart vader lego
		glm::mat4 modelmatrixColliderDart = glm::mat4(modelMatrixDart);
		AbstractModel::OBB obb;
		modelmatrixColliderDart = glm::scale(modelmatrixColliderDart, glm::vec3(0.5, 0.5, 0.5));
		modelmatrixColliderDart = glm::translate(modelmatrixColliderDart,
				glm::vec3(modelDartLegoBody.getObb().c.x,
						modelDartLegoBody.getObb().c.y,
						modelDartLegoBody.getObb().c.z));
		obb.c = glm::vec3(modelmatrixColliderDart[3][0], modelmatrixColliderDart[3][1], modelmatrixColliderDart[3][2]);
		obb.dims = modelDartLegoBody.getObb().dims * glm::vec3(0.5, 0.5, 0.5);
		obb.orientation = glm::quat_cast(modelMatrixDart);
		modelmatrixColliderDart = glm::scale(modelmatrixColliderDart,
				glm::vec3(modelDartLegoBody.getObb().dims.x,
						modelDartLegoBody.getObb().dims.y,
						modelDartLegoBody.getObb().dims.z));
		boxCollider.enableWireMode();
		boxCollider.render(modelmatrixColliderDart);
		// Collider de la via
		glm::mat4 modelMatrixColliderRail= glm::mat4(1.0);
		modelMatrixColliderRail = glm::translate(modelMatrixColliderRail, glm::vec3(-10.0, -1.7, 25.0));
		modelMatrixColliderRail = glm::scale(modelMatrixColliderRail, glm::vec3(1.0, 1.0, 1.0));
		modelMatrixColliderRail = glm::translate(modelMatrixColliderRail,
				glm::vec3(modelRail.getObb().c.x, modelRail.getObb().c.y,
						modelRail.getObb().c.z));
		modelMatrixColliderRail = glm::scale(modelMatrixColliderRail,
				glm::vec3(modelRail.getObb().dims.x, modelRail.getObb().dims.y,
						modelRail.getObb().dims.z));
		boxCollider.enableWireMode();
		boxCollider.render(modelMatrixColliderRail);
		// Collider del aricraft
		AbstractModel::SBB sbb;
		glm::mat4 modelMatrixColliderAircraft = glm::mat4(matrixModelAirCraft);
		modelMatrixColliderAircraft = glm::scale(modelMatrixColliderAircraft,
				glm::vec3(1.0, 1.0, 1.0));
		modelMatrixColliderAircraft = glm::translate(
				modelMatrixColliderAircraft,
				glm::vec3(modelAirCraft.getSbb().c.x,
						modelAirCraft.getSbb().c.y,
						modelAirCraft.getSbb().c.z));
		sbb.c = glm::vec3(modelMatrixColliderAircraft[3][0], modelMatrixColliderAircraft[3][1], modelMatrixColliderAircraft[3][2]);
		sbb.ratio = modelAirCraft.getSbb().ratio * 1.0;
		modelMatrixColliderAircraft = glm::scale(modelMatrixColliderAircraft,
				glm::vec3(modelAirCraft.getSbb().ratio * 2.0,
						modelAirCraft.getSbb().ratio * 2.0,
						modelAirCraft.getSbb().ratio * 2.0));
		sphereCollider.enableWireMode();
		sphereCollider.render(modelMatrixColliderAircraft);

		modelMatrixColliderAircraft = glm::mat4(matrixModelAirCraft);
		modelMatrixColliderAircraft = glm::scale(modelMatrixColliderAircraft,
				glm::vec3(1.0, 1.0, 1.0));
		modelMatrixColliderAircraft = glm::translate(
				modelMatrixColliderAircraft,
				glm::vec3(modelAirCraft.getObb().c.x,
						modelAirCraft.getObb().c.y,
						modelAirCraft.getObb().c.z));
		modelMatrixColliderAircraft = glm::scale(modelMatrixColliderAircraft,
				glm::vec3(modelAirCraft.getObb().dims.x,
						modelAirCraft.getObb().dims.y,
						modelAirCraft.getObb().dims.z));
		boxCollider.enableWireMode();
		boxCollider.render(modelMatrixColliderAircraft);

		// Esto es para ilustrar la transformacion inversa de los coliders
		glm::vec3 cinv = glm::inverse(obb.orientation) * glm::vec4(sbb.c, 1.0);
		glm::mat4 invColliderS = glm::mat4(1.0);
		invColliderS = glm::translate(invColliderS, cinv);
		invColliderS = glm::scale(invColliderS, glm::vec3(sbb.ratio * 2.0, sbb.ratio * 2.0, sbb.ratio * 2.0));
		sphereCollider.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
		sphereCollider.enableWireMode();
		sphereCollider.render(invColliderS);

		glm::vec3 cinv2 = glm::inverse(obb.orientation) * glm::vec4(obb.c, 1.0);
		glm::mat4 invColliderB = glm::mat4(1.0);
		invColliderB = glm::translate(invColliderB, cinv2);
		invColliderB = glm::scale(invColliderB, obb.dims);
		boxCollider.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
		boxCollider.enableWireMode();
		boxCollider.render(invColliderB);

		// Se regresa el color blanco
		sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
		boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));


		if(testSphereOBox(sbb, obb))
			std::cout << "Colision aircraft vs dart vader" << std::endl;



		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		off.x += 0.001;
		off.y += 0.001;

		switch(state){
		case 0:
			//std::cout << "Incio del movimiento" << std::endl;
			state = 1;
			offsetAircraft = 0.0;
			break;
		case 1:
			//std::cout << "Avanzando" << std::endl;
			if(offsetAircraft >= 10.0){
				offsetAircraft = 0.0;
				state = 2;
			}else{
				matrixModelAirCraft = glm::translate(matrixModelAirCraft, glm::vec3(0.0, 0.0, -0.01));
				offsetAircraft += 0.01;
			}
			break;
		case 2:
			//std::cout << "Giro" << std::endl;
			if(offsetAircraft > 90.0)
				state = 0;
			else{
				matrixModelAirCraft = glm::translate(matrixModelAirCraft,
						glm::vec3(0, 0, -0.01));
				matrixModelAirCraft = glm::rotate(matrixModelAirCraft,
						glm::radians(0.1f), glm::vec3(0.0, 1.0, 0.0f));
				offsetAircraft += 0.1;
			}
			break;
		}

		advanceDartBody = 0.0;
		rotDartBody = 0.0;

		glfwSwapBuffers(window);
	}
	if(record)
		myfile.close();
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
