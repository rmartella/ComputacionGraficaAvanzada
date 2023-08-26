/*
 * Texture.cpp
 *
 *  Created on: 10/04/2015
 *      Author: rey
 */

#include "Headers/Texture.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "Headers/stb_image.h"

Texture::Texture(const std::string& FileName) {
	this->m_fileName = FileName;
}

Texture::Texture(GLenum TextureTarget, const std::string& FileName) :
		m_textureObj(0), type("") {
	this->m_textureTarget = TextureTarget;
	this->m_fileName = FileName;
}

Texture::~Texture() {
	// glDeleteTextures(1, &m_textureObj);
}

void Texture::loadImage(bool flipImage) {
	const char * filename = m_fileName.c_str();
	this->data = stbi_load(filename, &this->imageWidth, &this->imageHeight, &this->channels, 0);
	std::cout << "La textura :=>" << this->m_fileName << " tiene " << this->channels << " Canales" << std::endl;
}

/*GLubyte* Texture::convertToData(FIBITMAP* bitmap, int &width, int &height) {
	// Se obtiene las dimensiones de la imagen.
	width = FreeImage_GetWidth(bitmap);
	height = FreeImage_GetHeight(bitmap);
	// Se obtiene un apuntador a los datos de la textura como un arreglo de unsigned bytes.
	return FreeImage_GetBits(bitmap);
}*/

void Texture::freeImage() {
	// Unload the 32-bit colour bitmap
	// Se desecha los datos de la textura
	stbi_image_free(data);
}

bool Texture::load() {

	this->loadImage();

	// Se genera un buffer para textura en la GPU
	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);

	// Se envian los datos de la textura
	glTexImage2D(GL_TEXTURE_2D, // Tipo de textura
			0, // Niveles del Mipmap
			channels == 3 ? GL_RGB : GL_RGBA, //Formato intero, RGBA
			imageWidth, // Ancho de la textura
			imageHeight, // Ancho de la textura
			0, // Borde de la textura
			channels == 3 ? GL_RGB : GL_RGBA, // Formato que se maneja la textura
			GL_UNSIGNED_BYTE, // Tipo de datos de la textura
			this->data); // Imagen que se usa para esta textura
	// Se indica el tipo de interpolacion para ajustar la imagen que se cargo a la GPU
	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Unload the 32-bit colour bitmap
	// Se desecha los datos de la textura
	freeImage();

	// Desenlazamos la textura
	glBindTexture(m_textureTarget, 0);
	return true;
}

void Texture::bind(GLenum TextureUnit) {
	// Se activan la unidade de textura para el objeto de textura con el que fue creado este objeto
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}
