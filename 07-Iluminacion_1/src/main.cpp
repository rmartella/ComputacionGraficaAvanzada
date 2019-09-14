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

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int screenWidth;
int screenHeight;

float r1 = 0.0, r2 = 0.0, r3 = 0.0, r4 = 0.0, r5 = 0.0;

GLFWwindow * window;

Shader shaderColor;
Shader shaderTexture;
Shader shaderIluminacion;

Sphere sphere1(20, 20);
Sphere sphereLamp(20, 20);
Cylinder cylinder1(4, 4, 0.5, 0.3);
Box box1;
Box box2;

// Descomentar
GLuint textureID1, textureID2, textureID3;

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

double deltaTime;

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

	sphere1.init();
	sphere1.setShader(&shaderIluminacion);

	sphereLamp.init();
	sphereLamp.setShader(&shaderColor);
	sphereLamp.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	cylinder1.init();
	cylinder1.setShader(&shaderIluminacion);

	box1.init();
	box1.setShader(&shaderIluminacion);

	box2.init();
	box2.setShader(&shaderIluminacion);

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

	if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		r1 += 0.001;
	if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		r2 += 0.001;
	if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		r3 += 0.001;
	if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		r4 += 0.001;
	if(glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		r5 += 0.001;
	if(glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		r1 -= 0.001;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	float angle = 0.0;
	float ratio = 5.0;

	while (psi) {
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

		glm::mat4 model = glm::mat4(1.0f);

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.0001;

		glm::mat4 lightModelmatrix = glm::rotate(model, angle,
				glm::vec3(1.0f, 0.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix,
				glm::vec3(0.0f, 0.0f, -ratio));
		sphereLamp.render(lightModelmatrix);

		shaderIluminacion.setVectorFloat3("light.position",
				glm::value_ptr(
						glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		shaderIluminacion.setVectorFloat3("light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderIluminacion.setVectorFloat3("light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.4, 0.4)));
		shaderIluminacion.setVectorFloat3("light.specular", glm::value_ptr(glm::vec3(0.5, 0.3, 0.2)));
		shaderIluminacion.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));

		glBindTexture(GL_TEXTURE_2D, textureID1);
		//box1.enableWireMode();
		box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		// Articulacion 1
		glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		j1 = glm::rotate(j1, r2, glm::vec3(0.0, 1.0, 0.0));
		j1 = glm::rotate(j1, r1, glm::vec3(0.0, 0.0, 1.0));
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
		j2 = glm::rotate(j2, r3, glm::vec3(0.0, 0.0, 1.0f));
		//sphere1.enableWireMode();
		glBindTexture(GL_TEXTURE_2D, textureID2);
		sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));

		// Hueso 2
		glm::mat4 l2 = glm::translate(j2, glm::vec3(0.25, 0.0, 0.0));
		l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		l2 = glm::scale(l2, glm::vec3(0.1, 0.5, 0.1));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder1.enableWireMode();
		cylinder1.render(l2);

		// Articulacion 3
		glm::mat4 j3 = glm::translate(j2, glm::vec3(0.5, 0.0, 0.0));
		//sphere1.enableWireMode();
		glBindTexture(GL_TEXTURE_2D, textureID2);
		sphere1.render(glm::scale(j3, glm::vec3(0.1, 0.1, 0.1)));

		// Articulacion 4 (Pierna)
		glm::mat4 j4 = glm::translate(model, glm::vec3(0.25, -0.5, 0.0));
		j4 = glm::rotate(j4, r4, glm::vec3(1.0f, 0.0f, 0.0f));
		j4 = glm::rotate(j4, r5, glm::vec3(0.0f, 0.0f, 1.0f));
		//sphere1.enableWireMode();
		glBindTexture(GL_TEXTURE_2D, textureID2);
		sphere1.render(glm::scale(j4, glm::vec3(0.1, 0.1, 0.1)));

		// Hueso (Pierna)
		glm::mat4 l3 = glm::translate(j4, glm::vec3(0.0f, -0.25f, 0.0f));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		//cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l3, glm::vec3(0.1, 0.5, 0.1)));

		// Articulacion 4 (Pierna)
		glm::mat4 j5 = glm::translate(j4, glm::vec3(0.0, -0.5, 0.0));
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
		box2.render(glm::scale(l5, glm::vec3(0.6, 0.1, 0.05)));
		//box1.enableWireMode();

		// Ojo
		glBindTexture(GL_TEXTURE_2D, textureID2);
		glm::mat4 l6 = glm::translate(model, glm::vec3(0.25, 0.25, 0.05));
		sphere1.render(glm::scale(l6, glm::vec3(0.2, 0.3, 0.05)));

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
