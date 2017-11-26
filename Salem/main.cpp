#pragma once

#include <SDL.h>
#include "Application.h"
#include "Instance.h"
#include "FireFly.h"
#include <glm\glm.hpp>

using namespace glm;

int main(int argc, char* argv[]) {
	//Application has to be called first
	Application app = Application("Salem", 1280, 720);

	AppDisk *salem = new AppDisk();
	Instance *instance = new Instance("cube/cube.obj");

	mat4 transform = mat4(1.0);

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
					if ((x == 5 && y < 3 && z == 9) || (x == 6 && y < 3 && z == 9)) {
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

	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {

			transform = mat4(1.0);
			transform = translate(transform, vec3(2.0f * x, -2.0f, -2.0f * z) + offset);
			instance->AddInstance(transform);

			if (x == 0 || x == 9 || z == 0 || z == 9) {
				for (int y = 1; y < 5; y++) {
					transform = translate(transform, vec3(0.0, 2.0, 0.0));
					instance->AddInstance(transform);
				}
			}
		}
	}
	//////////////////////////////////ROOM 3 ROOF///////////////////////////////////////
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
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

	//Shader information
	instance = (Instance*)salem->AddObject(instance, true, "geometry_instance_no_map"); // All deferred shading needs to use geometry shader
	instance->SetMaterialMaps("Assets/Textures/wall.jpg", "Assets/Textures/container2_specular.bmp", "");

	//Violin
	//Object * object = salem->AddObject("violinman/violinman.dae", true, "skinned");
	//object->Translate(vec3(10.0f, -1.0f, -27.0f));
	//object->Scale(vec3(0.0001f));
	//object->SetMaterialMaps("Assets/Textures/parasiteZombie_diffuse.png",
	//	"Assets/Textures/parasiteZombie_specular.png",
	//	"Assets/Textures/parasiteZombie_normal.png");

	//gangnam
	/*object = salem->AddObject("nightshade/nightshade.dae", true, "skinned");
	object->Translate(vec3(7.0f, -1.0f, -27.0f));
	object->Scale(vec3(0.0001f));
	object->SetMaterialMaps("Assets/Textures/nightshade_diffuse.png",
		"Assets/Textures/nightshade_specular.png",
		"Assets/Textures/nightshade_normal.png");*/


	Instance *shroom = new Instance("boletus/boletus.dae");

	mat4 base = mat4(1.0);
	base = scale(base,  vec3(0.02f));
	base = rotate(base, radians(-90.0f), vec3(1,0,0));

	mat4 mTransform(1.0);
	mTransform = translate(mTransform, vec3(7.0f, -1.0f, -10.0f));

	shroom->AddInstance(mTransform * base);
	for (int i = 0; i < 60; i++) {
		mTransform = mat4(1.0);
		mTransform = translate(mTransform, vec3(rand() % 20, -1.0f, -rand() % 20));
		mTransform = scale(mTransform, vec3((rand() % 50)/10.0f));
		shroom->AddInstance(mTransform * base);
	}

	for (int i = 0; i < 30; i++) {
		mTransform = mat4(1.0);
		mTransform = translate(mTransform, vec3(rand() % 19, 7.0f, -rand() % 19));
		mTransform = rotate(mTransform, radians(180.0f), vec3(1, 0, 0));
		mTransform = scale(mTransform, vec3((rand() % 20) / 10.0f));
		shroom->AddInstance(mTransform * base);
	}

	for (int i = 0; i < 30; i++) {
		mTransform = mat4(1.0);
		mTransform = translate(mTransform, vec3(1, rand() % 19 - 1, -rand() % 19));
		mTransform = rotate(mTransform, radians(-90.0f), vec3(0, 0, 1));
		mTransform = scale(mTransform, vec3((rand() % 20) / 10.0f));
		shroom->AddInstance(mTransform * base);
	}
	//mTransform = mat4(1.0);
	//shroom->AddInstance(mTransform * base);
	//mTransform = mat4(1.0);
	//shroom->AddInstance(mTransform * base);
	shroom = (Instance*)salem->AddObject(shroom, true, "geometry_instance_no_map"); // All deferred shading needs to use geometry shader

	/* Fire Flies */
	FireFly * firefly = new FireFly();
	firefly = (FireFly*)salem->AddObject(firefly, false, "particles");
	firefly->Translate(vec3(15.0f, 6.0f, -5.0f));

	app.SetDisk(salem); 
	app.Run();
	return 0;
}