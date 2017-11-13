#pragma once
#include <string>

class Object;

class AppDisk
{
public:
	AppDisk();
	~AppDisk();

	void Start();
	void Update();
	void Render();
	void Input();

	void AddObject(std::string path);
	void AddObject(Object * object);
private:
	struct impl;

	impl * pImpl;
};

