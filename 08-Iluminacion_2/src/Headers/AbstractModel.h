/*
Creado por Reynaldo Martell
Computacion Grafica e Interaccion Humano Computadora
Fecha: 08/02/2018
*/

#ifndef ABSTRACTMODEL_H
#define ABSTRACTMODEL_H
#include <iostream>
#include <vector>
#include <memory>
#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/glew.h>
#include "Shader.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class AbstractModel
{
public:
	enum TypeModel {
		BOX, SPHERE, CYLINDER, QUAD, TRIANGLE, NUM_MODELS = TRIANGLE + 1
	};

	class SBB {
	public:
		SBB() {
		}

		SBB(glm::vec3 c, float ratio) {
			this->c = c;
			this->ratio = ratio;
		}

		glm::vec3 c;
		float ratio;
	};

	class AABB {
	public:
		AABB() {
		}

		AABB(glm::vec3 mins, glm::vec3 maxs) {
			this->mins = mins;
			this->maxs = maxs;
		}
		AABB(glm::vec3 c, float width, float height, float length) {
			mins.x = c.x - width / 2.0;
			mins.y = c.y - height / 2.0;
			mins.z = c.z - length / 2.0;
			maxs.x = c.x + width / 2.0;
			maxs.y = c.y + height / 2.0;
			maxs.z = c.z + length / 2.0;
		}
		AABB(float minx, float miny, float minz, float maxx, float maxy, float maxz) {
			mins.x = minx;
			mins.y = miny;
			mins.z = minz;
			maxs.x = maxx;
			maxs.y = maxy;
			maxs.z = maxz;
		}
		glm::vec3 mins;
		glm::vec3 maxs;
	};

	class Vertex {
	public:
		Vertex() {
		}

		Vertex(glm::vec3 m_pos, glm::vec2 m_tex, glm::vec3 m_normal) : m_pos(m_pos), m_tex(m_tex), m_normal(m_normal) {
		}

	public:
		glm::vec3 m_pos;
		glm::vec2 m_tex;
		glm::vec3 m_normal;
	};

	class VertexColor {
	public:
		VertexColor() {
		}

		VertexColor(glm::vec3 m_pos, glm::vec3 m_color) : m_pos(m_pos), m_color(m_color) {
		}
	public:
		glm::vec3 m_pos;
		glm::vec3 m_color;
	};

	void init() {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexArray.size() * sizeof(vertexArray[0]), vertexArray.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(index[0]), index.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid*)(sizeof(vertexArray[0].m_pos)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid*)(sizeof(vertexArray[0].m_pos) + sizeof(vertexArray[0].m_tex)));
		glEnableVertexAttribArray(3);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void update() {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		GLvoid* p = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(p, vertexArray.data(), vertexArray.size() * sizeof(vertexArray[0]));
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	void render(glm::mat4 parentTrans = glm::mat4(1.0f)) {
		render(0, index.size(), parentTrans);
	}

	void render(int indexInit, int indexSize, glm::mat4 parentTrans = glm::mat4(1.0f)) {
		shader_ptr->turnOn();
		glBindVertexArray(VAO);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), this->scale);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), this->position);
		glm::quat oX = glm::angleAxis<float>(glm::radians(orientation.x), glm::vec3(1.0, 0.0, 0.0));
		glm::quat oY = glm::angleAxis<float>(glm::radians(orientation.y), glm::vec3(0.0, 1.0, 0.0));
		glm::quat oZ = glm::angleAxis<float>(glm::radians(orientation.z), glm::vec3(0.0, 0.0, 1.0));
		glm::quat ori = oZ * oY * oX;
		glm::mat4 modelMatrix = parentTrans * translate * glm::mat4_cast(ori) * scale;
		this->shader_ptr->setMatrix4("model", 1, GL_FALSE, glm::value_ptr(modelMatrix));
		this->shader_ptr->setVectorFloat4("ourColor", glm::value_ptr(color));
		glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, (GLuint*)(indexInit * sizeof(GLuint)));
		glBindVertexArray(0);
		shader_ptr->turnOff();
		this->enableFillMode();
	}

	void destroy() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &EBO);
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &VAO);
	}

	void setShader(Shader * shader_ptr) {
		this->shader_ptr = shader_ptr;
	}

	Shader * getShader() {
		return this->shader_ptr;
	}

	glm::vec3 getPosition() {
		return this->position;
	}

	glm::vec3 getScale() {
		return this->scale;
	}

	glm::vec3 getOrientation() {
		return this->orientation;
	}

	void setPosition(glm::vec3 position) {
		this->position = position;
	}

	void setScale(glm::vec3 scale) {
		this->scale = scale;
	}

	void setOrientation(glm::vec3 orientation) {
		this->orientation = orientation;
	}

	void enableWireMode() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void enableFillMode() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	TypeModel getTypeModel() {
		return typeModel;
	}

	void setTypeModel(TypeModel typeModel) {
		this->typeModel = typeModel;
	}

	void setColor(const glm::vec4& color) {
		this->shader_ptr->turnOn();
		this->shader_ptr->setVectorFloat4("ourColor", glm::value_ptr(color));
		this->shader_ptr->turnOff();
		this->color = color;
	}

protected:
	Shader * shader_ptr;
	glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
	glm::vec4 color;
	glm::vec3 orientation;
	GLuint VAO, VBO, EBO;
	std::vector<Vertex> vertexArray;
	std::vector<GLuint> index;
	TypeModel typeModel;
};

#endif // ABSTRACTMODEL_H
