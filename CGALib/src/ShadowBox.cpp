#include "Headers/ShadowBox.h"

ShadowBox::ShadowBox(glm::vec3 direction, Camera * camera, float shadowDistance, float nearPlane, float fovy){
	this->front = glm::normalize(direction);
	this->camera = camera;
	this->shadowDistance = shadowDistance;
	this->nearPlane = nearPlane;
	this->fovy = fovy;
}

void ShadowBox::update(int screenWidth, int screenHeight){

	float asr = (float) screenWidth / (float) screenHeight;
	float nearHeight = 2.0 * nearPlane * tan(glm::radians(fovy / 2.0f));
	float farHeight = 2.0 * shadowDistance * tan(glm::radians(fovy / 2.0f));
	float nearWidth = 2.0 * nearPlane * asr * tan(glm::radians(fovy / 2.0f));
	float farWidth = 2.0 * shadowDistance * asr * tan(glm::radians(fovy / 2.0f));

	glm::vec3 centerNear = camera->getPosition() + camera->getFront() * nearPlane;
	glm::vec3 centerFar = camera->getPosition() + camera->getFront() * shadowDistance;

	glm::vec3 p0 = centerNear + nearWidth * camera->getRight()
			+ nearHeight * camera->getUp();
	glm::vec3 p1 = centerNear - nearWidth * camera->getRight()
			+ nearHeight * camera->getUp();
	glm::vec3 p2 = centerNear - nearWidth * camera->getRight()
			- nearHeight * camera->getUp();
	glm::vec3 p3 = centerNear + nearWidth * camera->getRight()
			- nearHeight * camera->getUp();
	glm::vec3 p4 = centerFar + farWidth * camera->getRight()
			+ farHeight * camera->getUp();
	glm::vec3 p5 = centerFar - farWidth * camera->getRight()
			+ farHeight * camera->getUp();
	glm::vec3 p6 = centerFar - farWidth * camera->getRight()
			- farHeight * camera->getUp();
	glm::vec3 p7 = centerFar + farWidth * camera->getRight()
			- farHeight * camera->getUp();

	right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), front));
	up = glm::normalize(glm::cross(front, right));

	glm::mat4 lightViewMatrix = glm::mat4(1.0f);
	lightViewMatrix[0] = glm::vec4(right, 0.0);
	lightViewMatrix[1] = glm::vec4(up, 0.0);
	lightViewMatrix[2] = glm::vec4(front, 0.0);

	p0 = glm::vec3(lightViewMatrix * glm::vec4(p0, 0.0));
	p1 = glm::vec3(lightViewMatrix * glm::vec4(p1, 0.0));
	p2 = glm::vec3(lightViewMatrix * glm::vec4(p2, 0.0));
	p3 = glm::vec3(lightViewMatrix * glm::vec4(p3, 0.0));
	p4 = glm::vec3(lightViewMatrix * glm::vec4(p4, 0.0));
	p5 = glm::vec3(lightViewMatrix * glm::vec4(p5, 0.0));
	p6 = glm::vec3(lightViewMatrix * glm::vec4(p6, 0.0));
	p7 = glm::vec3(lightViewMatrix * glm::vec4(p7, 0.0));

	for(int i = 0; i < 3; i++){
		mins[i] = FLT_MAX;
		maxs[i] = -FLT_MAX;
	}

	for(int i = 0; i < 3; i++){
		if(p0[i] < mins[i])
			mins[i] = p0[i];
		if(p1[i] < mins[i])
			mins[i] = p1[i];
		if(p2[i] < mins[i])
			mins[i] = p2[i];
		if(p3[i] < mins[i])
			mins[i] = p3[i];
		if(p4[i] < mins[i])
			mins[i] = p4[i];
		if(p5[i] < mins[i])
			mins[i] = p5[i];
		if(p6[i] < mins[i])
			mins[i] = p6[i];
		if(p7[i] < mins[i])
			mins[i] = p7[i];
		if(p0[i] > maxs[i])
			maxs[i] = p0[i];
		if(p1[i] > maxs[i])
			maxs[i] = p1[i];
		if(p2[i] > maxs[i])
			maxs[i] = p2[i];
		if(p3[i] > maxs[i])
			maxs[i] = p3[i];
		if(p4[i] > maxs[i])
			maxs[i] = p4[i];
		if(p5[i] > maxs[i])
			maxs[i] = p5[i];
		if(p6[i] > maxs[i])
			maxs[i] = p6[i];
		if(p7[i] > maxs[i])
			maxs[i] = p7[i];
	}
}

glm::vec3 ShadowBox::getCenter(){
	glm::mat4 lightViewMatrix = glm::mat4(1.0f);
	lightViewMatrix[0] = glm::vec4(right, 0.0);
	lightViewMatrix[1] = glm::vec4(up, 0.0);
	lightViewMatrix[2] = glm::vec4(front, 0.0);
	glm::mat4 lightViewMatrixInv = glm::inverse(lightViewMatrix);
	glm::vec3 mins = glm::vec3(lightViewMatrixInv * glm::vec4(this->mins, 0));
	glm::vec3 maxs = glm::vec3(lightViewMatrixInv * glm::vec4(this->maxs, 0));
	return (maxs + mins) / 2.0f;
}

float ShadowBox::getWidth(){
	return (maxs[0] - mins[0]);
}

float ShadowBox::getHeight(){
	return (maxs[1] - mins[1]);
}

float ShadowBox::getLength(){
	return (maxs[2] - mins[2]);
}

