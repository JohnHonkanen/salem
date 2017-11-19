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
	//Instance *instance = new Instance("nightshade");

	//mat4 transform = mat4(1.0);

	//for (int i = 0; i < 10; i++) {
	//	for (int j = 0; j < 10; j++) {
	//		for (int k = 0; k < 10; k++) {
	//			transform = mat4(1.0);
	//			transform = translate(transform, vec3(5.0f * i ,5.0f * j, -5.0f * k));
	//			mat4 scaleM(1.0);
	//			scaleM = scale(scaleM, vec3(0.02f));
	//			instance->AddInstance(transform * scaleM);
	//		}
	//		
	//	}

	//	
	//	
	//	
	//}

	////Shader information
	//instance = (Instance*)salem->AddObject(instance, true, "geometry_instance"); // All deferred shading needs to use geometry shader
	
	Object *object = salem->AddObject("nightshade/nightshade.dae", true, "geometry");
	object->Translate(vec3(0.0f ,0.0f, -15.0f));
	object->Scale(vec3(0.001f));
	object->SetMaterialMaps("Assets/Textures/Nightshade_diffuse.png", "Nightshade_specular.png", "Nightshade_normal.png");

	/*Object *object2 = salem->AddObject("cube", true, "geometry");
	object2->Translate(vec3(10.0f, 0.0f, -15.0f));
	object2->SetMaterialMaps("Assets/Textures/container2.bmp", "", "");*/
	app.SetDisk(salem); 
	app.Run();
	return 0;
}