#include "Model.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "ShaderManager.h"
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;

#define STORED_VERTEX       0
#define STORED_COLOR        1
#define STORED_UV           2
#define STORED_NORMAL		3
#define STORED_INDEX		4
#define STORED_TANGENT		5
#define STORED_BITANGENT	6

struct MeshData {
	std::vector<GLfloat> vertexArray;
	std::vector<GLfloat> normalArray;
	std::vector<GLfloat> uvArray;
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

struct Model::impl {
	string directory;
	string path;

	vector<MeshData> data;
	vector<Material> materials;

	vector<GLuint> VAO;
	vector<vector<GLuint *>> VBO;

	void GenerateVAO();
	void LoadModel();
	MeshData LoadData(aiMesh *mesh);
	Material LoadMaterial(const aiScene *scene, int index);
	string getTexturePath(aiMaterial *material, aiTextureType type);
};

Model::Model(string path)
{
	pImpl = new impl();
	pImpl->path = path;
	pImpl->LoadModel();
	pImpl->GenerateVAO();
}


Model::~Model()
{
	delete pImpl;
}

void Model::Render(Renderer *r)
{
	glm::mat4 projection, view;

	r->GetProjection(projection, view);

	ShaderManager shaderManager = r->GetShaderManager();
	GLuint program = r->GetShader(pImpl->materials[0].shader); // <---- May need to change Material[0] when we do deferred shading.

	for (int i = 0; i < pImpl->VAO.size(); i++) {
		glUseProgram(program);

		glm::mat4 model(1.0f);
		model = translate(model, vec3(0.0f, 0.0f, -3.0f));
		
		shaderManager.SetUniformMatrix4fv(program, "projection", projection);
		shaderManager.SetUniformMatrix4fv(program, "view", view);
		shaderManager.SetUniformMatrix4fv(program, "model", model);

		glBindVertexArray(pImpl->VAO[i]);
		glDrawElements(GL_TRIANGLES, pImpl->data[i].indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}
	cout << pImpl->path << endl;
}

void Model::impl::GenerateVAO()
{
	for (int i = 0; i < data.size(); i++) {
		GLuint vao;
		GLuint vertexBuffer, uvBuffer, normalBuffer, elementBuffer;
		GLuint* meshBuffers = new GLuint[4];

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &elementBuffer);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, data[i].vertexArray.size() * sizeof(GLfloat), &data[i].vertexArray[0], GL_STATIC_DRAW);

		meshBuffers[STORED_VERTEX] = vertexBuffer;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data[i].indices.size() * sizeof(GLfloat), &data[i].indices[0], GL_STATIC_DRAW);

		meshBuffers[STORED_INDEX] = elementBuffer;

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// TexCoord attribute
		if (!data[i].uvArray.empty()) {
			glGenBuffers(1, &uvBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
			glBufferData(GL_ARRAY_BUFFER, data[i].uvArray.size() * sizeof(GLfloat), &data[i].uvArray[0], GL_STATIC_DRAW);
			glVertexAttribPointer(STORED_UV, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glEnableVertexAttribArray(STORED_UV);
			meshBuffers[STORED_UV] = uvBuffer;
		}

		// Normals attribute
		if (!data[i].normalArray.empty()) {
			glGenBuffers(1, &normalBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
			glBufferData(GL_ARRAY_BUFFER, data[i].normalArray.size() * sizeof(GLfloat), &data[i].normalArray[0], GL_STATIC_DRAW);
			glVertexAttribPointer(STORED_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); //3 * sizeof(GLfloat)
			glEnableVertexAttribArray(STORED_NORMAL); // Set location in shader
			meshBuffers[STORED_NORMAL] = normalBuffer;
		}

		glBindVertexArray(0); // Unbind VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		VAO.push_back(vao);
		vector<GLuint *> meshBufferList;
		meshBufferList.push_back(meshBuffers);
		VBO.push_back(meshBufferList);
	}
}

void Model::impl::LoadModel()
{
	Assimp::Importer importer;

	//check if file exists
	std::ifstream fin(path.c_str());
	if (!fin.fail()) {
		fin.close();
	}
	else {
		printf("Couldn't open file: %s\n", path.c_str());
		printf("%s\n", importer.GetErrorString());
		return;
	}

	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
	directory = path.substr(0, path.find_last_of('/'));

	for (int i = 0; i < scene->mNumMeshes; i++) {
		MeshData mData = LoadData(scene->mMeshes[i]);
		data.push_back(mData);

		Material mat;
		if (scene->mMeshes[i]->mMaterialIndex >= 0) {
			mat = LoadMaterial(scene, i);
		}
		materials.push_back(mat);
	}
}

MeshData Model::impl::LoadData(aiMesh * mesh)
{
	std::vector<GLfloat> vertexArray;
	std::vector<GLfloat> normalArray;
	std::vector<GLfloat> uvArray;
	std::vector<GLuint> indexArray;

	GLuint numVerts;
	GLuint indexCount = 0;

	numVerts = mesh->mNumFaces * 3;
	indexCount = mesh->mNumFaces;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];

		for (int j = 0; j < 3; j++)
		{
			if (mesh->HasTextureCoords(0)) {
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[j]];
				uvArray.push_back(uv.x);
				uvArray.push_back(uv.y);
			}


			if (mesh->HasNormals()) {
				aiVector3D normal = mesh->mNormals[face.mIndices[j]];
				normalArray.push_back(normal.x);
				normalArray.push_back(normal.y);
				normalArray.push_back(normal.z);
			}


			aiVector3D pos = mesh->mVertices[face.mIndices[j]];
			vertexArray.push_back(pos.x);
			vertexArray.push_back(pos.y);
			vertexArray.push_back(pos.z);
		}
		for (int j = 0; j < face.mNumIndices; j++) {
			indexArray.push_back(face.mIndices[j]);

		}

	}

	MeshData data;
	data.vertexArray = vertexArray;
	data.indices = indexArray;
	data.uvArray = uvArray;
	data.normalArray = normalArray;
	data.numVerts = numVerts;
	data.indexCount = indexArray.size();

	return data;
}

Material Model::impl::LoadMaterial(const aiScene * scene, int index)
{
	aiMesh *mesh = scene->mMeshes[index];
	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	Material mat;
	mat.diffuseMap = getTexturePath(material, aiTextureType_DIFFUSE);
	mat.specularMap = getTexturePath(material, aiTextureType_SPECULAR);
	mat.normalMap = getTexturePath(material, aiTextureType_NORMALS);
	mat.emissionMap = getTexturePath(material, aiTextureType_EMISSIVE);
	mat.alphaMap = getTexturePath(material, aiTextureType_OPACITY);
	mat.occulusionMap = getTexturePath(material, aiTextureType_LIGHTMAP);

	return mat;
}

string Model::impl::getTexturePath(aiMaterial * material, aiTextureType type)
{
	//Might have to change of there are multiple texture of 1 type
	string path = "";
	if (material->GetTextureCount(type) > 0) {
		aiString str;
		material->GetTexture(type, 0, &str);

		path = directory + '/' + str.C_Str();
	}

	return path;
}
