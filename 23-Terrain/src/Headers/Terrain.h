/*
 * Terrain.h
 *
 *  Created on: Nov 14, 2019
 *      Author: rey
 */

#ifndef SRC_TERRAIN_H_
#define SRC_TERRAIN_H_

#include "AbstractModel.h"
#include "Texture.h"

class Terrain : public AbstractModel{
public:
	Terrain(float gridX, float gridZ, float size, float maxHeight, std::string heightMap);
	float getHeightTerrain(float worldX, float worldZ);
	virtual ~Terrain();
private:
	float x, z;
	float size;
	float maxHeight;
	float MAX_PIXEL_COLOR = 256 * 256 * 256;
	std::string heightMap;
	float ** heights;
	int imageWidth, imageHeight;
	glm::vec3 computeNormal(int x, int z, unsigned char * data, int imageWidth, int imageHeight);
	float getHeight(int x, int z, unsigned char * data, int imageWidth, int imageHeight);
	float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
};

#endif /* SRC_TERRAIN_H_ */
