#pragma once
#include "Object.h"

class Camera: public Object 

{
public:
	Camera();
	~Camera();

	glm::mat4 GetView();

	void Input();
	void Update(float dt);
private:
	struct impl;
	impl* pImpl;
};

