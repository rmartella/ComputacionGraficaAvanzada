/*
 * Colisiones.h
 *
 *  Created on: Dec 18, 2019
 *      Author: rey
 */

#ifndef COLISIONES_H_
#define COLISIONES_H_

#include <map>
#include "AbstractModel.h"

void addOrUpdateColliders(
		std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
		std::string name, AbstractModel::OBB collider, glm::mat4 transform) {
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			colliders.find(name);
	if (it != colliders.end()){
		std::get<0>(it->second) = collider;
		std::get<2>(it->second) = transform;
	}else
		colliders[name] = std::make_tuple(collider, glm::mat4(1.0), transform);
}

void addOrUpdateColliders(
		std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > &colliders,
		std::string name) {
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			colliders.find(name);
	if (it != colliders.end())
		std::get<1>(it->second) = std::get<2>(it->second);
}

void addOrUpdateColliders(
		std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > &colliders,
		std::string name, AbstractModel::SBB collider, glm::mat4 transform) {
	std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			colliders.find(name);
	if (it != colliders.end()){
		std::get<0>(it->second) = collider;
		std::get<2>(it->second) = transform;
	}else
		colliders[name] = std::make_tuple(collider, glm::mat4(1.0), transform);
}

void addOrUpdateColliders(
		std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > &colliders,
		std::string name) {
	std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			colliders.find(name);
	if (it != colliders.end())
		std::get<1>(it->second) = std::get<2>(it->second);
}

void addOrUpdateCollisionDetection(std::map<std::string, bool> &collisionDetector,
		std::string name, bool isCollision) {
	std::map<std::string, bool>::iterator colIt = collisionDetector.find(name);
	if(colIt != collisionDetector.end()){
		if(!colIt->second)
			colIt->second = isCollision;
	}else
		collisionDetector[name] = isCollision;
}

bool raySphereIntersect(glm::vec3 orig, glm::vec3 dest, glm::vec3 dir, AbstractModel::SBB sbb, float &t) {
	// Vector del Origen del rayo al centro de la esfera.
	glm::vec3 vDirToSphere = sbb.c - orig;

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
	return glm::distance(sbb.c, vClosestPoint) <= sbb.ratio;
}

bool testSphereSphereIntersection(AbstractModel::SBB sbb1, AbstractModel::SBB sbb2) {
	float d = glm::distance(sbb1.c, sbb2.c);
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

bool testSphereOBox(AbstractModel::SBB sbb, AbstractModel::OBB obb){
	float d = 0;
	glm::quat qinv = glm::inverse(obb.u);
	sbb.c = qinv * glm::vec4(sbb.c, 1.0);
	obb.c = qinv * glm::vec4(obb.c, 1.0);
	AbstractModel::AABB aabb;
	aabb.mins = obb.c - obb.e;
	aabb.maxs = obb.c + obb.e;
	if (sbb.c[0] >= aabb.mins[0] && sbb.c[0] <= aabb.maxs[0]
			&& sbb.c[1] >= aabb.mins[1] && sbb.c[1] <= aabb.maxs[1]
			&& sbb.c[2] >= aabb.mins[2] && sbb.c[2] <= aabb.maxs[2])
		return true;
	for (int i = 0; i < 3; i++){
		if(sbb.c[i] < aabb.mins[i])
			d += (sbb.c[i] - aabb.mins[i]) * (sbb.c[i] - aabb.mins[i]);
		else if(sbb.c[i] > aabb.maxs[i])
			d += (sbb.c[i] - aabb.maxs[i]) * (sbb.c[i] - aabb.maxs[i]);
	}
	if(d <= sbb.ratio * sbb.ratio)
		return true;
	return false;
}
bool testOBBOBB(AbstractModel::OBB a, AbstractModel::OBB b){
	float EPSILON = 0.0001;
	float ra, rb;
	glm::mat3 R = glm::mat4(0.0), AbsR = glm::mat4(0.0);
	// compute the matrix axis from the quaternions
	glm::mat3 matA = glm::mat3(a.u);
	glm::mat3 matB = glm::mat3(b.u);
	// Compute rotation matrix expressing b in a's coordinate frames
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i][j] = glm::dot(matA[i], matB[j]);
	// Compute translation vector
	glm::vec3 t = b.c - a.c;
	// Bring translation into a’s coordinate frame
	t = glm::vec3(glm::dot(t, matA[0]), glm::dot(t, matA[1]), glm::dot(t, matA[2]));
	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			AbsR[i][j] = fabs(R[i][j]) + EPSILON;
	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) {
		ra = a.e[i];
		rb = b.e[0] * AbsR[i][0] + b.e[1] * AbsR[i][1] + b.e[2] * AbsR[i][2];
		if (fabs(t[i]) > ra + rb) return false;
	}
	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) {
		ra = a.e[0] * AbsR[0][i] + a.e[1] * AbsR[1][i] + a.e[2] * AbsR[2][i];
		rb = b.e[i];
		if (fabs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
	}
	// Test axis L = A0 x B0
	ra = a.e[1] * AbsR[2][0] + a.e[2] * AbsR[1][0];
	rb = b.e[1] * AbsR[0][2] + b.e[2] * AbsR[0][1];
	if(fabs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;

	// Test axis L = A0 x B1
	ra = a.e[1] * AbsR[2][1] + a.e[2] * AbsR[1][1];
	rb = b.e[0] * AbsR[0][2] + b.e[2] * AbsR[0][0];
	if(fabs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return false;

	// Test axis L = A0 x B2
	ra = a.e[1] * AbsR[2][2] + a.e[2] * AbsR[1][2];
	rb = b.e[0] * AbsR[0][1] + b.e[1] * AbsR[0][0];
	if(fabs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;

	// Test axis L = A1 x B0
	ra = a.e[0] * AbsR[2][0] + a.e[2] * AbsR[0][0];
	rb = b.e[1] * AbsR[1][2] + b.e[2] * AbsR[1][1];
	if(fabs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;

	// Test axis L = A1 x B1
	ra = a.e[0] * AbsR[2][1] + a.e[2] * AbsR[0][1];
	rb = b.e[0] * AbsR[1][2] + b.e[2] * AbsR[1][0];
	if(fabs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;

	// Test axis L = A1 x B2
	ra = a.e[0] * AbsR[2][2] + a.e[2] * AbsR[0][2];
	rb = b.e[0] * AbsR[1][1] + b.e[1] * AbsR[1][0];
	if(fabs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;

	// Test axis L = A2 x B0
	ra = a.e[0] * AbsR[1][0] + a.e[1] * AbsR[0][0];
	rb = b.e[1] * AbsR[2][2] + b.e[2] * AbsR[2][1];
	if(fabs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;

	// Test axis L = A2 x B1
	ra = a.e[0] * AbsR[1][1] + a.e[1] * AbsR[0][1];
	rb = b.e[0] * AbsR[2][2] + b.e[2] * AbsR[2][0];
	if(fabs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;

	// Test axis L = A2 x B2
	ra = a.e[0] * AbsR[1][2] + a.e[1] * AbsR[0][2];
	rb = b.e[0] * AbsR[2][1] + b.e[1] * AbsR[2][0];
	if(fabs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;

	return true;
}


#endif /* COLISIONES_H_ */
