/*
 * Mesh.h
 *
 *  Created on: 13/09/2016
 *      Author: rey
 */

#ifndef HEADERS_MESH_H_
#define HEADERS_MESH_H_

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Shader.h"

struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture*> textures;
private:
	GLuint VAO, VBO, EBO;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
			std::vector<Texture*> textures);
	void render(Shader * shader);
	void destroy();
	virtual ~Mesh();
private:
	void loadMesh();
};

#endif /* HEADERS_MESH_H_ */
