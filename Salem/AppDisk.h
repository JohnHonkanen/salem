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

	Object * AddObject(std::string path, bool deferred, const char* shader);
	Object * AddObject(Object * object, bool deferred, const char* shader);
private:
	struct impl;

	impl * pImpl;
};

