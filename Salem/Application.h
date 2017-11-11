#pragma once
#include "AppDisk.h"
typedef unsigned int uint;
class Application
{
public:
	Application();
	Application(const char* name, uint width, uint height);
	~Application();

	void SetDisk(AppDisk *program);
	void Run();
private:
	struct impl;

	impl * pImpl;
};

