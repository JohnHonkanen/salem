#pragma once

#include <SDL.h>
#include "Application.h"
#include "Instance.h"
#include "FireFly.h"
#include <glm\glm.hpp>
#include <ctime>

using namespace glm;

int main(int argc, char* argv[]) {
	srand(time(NULL));
	//Application has to be called first
	Application app = Application("Salem", 1280, 720);

	AppDisk *salem = new AppDisk();
	Instance *instance = new Instance("cube/cube.obj");

	mat4 transform = mat4(1.0);

	PointLight light0{
		vec3(10.0f, 5.0f, 15.0f), // Position
		vec3(1.0f), // Ambient
		vec3(1.0f), // Diffuse
		vec3(0.15f), // Specular
		1.0f, 0.1f, 3.0f // constant, linear, quadratic
	};
	///Lights////
	salem->AddPointLights(light0);

	transform = mat4(1.0);
	transform = translate(transform, vec3(14.0f, 0.0f, 25.0f));
	instance->AddInstance(transform);

	transform = mat4(1.0);
	transform = translate(transform, vec3(14.0f, 2.0f, 28.0f));
	instance->AddInstance(transform);

	transform = mat4(1.0);
	transform = translate(transform, vec3(10.0f, 0.0f, 25.0f));
	instance->AddInstance(transform);
	///Lights///
	//////////////////////////////////OutSide////////////////////////////////////////
	for (int x = 0; x < 20; x++) {
		for (int z = 0; z < 20; z++) {

			transform = mat4(1.0);
			transform = translate(transform, vec3(2.0f * x, -2.0f, + 2.0f * z));
			instance->AddInstance(transform);
		}
	}

	/////////////////////////////////OutSide/////////////////////////////////////////
	//////////////////////////////////ROOM 1/////////////////////////////////////////
	//////////////////////////////////Floor and Walls////////////////////////////////
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {

			transform = mat4(1.0);
			transform = translate(transform, vec3(2.0f * x, -2.0f, -2.0f * z));
			instance->AddInstance(transform);
			if (x == 0 || x == 9 || z == 0 || z == 9) {
				for (int y = 1; y < 5; y++) {
					/////////ROOM DOOR//////////////////////////////////////////////
					if ((x == 5 && y < 3 && z == 9) || (x == 6 && y < 3 && z == 9) || z == 0) {
						//No walls, for door
					}
					else {
						transform = translate(transform, vec3(0.0, 2.0, 0.0));
						instance->AddInstance(transform);
					}

					
				}
			}
		}
	}
	//////////////////////////////////ROOM 1 ROOF///////////////////////////////////////
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			transform = mat4(1.0);
			transform = translate(transform, vec3(2.0f * x, 8.0f, -2.0f * z));
			instance->AddInstance(transform);
		}
	}

	//////////////////////////////////End ROOM 1/////////////////////////////////////////
	/////////////////////////////////ROOM 2 ////////////////////////////////////////////

	vec3 offset = vec3(0, 0, -20);

	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {

			//////////////////////////HOLE IN FLOOR/////////////////////////////////
			if (x == 7 && z == 5) {
				//Hole in floor
			}
			else {
				transform = mat4(1.0);
				transform = translate(transform, vec3(2.0f * x, -2.0f, -2.0f * z) + offset);
				instance->AddInstance(transform);
			}
			
			if (x == 0 || x == 9 || z == 0 || z == 9) {
				for (int y = 1; y < 5; y++) {
					/////////ROOM DOOR//////////////////////////////////////////////
					if ((x == 5 && y < 3 && z == 0) || (x == 6 && y < 3 && z == 0)) {
						//No walls, for door
					}
					else {
						transform = translate(transform, vec3(0.0, 2.0, 0.0));
						instance->AddInstance(transform);
					}


				}
			}
		}
	}
	//////////////////////////////////ROOM 2 ROOF///////////////////////////////////////
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			transform = mat4(1.0);
			transform = translate(transform, vec3(2.0f * x, 8.0f, -2.0f * z) + offset);
			instance->AddInstance(transform);
		}
	}
	////////////////////////////////End ROOM 2//////////////////////////////////////////
	/////////////////////////////////ROOM 3 ////////////////////////////////////////////

	offset += vec3(0, -12, 0);

	for (int x = 0; x < 70; x++) {
		for (int z = 0; z < 70; z++) {

			transform = mat4(1.0);
			transform = translate(transform, vec3(2.0f * x, -2.0f, -2.0f * z) + offset);
			instance->AddInstance(transform);

			if (x == 0 || x == 69 || z == 0 || z == 69) {
				for (int y = 1; y < 5; y++) {
					transform = translate(transform, vec3(0.0, 2.0, 0.0));
					instance->AddInstance(transform);
				}
			}
		}
	}
	//////////////////////////////////ROOM 3 ROOF///////////////////////////////////////
	for (int x = 0; x < 70; x++) {
		for (int z = 0; z < 70; z++) {
			if (x == 7 && z == 5) {
				//Hole in roof
			}
			else {
				transform = mat4(1.0);
				transform = translate(transform, vec3(2.0f * x, 8.0f, -2.0f * z) + offset);
				instance->AddInstance(transform);
			}
			
		}
	}
	////////////////////////////////End ROOM 3//////////////////////////////////////////
	PointLight light3{
		vec3(70.0f, 2.0f, -70.0f) + offset, // Position
		vec3(80.0f), // Ambient
		vec3(80.0f), // Diffuse
		vec3(0.15f), // Specular
		1.0f, 0.1f, 3.0f // constant, linear, quadratic
	};

	transform = mat4(1.0);
	transform = translate(transform, vec3(70.0f, 2.0f, -70.0f) + offset);
	instance->AddInstance(transform);

	//Shader information
	instance = (Instance*)salem->AddObject(instance, true, "geometry_instance_no_map"); // All deferred shading needs to use geometry shader
	instance->SetMaterialMaps("Assets/Textures/wall.jpg", "Assets/Textures/wall.jpg", "", "");

	Instance *shroom = new Instance("boletus/boletus.dae");

	mat4 base = mat4(1.0);
	base = scale(base,  vec3(0.02f));
	base = rotate(base, radians(-90.0f), vec3(1,0,0));

	mat4 mTransform(1.0);
	mTransform = translate(mTransform, vec3(7.0f, -1.0f, -10.0f));

	shroom->AddInstance(mTransform * base);
	for (int i = 0; i < 15; i++) {
		mTransform = mat4(1.0);
		mTransform = translate(mTransform, vec3(rand() % 17, -1.0f, -rand() % 20));
		mTransform = scale(mTransform, vec3((rand() % 50)/10.0f));
		shroom->AddInstance(mTransform * base);
	}

	for (int i = 0; i < 30; i++) {
		mTransform = mat4(1.0);
		mTransform = translate(mTransform, vec3(rand() % 17, 7.0f, -rand() % 19));
		mTransform = rotate(mTransform, radians(180.0f), vec3(1, 0, 0));
		mTransform = scale(mTransform, vec3((rand() % 20) / 10.0f));
		shroom->AddInstance(mTransform * base);
	}

	for (int i = 0; i < 30; i++) {
		mTransform = mat4(1.0);
		mTransform = translate(mTransform, vec3(1, rand() % 8, -rand() % 19));
		mTransform = rotate(mTransform, radians(-90.0f), vec3(0, 0, 1));
		mTransform = scale(mTransform, vec3((rand() % 20) / 10.0f));
		shroom->AddInstance(mTransform * base);
	}

	shroom = (Instance*)salem->AddObject(shroom, true, "geometry_instance_no_map"); // All deferred shading needs to use geometry shader


	//gangnam
	Object * object = salem->AddObject("nightshade/nightshade.dae", true, "skinned");
	object->Translate(vec3(7.0f, -1.0f, -27.0f));
	object->Scale(vec3(0.0003f));
	object->SetMaterialMaps("Assets/Textures/nightshade_diffuse.png",
		"Assets/Textures/nightshade_specular.png",
		"Assets/Textures/nightshade_normal.png");

	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			vec3 position = vec3(20.0f + 10.0f * x, 0.0f,  -20.0f + -10.0f * z) + offset;
			vec3 ambient = vec3((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
			vec3 diffuse = ambient;//vec3((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f);
			vec3 specular = vec3(1.0f);

			PointLight light{
				position, // Position
				ambient, // Ambient
				diffuse, // Diffuse
				specular, // Specular
				1.0f, 1.0f, 3.0f // constant, linear, quadratic
			};

			FireFly * firefly = new FireFly(ambient);
			firefly = (FireFly*)salem->AddObject(firefly, false, "particles");
			firefly->Translate(position + vec3(0,1,0));

			salem->AddPointLights(light);			
		}
	}

	salem->AddPointLights(light3);

	app.SetDisk(salem); 
	app.Run();
	return 0;
}