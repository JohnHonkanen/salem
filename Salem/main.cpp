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
	Instance *instance = new Instance("nightshade");
	mat4 transform = mat4(1.0);
	transform = translate(transform, vec3(2.0f ,0, -5.0f));
	transform = scale(transform, vec3(0.02f));
	instance->AddInstance(transform);
	transform = glm::mat4(1.0f);
	transform = translate(transform, vec3(-4.0f, 0, -5.0f));
	transform = scale(transform, vec3(0.02f));
	instance->AddInstance(transform);

	//Shader information
	instance = (Instance*)salem->AddObject(instance, true, "geometry_instance"); // All deferred shading needs to use geometry shader
	
	Object *object = salem->AddObject("cube", true, "geometry");
	object->Translate(vec3(0.0f ,0.0f, -40.0f));
	object->Scale(vec3(3.0f));
	object->SetMaterialMaps("Assets/Textures/container2.bmp", "", "");

	Object *object2 = salem->AddObject("cube", true, "geometry");
	object2->Translate(vec3(10.0f, 0.0f, -15.0f));
	object2->SetMaterialMaps("Assets/Textures/container2.bmp", "", "");
	app.SetDisk(salem); 
	app.Run();
	return 0;
}