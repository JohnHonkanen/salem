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
	glm::vec3 movement;
	int rotate;
	float cameraSpeed = 7.5f;
	float cameraRotateSpeed = 1.5f;
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
	return glm::lookAt(glm::vec3(transformMatrix[3]), glm::vec3(transformMatrix[3]) - Front(), Up());
}

void Camera::Input()
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

	pImpl->movement = tempMovement;
	pImpl->rotate = tempRotate;
}

void Camera::Update(float dt)
{
	Translate(((Front() * pImpl->movement.z) + (Right() * pImpl->movement.x)) * dt * pImpl->cameraSpeed);
	
	Rotate(glm::vec3(0.0f, 1.0f, 0.0f) * float(pImpl->rotate) * dt * pImpl->cameraRotateSpeed);
}
