#include "ginkgopch.h"
#include "shader.h"

#include <fstream>

Shader::Shader() {
	programId = glCreateProgram();
}

void Shader::AddShader(const char* file, const GLenum type) {
	std::ifstream f;
	f.open(file, std::ios_base::binary);
	f.seekg(0, std::ios_base::end);
	int length = (int)f.tellg();

	char* src = new char[length + 1];
	f.seekg(0, std::ios_base::beg);
	f.read(src, length);
	src[length] = char(0);

	const char* srcptr[1] = { src };
	int shader = glCreateShader(type);
	glAttachShader(programId, shader);
	glShaderSource(shader, 1, srcptr, NULL);
	glCompileShader(shader);
	delete [] src;

	// TODO: report to ImGUI logger
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* buffer = new char[length];
		glGetShaderInfoLog(shader, length, NULL, buffer);
		printf("Compile log (%s):\n%s\n", file, buffer);
		delete [] buffer;
	} else {
		std::cout << "Shader " << file << " compiled successfully." << std::endl;
	}

	glDeleteShader(shader);
}

void Shader::LinkProgram() {
	glLinkProgram(programId);
	int status;
	glGetProgramiv(programId, GL_LINK_STATUS, &status);

	// TODO: report to ImGUI logger
	if (status != 1) {
		int length;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
		char* buffer = new char[length];
		glGetProgramInfoLog(programId, length, NULL, buffer);
		printf("Link log:\n%s\n", buffer);
		delete [] buffer;
	}
}

void Shader::TransformFeedbackVaryings(GLsizei count, const char** varyings,GLenum bufferMode)
{
	glTransformFeedbackVaryings(programId,count, varyings,GL_INTERLEAVED_ATTRIBS);
}

void Shader::UseShader() {
	glUseProgram(programId);
}

void Shader::UnuseShader() {
	glUseProgram(0);
}
