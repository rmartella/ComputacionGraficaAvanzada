/*
 * Terrain.h
 *
 *  Created on: Nov 14, 2019
 *      Author: rey
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

#include "AbstractModel.h"
#include "Texture.h"

class DLL_PUBLIC Terrain : public AbstractModel{
public:
	Terrain(float gridX, float gridZ, float size, float maxHeight, std::string heightMap);
	float getHeightTerrain(float worldX, float worldZ);
	glm::vec3 getNormalTerrain(float worldX, float worldZ);
	virtual ~Terrain();
private:
	float x, z;
	float size;
	float maxHeight;
	float MAX_PIXEL_COLOR = 256 * 256 * 256;
	std::string heightMap;
	float ** heights;
	glm::vec3 ** normals;
	int imageWidth, imageHeight;
	glm::vec3 computeNormal(int x, int z, unsigned char * data, int imageWidth, int imageHeight);
	float getHeight(int x, int z, unsigned char * data, int imageWidth, int imageHeight);
	float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
	glm::vec3 barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
			glm::vec2 pos, glm::vec3 n1, glm::vec3 n2, glm::vec3 n3);
};

#endif /* SRC_TERRAIN_H_ */
