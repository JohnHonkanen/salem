#pragma once
#include <glm\glm.hpp>

using namespace glm;
class Collider
{
public:
	Collider();
	Collider(vec3 halfExtents, vec3 offset);
	bool isColliding(Collider other);
	~Collider();
private:
	struct impl;
	impl * pImpl;
};

