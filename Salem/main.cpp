#include "Application.h"
#include "Instance.h"
#include <glm\glm.hpp>

using namespace glm;

int main() {
	//Application has to be called first
	Application app = Application("Salem", 1280, 720);

	AppDisk *salem = new AppDisk();
	Instance *instance = new Instance("Assets/Cube/cube.obj");
	mat4 transform = mat4(1.0);
	transform = translate(transform, vec3(2.0f ,0, -4.0f));
	instance->AddInstance(transform);
	transform = translate(transform, vec3(-4.0f, 0, 0.0f));
	instance->AddInstance(transform);

	salem->AddObject(instance);
	
	app.SetDisk(salem); 
	app.Run();
	return 0;
}