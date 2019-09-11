#ifndef _Shader_H
#define _Shader_H

#include <string>
#include <fstream>
#include <GL/glew.h>
#include <vector>

class Shader {
public:

	~Shader() {
		destroy();
	}

	std::string loadShaderFile(std::string strFile);

	void initialize(std::string strVertexFile, std::string strFragmentFile);

	GLint getUniformLocation(std::string strVariable);

	void setMatrix4(std::string strVariable, GLsizei count, GLboolean transpose,
			const GLfloat *value) {
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniformMatrix4fv(id, count, transpose, value);
	}

	void setMatrix2(std::string strVariable, GLsizei count, GLboolean transpose,
			const GLfloat *value){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniformMatrix3fv(id, count, transpose, value);
	}

	void setFloat(std::string strVariable, float value){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniform1f(id, value);
	}

	void setVectorFloat2(std::string strVariable, const GLfloat *value){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniform2fv(id, 1, value);
	}

	void setVectorFloat2(std::string strVariable, const GLfloat value1, const GLfloat value2){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniform2f(id, value1, value2);
	}

	void setVectorFloat3(std::string strVariable, const GLfloat *value){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniform3fv(id, 1, value);
	}

	void setVectorFloat3(std::string strVariable, const GLfloat value1, const GLfloat value2, const GLfloat value3){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniform3f(id, value1, value2, value3);
	}

	void setVectorFloat4(std::string strVariable, const GLfloat *value){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniform4fv(id, 1, value);
	}

	void setVectorFloat4(std::string strVariable, const GLfloat value1, const GLfloat value2, const GLfloat value3, const GLfloat value4){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniform4f(id, value1, value2, value3, value4);
	}

	void turnOn() {
		glUseProgram(ShaderProgramId);
	}

	void turnOff() {
		glUseProgram(0);
	}

	void destroy();

private:

	GLuint VertexShaderId;
	GLuint FragmentShaderId;
	GLuint ShaderProgramId;
};

#endif
