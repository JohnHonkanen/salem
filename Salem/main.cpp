#pragma once

#include <SDL.h>
#include "Application.h"
#include "Instance.h"
#include <glm\glm.hpp>

using namespace glm;

int main(int argc, char* argv[]) {
	//Application has to be called first
	Application app = Application("Salem", 1280, 720);

	AppDisk *salem = new AppDisk();
	Instance *instance = new Instance("Assets/Models/boletus/boletus.dae");
	mat4 transform = mat4(1.0);
	transform = translate(transform, vec3(2.0f ,0, -4.0f));
	transform = scale(transform, vec3(0.05f));
	instance->AddInstance(transform);
	transform = glm::mat4(1.0f);
	transform = translate(transform, vec3(-4.0f, 0, -4.0f));
	transform = scale(transform, vec3(0.05f));
	instance->AddInstance(transform);

	salem->AddObject(instance);
	
	app.SetDisk(salem); 
	app.Run();
	return 0;
}