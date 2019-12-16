/*
 * Texture.h
 *
 *  Created on: 10/04/2015
 *      Author: rey
 */
#pragma once

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
#include <GL/glew.h>
#include <FreeImage.h>

class DLL_PUBLIC Texture {
public:

	Texture(const std::string& FileName);
	Texture(GLenum TextureTarget, const std::string& FileName);
	FIBITMAP* loadImage( bool flipImage = false);
	GLubyte* convertToData(FIBITMAP* bitmap, int &width, int &height);
	void freeImage(FIBITMAP* bitmap);
	bool load();
	void bind(GLenum TextureUnit);
	virtual ~Texture();

	/*
	Estos setters y getters se agregan para el cargador de modelos
	Type: Representa el tipo de material.
	FileName: Se utiliza para recuperar la textura por nombre.
	*/
	std::string getType() {
		return type;
	}
	void setType(std::string type) {
		this->type = type;
	}
	std::string getFileName() {
		return m_fileName;
	}

private:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	std::string type;
};
