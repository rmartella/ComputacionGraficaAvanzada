#include "Camera.h"

#ifndef SHADOWBOX_H_
#define SHADOWBOX_H_

class ShadowBox{
public:
	ShadowBox(glm::vec3 direction, Camera * camera, float shadowDistance, float nearPlane, float fovy);
	void update(int screenWidth, int screenHeight);
	glm::vec3 getCenter();
	float getWidth();
	float getHeight();
	float getLength();
private:
glm::vec3 front;
glm::vec3 up;
glm::vec3 right;
glm::vec3 mins;
glm::vec3 maxs;
Camera *camera;
float shadowDistance;
float nearPlane;
float fovy;
};


#endif /* SHADOWBOX_H_ */
