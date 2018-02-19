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

//Texture include
#include <FreeImage.h>

GLuint VBO, VAO, EBO, texture;
Shader shader;

struct VertexColorTexture {
	glm::vec3 m_Pos;
	glm::vec3 m_Color;
	glm::vec2 m_TexCoord;
};

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

	shader.initialize("../Shaders/colorTexture.vs", "../Shaders/colorTexture.fs");

	// Set up vertex data (and buffer(s)) and attribute pointers
	VertexColorTexture vertices[4] = { { glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2{ 1.0f, 1.0f } }, // 0
									   { glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2{ 1.0f, 0.0f } }, // 1
									   { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2{ 0.0f, 0.0f } }, // 2
									   { glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2{ 0.0f, 1.0f } }, // 3
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*) sizeof(vertices[0].m_Pos));
	glEnableVertexAttribArray(1);
	// Texture Mapping attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*)(sizeof(vertices[0].m_Color) + sizeof(vertices[0].m_Pos)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

						  //This for load a texture with FreeImage lib
						  // Se obtiene del nombre del archivo a cargar en la GPU como un apuntador a un arreglo de chars
	const char* filename = "../Textures/test.png";

	// Determina el formato de la imagen
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

	// Si la imagen no es encontrada termina el programa
	if (format == -1) {
		std::cout << "No se encontró la imagen: " << filename << " - Abortando."
			<< std::endl;
		exit(-1);
	}
	// Si el formato no es soportado por FreeImage termina el programa
	if (format == FIF_UNKNOWN) {
		std::cout
			<< "No se puede determinar el formato de imagen - validarla extensión del archivo..."
			<< std::endl;

		// Se obtiene el formato del archivo
		format = FreeImage_GetFIFFromFilename(filename);

		// Revisa si la librería es capaz de leer el formato
		if (!FreeImage_FIFSupportsReading(format)) {
			std::cout << "El formato de la imagen no puede ser leeído!"
				<< std::endl;
			exit(-1);
		}
	}
	// Si es valida la imagen y puede ser leeído, se carga la imagen en un bitap
	FIBITMAP* bitmap = FreeImage_Load(format, filename);
	FreeImage_FlipVertical(bitmap);

	// Obtiene el número de bits por pixel de la imagen
	int bitsPerPixel = FreeImage_GetBPP(bitmap);

	// Convierte la imagen a 32 bits (8 bits por canal).
	FIBITMAP* bitmap32;
	if (bitsPerPixel == 32) {
		/*std::cout << "Source image has " << bitsPerPixel
		<< " bits per pixel. Skipping conversion." << std::endl;*/
		bitmap32 = bitmap;
	}
	else {
		/*std::cout << "Source image has " << bitsPerPixel
		<< " bits per pixel. Converting to 32-bit colour." << std::endl;*/
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
	}
	// Se obtiene las dimensiones de la imagen.
	int imageWidth = FreeImage_GetWidth(bitmap32);
	int imageHeight = FreeImage_GetHeight(bitmap32);
	/*std::cout << "Image: " << m_fileName << " is size: " << imageWidth << "x"
	<< imageHeight << "." << std::endl;*/

	// Se obtiene un apuntador a los datos de la textura como un arreglo de unsigned bytes.
	GLubyte* textureData = FreeImage_GetBits(bitmap32);

	// Load and create a texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unload the 32-bit colour bitmap
	// Se desecha los datos de la textura
	FreeImage_Unload(bitmap32);

	// Si se convirtio la imagen a 32 bits por pixel eliminamos el bitmap
	if (bitsPerPixel != 32) {
		FreeImage_Unload(bitmap);
	}

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();
	shader.destroy();

	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

	glDeleteTextures(1, &texture);
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		inputManager.keyPressed(InputManager::InputCodes::kEscape, deltaTime);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	inputManager.mouseMoved(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT && state == GLFW_PRESS)
		inputManager.mouseClicked(InputManager::MouseCodes::button_right);
	else if (button == GLFW_MOUSE_BUTTON_LEFT && state == GLFW_PRESS)
		inputManager.mouseClicked(InputManager::MouseCodes::button_left);
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && state == GLFW_PRESS)
		inputManager.mouseClicked(InputManager::MouseCodes::button_middle);
}

bool processInput(bool continueApplication){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
		|| glfwWindowShouldClose(window) != 0) {
		return false;
	}
	deltaTime = 1 / TimeManager::Instance().CalculateFrameRate(false);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		
		shader.turnOn();

		// Bind Texture
		glBindTexture(GL_TEXTURE_2D, texture);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		shader.turnOff();

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

