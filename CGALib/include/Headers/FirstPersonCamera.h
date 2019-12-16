
#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

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

#define YAW	-90.0f
#define PITCH 0.0f

#include "Camera.h"

class DLL_PUBLIC FirstPersonCamera : public Camera
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
