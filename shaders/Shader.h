#pragma once

#include <string>
#include "../gl/glew.h"
#include <windows.h>

#include "../glm/glm.hpp"

//GLSL wrapper
class Shader
{
public:
	Shader(void);
	~Shader(void);

private:
	Shader(const Shader&);
	//Shader& operator=(const Shader&);

private:
	std::string m_name;
	GLuint m_vertexShader;       //identifier for the vertex shader
	GLuint m_fragmentShader;     //identifier for the fragment shader
	GLuint m_geometryShader;     //identifier for the fragment shader
	GLuint m_programObject;      //identifier for the program- this is used when rendering.

public:
	//Returns what we need for rendering
	GLuint handle(void) const { return m_programObject; } 

	//Loads the shader program from two text files
	bool load(const std::string name, const char* vertexFilename, const char* fragmentFilename);

	bool load3(const std::string name, const char * vertexFilename, const char * fragmentFilename, const char* geometryFilename);
	
	//Utility functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec2(const std::string &name, const glm::vec2 &value) const;
	void setVec2(const std::string &name, float x, float y) const;
	void setVec3(const std::string &name, const glm::vec3 &value) const;
	void setVec3(const std::string &name, float x, float y, float z) const;
	void setVec4(const std::string &name, const glm::vec4 &value) const;
	void setVec4(const std::string &name, float x, float y, float z, float w) const;
	void setMat2(const std::string &name, const glm::mat2 &mat) const;
	void setMat3(const std::string &name, const glm::mat3 &mat) const;
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
	GLuint loadShader(const char* filename, const GLenum type) const;
	std::string shaderInfoLog(const GLuint shader) const;
	std::string programInfoLog(const GLuint program) const;
};