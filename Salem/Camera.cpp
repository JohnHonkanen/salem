#include "Camera.h"
#include "glm\glm.hpp"
#include "glm\gtx\compatibility.hpp"
#include <SDL.h>
#include <iostream>


struct Camera::impl {
	glm::mat4 view;
	glm::vec3 up;

	glm::vec3 movement;
	int rotate;
	float cameraSpeed = 7.5f;
	float cameraRotateSpeed = 1.5f;

	float lastX = 640.0f;
	float lastY = 360.0f;

	float yaw;
	float pitch;

};

Camera::Camera()
{
	pImpl = new impl();

	Translate(vec3(3.0f, 0.0f, -5.0f));
}


Camera::~Camera()
{
	delete pImpl;

}

glm::mat4 Camera::GetView()
{
	mat4 lightProjection, lightView;
	float near_plane = 1.0f, far_plane = 2.5f;
	vec3 lightPos(12.0f, 0.0f, -0.0); // Need to update once test is completed
	lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::normalize(vec3(12.0f, -1.0f, -30.0f) - lightPos), glm::vec3(0.0f, 1.0f, 0.0f));
	return lightView;
	
	//return glm::lookAt(glm::vec3(transformMatrix[3]), glm::vec3(transformMatrix[3]) - Front(), Up());
}

void Camera::Input(SDL_Event* sdlEvent)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	glm::vec3 tempMovement = glm::vec3(0);
	int tempRotate = 0;
	float dt = 0.1f;

	// Forward
	if (keys[SDL_SCANCODE_W]) {
		tempMovement.z -= 1;
	}

	// Backward 
	if (keys[SDL_SCANCODE_S]) {
		tempMovement.z += 1;
	}

	// Left 
	if (keys[SDL_SCANCODE_A]) {
		tempMovement.x -= 1;
	}

	// Right
	if (keys[SDL_SCANCODE_D]) {
		tempMovement.x += 1;
	}

	// Left Rotation
	if (keys[SDL_SCANCODE_Q]) {
		tempRotate += 1;
	}

	// Right Rotation
	if (keys[SDL_SCANCODE_E]) {
		tempRotate -= 1;
	}


	if (sdlEvent->type == SDL_MOUSEMOTION || sdlEvent->type == SDL_MOUSEBUTTONDOWN || sdlEvent->type == SDL_MOUSEBUTTONUP) {

		//Get Mouse Position

		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		//std::cout << mouseX << "  , " << mouseY << std::endl;

		float xoffset = mouseX - pImpl->lastX;
		float yoffset = pImpl->lastY - mouseY; // Reversed since y-coordinate range from bottom to top.

		glm::vec2 mouseDir(xoffset, yoffset);

		pImpl->lastX = mouseX;
		pImpl->lastY = mouseY;

		float mouseSensitivity = 0.1f;
		float smoothing = 2.0f;

		mouseDir = mouseDir * vec2(mouseSensitivity * smoothing);
		glm::vec2 smoothV;

		smoothV.x = glm::lerp(smoothV.x, mouseDir.x, 1.0f / smoothing);
		smoothV.y = glm::lerp(smoothV.y, mouseDir.y, 1.0f / smoothing);

		//xoffset *= mouseSensitivity;
		//yoffset *= mouseSensitivity;

		pImpl->yaw -= smoothV.x;
		pImpl->pitch += smoothV.y;

		pImpl->pitch = glm::clamp(pImpl->pitch, -91.0f, 91.0f);
		SetRotation(glm::vec3(glm::radians(pImpl->pitch), glm::radians(pImpl->yaw), 0.0f));
		//Rotate(glm::vec3(xoffset, yoffset, 0.0f));

		pImpl->lastX = 640.0f;
		pImpl->lastY = 360.0f;

		

		//if (pImpl->pitch > 89.0f) {
		//	pImpl->pitch = 89.0f;
		//}

		//if (pImpl->pitch < -89.0f) {
		//	pImpl->pitch = -89.0f;
		//}

		//std::cout << pImpl->yaw << "  , " << pImpl->pitch << std::endl;

	}


	pImpl->movement = tempMovement;
	pImpl->rotate = tempRotate;
}

void Camera::Update(float dt)
{
	Translate(((Front() * pImpl->movement.z) + (Right() * pImpl->movement.x)) * dt * pImpl->cameraSpeed);
	
	Rotate(glm::vec3(0.0f, 1.0f, 0.0f) * float(pImpl->rotate) * dt * pImpl->cameraRotateSpeed);
}
