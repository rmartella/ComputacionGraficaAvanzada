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

#include "AbstractModel.h"
#include "Texture.h"
#include "Shader.h"

class Mesh : public AbstractModel{
public:
	std::vector<Texture*> textures;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
			std::vector<Texture*> textures);
	~Mesh();
	void render(glm::mat4 parentTrans = glm::mat4(1.0f));
	//void destroy();
	virtual bool rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection);

};

#endif /* HEADERS_MESH_H_ */
