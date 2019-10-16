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
//Texture includes
#include "Headers/Texture.h"
//Model loader include
#include "Headers/Model.h"
#include "Headers/AnimationUtils.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int screenWidth;
int screenHeight;

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

// Descomentar
GLuint textureID1, textureID2, textureID3, textureCespedID, textureWaterID;
GLuint skyboxTextureID;

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

double deltaTime;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;

float roty = 0.0;
float advancez = 0.0;
// Variables to animations keyframes
std::ofstream myfile;
std::string fileName = "../animaciones/animation.txt";
std::vector<std::vector<glm::mat4>> keyFrames;
bool record = false;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);

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

	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	camera->setPosition(glm::vec3(0.0, 0.0, 6.0));

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
		camera->mouseMoveCamera(offsetX, offsetY, 0.005);
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
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);

	if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		r1 += 0.001;
	else if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		r1 -= 0.001;
	if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		r2 += 0.001;
	if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		r2 -= 0.001;
	if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		r3 += 0.001;
	if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		r3 -= 0.001;
	if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		r4 += 0.001;
	if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		r4 -= 0.001;

	if(availableSave && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
		saveFrame = true;
		availableSave = false;
	}if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
		availableSave = true;

	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		advancez = 0.001;
	if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		advancez = -0.001;
	if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		roty = 0.001;
	if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		roty = -0.001;
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
		keyFrames = getKeyFrames(fileName);
	}

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
			glm::vec3(10.0, 2.0, 20.0));
	int state = 0;
	float offsetAircraft = 0.0;

	/*std::cout << "Size of the keyframes:" << keyFrames.size() << std::endl;
	std::cout << "Size of the keyframes[0]:" << keyFrames[0].size() << std::endl;*/
	int indexNext = 1;
	float interpolation = 0.0;
	int maxNumPasos = 150;
	int numPasos = 0;

	if(record)
		myfile.open(fileName);
	else
		keyFrames = getKeyFrames(fileName);

	// Esto es para mover el modelo con las flechas
	glm::mat4 model = glm::mat4(1.0f);

	while (psi) {
		// Variables to animations keyframes
		std::vector<glm::mat4> matrixList;

		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		//glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 100.0f);
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

		glm::mat4 lightModelmatrix = glm::rotate(model, angle,
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

		/*****************************************************
		 * This is for save the keyframes
		 *****************************************************/
		if(record){
			model = glm::translate(model, glm::vec3(0.0, 0.0, advancez));
			model = glm::rotate(model, roty, glm::vec3(0, 1, 0));
			glBindTexture(GL_TEXTURE_2D, textureID1);
			//box1.enableWireMode();
			box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));
			glBindTexture(GL_TEXTURE_2D, 0);

			// Articulacion 1
			glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
			j1 = glm::rotate(j1, r1, glm::vec3(1.0, 0.0, 0.0));
			j1 = glm::rotate(j1, glm::radians(-60.0f), glm::vec3(0.0, 0.0, 1.0));
			// Push de la matriz de la articulacion
			matrixList.push_back(glm::inverse(model) * j1);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			//sphere1.enableWireMode();
			sphere1.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));
			glBindTexture(GL_TEXTURE_2D, 0);

			// Hueso 1
			glm::mat4 l1 = glm::translate(j1, glm::vec3(0.25, 0.0, 0.0));
			l1 = glm::rotate(l1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			l1 = glm::scale(l1, glm::vec3(0.1, 0.5, 0.1));
			glBindTexture(GL_TEXTURE_2D, textureID3);
			//cylinder1.enableWireMode();
			cylinder1.render(l1);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Articulacion 2
			glm::mat4 j2 = glm::translate(j1, glm::vec3(0.5, 0.0, 0.0));
			j2 = glm::rotate(j2, r2, glm::vec3(0.0, 1.0, 0.0f));
			//sphere1.enableWireMode();
			// Push de la matriz de la articulacion
			matrixList.push_back(glm::inverse(model) * j2);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));

			// Hueso 2
			glm::mat4 l2 = glm::translate(j2, glm::vec3(0.25, 0.0, 0.0));
			l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			l2 = glm::scale(l2, glm::vec3(0.1, 0.5, 0.1));
			glBindTexture(GL_TEXTURE_2D, textureID3);
			//cylinder1.enableWireMode();
			cylinder1.render(l2);

			// Articulacion 3
			glm::mat4 j3 = glm::translate(j2, glm::vec3(0.5, 0.0, 0.0));
			//sphere1.enableWireMode();
			glBindTexture(GL_TEXTURE_2D, textureID2);;
			sphere1.render(glm::scale(j3, glm::vec3(0.1, 0.1, 0.1)));

			// Articulacion 4 (Pierna)
			glm::mat4 j4 = glm::translate(model, glm::vec3(0.25, -0.5, 0.0));
			j4 = glm::rotate(j4, r3, glm::vec3(1.0f, 0.0f, 0.0f));
			j4 = glm::rotate(j4, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			//sphere1.enableWireMode();
			// Push de la matriz de la articulacion
			matrixList.push_back(glm::inverse(model) * j4);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			sphere1.render(glm::scale(j4, glm::vec3(0.1, 0.1, 0.1)));

			// Hueso (Pierna)
			glm::mat4 l3 = glm::translate(j4, glm::vec3(0.0f, -0.25f, 0.0f));
			glBindTexture(GL_TEXTURE_2D, textureID3);
			//cylinder1.enableWireMode();
			cylinder1.render(glm::scale(l3, glm::vec3(0.1, 0.5, 0.1)));

			// Articulacion 4 (Pierna)
			glm::mat4 j5 = glm::translate(j4, glm::vec3(0.0, -0.5, 0.0));
			j5 = glm::rotate(j5, r4, glm::vec3(1.0, 0.0, 0.0));
			j5 = glm::rotate(j5, glm::radians(-10.0f), glm::vec3(0.0, 0.0, 1.0));
			// Push de la matriz de la articulacion
			matrixList.push_back(glm::inverse(model) * j5);
			//sphere1.enableWireMode();
			glBindTexture(GL_TEXTURE_2D, textureID2);
			sphere1.render(glm::scale(j5, glm::vec3(0.1, 0.1, 0.1)));

			// Hueso (Pierna)
			glm::mat4 l4 = glm::translate(j5, glm::vec3(0.0f, -0.25f, 0.0f));
			//cylinder1.enableWireMode();
			glBindTexture(GL_TEXTURE_2D, textureID3);
			cylinder1.render(glm::scale(l4, glm::vec3(0.1, 0.5, 0.1)));
			glBindTexture(GL_TEXTURE_2D, 0);

			// Boca
			glm::mat4 l5 = glm::translate(model, glm::vec3(0.0, -0.3, 0.05));
			glBindTexture(GL_TEXTURE_2D, textureID2);
			//box1.enableWireMode();
			box2.render(glm::scale(l5, glm::vec3(0.6, 0.1, 0.05)));

			// Ojo
			glBindTexture(GL_TEXTURE_2D, textureID2);
			glm::mat4 l6 = glm::translate(model, glm::vec3(0.25, 0.25, 0.05));
			sphere1.render(glm::scale(l6, glm::vec3(0.2, 0.3, 0.05)));
		}
		else if (keyFrames.size() > 0 &&
				keyFrames[indexNext - 1].size() == 5 && keyFrames[indexNext].size() == 5) {

			interpolation = numPasos / (float)maxNumPasos;

			model = glm::translate(model, glm::vec3(0.0, 0.0, advancez));
			model = glm::rotate(model, roty, glm::vec3(0, 1, 0));
			glBindTexture(GL_TEXTURE_2D, textureID1);
			//box1.enableWireMode();
			box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));
			glBindTexture(GL_TEXTURE_2D, 0);

			// Articulacion 1
			glm::mat4 j1 = model * interpolate(keyFrames,
					indexNext, 0, interpolation);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			//sphere1.enableWireMode();
			sphere1.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));
			glBindTexture(GL_TEXTURE_2D, 0);

			// Hueso 1
			glm::mat4 l1 = glm::translate(j1, glm::vec3(0.25, 0.0, 0.0));
			l1 = glm::rotate(l1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			l1 = glm::scale(l1, glm::vec3(0.1, 0.5, 0.1));
			glBindTexture(GL_TEXTURE_2D, textureID3);
			//cylinder1.enableWireMode();
			cylinder1.render(l1);
			glBindTexture(GL_TEXTURE_2D, 0);

			// Articulacion 2
			glm::mat4 j2 = model * interpolate(keyFrames,
					indexNext, 1, interpolation);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));

			// Hueso 2
			glm::mat4 l2 = glm::translate(j2, glm::vec3(0.25, 0.0, 0.0));
			l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			l2 = glm::scale(l2, glm::vec3(0.1, 0.5, 0.1));
			glBindTexture(GL_TEXTURE_2D, textureID3);
			//cylinder1.enableWireMode();
			cylinder1.render(l2);

			// Articulacion 3
			glm::mat4 j3 = glm::translate(j2, glm::vec3(0.5, 0.0, 0.0));
			//sphere1.enableWireMode();
			glBindTexture(GL_TEXTURE_2D, textureID2);;
			sphere1.render(glm::scale(j3, glm::vec3(0.1, 0.1, 0.1)));

			// Articulacion 4 (Pierna)
			glm::mat4 j4 = model * interpolate(keyFrames,
					indexNext, 2, interpolation);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			sphere1.render(glm::scale(j4, glm::vec3(0.1, 0.1, 0.1)));

			// Hueso (Pierna)
			glm::mat4 l3 = glm::translate(j4, glm::vec3(0.0f, -0.25f, 0.0f));
			glBindTexture(GL_TEXTURE_2D, textureID3);
			//cylinder1.enableWireMode();
			cylinder1.render(glm::scale(l3, glm::vec3(0.1, 0.5, 0.1)));

			// Articulacion 4 (Pierna)
			glm::mat4 j5 = model * interpolate(keyFrames,
					indexNext, 3, interpolation);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			sphere1.render(glm::scale(j5, glm::vec3(0.1, 0.1, 0.1)));

			// Hueso (Pierna)
			glm::mat4 l4 = glm::translate(j5, glm::vec3(0.0f, -0.25f, 0.0f));
			//cylinder1.enableWireMode();
			glBindTexture(GL_TEXTURE_2D, textureID3);
			cylinder1.render(glm::scale(l4, glm::vec3(0.1, 0.5, 0.1)));
			glBindTexture(GL_TEXTURE_2D, 0);

			// Boca
			glm::mat4 l5 = glm::translate(model, glm::vec3(0.0, -0.3, 0.05));
			glBindTexture(GL_TEXTURE_2D, textureID2);
			box2.render(glm::scale(l5, glm::vec3(0.6, 0.1, 0.05)));
			//box1.enableWireMode();

			// Ojo
			glBindTexture(GL_TEXTURE_2D, textureID2);
			glm::mat4 l6 = glm::translate(model, glm::vec3(0.25, 0.25, 0.05));
			sphere1.render(glm::scale(l6, glm::vec3(0.2, 0.3, 0.05)));

			numPasos++;
			if (interpolation > 1.0) {
				numPasos = 0;
				indexNext++;
			}
			if (indexNext > keyFrames.size() - 1)
				indexNext = 1;
		}

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
			std::cout << "Incio del movimiento" << std::endl;
			state = 1;
			offsetAircraft = 0.0;
			break;
		case 1:
			std::cout << "Avanzando" << std::endl;
			if(offsetAircraft >= 10.0){
				offsetAircraft = 0.0;
				state = 2;
			}else{
				matrixModelAirCraft = glm::translate(matrixModelAirCraft, glm::vec3(0.0, 0.0, -0.01));
				offsetAircraft += 0.01;
			}
			break;
		case 2:
			std::cout << "Giro" << std::endl;
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

		if (saveFrame) {
			appendFrame(myfile, matrixList);
			saveFrame = false;
		}

		advancez = 0.0;
		roty = 0.0;

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
