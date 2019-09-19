
#ifndef BOX_H
#define BOX_H
#include "AbstractModel.h"

class Box : public AbstractModel
{
public:
	Box();
	~Box();
	AABB aabb;
};

#endif // BOX_H
