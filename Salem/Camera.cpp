#include "Camera.h"
#include "glm\glm.hpp"
#include <SDL.h>

struct Camera::impl {
	glm::mat4 view;
	glm::vec3 up;

	/*float FOV = 90.0f;
	float aspectRatio = float(16.0f / 9.0f);
	float near = 0.1f;
	float far = 100.0f;
*/

};

Camera::Camera()
{
	pImpl = new impl();
}


Camera::~Camera()
{
	delete pImpl;

}

glm::mat4 Camera::GetView()
{
	return glm::lookAt(glm::vec3(transformMatrix[3]), Front(), Up());
}

void Camera::Input()
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	// Forward in positive Z
	if (keys[SDL_SCANCODE_W]) {
		Translate(Front());
	}

	// Backward in negative Z (Going inverse of camera front.
	if (keys[SDL_SCANCODE_S]) {
		Translate(-Front());
	}

	// Left 
	if (keys[SDL_SCANCODE_A]) {
		Translate(-Right());
	}

	// Right
	if (keys[SDL_SCANCODE_D]) {
		Translate(Right());
	}


}
