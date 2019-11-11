#ifndef CYLINDER_H
#define CYLINDER_H
#define _USE_MATH_DEFINES
#include <cmath>
#include "AbstractModel.h"

class Cylinder : public AbstractModel
{
public:
	Cylinder(int slices, int stacks, float topRadius = 1.0, float bottomRadius = 1.0, float height = 1.0);
	~Cylinder();
	int getSlices() {
		return this->slices;
	}

	int getStacks() {
		return this->stacks;
	}
private:
	int slices;
	int stacks;
};

#endif // CYLINDER_H
