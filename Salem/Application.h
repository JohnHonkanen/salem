#pragma once
#include <memory>
#include "AppDisk.h"
typedef unsigned int uint;
class Application
{
public:
	Application(const char* name, uint width, uint height);
	~Application();

	void Run();
private:
	struct impl;

	impl * pImpl;
};

