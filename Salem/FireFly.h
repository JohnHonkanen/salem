#pragma once
#include "Object.h"
#include <glm\glm.hpp>

using namespace glm;

class FireFly : public Object
{
public:
	FireFly(vec3 color);
	~FireFly();
	void Render(Renderer *r);
	void Update(float dt);

	void SetShader(const char * shader);
private:
	struct impl;
	impl *pImpl;
};

