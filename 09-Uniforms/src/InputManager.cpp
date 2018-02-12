#include <iostream>

#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"

void InputManager::keyPressed(InputCodes code, float deltaTime) {
	switch (code) {
	case kEscape:
		exitApp = true;
		break;
	}
}

void InputManager::mouseMoved(float mouseX, float mouseY) {
	lastMousePos.x = mouseX;
	lastMousePos.y = mouseY;
}

void InputManager::mouseClicked(MouseCodes code) {
	switch (code) {
	case button_right:
		std::cout << "lastMousePos.x:" << lastMousePos.x << std::endl;
		break;
	case button_left:
		std::cout << "lastMousePos.y:" << lastMousePos.y << std::endl;
		break;
	case button_middle:
		std::cout << "lastMousePos.x:" << lastMousePos.x << std::endl;
		std::cout << "lastMousePos.y:" << lastMousePos.y << std::endl;
		break;
	}
}

void InputManager::mouseScroll(float yoffset) {
}
