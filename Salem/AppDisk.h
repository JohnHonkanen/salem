#pragma once
#include <string>

class AppDisk
{
public:
	AppDisk();
	~AppDisk();

	void Update(float dt);
	void Render();
	void Input();

	void addObject(std::string path);
private:
	struct impl;

	impl * pImpl;
};

