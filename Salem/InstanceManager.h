#pragma once

#include <string>

class Model;

class InstanceManager
{
public:
	InstanceManager();
	~InstanceManager();

	Model* GetModel(std::string path);

private:
	struct impl;
	impl* pImpl;

};

