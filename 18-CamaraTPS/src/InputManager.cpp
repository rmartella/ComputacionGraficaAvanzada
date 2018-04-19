#include <iostream>

#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"

InputCodes InputManager::toApplicationKey(int key) {
	switch (key) {
	case 256:
		return InputCodes::kEscape;
	case 87:
		return InputCodes::W;
	case 83:
		return InputCodes::S;
	case 65:
		return InputCodes::A;
	case 68:
		return InputCodes::D;
	case 265:
		return InputCodes::Up;
	case 264:
		return InputCodes::Down;
	case 263:
		return InputCodes::Left;
	case 262:
		return InputCodes::Right;
	case 79:
		return InputCodes::o;
	case 80:
		return InputCodes::p;
	}
}

State InputManager::toApplicationState(int state) {
	switch (state) {
	case 0:
		return State::RELESED;
	case 1:
		return State::PRESSED;
	}
}

MouseButtonIndex InputManager::toMouseButtonIndex(int button) {
	switch (button) {
	case 0:
		return MouseButtonIndex::LEFT;
	case 1:
		return MouseButtonIndex::RIGHT;
	case 2:
		return MouseButtonIndex::MIDDLE;
	}
}

void InputManager::keyPressed(InputCodes code, float deltaTime, State state) {
	keyState[code] = state;
}

void InputManager::mouseMoved(float mouseX, float mouseY) {
	if (mouseButtomState[MouseButtonIndex::LEFT]
		|| mouseButtomState[MouseButtonIndex::RIGHT]) {
		deltax = mouseX - lastMousePos.x;
		deltay = mouseY - lastMousePos.y;
	}
	else {
		deltax = 0;
		deltay = 0;
	}
	lastMousePos.x = mouseX;
	lastMousePos.y = mouseY;
}

void InputManager::mouseClicked(MouseButtonIndex code, float mouseX,
	float mouseY, State state) {
	mouseButtomState[code] = state;
	switch (code) {
	case RIGHT:
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		break;
	case LEFT:
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		break;
	case MIDDLE:
		break;
	}
}

void InputManager::mouseScroll(float yoffset) {
	scrollYoffset = yoffset;
}

void InputManager::do_movement(float deltaTime) {
	// Camera controls
	float cameraSpeed = 50.0f * deltaTime;

	if (keyState[InputCodes::o])
		roty += 0.1;
	if (keyState[InputCodes::p])
		roty -= 0.1;

	// Calculate zoom
	float zoomLevel = scrollYoffset * cameraSpeed;
	distanceFromPlayer -= zoomLevel;

	// Calculate pitch
	if (mouseButtomState[MouseButtonIndex::RIGHT])
		pitch -= deltay * cameraSpeed;
	std::cout << "pitch:" << pitch << std::endl;

	// Calculate Angle Arround
	if (mouseButtomState[MouseButtonIndex::LEFT])
		angleAroundPlayer -= deltax * cameraSpeed;
	std::cout << "angleAroundPlayer:" << angleAroundPlayer << std::endl;

	//Calculate Horizontal distance
	float horizontalDistance = distanceFromPlayer
		* glm::cos(glm::radians(pitch));
	//Calculate Vertical distance
	float verticalDistance = distanceFromPlayer * glm::sin(glm::radians(pitch));

	//Calculate camera position
	float theta = roty + angleAroundPlayer;
	float offsetx = horizontalDistance * glm::sin(glm::radians(theta));
	float offsetz = horizontalDistance * glm::cos(glm::radians(theta));
	cameraPos.x = camera_look_at.x - offsetx;
	cameraPos.z = camera_look_at.z - offsetz;
	cameraPos.y = camera_look_at.y + verticalDistance;

	yaw = 180 - theta;

	scrollYoffset = 0;
	deltax = 0;
	deltay = 0;

}