#pragma once

#include "ginkgopch.h"

class Shader {
public:
	Shader();

	void AddShader(const char* file, const GLenum type);

	void LinkProgram();
	void TransformFeedbackVaryings(GLsizei count, const char** varyings, GLenum bufferMode);
	void UseShader();
	void UnuseShader();

	void DeleteShader() { glDeleteProgram(programId); }

	int programId;
private:
};