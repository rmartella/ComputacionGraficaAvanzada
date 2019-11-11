/*
 * Texture.h
 *
 *  Created on: 10/04/2015
 *      Author: rey
 */
#pragma once

#include <string>
#include <GL/glew.h>
#include <FreeImage.h>

class Texture {
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
