#include "Model.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "TextureManager.h"
#include <glm\gtc\matrix_transform.hpp>

using namespace glm;
using namespace Assimp;

#define STORED_VERTEX       0
#define STORED_COLOR        1
#define STORED_UV           2
#define STORED_NORMAL		3
#define STORED_INDEX		4
#define STORED_TANGENT		5
#define STORED_BITANGENT	6
#define STORED_BONES_ID		11
#define STORED_BONES_WEIGHT 12

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

struct Model::impl {
	float totalTime = 0;
	string pathToDirectory = "Assets/Models/";

	int formatsAllowed = 3;

	string formats[3] = {"obj", "dae", "md5mesh"};
	string directory;
	string path;

	vector<MeshData> data;
	vector<Material> materials;

	vector<GLuint> VAO;
	vector<vector<GLuint *>> VBO;

	const aiScene *scene;
	vector<Mesh> meshes;
	mat4 globalInverseTransform;
	map<string, uint> boneMapping;
	vector<BoneInfo> boneInfo;
	vector<mat4> boneTransforms;

	uint numBones = 0;

	bool playAnimation = true ;
	map<string, const aiAnimation*> animations;
	const aiAnimation* activeAnimation;


	void GenerateVAO();
	void LoadModel(Assimp::Importer &importer);

	mat4 AiToGLM(aiMatrix4x4 matrix);
	vec3 AiToGLM(aiVector3D vector3d);
	void LoadData(uint meshIndex, aiMesh *mesh, MeshData &data);
	Material LoadMaterial(const aiScene *scene, int index);
	string getTexturePath(aiMaterial *material, aiTextureType type);

	void LoadBones(unsigned int index, const aiMesh* mesh, vector<VertexBoneData> &bones);

	void BoneTransform(float timeInSec, vector<mat4> &transforms);
	void ReadNodeHeirachy(float animationTime, const aiNode* node, const mat4 &parentTransform);
	const aiNodeAnim * FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);

	void CalcInterpolatedScaling(aiVector3D &out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalcInterpolatedPosition(aiVector3D &out, float animationTime, const aiNodeAnim* nodeAnim);
	void CalcInterpolatedRotation(aiQuaternion &out, float animationTime, const aiNodeAnim* nodeAnim);

	uint FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
	uint FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
	uint FindPosition(float animationTime, const aiNodeAnim* nodeAnim);

	void LoadAnimation(string name, const aiAnimation * animaition);
	void setActiveAnimation(string name);
};

Model::Model(string path)
{
	Assimp::Importer importer;
	pImpl = new impl();
	pImpl->path = path;
	pImpl->LoadModel(importer);
	pImpl->GenerateVAO();
}

Model::Model(string path, Assimp::Importer &importer)
{
	pImpl = new impl();
	pImpl->path = path;
	pImpl->LoadModel(importer);
	pImpl->GenerateVAO();

	if (pImpl->scene->HasAnimations()) {
		pImpl->LoadAnimation("default", pImpl->scene->mAnimations[0]);
		pImpl->setActiveAnimation("default");
	}
	
}


Model::~Model()
{
	delete pImpl->scene;
	delete pImpl;
}

#define MAX_BONES 100

void Model::Update(float dt)
{
	if (pImpl->scene->HasAnimations() && pImpl->playAnimation) {
		pImpl->boneTransforms.clear();
		pImpl->totalTime = dt + pImpl->totalTime;

		pImpl->BoneTransform(pImpl->totalTime, pImpl->boneTransforms);
	}
	else {
		pImpl->boneTransforms.clear();
		pImpl->totalTime = 0;
		pImpl->BoneTransform(pImpl->totalTime, pImpl->boneTransforms);
	}
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

	for (uint i = 0; i < pImpl->boneTransforms.size(); i++) {
		string location = "bones[" + to_string(i) + "]";
		shaderManager->SetUniformMatrix4fv(program, location.c_str(), pImpl->boneTransforms[i]);
	}

	for (int i = 0; i < pImpl->meshes.size(); i++) {
		
		shaderManager->SetUniformMatrix4fv(program, "projection", projection);
		shaderManager->SetUniformMatrix4fv(program, "view", view);
		shaderManager->SetUniformMatrix4fv(program, "model", modelMatrix);

		// Bind Map textures to texture units
		shaderManager->SetUniformLocation1i(program, "diffuseMap", 0);
		shaderManager->SetUniformLocation1i(program, "specularMap", 1);
		shaderManager->SetUniformLocation1i(program, "emissionMap", 2);
		shaderManager->SetUniformLocation1i(program, "normalMap", 3);

		

		if (pImpl->materials[i].diffuseMap != "") {
			unsigned int diffuseMap = textureManager->GetTexture(pImpl->materials[i].diffuseMap);
			// Bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
		}
		else {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		if (pImpl->materials[i].specularMap != "") {
			unsigned int specularMap = textureManager->GetTexture(pImpl->materials[i].specularMap);
			// Bind specular map
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);
		}
		else {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		if (pImpl->materials[i].emissionMap != "") {
			unsigned int emissionMap = textureManager->GetTexture(pImpl->materials[i].emissionMap);
			// Bind emission map
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, emissionMap);
		}
		else {
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
			
		if (pImpl->materials[i].normalMap != "") {
			unsigned int normalMap = textureManager->GetTexture(pImpl->materials[i].normalMap);
			// Bind specular map
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, normalMap);
		}
		else {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glBindVertexArray(pImpl->VAO[0]);
		glDrawElementsBaseVertex(GL_TRIANGLES, pImpl->meshes[i].numIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint) * pImpl->meshes[i].baseIndex),
			pImpl->meshes[i].baseVertex);
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

void Model::SetBoneUniforms(unsigned int shader, ShaderManager *shaderM)
{
	for (uint i = 0; i < pImpl->boneTransforms.size(); i++) {
		string location = "bones[" + to_string(i) + "]";
		shaderM->SetUniformMatrix4fv(shader, location.c_str(), pImpl->boneTransforms[i]);
	}
}

void Model::impl::GenerateVAO()
{
	for (int i = 0; i < data.size(); i++) {
		GLuint vao;
		GLuint vertexBuffer, uvBuffer, normalBuffer, elementBuffer, tangentBuffer, bitangentBuffer, boneBuffer;
		GLuint* meshBuffers = new GLuint[7];

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

		//BoneBuffer
		if (!data[i].boneArray.empty()) {
			glGenBuffers(1, &boneBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, boneBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data[i].boneArray[0]) * data[i].boneArray.size(), &data[i].boneArray[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(STORED_BONES_ID);
			glVertexAttribIPointer(STORED_BONES_ID, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
			glEnableVertexAttribArray(STORED_BONES_WEIGHT);
			glVertexAttribPointer(STORED_BONES_WEIGHT, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
			meshBuffers[7] = boneBuffer;
		}



		glBindVertexArray(0); // Unbind VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		VAO.push_back(vao);
		vector<GLuint *> meshBufferList;
		meshBufferList.push_back(meshBuffers);
		VBO.push_back(meshBufferList);
	}
}

void Model::impl::LoadModel(Assimp::Importer &importer)
{
	
	string pathToModel;
	
	pathToModel = pathToDirectory + path;

	//check if file exists
	std::ifstream fin(pathToModel.c_str());
	if (!fin.fail()) {
		fin.close();
	}
	else {
		printf("Couldn't open file: %s\n", pathToModel.c_str());
		printf("%s\n", importer.GetErrorString());
		return;
	}
	
	
	importer.ReadFile(pathToModel, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
	scene = importer.GetOrphanedScene();
	directory = pathToModel.substr(0, pathToModel.find_last_of('/'));

	globalInverseTransform = AiToGLM(scene->mRootNode->mTransformation.Inverse());

	meshes.resize(scene->mNumMeshes);

	uint numVertices = 0;
	uint numIndices = 0;

	for (uint i = 0; i < meshes.size(); i++) {
		meshes[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		meshes[i].baseVertex = numVertices;
		meshes[i].baseIndex = numIndices;

		numVertices += scene->mMeshes[i]->mNumVertices;
		numIndices += meshes[i].numIndices;
	}

	MeshData mData;
	mData.vertexArray.reserve(numVertices);
	mData.normalArray.reserve(numVertices);
	mData.uvArray.reserve(numVertices);
	mData.indices.reserve(numIndices);
	mData.tangentArray.reserve(numVertices);
	mData.bitangentArray.reserve(numVertices);
	mData.boneArray.resize(numVertices);
	mData.numVerts = numVertices;
	mData.indexCount = numIndices;

	for (int i = 0; i < meshes.size(); i++) {
		LoadData(i, scene->mMeshes[i], mData);

		Material mat;
		if (scene->mMeshes[i]->mMaterialIndex >= 0) {
			mat = LoadMaterial(scene, i);
		}
		materials.push_back(mat);
	}

	data.push_back(mData);
}

mat4 Model::impl::AiToGLM(aiMatrix4x4 matrix)
{
	/*return mat4(matrix.a1, matrix.a2, matrix.a3, matrix.a4,
		matrix.b1, matrix.b2, matrix.b3, matrix.b4,
		matrix.c1, matrix.c2, matrix.c3, matrix.c4,
		matrix.d1, matrix.d2, matrix.d3, matrix.d4);*/
	//Need to Double check order
	return mat4(matrix.a1, matrix.b1, matrix.c1, matrix.d1,
				matrix.a2, matrix.b2, matrix.c2, matrix.d2, 
				matrix.a3, matrix.b3, matrix.c3, matrix.d3, 
				matrix.a4, matrix.b4, matrix.c4, matrix.d4);
}

vec3 Model::impl::AiToGLM(aiVector3D vector3d)
{
	return vec3(vector3d.x, vector3d.y, vector3d.z);
}

void Model::impl::LoadData(uint meshIndex, aiMesh * mesh, MeshData &data)
{
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
				data.uvArray.push_back(uv.x);
				data.uvArray.push_back(uv.y);
			}


			if (mesh->HasNormals()) {
				aiVector3D normal = mesh->mNormals[face.mIndices[j]];
				data.normalArray.push_back(normal.x);
				data.normalArray.push_back(normal.y);
				data.normalArray.push_back(normal.z);
			}

			if (mesh->HasTangentsAndBitangents()) {
				// Tangents
				aiVector3D tangent = mesh->mTangents[face.mIndices[j]];
				data.tangentArray.push_back(tangent.x);
				data.tangentArray.push_back(tangent.y);
				data.tangentArray.push_back(tangent.z);

				// Bitangents
				aiVector3D bitangent = mesh->mBitangents[face.mIndices[j]];
				data.bitangentArray.push_back(bitangent.x);
				data.bitangentArray.push_back(bitangent.y);
				data.bitangentArray.push_back(bitangent.z);
			}

			aiVector3D pos = mesh->mVertices[face.mIndices[j]];
			data.vertexArray.push_back(pos.x);
			data.vertexArray.push_back(pos.y);
			data.vertexArray.push_back(pos.z);
		}
		for (int j = 0; j < face.mNumIndices; j++) {
			data.indices.push_back(face.mIndices[j]);

		}
	}

	/*Loads the Bones*/
	LoadBones(meshIndex, mesh, data.boneArray);
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

void Model::impl::LoadBones(unsigned int index, const aiMesh * mesh, vector<VertexBoneData>& bones)
{
	for (uint i = 0; i < mesh->mNumBones; i++) {
		uint boneIndex = 0;
		string boneName(mesh->mBones[i]->mName.data);

		if (boneMapping.find(boneName) == boneMapping.end()) {
			/*Allocate Index to a new bone*/
			boneIndex = numBones;
			numBones++;

			BoneInfo info;
			boneInfo.push_back(info);
			boneInfo[boneIndex].boneOffset = AiToGLM(mesh->mBones[i]->mOffsetMatrix);
			boneMapping[boneName] = boneIndex;
		}
		else {
			boneIndex = boneMapping[boneName];
		}

		for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			uint vertexId = meshes[index].baseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones[vertexId].AddBoneData(boneIndex, weight);
		}
	}
}

void Model::impl::BoneTransform(float timeInSec, vector<mat4> &transforms)
{
	if (!scene->HasAnimations()) {
		return;
	}
	mat4 identity(1.0f);

	float ticksPerSecond = (float)(activeAnimation->mTicksPerSecond != 0 ? activeAnimation->mTicksPerSecond : 25.0f);
	float timeInTicks = timeInSec * ticksPerSecond;
	float animationTime = fmod(timeInTicks, (float)activeAnimation->mDuration);

	ReadNodeHeirachy(animationTime, scene->mRootNode, identity);

	transforms.resize(numBones);

	for (uint i = 0; i < numBones; i++) {
		transforms[i] = boneInfo[i].finalTransformation;
	}
}

void Model::impl::ReadNodeHeirachy(float animationTime, const aiNode * node, const mat4 & parentTransform)
{
	string nodeName(node->mName.data);

	const aiAnimation *animation = activeAnimation;
	mat4 nodeTransformation = AiToGLM(node->mTransformation);

	const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);

	if (nodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D scaling;
		CalcInterpolatedScaling(scaling, animationTime, nodeAnim);
		mat4 scaleMatrix(1.0);
		scaleMatrix = scale(scaleMatrix, AiToGLM(scaling));

		// Interpolate rotationQ and generate rotation transformation matrix
		aiQuaternion rotationQ;
		CalcInterpolatedRotation(rotationQ, animationTime, nodeAnim);
		mat4 rotationMatrix = AiToGLM(aiMatrix4x4(rotationQ.GetMatrix()));

		// Interpolate translation and generate translation transformation matrix
		aiVector3D translation;
		CalcInterpolatedPosition(translation, animationTime, nodeAnim);
		mat4 translationMatrix(1.0);
		translationMatrix = translate(translationMatrix, AiToGLM(translation));

		nodeTransformation = translationMatrix * rotationMatrix * scaleMatrix;
	}

	mat4 globalTransformation = parentTransform * nodeTransformation;

	if (boneMapping.find(nodeName) != boneMapping.end()) {
		uint boneIndex = boneMapping[nodeName];
		boneInfo[boneIndex].finalTransformation = globalTransformation * boneInfo[boneIndex].boneOffset;
		int test = 0;
	}

	for (uint i = 0; i < node->mNumChildren; i++) {
		ReadNodeHeirachy(animationTime, node->mChildren[i], globalTransformation);
	}
}

const aiNodeAnim * Model::impl::FindNodeAnim(const aiAnimation * animation, const string nodeName)
{
	for (uint i = 0; i < animation->mNumChannels; i++) {
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		if (string(nodeAnim->mNodeName.data) == nodeName) {
			return nodeAnim;
		}
	}

	return NULL;
}

void Model::impl::CalcInterpolatedScaling(aiVector3D & out, float animationTime, const aiNodeAnim * nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 1) {
		out = nodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint scalingIndex = FindScaling(animationTime, nodeAnim);
	uint nextScalingIndex = (scalingIndex + 1);

	assert(scalingIndex < nodeAnim->mNumScalingKeys);

	float deltaTime = (float)(nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
	float factor = (animationTime - (float)nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D &start = nodeAnim->mScalingKeys[scalingIndex].mValue;
	const aiVector3D &end = nodeAnim->mScalingKeys[nextScalingIndex].mValue;

	aiVector3D delta = end - start;

	out = start + factor * delta;
}

void Model::impl::CalcInterpolatedPosition(aiVector3D & out, float animationTime, const aiNodeAnim * nodeAnim)
{
	if (nodeAnim->mNumPositionKeys == 1) {
		out = nodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint positionIndex = FindPosition(animationTime, nodeAnim);
	uint nextPositionIndex = (positionIndex + 1);

	assert(nextPositionIndex < nodeAnim->mNumPositionKeys);

	float deltaTime = (float)(nodeAnim->mPositionKeys[nextPositionIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime);
	float factor = (animationTime - (float)nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D &start = nodeAnim->mPositionKeys[positionIndex].mValue;
	const aiVector3D &end = nodeAnim->mPositionKeys[nextPositionIndex].mValue;

	aiVector3D delta = end - start;

	out = start + factor * delta;
}

void Model::impl::CalcInterpolatedRotation(aiQuaternion & out, float animationTime, const aiNodeAnim * nodeAnim)
{
	if (nodeAnim->mNumRotationKeys == 1) {
		out = nodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint rotationIndex = FindRotation(animationTime, nodeAnim);
	uint nextRotationIndex = (rotationIndex + 1);

	assert(nextRotationIndex < nodeAnim->mNumRotationKeys);

	float deltaTime = (float)(nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
	float factor = (animationTime - (float)nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiQuaternion &startRotationQ = nodeAnim->mRotationKeys[rotationIndex].mValue;
	const aiQuaternion &endRotationQ = nodeAnim->mRotationKeys[nextRotationIndex].mValue;

	aiQuaternion::Interpolate(out, startRotationQ, endRotationQ, factor);

	out = out.Normalize();
}

uint Model::impl::FindScaling(float animationTime, const aiNodeAnim * nodeAnim)
{
	assert(nodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

uint Model::impl::FindRotation(float animationTime, const aiNodeAnim * nodeAnim)
{
	assert(nodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < nodeAnim->mNumRotationKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

uint Model::impl::FindPosition(float animationTime, const aiNodeAnim * nodeAnim)
{
	assert(nodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
		if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

void Model::impl::LoadAnimation(string name, const aiAnimation * animation)
{
	animations[name] = animation;
}

void Model::impl::setActiveAnimation(string name)
{
	totalTime = 0;
	activeAnimation = animations[name];
}

void VertexBoneData::AddBoneData(uint boneId, float weight)
{
	int smallest = 0;
	for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(ids); i++) {
		if (weights[i] == 0.0) {
			ids[i] = boneId;
			weights[i] = weight;
			return;
		}

		if (weights[i] < weights[smallest]) {
			smallest = i;
		}
	}

	ids[smallest] = boneId;
	weights[smallest] = weight;
	return;
	//Should never get here, more bones than we have space for
	assert(0);
}
