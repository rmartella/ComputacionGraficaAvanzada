/*
* collision.h
*
*  Created on: 13/10/2016
*      Author: rey
*/

#ifndef HEADERS_COLLISION_H_
#define HEADERS_COLLISION_H_

#include <float.h>
#include "Mesh.h"
#include "definition.h"

AABB getAABB(std::vector<Mesh> meshes) {
	AABB aabb;
	aabb.min.x = FLT_MAX;
	aabb.min.y = FLT_MAX;
	aabb.min.z = FLT_MAX;
	aabb.max.x = -FLT_MAX;
	aabb.max.y = -FLT_MAX;
	aabb.max.z = -FLT_MAX;

	for (int i = 0; i < meshes.size(); i++) {
		for (int j = 0; j < meshes[i].vertices.size(); j++) {
			if (meshes[i].vertices[j].Position.x < aabb.min.x)
				aabb.min.x = meshes[i].vertices[j].Position.x;
			if (meshes[i].vertices[j].Position.x > aabb.max.x)
				aabb.max.x = meshes[i].vertices[j].Position.x;
			if (meshes[i].vertices[j].Position.y < aabb.min.y)
				aabb.min.y = meshes[i].vertices[j].Position.y;
			if (meshes[i].vertices[j].Position.y > aabb.max.y)
				aabb.max.y = meshes[i].vertices[j].Position.y;
			if (meshes[i].vertices[j].Position.z < aabb.min.z)
				aabb.min.z = meshes[i].vertices[j].Position.z;
			if (meshes[i].vertices[j].Position.z > aabb.max.z)
				aabb.max.z = meshes[i].vertices[j].Position.z;
		}
	}
	return aabb;
}

SBB getSBB(std::vector<Mesh> meshes) {
	AABB aabb = getAABB(meshes);
	SBB sbb;
	sbb.center = glm::vec3((aabb.min.x + aabb.max.x) / 2.0f,
		(aabb.min.y + aabb.max.y) / 2.0f, (aabb.min.z + aabb.max.z) / 2.0f);
	sbb.ratio = sqrt(
		pow(aabb.min.x - aabb.max.x, 2) + pow(aabb.min.y - aabb.max.y, 2)
		+ pow(aabb.min.z - aabb.max.z, 2)) / 2.0f;
	return sbb;
}

bool raySphereIntersect(glm::vec3 orig, glm::vec3 dest, glm::vec3 dir,
	glm::vec3 vSphereCenter, float sphereRadius, float &t) {
	// Vector del Origen del rayo al centro de la esfera.
	glm::vec3 vDirToSphere = vSphereCenter - orig;

	// Distancia del origen al destino del rayo.
	float fLineLength = glm::distance(orig, dest);

	// Proyección escalar de vDirToSphere sobre la direccion del rayo.
	t = glm::dot(vDirToSphere, dir);

	glm::vec3 vClosestPoint;
	// Si la distancia proyectada del origen es menor o igual que cero
	// Significa que el punto mas cercano al centro es el origen.
	if (t <= 0.0f)
		vClosestPoint = orig;
	// Si la proyección escalar del origen es mayor a distancia del origen
	// al destino, el punto mas cercano es el destino.
	else if (t >= fLineLength)
		vClosestPoint = dest;
	// En caso contrario de calcula el punto sobre la linea usando t.
	else
		vClosestPoint = orig + dir * (t);

	// Se pureba si el punto mas cercao esta contenido en el radio de la esfera.
	return glm::distance(vSphereCenter, vClosestPoint) <= sphereRadius;
}

bool testSphereSphereIntersection(SBB sbb1, SBB sbb2) {
	float d = glm::distance(sbb1.center, sbb2.center);
	/*float d = sqrt(
	pow(sbb1.center.x - sbb2.center.x, 2)
	+ pow(sbb1.center.y - sbb2.center.y, 2)
	+ pow(sbb1.center.y - sbb2.center.y, 2));
	std::cout << "d:" << d << std::endl;
	std::cout << "sum:" << sbb1.ratio + sbb2.ratio << std::endl;*/
	if (d <= (sbb1.ratio + sbb2.ratio))
		return true;
	return false;
}

#endif /* HEADERS_COLLISION_H_ */