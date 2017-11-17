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

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 5; k++) {
				transform = mat4(1.0);
				transform = translate(transform, vec3(5.0f * i ,5.0f * j, -5.0f * k));
				mat4 scaleM(1.0);
				scaleM = scale(scaleM, vec3(0.02f));
				instance->AddInstance(transform * scaleM);
			}
			
		}

		
		
		
	}

	//Shader information
	instance = (Instance*)salem->AddObject(instance, true, "geometry_instance"); // All deferred shading needs to use geometry shader
	
	Object *object = salem->AddObject("cube", true, "geometry");
	object->Translate(vec3(20.0f ,0.0f, -40.0f));
	object->Scale(vec3(3.0f));
	object->SetMaterialMaps("Assets/Textures/container2.bmp", "Assets/Textures/container2_specular.bmp", "Assets/Textures/cube_normal.png");

	Object *object2 = salem->AddObject("cube", true, "geometry");
	object2->Translate(vec3(10.0f, 0.0f, -15.0f));
	object2->SetMaterialMaps("Assets/Textures/container2.bmp", "Assets/Textures/container2_specular.bmp", "Assets/Textures/cube_normal.png");
	app.SetDisk(salem); 
	app.Run();
	return 0;
}