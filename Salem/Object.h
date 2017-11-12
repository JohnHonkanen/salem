#pragma once
#include <glm\glm.hpp>
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
private:
	struct impl;
	impl * pImpl;
};

