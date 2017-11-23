#include "Collider.h"
struct Collider::impl {
	vec3 position;
	vec3 halfExtents;
	vec3 contactNormal;

	vec3 offset;
};

Collider::Collider()
{
	pImpl = new impl;
}

Collider::Collider(vec3 halfExtents, vec3 offset)
{
	pImpl = new impl;
	pImpl->halfExtents = halfExtents;
	pImpl->offset = offset;
}

bool Collider::isColliding(Collider other)
{
	vec3 box1Min = pImpl->position - pImpl->halfExtents;
	vec3 box1Max = pImpl->position + pImpl->halfExtents;

	vec3 box2Min = other.pImpl->position - other.pImpl->halfExtents;
	vec3 box2Max = other.pImpl->position + other.pImpl->halfExtents;

	return (box1Min.x <= box2Max.x && box1Max.x >= box2Min.x) &&
		(box1Min.y <= box2Max.y && box1Max.y >= box2Min.y) &&
		(box1Min.z <= box2Max.z && box1Max.z >= box2Min.z);
	return false;
}


Collider::~Collider()
{
	delete pImpl;
}
