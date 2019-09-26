
#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H
#define YAW	-90.0f
#define PITCH 0.0f

#include "Camera.h"

class FirstPersonCamera : public Camera
{
public:
	FirstPersonCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp =
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
		float yaw = YAW, float pitch = PITCH, float speed = SPEED, float sensitivity = SENSITIVTY);
	void mouseMoveCamera(float xoffset, float yoffset, float dt);
	void scrollMoveCamera(float soffset, float dt);
	void updateCamera();
	void moveFrontCamera(bool dir, float dt);
	void moveRightCamera(bool dir, float dt);
};

#endif // FIRSTPERSONCAMERA_H
