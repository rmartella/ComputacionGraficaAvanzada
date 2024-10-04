#ifndef TERRAINTESSELLATION_H_
#define TERRAINTESSELLATION_H_

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

#include "AbstractModel.h"
#include "Texture.h"
#include "Terrain.h"

class DLL_PUBLIC TerrainTessellation : public Terrain{
public:
	TerrainTessellation(float yScale, float yShift, std::string heightMap, int rez, int num_patches);
  void render(glm::mat4 parentTrans = glm::mat4(1.0f));
  void init();
	virtual ~TerrainTessellation();
private:
	std::string heightMap;
  GLuint textureHeightMapID;
	int imageWidth, imageHeight, rez, num_patches;
};

#endif /* SRC_TERRAINTESSELLATION_H_ */
