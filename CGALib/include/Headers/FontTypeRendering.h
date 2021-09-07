#ifndef FONTTYPERENDERING_H_
#define FONTTYPERENDERING_H_

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

#include <GL/glew.h>

#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace FontTypeRendering {

/**
 * Clase que se encarga de renderizar los texeles, esta clase utiliza la librería
 * FreeType para cargar los TFF
 */
class DLL_PUBLIC FontTypeRendering {

	/**
	 * Vertex Shader para el renderizado de las fuentes en openGL.
	 */
	static const char *VERTEX_SHADER;
	/**
	 * Fragment Shader para el renderizado de las fuentes en openGL.
	 */
	static const char *FRAGMENT_SHADER;

	/**
	 * Escala en x de la fuente
	 */
	double SCALEX;
	/**
	 * Escala en y de la fuente
	 */
	double SCALEY;

public:
	/**
	 * Constructor para el renderizado de texto en openGL.
	 * @param ScreenWidth Ancho de ventana.
	 * @param ScreenHeight Largo de ventana.
	 */
	FontTypeRendering(double ScreenWidth, double ScreenHeight);
	/**
	 * Inicialización del renderizado, este método carga el shader, los compila,
	 * los enlaza al programa para los shaders, crea los buffers, carga las TTF
	 * Y genera la textura para el renderizado.
	 */
	void Initialize();
	/**
	 * Método que renderiza un texto en el espacio de la pantalla.
	 * @param str Cadena a renderizar.
	 * @param x Coordenada en X.
	 * @param y Coordenada en Y.
	 */
	void render(const std::string &str, float x, float y);
	/**
	 * Destructor de clase.
	 */
	virtual ~FontTypeRendering();
private:
	void render_text(const std::string &str, FT_Face face, float x, float y,
			float sx, float sy);
private:
	GLuint texture, sampler;
	GLuint vbo, vao;
	GLuint vs, fs, program;
	GLuint texUniform, colorUniform;
	FT_Library ft_lib { nullptr };
	FT_Face face { nullptr };
};

} /* namespace FontTypeRendering */

#endif /* FONTTYPERENDERING_H_ */
