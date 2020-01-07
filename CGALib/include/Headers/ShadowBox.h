#include "Camera.h"

#ifndef SHADOWBOX_H_
#define SHADOWBOX_H_

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

class DLL_PUBLIC ShadowBox{
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
