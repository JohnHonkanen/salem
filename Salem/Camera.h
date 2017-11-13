#pragma once
#include "Object.h"
#include <SDL.h>

class Camera: public Object 

{
public:
	Camera();
	~Camera();

	glm::mat4 GetView();

	void Input(SDL_Event* sdlEvent);
	void Update(float dt);

private:
	struct impl;
	impl* pImpl;
};

