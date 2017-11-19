#pragma once
#include <string>
#include <glm\glm.hpp>
#include <vector>
#include "Renderer.h"
#include <assimp\Importer.hpp>

using namespace std;
using namespace glm;

#define BONES_PER_VERTEX 4
struct BoneInfo {
	mat4 boneOffset = mat4(0);
	mat4 finalTransformation= mat4(0);
};

struct VertexBoneData {
	unsigned int ids[BONES_PER_VERTEX];
	float weights[BONES_PER_VERTEX];

	void AddBoneData(uint boneId, float weight);

};

struct Mesh {
	unsigned int numIndices;
	unsigned int baseVertex;
	unsigned int baseIndex;
};

struct MeshData {
	std::vector<GLfloat> vertexArray;
	std::vector<GLfloat> normalArray;
	std::vector<GLfloat> uvArray;
	std::vector<GLfloat> tangentArray;
	std::vector<GLfloat> bitangentArray;
	std::vector<VertexBoneData> boneArray;

	std::vector<GLuint> indices;
	GLuint numVerts;
	GLuint indexCount;
};



struct Material {
	std::string textureDirectory;
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
	Model(string path, Assimp::Importer &importer);
	~Model();

	void Update(float dt);
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

