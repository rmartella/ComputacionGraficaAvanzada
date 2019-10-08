#ifndef SPHERE_H
#define SPHERE_H
#define _USE_MATH_DEFINES
#include <cmath>
#include "AbstractModel.h"

class Sphere : public AbstractModel
{
public:
	Sphere(int slices, int stacks, float ratio = 0.5);
	~Sphere();
private:
	SBB sbb;
};

#endif // SPHERE_H
