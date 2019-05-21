/*
 * Sphere.cpp
 *
 *  Created on: 06/09/2016
 *      Author: rey
 */

#include "Headers/Sphere.h"

Sphere::Sphere(float ratio, int slices, int stacks, MODEL_MODE mode) :
		ratio(ratio), slices(slices), stacks(stacks), VAO(0), VBO(0), EBO(0), mode(
				mode) {
}

Sphere::~Sphere() {

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

void Sphere::init() {
	if (mode == MODEL_MODE::VERTEX_COLOR)
		vertexC.resize(((slices + 1) * (stacks + 1)));
	else if (mode == MODEL_MODE::VERTEX_LIGHT_COLOR)
		vertexLC.resize(((slices + 1) * (stacks + 1)));
	else if (mode == MODEL_MODE::VERTEX_LIGHT_TEXTURE)
		vertexLT.resize(((slices + 1) * (stacks + 1)));
	index.resize((slices * stacks + slices) * 6);
	for (int i = 0; i <= stacks; ++i) {
		float V = i / (float) stacks;
		float phi = V * M_PI;

		for (int j = 0; j <= slices; ++j) {
			float U = j / (float) slices;
			float theta = U * M_PI * 2.0;

			float X = cos(theta) * sin(phi);
			float Y = cos(phi);
			float Z = sin(theta) * sin(phi);
			if (mode == MODEL_MODE::VERTEX_COLOR) {
				vertexC[i * (slices + 1) + j].position = ratio
						* glm::vec3(X, Y, Z);
				vertexC[i * (slices + 1) + j].color = glm::vec3(0.0f, 1.0f,
						0.0f);
			} else if (mode == MODEL_MODE::VERTEX_LIGHT_COLOR) {
				vertexLC[i * (slices + 1) + j].position = ratio
						* glm::vec3(X, Y, Z);
				vertexLC[i * (slices + 1) + j].color = glm::vec3(0.0f, 1.0f,
						0.0f);
				vertexLC[i * (slices + 1) + j].normal = glm::vec3(X, Y, Z);
			} else if (mode == MODEL_MODE::VERTEX_LIGHT_TEXTURE) {
				vertexLT[i * (slices + 1) + j].position = ratio
						* glm::vec3(X, Y, Z);
				vertexLT[i * (slices + 1) + j].texture = glm::vec2(U, V);
				vertexLT[i * (slices + 1) + j].normal = glm::vec3(X, Y, Z);
			}
		}
	}

	for (int i = 0; i < slices * stacks + slices; ++i) {
		index[i * 6] = i;
		index[i * 6 + 1] = i + slices + 1;
		index[i * 6 + 2] = i + slices;
		index[i * 6 + 3] = i + slices + 1;
		index[i * 6 + 4] = i;
		index[i * 6 + 5] = i + 1;
	}
}

void Sphere::load() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	size_t stride;

	size_t offset1 = 0;
	size_t offset2 = 0;
	size_t offset3 = 0;

	if (mode == MODEL_MODE::VERTEX_COLOR) {
		glBufferData(GL_ARRAY_BUFFER, vertexC.size() * sizeof(glm::vec3) * 2,
				vertexC.data(),
				GL_STATIC_DRAW);
		stride = sizeof(vertexC[0]);
		offset1 = 0;
	} else if (mode == MODEL_MODE::VERTEX_LIGHT_COLOR) {
		glBufferData(GL_ARRAY_BUFFER, vertexLC.size() * sizeof(glm::vec3) * 3,
				vertexLC.data(),
				GL_STATIC_DRAW);
		stride = sizeof(vertexLC[0]);
		offset1 = 0;
		offset2 = sizeof(vertexLC[0].position);
		offset3 = sizeof(vertexLC[0].position) + sizeof(vertexLC[0].color);
	} else if (mode == MODEL_MODE::VERTEX_LIGHT_TEXTURE) {
		glBufferData(GL_ARRAY_BUFFER,
				vertexLT.size() * (sizeof(glm::vec3) * 2 + sizeof(glm::vec2)),
				vertexLT.data(),
				GL_STATIC_DRAW);
		stride = sizeof(vertexLT[0]);
		offset1 = 0;
		offset2 = sizeof(vertexLT[0].position);
		offset3 = sizeof(vertexLT[0].position) + sizeof(vertexLT[0].texture);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint),
			index.data(),
			GL_STATIC_DRAW);

	// First attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) offset1);
	glEnableVertexAttribArray(0);
	// Second attribute
	if (mode == MODEL_MODE::VERTEX_LIGHT_TEXTURE)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
				(GLvoid*) offset2);
	else
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
				(GLvoid*) offset2);
	glEnableVertexAttribArray(1);
	// Thrid attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) offset3);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

}

void Sphere::render() {

	glBindVertexArray(VAO);
	glDrawElements( GL_TRIANGLES, index.size(), GL_UNSIGNED_INT,
			(GLvoid*) (sizeof(GLuint) * 0));
	glBindVertexArray(0);

}

