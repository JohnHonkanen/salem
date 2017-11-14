#pragma once
#include "Object.h"
#include "Model.h"
#include <string>
#include <glm\glm.hpp>

using namespace std;
class InstanceManager;
class Instance : public Object
{
public:
	Instance();
	Instance(string path);
	~Instance();
	void Render(Renderer *r, const char * shader);
	void AddInstance(glm::mat4 transform);
	void Init(InstanceManager *manager);
private:
	struct impl;
	impl * pImpl;
};

