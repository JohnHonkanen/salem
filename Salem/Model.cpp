#include "Model.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <fstream>
#include <iostream>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "ShaderManager.h"
#include "TextureManager.h"
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;

#define STORED_VERTEX       0
#define STORED_COLOR        1
#define STORED_UV           2
#define STORED_NORMAL		3
#define STORED_INDEX		4
#define STORED_TANGENT		5
#define STORED_BITANGENT	6

struct Model::impl {
	string pathToDirectory = "Assets/Models/";

	int formatsAllowed = 2;

	string formats[2] = {"obj", "dae"};
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

void Model::Render(Renderer *r, glm::mat4 modelMatrix)
{
	glm::mat4 projection, view;

	r->GetProjection(projection, view);

	ShaderManager * shaderManager = r->GetShaderManager();
	GLuint program = r->GetShader(pImpl->materials[0].shader); // <---- May need to change Material[0] when we do deferred shading.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisable(GL_CULL_FACE);
	TextureManager* textureManager = r->GetTextureManager();

	glUseProgram(program);

	for (int i = 0; i < pImpl->VAO.size(); i++) {
		
		
		

		shaderManager->SetUniformMatrix4fv(program, "projection", projection);
		shaderManager->SetUniformMatrix4fv(program, "view", view);
		shaderManager->SetUniformMatrix4fv(program, "model", modelMatrix);

		// Bind Map textures to texture units
		shaderManager->SetUniformLocation1i(program, "diffuseMap", 0);
		shaderManager->SetUniformLocation1i(program, "specularMap", 1);
		shaderManager->SetUniformLocation1i(program, "emissionMap", 2);
		shaderManager->SetUniformLocation1i(program, "normalMap", 3);

		unsigned int diffuseMap = textureManager->GetTexture(pImpl->materials[i].diffuseMap);
		unsigned int specularMap = textureManager->GetTexture(pImpl->materials[i].specularMap);
		//unsigned int emissionMap = textureManager.GetTexture(materials[i].emissionMap);
		unsigned int normalMap = textureManager->GetTexture(pImpl->materials[i].normalMap);

		// Bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		// Bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		//// Bind emission map
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, emissionMap);

		// Bind specular map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMap);

		glBindVertexArray(pImpl->VAO[i]);
		glDrawElements(GL_TRIANGLES, pImpl->data[i].indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);

	}
}

vector<MeshData> Model::GetData()
{
	return pImpl->data;
}

vector<Material> Model::GetMaterial()
{
	return pImpl->materials;
}

vector<GLuint> Model::GetVAO()
{
	return pImpl->VAO;
}

void Model::SetMaterialMaps(const char * diffuseMap, const char * specularMap, const char * normalMap)
{
	pImpl->materials[0].textureDirectory = "Assets/Textures/";
	pImpl->materials[0].diffuseMap = diffuseMap;
	pImpl->materials[0].specularMap = specularMap;
	pImpl->materials[0].normalMap = normalMap;
}

void Model::SetShader(const char * shader)
{
	pImpl->materials[0].shader = shader;
}

void Model::impl::GenerateVAO()
{
	for (int i = 0; i < data.size(); i++) {
		GLuint vao;
		GLuint vertexBuffer, uvBuffer, normalBuffer, elementBuffer, tangentBuffer, bitangentBuffer;
		GLuint* meshBuffers = new GLuint[6];

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
			glVertexAttribPointer(STORED_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glEnableVertexAttribArray(STORED_NORMAL); // Set location in shader
			meshBuffers[STORED_NORMAL] = normalBuffer;
		}


		// Tangent attribute
		if (!data[i].tangentArray.empty()) {
			glGenBuffers(1, &tangentBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
			glBufferData(GL_ARRAY_BUFFER, data[i].tangentArray.size() * sizeof(GLfloat), &data[i].tangentArray[0], GL_STATIC_DRAW);
			glVertexAttribPointer(STORED_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glEnableVertexAttribArray(STORED_TANGENT); // Set location in shader
			meshBuffers[STORED_TANGENT] = tangentBuffer;
		}

		// Bitangent attribute
		if (!data[i].bitangentArray.empty()) {
			glGenBuffers(1, &bitangentBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
			glBufferData(GL_ARRAY_BUFFER, data[i].bitangentArray.size() * sizeof(GLfloat), &data[i].bitangentArray[0], GL_STATIC_DRAW);
			glVertexAttribPointer(STORED_BITANGENT, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); 
			glEnableVertexAttribArray(STORED_BITANGENT); // Set location in shader
			meshBuffers[STORED_BITANGENT] = bitangentBuffer;
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
	string pathToModel;
	bool foundFormat = false;
	int i = 0;
	if (!foundFormat) {
		pathToModel = pathToDirectory + path + "/" + path + "." + formats[i];

		//check if file exists
		std::ifstream fin(pathToModel.c_str());
		if (!fin.fail()) {
			fin.close();
			foundFormat = true;
		}

		i++;

		if(i > formatsAllowed){
			printf("Couldn't open file: %s\n", pathToModel.c_str());
			printf("%s\n", importer.GetErrorString());
			return;
		}
	}
	

	const aiScene *scene = importer.ReadFile(pathToModel, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
	directory = pathToModel.substr(0, path.find_last_of('/'));

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
	std::vector<GLfloat> tangentArray;
	std::vector<GLfloat> bitangentArray;


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

			if (mesh->HasTangentsAndBitangents()) {
				// Tangents
				aiVector3D tangent = mesh->mTangents[face.mIndices[j]];
				tangentArray.push_back(tangent.x);
				tangentArray.push_back(tangent.y);
				tangentArray.push_back(tangent.z);

				// Bitangents
				aiVector3D bitangent = mesh->mBitangents[face.mIndices[j]];
				bitangentArray.push_back(bitangent.x);
				bitangentArray.push_back(bitangent.y);
				bitangentArray.push_back(bitangent.z);
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
	data.tangentArray = tangentArray;
	data.bitangentArray = bitangentArray;

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
