#ifndef _Shader_H
#define _Shader_H

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

#include <string>
#include <fstream>
#include <GL/glew.h>
#include <vector>
#include <algorithm>

class DLL_PUBLIC Shader {
public:

	~Shader() {
		destroy();
	}

	std::string loadShaderFile(std::string strFile);

	void initialize(std::string strVertexFile, std::string strFragmentFile, std::vector<std::string> nameFeedback = {});

	GLint getUniformLocation(std::string strVariable);

	void setMatrix4(std::string strVariable, GLsizei count, GLboolean transpose,
			const GLfloat *value) {
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniformMatrix4fv(id, count, transpose, value);
	}

	void setMatrix3(std::string strVariable, GLsizei count, GLboolean transpose,
			const GLfloat *value) {
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniformMatrix3fv(id, count, transpose, value);
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

	void setInt(std::string strVariable, int value){
		this->turnOn();
		GLint id = this->getUniformLocation(strVariable);
		glUniform1i(id, value);
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
	char * convert(const std::string &s);

private:

	GLuint VertexShaderId;
	GLuint FragmentShaderId;
	GLuint ShaderProgramId;
};

#endif
