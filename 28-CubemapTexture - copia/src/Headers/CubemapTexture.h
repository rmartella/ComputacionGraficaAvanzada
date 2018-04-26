/*
 * CubemapTexture.h
 *
 *  Created on: 16/05/2015
 *      Author: rey
 */

#ifndef CUBEMAPTEXTURE_H_
#define CUBEMAPTEXTURE_H_

#include <string>
#include <GL/glew.h>
//#include <ImageMagick-6/Magick++.h>
#include <FreeImage.h>

using namespace std;

/**
 * Clase encargada de crear el skydome, genera el buffer GL_TEXTURE_CUBE_MAP.
 */
class CubemapTexture {
 public:
  /**
   * Constructor del skydome, solo inicializa las texturas de las caras del domo.
   * @param Directory Directorio de las imagenes.
   * @param PosXFilename Cara izquierda.
   * @param NegXFilename Cara derecha.
   * @param PosYFilename Cara superior.
   * @param NegYFilename Cara inferior.
   * @param PosZFilename Cara frontal.
   * @param NegZFilename Cara fondo.
   */
  CubemapTexture(const string& Directory, const string& PosXFilename,
                 const string& NegXFilename, const string& PosYFilename,
                 const string& NegYFilename, const string& PosZFilename,
                 const string& NegZFilename);

  /**
   * Destructor de la clase textura
   */
  virtual ~CubemapTexture();

  /**
   * Constructor de parametros de la textura.
   * @param TextureTarget Tipo de textura que será objetivo para este objeto
   * @param FileName Ruta del archivo donde se encuentran las texturas
   */
  bool Load();

  /**
   * Este método hace el enalce del objeto de textura con su unidad de textura.
   * De acuerdo al diseño de la arquitectura puede haber diferentes objetos de textura
   * para un módelo, si se desea enlazar una unidad de textura a un objeto de textura digamos A
   * lo pirmero que se hace es activar la unidad de textura 0.
   * @param TextureUnit
   */
  void Bind(GLenum TextureUnit);

 private:
  string fileNames[6];
  GLuint textureObj;
};

#endif /* CUBEMAPTEXTURE_H_ */
