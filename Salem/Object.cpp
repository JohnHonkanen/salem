#include "Object.h"
#include "Model.h"
#include "Renderer.h"
#include "InstanceManager.h"

struct Object::impl {
	Model *model;
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

void Object::Init(InstanceManager *manager)
{
}

void Object::Update(float dt)
{
	pImpl->model->Update(dt);
}

void Object::Input()
{
}

void Object::Render(Renderer *r)
{
	pImpl->model->Render(r, modelMatrix);
}

void Object::Translate(vec3 translation)
{
	transformMatrix = translate(transformMatrix, translation);
	CalculateModelMatrix();
}

void Object::Rotate(glm::vec3 rotation)
{
	rotationMatrix = quat(rotation) *rotationMatrix;
	CalculateModelMatrix();
}

void Object::Scale(glm::vec3 scale)
{
	scaleMatrix = glm::scale(scaleMatrix, scale);
	CalculateModelMatrix();
}

void Object::SetRotation(glm::vec3 rotation)
{
	rotationMatrix = quat(rotation);
	CalculateModelMatrix();
}

vec3 Object::Front()
{
	vec4 v = rotationMatrix * vec4(0,0,1,1);
	return vec3(v);
}

vec3 Object::Right()
{
	vec4 v = rotationMatrix * vec4(1, 0, 0, 1);
	return vec3(v);
}

vec3 Object::Up()
{
	vec4 v = rotationMatrix * vec4(0, 1, 0, 1);
	return vec3(v);
}

glm::mat4 Object::GetModelMatrix()
{
	return modelMatrix;
}

void Object::SetMaterialMaps(const char * diffuseMap, const char * specularMap, const char * normalMap)
{
	pImpl->model->SetMaterialMaps(diffuseMap, specularMap, normalMap);
}

void Object::SetShader(const char * shader)
{
	pImpl->model->SetShader(shader);
}

void Object::CalculateModelMatrix()
{
	modelMatrix = transformMatrix * toMat4(rotationMatrix) * scaleMatrix;
}
