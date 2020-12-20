// Includes
#include "Shader.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(void)
{
	m_name = "";
	m_vertexShader = 0;
	m_fragmentShader = 0;
	m_geometryShader = 0;
	m_programObject = 0;
}


Shader::~Shader(void)
{
	glUseProgram(0);
	glDeleteShader(m_vertexShader);
	glDeleteShader(m_fragmentShader);
	glDeleteShader(m_geometryShader);
	glDeleteProgram(m_programObject);
}


bool Shader::load(const std::string name, const char* vertexFilename, const char* fragmentFilename)
{
	m_name = name;
	GLint success = 0;
	
	//here we create a vertex shader and set the shader source
	m_vertexShader = loadShader(vertexFilename, GL_VERTEX_SHADER);
	
	//next compile the vertex shader
	glCompileShader(m_vertexShader);
	
	//check if it compiled ok.
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		std::cout << std::endl << "Error compiling GLSL vertex shader: '" << vertexFilename << "'" << std::endl << std::endl;
		std::cout << "Shader info log:" << std::endl << shaderInfoLog(m_vertexShader) << std::endl;
		return false;
	}
	else {
		//create a fragment shader and set the shader source
		m_fragmentShader = loadShader(fragmentFilename, GL_FRAGMENT_SHADER);

		//compile the fragment shader
		glCompileShader(m_fragmentShader);

		//check for any compiling errors
		glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success) {
			std::cout << std::endl << "Error compiling GLSL fragment shader: '" << fragmentFilename << "'" << std::endl << std::endl;
			std::cout << "Shader info log:" << std::endl << shaderInfoLog(m_fragmentShader) << std::endl;
			return false;
		}
		else {
			//create the program
			m_programObject = glCreateProgram();

			//attach the vertex and fragment shaders
			glAttachShader(m_programObject, m_vertexShader);
			glAttachShader(m_programObject, m_fragmentShader);

			//link it all together
			glLinkProgram(m_programObject);

			//check for any errors with the shader program
			glGetProgramiv(m_programObject, GL_LINK_STATUS, &success);
        
			if (!success) {
				std::cout << std::endl << "Error linking GLSL shaders into a shader program." << std::endl;
				std::cout << "GLSL vertex shader: '" << vertexFilename << "'" << std::endl;
				std::cout << "GLSL fragment shader: '" << fragmentFilename << "'" << std::endl << std::endl;
				std::cout << "Program info log:" << std::endl << programInfoLog(m_programObject) << std::endl;
				return false;
			}
		}
    }
	std::cout << "Loaded GLSL program: '" << m_name << "'" << std::endl;
	return true;
}

bool Shader::load3(const std::string name, const char* vertexFilename, const char* fragmentFilename, const char* geometryFilename)
{
	m_name = name;
	GLint success = 0;

	//here we create a vertex shader and set the shader source
	m_vertexShader = loadShader(vertexFilename, GL_VERTEX_SHADER);

	//next compile the vertex shader
	glCompileShader(m_vertexShader);

	//check if it compiled ok.
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		std::cout << std::endl << "Error compiling GLSL vertex shader: '" << vertexFilename << "'" << std::endl << std::endl;
		std::cout << "Shader info log:" << std::endl << shaderInfoLog(m_vertexShader) << std::endl;
		return false;
	}
	else {
		//create a fragment shader and set the shader source
		m_fragmentShader = loadShader(fragmentFilename, GL_FRAGMENT_SHADER);

		//compile the fragment shader
		glCompileShader(m_fragmentShader);

		//check for any compiling errors
		glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success) {
			std::cout << std::endl << "Error compiling GLSL fragment shader: '" << fragmentFilename << "'" << std::endl << std::endl;
			std::cout << "Shader info log:" << std::endl << shaderInfoLog(m_fragmentShader) << std::endl;
			return false;
		}
		else {
			//create a geometry shader and set the shader source
			m_geometryShader = loadShader(geometryFilename, GL_GEOMETRY_SHADER);

			//compile the fragment shader
			glCompileShader(m_geometryShader);

			//check for any compiling errors
			glGetShaderiv(m_geometryShader, GL_COMPILE_STATUS, &success);

			if (!success) {
				std::cout << std::endl << "Error compiling GLSL geometry shader: '" << geometryFilename << "'" << std::endl << std::endl;
				std::cout << "Shader info log:" << std::endl << shaderInfoLog(m_geometryShader) << std::endl;
				return false;
			}
			else {
				//create the program
				m_programObject = glCreateProgram();

				//attach the vertex and fragment shaders
				glAttachShader(m_programObject, m_vertexShader);
				glAttachShader(m_programObject, m_fragmentShader);
				glAttachShader(m_programObject, m_geometryShader);

				//link it all together
				glLinkProgram(m_programObject);

				//check for any errors with the shader program
				glGetProgramiv(m_programObject, GL_LINK_STATUS, &success);

				if (!success) {
					std::cout << std::endl << "Error linking GLSL shaders into a shader program." << std::endl;
					std::cout << "GLSL vertex shader: '" << vertexFilename << "'" << std::endl;
					std::cout << "GLSL fragment shader: '" << fragmentFilename << "'" << std::endl << std::endl;
					std::cout << "GLSL geometry shader: '" << geometryFilename << "'" << std::endl << std::endl;
					std::cout << "Program info log:" << std::endl << programInfoLog(m_programObject) << std::endl;
					return false;
				}
			}
		}
		std::cout << "Loaded GLSL program: '" << m_name << "'" << std::endl;
		return true;
	}
}

//reads the shader from a file and defines the shader source
GLuint Shader::loadShader(const char* filename, GLenum type) const {
	std::string source;
	std::ifstream file(filename, std::ios::binary);

	if (file.is_open())	{
		file.seekg(0, std::ios::end);

		//static cast used to convert between types.
		unsigned int fileSize = static_cast<unsigned int>(file.tellg());
		source.resize(fileSize);
		file.seekg(0, std::ios::beg);
		file.read(&source[0], fileSize);
	}

	GLuint shader = glCreateShader(type);

	//the reinterpret cast is used to convert between pointer types
	const GLchar* glSource = reinterpret_cast<const GLchar*>(source.c_str());
	glShaderSource(shader, 1, &glSource, 0);

	return shader;
}

//Errors and other info for the shaders and the program are obtained
//with the following functions.
std::string Shader::shaderInfoLog(const GLuint shader) const {
	GLsizei infoLogSize = 0;
	std::string infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
	infoLog.resize(infoLogSize);
	glGetShaderInfoLog(shader, infoLogSize, &infoLogSize, &infoLog[0]);

	return infoLog;
}

std::string Shader::programInfoLog(const GLuint program) const {
	GLsizei infoLogSize = 0;
	std::string infoLog;

	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
	infoLog.resize(infoLogSize);
	glGetProgramInfoLog(program, infoLogSize, &infoLogSize, &infoLog[0]);

	return infoLog;
}

//Utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(m_programObject, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(m_programObject, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(m_programObject, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
	glUniform2fv(glGetUniformLocation(m_programObject, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const {
	glUniform2f(glGetUniformLocation(m_programObject, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
	glUniform3fv(glGetUniformLocation(m_programObject, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(m_programObject, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
	glUniform4fv(glGetUniformLocation(m_programObject, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(m_programObject, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
	glUniformMatrix2fv(glGetUniformLocation(m_programObject, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
	glUniformMatrix3fv(glGetUniformLocation(m_programObject, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
	glUniformMatrix4fv(glGetUniformLocation(m_programObject, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}