#ifndef CAMERA_H
#define CAMERA_H

#define SPEED 3.0f
#define SENSITIVTY 25.0f

#include <iostream>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	virtual void mouseMoveCamera(float xoffset, float yoffset, float dt) = 0;
	virtual void scrollMoveCamera(float soffset, float dt) = 0;
	virtual void updateCamera() = 0;
	glm::mat4 getViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	void setPosition(glm::vec3 position) {
		this->position = position;
	}

	glm::vec3 getPosition() {
		return this->position;
	}

	glm::vec3 getUp() {
		return this->up;
	}

	glm::vec3 getFront() {
		return this->front;
	}

	float getSensitivity() {
		return this->sensitivity;
	}

	void setSensitivity(float sensitivity) {
		this->sensitivity = sensitivity;
	}

	float getSpeed() {
		return this->speed;
	}

	void setSpeed(float speed) {
		this->speed = speed;
	}


protected:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;
	float sensitivity;
	float speed;
};

#endif // CAMERA_H
