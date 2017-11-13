#pragma once
#include <string>
#include <SDL.h>

class AppDisk
{
public:
	AppDisk();
	~AppDisk();

	void Update(float dt);
	void Render();
	void Input(SDL_Event* sdlEvent);

	void addObject(std::string path);
private:
	struct impl;

	impl * pImpl;
};

