#pragma once
#include <string>
#include <SDL.h>
#include "PointLight.h"

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

	Object * AddObject(std::string path, bool deferred, const char* shader);
	Object * AddObject(Object * object, bool deferred, const char* shader);

	void AddPointLights(PointLight light);
private:
	struct impl;

	impl * pImpl;
};

