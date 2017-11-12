#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;

class Renderer;
class Model;
class Object
{
public:
	Object();
	Object(Model *m);
	~Object();

	virtual void Render(Renderer *r);
	void Translate(glm::vec3 translation);
	void Rotate(glm::vec3 rotation);
	void Scale(glm::vec3 scale);

	glm::mat4 GetModelMatrix();

protected:
	mat4 transformMatrix;
	quat rotationMatrix;
	mat4 scaleMatrix;

	mat4 modelMatrix;

	void CalculateModelMatrix();
private:
	struct impl;
	impl * pImpl;
};

