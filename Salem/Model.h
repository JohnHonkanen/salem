#pragma once
#include <string>
#include <glm\glm.hpp>
#include <vector>
#include "Renderer.h"
using namespace std;

struct MeshData {
	std::vector<GLfloat> vertexArray;
	std::vector<GLfloat> normalArray;
	std::vector<GLfloat> uvArray;
	std::vector<GLfloat> tangentArray;
	std::vector<GLfloat> bitangentArray;

	std::vector<GLuint> indices;
	GLuint numVerts;
	GLuint indexCount;
};

struct Material {
	std::string shader = "default";
	std::string diffuseMap; //Color Map
	std::string normalMap;
	std::string specularMap;
	std::string occulusionMap;
	std::string emissionMap;
	std::string alphaMap;

	glm::vec4 color;
	glm::vec4 specular;
	glm::vec4 diffuse;
	float shininess;
};


class Model
{
public:
	Model(string path);
	~Model();

	void Render(Renderer *r, glm::mat4 modelMatrix);

	vector<MeshData> GetData();
	vector<Material> GetMaterial();
	vector<GLuint> GetVAO();
	void SetMaterialMaps(const char * diffuseMap, const char * specularMap, const char * normalMap);
	void SetShader(const char * shader);
private:
	struct impl;

	impl *pImpl;
};

