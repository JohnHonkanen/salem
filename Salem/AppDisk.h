#pragma once
#include <string>
#include <SDL.h>
class Object;


class AppDisk
{
public:
	AppDisk();
	~AppDisk();


	void Start();
	void Update(float dt);
	void Render();
	void Input(SDL_Event* sdlEvent);

	void AddObject(std::string path);
	void AddObject(Object * object);
private:
	struct impl;

	impl * pImpl;
};

