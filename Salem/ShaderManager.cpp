#include "ShaderManager.h"
#include <glm\gtc\type_ptr.hpp>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>


struct ShaderManager::impl {
	map<string, GLuint> shaders;

	void CreateShader(string name, const char* vertexPath, const char* fragmentPath);
	void Init();
};


ShaderManager::ShaderManager()
{
	pImpl = new impl();
	pImpl->Init();
}


ShaderManager::~ShaderManager()
{
	delete pImpl;
}

GLuint ShaderManager::GetShader(string name)
{
	return pImpl->shaders[name];
}

void ShaderManager::SetUniformMatrix4fv(GLuint program, const char * location, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(program, location), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderManager::SetUniformLocation3f(GLuint program, string location, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(program, location.c_str()), x, y, z);
}

void ShaderManager::SetUniformLocation1f(GLuint program, string location, float x)
{
	glUniform1f(glGetUniformLocation(program, location.c_str()), x);
}

void ShaderManager::SetUniformLocation1i(GLuint program, const char * location, int x)
{
	glUniform1i(glGetUniformLocation(program, location), x);
}

void ShaderManager::impl::CreateShader(string name, const char * vertexPath, const char * fragmentPath)
{
	// 1. Retrieve the vertex/fragment source code from destination file path.
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	GLuint program;

	// Ensure ifsteam objects can throw exceptions.
	vShaderFile.exceptions(ifstream::badbit);
	fShaderFile.exceptions(ifstream::badbit);

	try {
		// Open Shader files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;

		// Read Shader file's buffer content into streams.
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close Shader Files
		vShaderFile.close();
		fShaderFile.close();

		// Convert Shader file streams into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e) {
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
	}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();

	// 2. Compile Shader and get length of each
	GLint vlen;
	GLint flen;
	GLuint vertex, fragment;
	GLint success;

	GLchar infoLog[512];

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	// Print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// Shader Program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	// Print linking errors if any
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	shaders.insert(pair<string, GLuint>(name, program));
}

void ShaderManager::impl::Init()
{
	CreateShader("default", "Shaders/HDRPass.vert", "Shaders/textured.frag");
	CreateShader("instance_shader", "Shaders/instance.vert", "Shaders/instance.frag");
	CreateShader("lightPass", "Shaders/HDRPass.vert", "Shaders/lightPass.frag");
	CreateShader("geometry", "Shaders/geometryPassNormal.vert", "Shaders/geometryPass.frag");
	CreateShader("skinned", "Shaders/skinnedMeshPass.vert", "Shaders/geometryPass.frag");
	CreateShader("skinned_instance", "Shaders/skinnedInstancePass.vert", "Shaders/geometryPass.frag");
	CreateShader("geometry_instance", "Shaders/geometryPass.vert", "Shaders/geometryPass.frag");
	CreateShader("geometry_instance_no_map", "Shaders/geometryPassNoMap.vert", "Shaders/geometryPassNoMap.frag");
	CreateShader("HDRPass", "Shaders/HDRPass.vert", "Shaders/HDRPass.frag");
	CreateShader("BloomPass", "Shaders/bloomPass.vert", "Shaders/bloomPass.frag");
	CreateShader("particles", "Shaders/particles.vert", "Shaders/particles.frag");
	CreateShader("depthMap", "Shaders/shadowMapDepth.vert", "Shaders/shadowMapDepth.frag");
	CreateShader("shadowMapping", "Shaders/shadowMapping.vert", "Shaders/shadowMapping.frag");
}
