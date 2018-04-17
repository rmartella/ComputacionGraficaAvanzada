/*
 * Sphere.h
 *
 *  Created on: 06/09/2016
 *      Author: rey
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include "definition.h"

class Sphere {
public:
	Sphere(float ratio, int slices, int stacks, MODEL_MODE mode);
	void init();
	void load();
	void render();
	virtual ~Sphere();
private:
	std::vector<VertexColor> vertexC;
	std::vector<VertexLightColor> vertexLC;
	std::vector<VertexLightTexture> vertexLT;
	std::vector<GLuint> index;
	float ratio;
	int slices;
	int stacks;
	MODEL_MODE mode;

	GLuint VAO, VBO, EBO;
};

#endif /* SPHERE_H_ */
