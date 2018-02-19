/*
 * Texture.h
 *
 *  Created on: 10/04/2015
 *      Author: rey
 */

#include <string>
#include <GL/glew.h>
#include <FreeImage.h>

class Texture {
public:

	Texture(GLenum TextureTarget, const std::string& FileName);

	bool load();
	void bind(GLenum TextureUnit);
	virtual ~Texture();
private:
	std::string m_fileName;
	GLenum m_textureTarget;
	GLuint m_textureObj;
};
