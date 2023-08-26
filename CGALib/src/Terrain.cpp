/*
 * Terrain.cpp
 *
 *  Created on: Nov 14, 2019
 *      Author: rey
 */

#include "Headers/Terrain.h"


Terrain::Terrain(float gridX, float gridZ, float size, float maxHeight, std::string heightMap) {
	this->x = gridX * size;
	this->z = gridZ * size;
	this->heightMap = heightMap;
	this->size = size;
	this->maxHeight = maxHeight;

	Texture textureHeightMap = Texture(heightMap);
	FIBITMAP * bitmap = textureHeightMap.loadImage(true);
	unsigned char * data = textureHeightMap.convertToData(bitmap, this->imageWidth, this->imageHeight);

	int VERTEX_COUNT = this->imageHeight;

	heights = new float*[VERTEX_COUNT];
	for(int i = 0; i < VERTEX_COUNT; ++i)
		heights[i] = new float[VERTEX_COUNT];

	normals = new glm::vec3*[VERTEX_COUNT];
	for(int i = 0; i < VERTEX_COUNT; ++i)
		normals[i] = new glm::vec3[VERTEX_COUNT];

	int count = VERTEX_COUNT * VERTEX_COUNT;
	vertexArray.resize(count * 3);
	index.resize( 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1));

	int vertexPointer = 0;
	for (int i = 0; i < VERTEX_COUNT; i++) {
		for (int j = 0; j < VERTEX_COUNT; j++) {
			float height = getHeight(j, i, data, imageWidth, this->imageHeight);
			glm::vec3 normal = computeNormal(j, i, data, imageWidth,
					this->imageHeight);
			heights[j][i] = height;
			normals[j][i] = normal;
			vertexArray[vertexPointer] = Vertex(
					glm::vec3(
							this->x
									+ (float) j / ((float) VERTEX_COUNT - 1)
											* size, height,
							this->z
									+ (float) i / ((float) VERTEX_COUNT - 1)
											* size),
					glm::vec2((float) j / ((float) VERTEX_COUNT - 1),
							(float) i / ((float) VERTEX_COUNT - 1)), normal);
			vertexPointer++;
		}
	}
	int pointer = 0;
	for(int gz = 0; gz < VERTEX_COUNT - 1; gz++){
		for(int gx = 0; gx < VERTEX_COUNT - 1; gx++){
			int topLeft = (gz * VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			index[pointer++] = topLeft;
			index[pointer++] = bottomLeft;
			index[pointer++] = topRight;
			index[pointer++] = topRight;
			index[pointer++] = bottomLeft;
			index[pointer++] = bottomRight;
		}
	}

	textureHeightMap.freeImage(bitmap);
}

Terrain::~Terrain() {
	int VERTEX_COUNT = this->imageHeight;
	for(int i = 0; i < VERTEX_COUNT; ++i)
			delete (heights[i]);
}

float Terrain::getHeight(int x, int z, unsigned char * data, int imageWidth, int imageHeight){
	if(x < 0 || x > imageWidth || z < 0 || z >  imageHeight)
		return 0;
	float b = data[x * 4 + z * (imageWidth * 4)];
	float g = data[x * 4 + z * (imageWidth * 4) + 1];
	float r = data[x * 4 + z * (imageWidth * 4) + 2];
	float height = b * g * r;
	height /= MAX_PIXEL_COLOR;
	//height -= 0.5;
	height *= maxHeight;
	/*height += maxPixelColour / 2.0;
	height /= maxPixelColour / 2.0f;
	height *= maxHeight;*/

	return height;

}

glm::vec3 Terrain::computeNormal(int x, int z, unsigned char * data, int imageWidth, int imageHeight){
	float heightL = getHeight(x - 1, z, data, imageWidth, imageHeight);
	float heightR = getHeight(x + 1, z, data, imageWidth, imageHeight);
	float heightD = getHeight(x, z - 1, data, imageWidth, imageHeight);
	float heightU = getHeight(x, z + 1, data, imageWidth, imageHeight);
	glm::vec3 normal = glm::vec3(heightL - heightR, 2.0, heightD - heightU);
	return glm::normalize(normal);
}

float Terrain::getHeightTerrain(float worldX, float worldZ){
	float terrainX = worldX - x - position.x;
	float terrainZ = worldZ - z - position.z;
	float gridSquareSize = size / ((float) this->imageHeight - 1);
	int gridX = floor(terrainX / gridSquareSize);
	int gridZ = floor(terrainZ / gridSquareSize);
	if(gridX < 0 || gridX > this->imageHeight - 1 || gridZ < 0 || gridZ > this->imageHeight - 1)
		return position.y;
	float xCoord = fmod(terrainX, gridSquareSize) / gridSquareSize;
	float zCoord = fmod(terrainZ, gridSquareSize) / gridSquareSize;
	float answer;
	if(xCoord <= (1 - zCoord)){
		answer = barryCentric(glm::vec3(0, heights[gridX][gridZ], 0),
				glm::vec3(1, heights[gridX + 1][gridZ], 0),
				glm::vec3(0, heights[gridX][gridZ + 1], 1),
				glm::vec2(xCoord, zCoord));
	}
	else{
		answer = barryCentric(glm::vec3(1, heights[gridX + 1][gridZ], 0),
				glm::vec3(1, heights[gridX + 1][gridZ + 1], 1),
				glm::vec3(0, heights[gridX][gridZ + 1], 1),
				glm::vec2(xCoord, zCoord));
	}
	return answer + position.y;
}

glm::vec3 Terrain::getNormalTerrain(float worldX, float worldZ){
	float terrainX = worldX - x - position.x;
	float terrainZ = worldZ - z - position.z;
	float gridSquareSize = size / ((float) this->imageHeight - 1);
	int gridX = floor(terrainX / gridSquareSize);
	int gridZ = floor(terrainZ / gridSquareSize);
	if(gridX < 0 || gridX > this->imageHeight - 1 || gridZ < 0 || gridZ > this->imageHeight - 1)
		return glm::vec3(0, 1, 0);
	float xCoord = fmod(terrainX, gridSquareSize) / gridSquareSize;
	float zCoord = fmod(terrainZ, gridSquareSize) / gridSquareSize;
	glm::vec3 answer;
	if(xCoord <= (1 - zCoord)){
		answer = barryCentric(glm::vec3(0, heights[gridX][gridZ], 0),
				glm::vec3(1, heights[gridX + 1][gridZ], 0),
				glm::vec3(0, heights[gridX][gridZ + 1], 1),
				glm::vec2(xCoord, zCoord), normals[gridX][gridZ],
				normals[gridX + 1][gridZ], normals[gridX][gridZ + 1]);
	}
	else{
		answer = barryCentric(glm::vec3(1, heights[gridX + 1][gridZ], 0),
				glm::vec3(1, heights[gridX + 1][gridZ + 1], 1),
				glm::vec3(0, heights[gridX][gridZ + 1], 1),
				glm::vec2(xCoord, zCoord), normals[gridX + 1][gridZ],
				normals[gridX + 1][gridZ + 1], normals[gridX][gridZ + 1]);
	}
	return glm::normalize(answer);
	//return glm::vec3(0, 1, 0);
}

float Terrain::barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos){
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

glm::vec3 Terrain::barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
		glm::vec2 pos, glm::vec3 n1, glm::vec3 n2, glm::vec3 n3) {
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z))
			/ det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z))
			/ det;
	float l3 = 1.0f - l1 - l2;
	return l1 * n1 + l2 * n2 + l3 * n3;
}

