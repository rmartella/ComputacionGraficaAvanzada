/*
 * FontTypeRendering.cpp
 *
 *  Created on: 14/05/2015
 *      Author: rey
 */

#include "Headers/FontTypeRendering.h"

namespace FontTypeRendering {

// String del Vertex shader para fuentes.
// Solo se posiciona los vertices en x, y
// Obtiene las corrdenadas UV para la fuente a pintar.
const char *FontTypeRendering::VERTEX_SHADER = ""
		"#version 410 core\n"
		"in vec4 in_Position;\n"
		"out vec2 texCoords;\n"
		"void main(void) {\n"
		"    gl_Position = vec4(in_Position.xy, 0, 1);\n"
		"    texCoords = in_Position.zw;\n"
		"}\n";

// String del Fragment shader a renderizar.
const char *FontTypeRendering::FRAGMENT_SHADER = ""
		"#version 410 core\n"
		"precision highp float;\n"
		"uniform sampler2D tex;\n"
		"uniform vec4 color;\n"
		"in vec2 texCoords;\n"
		"out vec4 fragColor;\n"
		"void main(void) {\n"
		"    fragColor = vec4(1, 1, 1, texture(tex, texCoords).r) * color;\n"
		"}\n";
/**
 * Constructor para el renderizado de texto en openGL.
 * @param ScreenWidth Ancho de ventana.
 * @param ScreenHeight Largo de ventana.
 */
FontTypeRendering::FontTypeRendering(double ScreenWidth, double ScreenHeight) {
	FontTypeRendering::SCALEX = 2.0 / ScreenWidth;
	FontTypeRendering::SCALEY = 2.0 / ScreenHeight;
}

/**
 * Destructor de clase.
 */
FontTypeRendering::~FontTypeRendering() {
}
/**
 * Inicializaci�n del renderizado, este m�todo carga el shader, los compila,
 * los enlaza al programa para los shaders, crea los buffers, carga las TTF
 * Y genera la textura para el renderizado.
 */
void FontTypeRendering::Initialize() {
	// Se limpian las caras, buffers para textra y shaders.
	auto cleanup = [&]() {
		FT_Done_Face (face);
		FT_Done_FreeType (ft_lib);
		glDeleteTextures(1, &texture);
		glDeleteSamplers(1, &sampler);
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
		glDeleteShader (vs);
		glDeleteShader (fs);
		glDeleteProgram (program);
	};
	// Inicializa y carga la face de freetype
	if (FT_Init_FreeType(&ft_lib) != 0) {
		std::cerr << "Couldn't initialize FreeType library\n";
		cleanup();
		return exit(-1);
	}
	// Se le indica a dicha cara el archivo ttf que se utilizar�.
	if (FT_New_Face(ft_lib, "../Fonts/arial.ttf", 0, &face) != 0) {
		std::cerr << "Unable to load arial.ttf\n";
		cleanup();
		return exit(-1);
	}
	// Inicializaci�n de los VBOs para la textura.
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glGenTextures(1, &texture);
	glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Inicializaci�n, compilaci�n y enlace de los shaders.
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &FontTypeRendering::VERTEX_SHADER, 0);
	glCompileShader (vs);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &FontTypeRendering::FRAGMENT_SHADER, 0);
	glCompileShader (fs);
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram (program);
	// Se colocan para que sea translucido el color de la textura, solo pinte las letras.
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(program);
	// Se enlace la posicion de los vertices de la textura.
	glBindAttribLocation(program, 0, "in_Position");
	// Obtiene los Ids de los shaders para textura y color.
	texUniform = glGetUniformLocation(program, "tex");
	colorUniform = glGetUniformLocation(program, "color");
}

/**
 * M�todo que renderiza un texto en el espacio de la pantalla.
 * @param str Cadena a renderizar.
 * @param x Coordenada en X.
 * @param y Coordenada en Y.
 */
void FontTypeRendering::render(const std::string &str, float x, float y) {
	// Se activa la unidad de textura.
	glActiveTexture (GL_TEXTURE0);
	// Se enalza hacia el tipo de textura.
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, sampler);
	glBindVertexArray (vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glUseProgram (program);
	// Se envia el color de la fuente.
	glUniform4f(colorUniform, 0.5, 1.0, 1.0, 1.0);
	// Se envia la textura a utilizar.
	glUniform1i(texUniform, 0);
	// Se coloca el tama�o en Pixeles de la fuente.
	FT_Set_Pixel_Sizes(face, 0, 12);
	// Renderiza la fuente.
	glEnable(GL_BLEND);
	render_text(str, face, x, y, SCALEX, SCALEY);
	// Se desabilita el sample de textura.
	glBindSampler(0, 0);
	glDisable(GL_BLEND);
}

/**
 * M�todo que se encarga de renderizar la textura.
 * @param str Texto a dibujar.
 * @param face Cara TTF
 * @param x Coordenada x espacio de la pantalla.
 * @param y Coordenada y espacio de la pantalla.
 * @param sx Escala en X de la fuente
 * @param sy Escala en y de la fuente
 */
void FontTypeRendering::render_text(const std::string &str, FT_Face face,
		float x, float y, float sx, float sy) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	const FT_GlyphSlot glyph = face->glyph;

	bool IsCullEnabled = glIsEnabled(GL_CULL_FACE);

	glDisable (GL_CULL_FACE);

	for (auto c : str) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
			continue;

		// Se envia los datos de la textura.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, glyph->bitmap.width,
				glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
				glyph->bitmap.buffer);

		// Se generan las posiciones en el espaci� de la pantall.
		const float vx = x + glyph->bitmap_left * sx;
		const float vy = y + glyph->bitmap_top * sy;
		// Se generan las escalas del mapeo.
		const float w = glyph->bitmap.width * sx;
		const float h = glyph->bitmap.rows * sy;

		// Generan las posiciones y coordenadas de mapeo.
		struct {
			float x, y, s, t;
		} data[6] = { { vx, vy, 0, 0 }, { vx, vy - h, 0, 1 },
				{ vx + w, vy, 1, 0 }, { vx + w, vy, 1, 0 },
				{ vx, vy - h, 0, 1 }, { vx + w, vy - h, 1, 1 } };

		// Se genera el buffer de los vertices.
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), data,
				GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (glyph->advance.x >> 6) * sx;
		y += (glyph->advance.y >> 6) * sy;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if (IsCullEnabled) {
		glEnable(GL_CULL_FACE);
	}
}

} /* namespace FontTypeRendering */
