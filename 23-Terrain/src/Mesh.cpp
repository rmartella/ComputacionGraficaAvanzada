/*
 * Mesh.cpp
 *
 *  Created on: 13/09/2016
 *      Author: rey
 */

#include "Headers/Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
		std::vector<Texture*> textures) {
	this->textures = textures;
	this->vertexArray = vertices;
	this->index = indices;
	this->VAO = 0;
	this->VBO = 0;
	this->EBO = 0;
	this->bones = nullptr;
	this->init();
}

Mesh::~Mesh() {
	this->destroy();
}

void Mesh::render(glm::mat4 parentTrans) {
	// Enlace de las texturas apropiadas
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	GLuint heightNr = 1;
	for (GLuint i = 0; i < this->textures.size(); i++) {
		// Recupera el numero de la textura (La N en diffuse_textureN)
		std::stringstream ss;
		std::string number;
		std::string name = this->textures[i]->getType();
		if (name == "texture_diffuse")
			ss << diffuseNr++; // Transfer GLuint to stream
		else if (name == "texture_specular")
			ss << specularNr++; // Transfer GLuint to stream
		else if (name == "texture_normal")
			ss << normalNr++; // Transfer GLuint to stream
		else if (name == "texture_height")
			ss << heightNr++; // Transfer GLuint to stream
		number = ss.str();

		// El numero de la textura se enlaza con su nombre y su unidad de textura.
		// Por ejemplo texture_diffuse1, texture_specular1
		glUniform1i(
				shader_ptr->getUniformLocation(
						("material." + name + number).c_str()), i);
		// Enlace de la textura
		this->textures[i]->bind(GL_TEXTURE0 + i);
	}

	// Se Dibuja la maya
	AbstractModel::render(parentTrans);

	// Siempre es buena practica colocar los datos defaults antes de ser configurados.
	for (GLuint i = 0; i < this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

bool Mesh::rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection) {
	return false;
}
