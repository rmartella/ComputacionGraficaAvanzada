//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"

// Texture include
#include "Headers/Texture.h"

// Camera include
#include "Headers/CameraFPS.h"

Shader lightingShader;
Shader lampShader;

GLuint VAO, VBO, EBO;

typedef struct _VertexColor {

	_VertexColor() {
	}

	_VertexColor(glm::vec3 position, glm::vec3 color) {
		this->position = position;
		this->color = color;
	}

	glm::vec3 position;
	glm::vec3 color;
} VertexColor;

typedef struct _VertexColorNormal {

	_VertexColorNormal() {
	}

	_VertexColorNormal(glm::vec3 position, glm::vec3 colorl, glm::vec3 normal) {
		this->position = position;
		this->color = colorl;
		this->normal = normal;
	}

	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
} VertexColorNormals;

int screenWidth;
int screenHeight;

GLFWwindow * window;
InputManager inputManager;
double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
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
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
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

	// Enable test depth, must be clear depth buffer bit
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	lightingShader.initialize("../Shaders/materiales.vs", "../Shaders/materiales.fs");
	lampShader.initialize("../Shaders/lampShader.vs", "../Shaders/lampShader.fs");

	VertexColorNormals vertex[24] =
	{
		// Face front
		{ glm::vec3(1, 1, 1), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0,
		0, 1) },{ glm::vec3(-1, 1, 1), glm::vec3(0.0, 1.0,
		0.0), glm::vec3(0, 0, 1) },{ glm::vec3(-1, -1, 1),
		glm::vec3(0.0, 0.0, 1.0), glm::vec3(0, 0, 1) },{
			glm::vec3(1, -1, 1), glm::vec3(1.0, 1.0, 0.0),
			glm::vec3(0, 0, 1) },
			// Face right
	{ glm::vec3(1, 1, -1), glm::vec3(0.0, 0.0, 1.0), glm::vec3(
		1, 0, 0) },{ glm::vec3(1, 1, 1), glm::vec3(1.0,
			0.0, 1.0), glm::vec3(1, 0, 0) },{ glm::vec3(1, -1,
				1), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1, 0, 0) },
	{ glm::vec3(1, -1, -1), glm::vec3(0.0, 1.0, 1.0), glm::vec3(
		1, 0, 0) },
		// Face back
	{ glm::vec3(-1, 1, -1), glm::vec3(0.0, 1.0, 1.0), glm::vec3(
		0, 0, -1) },{ glm::vec3(1, 1, -1), glm::vec3(1.0,
			0.0, 1.0), glm::vec3(0, 0, -1) },
	{ glm::vec3(1, -1, -1), glm::vec3(1.0, 0.0, 1.0), glm::vec3(
		0, 0, -1) },{ glm::vec3(-1, -1, -1), glm::vec3(1.0,
			0.0, 1.0), glm::vec3(0, 0, -1) },
		//Face Left
	{ glm::vec3(-1, 1, 1), glm::vec3(1.0, 0.0, 0.0), glm::vec3(
		-1, 0, 0) },{ glm::vec3(-1, 1, -1), glm::vec3(1.0,
			1.0, 0.0), glm::vec3(-1, 0, 0) },{ glm::vec3(-1,
				-1, -1), glm::vec3(0.0, 0.0, 1.0), glm::vec3(-1, 0,
					0) },{ glm::vec3(-1, -1, 1), glm::vec3(0.0, 1.0,
						0.0), glm::vec3(-1, 0, 0) },
		// Face up
	{ glm::vec3(1, 1, 1), glm::vec3(1.0, 0.0, 1.0), glm::vec3(0,
		1, 0) },{ glm::vec3(1, 1, -1), glm::vec3(0.0, 0.0,
			1.0), glm::vec3(0, 1, 0) },{ glm::vec3(-1, 1, -1),
		glm::vec3(1.0, 1.0, .0), glm::vec3(0, 1, 0) },{
			glm::vec3(-1, 1, 1), glm::vec3(0.0, 1.0, 1.0),
			glm::vec3(0, 1, 0) },
			// Face down
	{ glm::vec3(1, -1, 1), glm::vec3(1.0, 0.0, 1.0), glm::vec3(
		0, -1, 0) },{ glm::vec3(-1, -1, 1), glm::vec3(1.0,
			1.0, 1.0), glm::vec3(0, -1, 0) },{ glm::vec3(-1,
				-1, -1), glm::vec3(0.0, 1.0, 1.0), glm::vec3(0, -1,
					0) },{ glm::vec3(1, -1, -1), glm::vec3(1.0, 1.0,
						0.0), glm::vec3(0, -1, 0) } };

	GLuint index[36] = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11,
		8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22,
		23, 20 };

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index,
		GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*) sizeof(vertex[0].position));
	glEnableVertexAttribArray(1);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*)(sizeof(vertex[0].position) + sizeof(vertex[0].color)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();
	lightingShader.destroy();
	lampShader.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	inputManager.keyPressed(inputManager.toApplicationKey(key), deltaTime * 10.0f,
		inputManager.toApplicationState(action));
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	inputManager.mouseMoved(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	inputManager.mouseClicked(inputManager.toMouseButtonIndex(button), xpos,
		ypos, inputManager.toApplicationState(state));
}

bool processInput(bool continueApplication) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
		|| glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	inputManager.do_movement(deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	while (psi) {
		psi = processInput(true);
		// This is new, need clear depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);		

		// Transformation matrices
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.3, 0.3, 0.3));
		glm::mat4 view = inputManager.getCameraFPS()->GetViewMatrix();
		glm::mat4 projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		glm::vec3 cameraPos = inputManager.getCameraFPS()->Position;

		lightingShader.turnOn();

		GLint objectColorLoc = lightingShader.getUniformLocation("objectColor");
		GLint lightPosLoc = lightingShader.getUniformLocation("lightPos");
		GLint viewPosLoc = lightingShader.getUniformLocation("viewPos");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

		GLint matAmbientLoc = lightingShader.getUniformLocation(
			"material.ambient");
		GLint matDiffuseLoc = lightingShader.getUniformLocation(
			"material.diffuse");
		GLint matSpecularLoc = lightingShader.getUniformLocation(
			"material.specular");
		GLint matShineLoc = lightingShader.getUniformLocation(
			"material.shininess");
		glUniform3f(matAmbientLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(matDiffuseLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(matSpecularLoc, 0.5f, 0.5f, 0.5f);
		glUniform1f(matShineLoc, 32.0f);

		GLint modelLoc = lightingShader.getUniformLocation("model");
		GLint viewLoc = lightingShader.getUniformLocation("view");
		GLint projLoc = lightingShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,
			(GLvoid*)(sizeof(GLuint) * 0));
		glBindVertexArray(0);

		lightingShader.turnOff();

		lampShader.turnOn();

		// Create transformations

		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,
			(GLvoid*)(sizeof(GLuint) * 0));
		glBindVertexArray(0);

		lampShader.turnOff();

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

