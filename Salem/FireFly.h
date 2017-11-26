#pragma once
#include "Object.h"
class FireFly : public Object
{
public:
	FireFly();
	~FireFly();
	void Render(Renderer *r);
	void Update(float dt);

	void SetShader(const char * shader);
private:
	struct impl;
	impl *pImpl;
};

