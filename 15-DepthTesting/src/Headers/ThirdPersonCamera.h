#ifndef THIRDPERSONCAMERA_H
#define THIRDPERSONCAMERA_H
#include "Camera.h"

class ThirdPersonCamera: public Camera
{
public:
    ThirdPersonCamera();
    void mouseMoveCamera(float xoffset, float yoffset, float dt);
    void scrollMoveCamera(float soffset, float dt);
    void updateCamera();
};

#endif // THIRDPERSONCAMERA_H
