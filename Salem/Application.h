#pragma once
#include <memory>
typedef unsigned int uint;
class Application
{
public:
	Application();
	Application(const char* name, uint width, uint height);
	~Application();

	void Run();
private:
	struct impl;

	impl * pImpl;
};

