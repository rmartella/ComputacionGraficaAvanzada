/*
 * CubemapTexture.cpp
 *
 *  Created on: 16/05/2015
 *      Author: rey
 */

#include <iostream>
#include "Headers/CubemapTexture.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

/**
 * Caras del skydome
 */
static const GLenum types[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

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
CubemapTexture::CubemapTexture(const string& Directory,
                               const string& PosXFilename,
                               const string& NegXFilename,
                               const string& PosYFilename,
                               const string& NegYFilename,
                               const string& PosZFilename,
                               const string& NegZFilename) {
  string::const_iterator it = Directory.end();
  it--;
  string BaseDir = (*it == '/') ? Directory : Directory + "/";

  fileNames[0] = BaseDir + PosXFilename;
  fileNames[1] = BaseDir + NegXFilename;
  fileNames[2] = BaseDir + PosYFilename;
  fileNames[3] = BaseDir + NegYFilename;
  fileNames[4] = BaseDir + PosZFilename;
  fileNames[5] = BaseDir + NegZFilename;

  textureObj = 0;
}

/**
 * Destructor de la clase textura
 */
CubemapTexture::~CubemapTexture() {
  if (textureObj != 0) {
    glDeleteTextures(1, &textureObj);
  }
}

/**
 * Constructor de parametros de la textura.
 * @param TextureTarget Tipo de textura que será objetivo para este objeto
 * @param FileName Ruta del archivo donde se encuentran las texturas
 */
bool CubemapTexture::Load() {
  glGenTextures(1, &textureObj);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);

  // Tipo de interpolación de mapeo de la textura a los trigangulos.
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Por cada cara del skydome.
  for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
    // Se obtiene del nombre del archivo a cargar en la GPU como un apuntador a un arreglo de chars
    const char* filename = fileNames[i].c_str();

    // Determina el formato de la imagen
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

    // Si la imagen no es encontrada termina el programa
    if (format == -1) {
      std::cout << "No se encontró la imagen: " << fileNames[i].c_str()
                << " - Abortando." << std::endl;
      exit(-1);
    }
    // Si el formato no es soportado por FreeImage termina el programa
    if (format == FIF_UNKNOWN) {
      std::cout
          << "No se puede determinar el formato de imagen - validarla extensión del archivo..."
          << std::endl;

      // Se obtiene el formato del archivo
      format = FreeImage_GetFIFFromFilename(filename);

      // Revisa si la librería es capaz de leer el formato
      if (!FreeImage_FIFSupportsReading(format)) {
        std::cout << "El formato de la imagen no puede ser leeído!"
                  << std::endl;
        exit(-1);
      }
    }
    // Si es valida la imagen y puede ser leeído, se carga la imagen en un bitap
    FIBITMAP* bitmap = FreeImage_Load(format, filename);
    FreeImage_FlipVertical(bitmap);

    // Obtiene el número de bits por pixel de la imagen
    int bitsPerPixel = FreeImage_GetBPP(bitmap);

    // Convierte la imagen a 32 bits (8 bits por canal).
    FIBITMAP* bitmap32;
    if (bitsPerPixel == 32) {
      /*std::cout << "Source image has " << bitsPerPixel
       << " bits per pixel. Skipping conversion." << std::endl;*/
      bitmap32 = bitmap;
    } else {
      /*std::cout << "Source image has " << bitsPerPixel
       << " bits per pixel. Converting to 32-bit colour." << std::endl;*/
      bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
    }
    // Se obtiene las dimensiones de la imagen.
    int imageWidth = FreeImage_GetWidth(bitmap32);
    int imageHeight = FreeImage_GetHeight(bitmap32);
    /*std::cout << "Image: " << m_fileName << " is size: " << imageWidth << "x"
     << imageHeight << "." << std::endl;*/

    // Se obtiene un apuntador a los datos de la textura como un arreglo de unsigned bytes.
    GLubyte* textureData = FreeImage_GetBits(bitmap32);

    // Se envian los datos de la textura
    glTexImage2D(types[i],    // Tipo de textura
        0,            // Niveles del Mipmap
        GL_RGBA,          //Formato intero, RGBA
        imageWidth,       // Ancho de la textura
        imageHeight,      // Ancho de la textura
        0,                // Borde de la textura
        GL_BGRA,          // Formato que se maneja la textura
        GL_UNSIGNED_BYTE,  // Tipo de datos de la textura
        textureData);     // Imagen que se usa para esta textura

    // Unload the 32-bit colour bitmap
    // Se desecha los datos de la textura
    FreeImage_Unload(bitmap32);

    // Si se convirtio la imagen a 32 bits por pixel eliminamos el bitmap
    if (bitsPerPixel != 32) {
      FreeImage_Unload(bitmap);
    }
  }

  // Se genera el BitMap.
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return true;
}

/**
 * Este método hace el enalce del objeto de textura con su unidad de textura.
 * De acuerdo al diseño de la arquitectura puede haber diferentes objetos de textura
 * para un módelo, si se desea enlazar una unidad de textura a un objeto de textura digamos A
 * lo pirmero que se hace es activar la unidad de textura 0.
 * @param TextureUnit
 */
void CubemapTexture::Bind(GLenum TextureUnit) {
  glActiveTexture(TextureUnit);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);
}

