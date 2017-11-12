#include "Object.h"
#include "Model.h"
#include "Renderer.h"
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;
struct Object::impl {
	Model *model;
	mat4 transformMatrix;
	quat rotationMatrix;
	mat4 scaleMatrix;

	mat4 modelMatrix;

	void CalculateModelMatrix();

};


Object::Object() : pImpl{ new impl() }
{
}

Object::Object(Model * m): pImpl{ new impl{m}}
{
}


Object::~Object()
{
	delete pImpl;
}

void Object::Render(Renderer *r)
{
	pImpl->model->Render(r, pImpl->modelMatrix);
}

void Object::Translate(vec3 translation)
{
	pImpl->transformMatrix = translate(pImpl->transformMatrix, translation);
	pImpl->CalculateModelMatrix();
}

void Object::Rotate(glm::vec3 rotation)
{
	pImpl->rotationMatrix = quat(rotation) * pImpl->rotationMatrix;
	pImpl->CalculateModelMatrix();
}

void Object::Scale(glm::vec3 scale)
{
	pImpl->scaleMatrix = glm::scale(pImpl->scaleMatrix, scale);
	pImpl->CalculateModelMatrix();
}

glm::mat4 Object::GetModelMatrix()
{
	return pImpl->modelMatrix;
}

void Object::impl::CalculateModelMatrix()
{
	modelMatrix = transformMatrix * toMat4(rotationMatrix) * scaleMatrix;
}
