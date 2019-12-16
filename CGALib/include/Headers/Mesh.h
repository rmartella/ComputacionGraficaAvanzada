/*
 * Mesh.h
 *
 *  Created on: 13/09/2016
 *      Author: rey
 */

#ifndef HEADERS_MESH_H_
#define HEADERS_MESH_H_

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

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "AbstractModel.h"
#include "Texture.h"
#include "Shader.h"
#include "Bones.h"

class DLL_PUBLIC Mesh : public AbstractModel{
public:
	std::vector<Texture*> textures;
	Bones* bones;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
			std::vector<Texture*> textures);
	~Mesh();
	void render(glm::mat4 parentTrans = glm::mat4(1.0f));
	//void destroy();
	virtual bool rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection);

};

#endif /* HEADERS_MESH_H_ */
