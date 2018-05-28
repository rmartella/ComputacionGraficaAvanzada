/*
 * Mesh.cpp
 *
 *  Created on: 13/09/2016
 *      Author: rey
 */

#include "Headers/Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
		std::vector<Texture*> textures) :
		vertices(vertices), indices(indices), textures(textures), VAO(0), VBO(
				0), EBO(0) {
	this->loadMesh();
}

Mesh::~Mesh() {
}

void Mesh::destroy() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDeleteBuffers(1, &EBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}

void Mesh::render(Shader * shader) {
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
				shader->getUniformLocation(
						("material." + name + number).c_str()), i);
		// Enlace de la textura
		this->textures[i]->bind(GL_TEXTURE0 + i);
	}

	// Se Dibuja la maya
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Siempre es buena practica colocar los datos defaults antes de ser configurados.
	for (GLuint i = 0; i < this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::loadMesh() {
	// Crea los buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	// Carga los datos de la maya en la GPU
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
			&this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
			&this->indices[0], GL_STATIC_DRAW);

	// Coloca los punteros a los atributos de vertices
	// Posicion de vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*) 0);
	// Vectores normales
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*) offsetof(Vertex, Normal));
	// Coordenadas de mapeo de textura
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
			(GLvoid*) offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

