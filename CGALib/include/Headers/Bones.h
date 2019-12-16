/*
 * Bone.h
 *
 *  Created on: 02/12/2016
 *      Author: rey
 */

#ifndef BONES_H_
#define BONES_H_

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
#include <vector>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// GL Includes
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

class DLL_PUBLIC Bones {
public:
	Bones(GLuint VAO, unsigned int numVertex);
	virtual ~Bones();
	void loadBones(unsigned int meshIndex, const aiMesh* pMesh);
	void bonesTransform(float timeInSeconds, std::vector<glm::mat4>& transforms,
			const aiScene* scene);
	int getNumBones(){
		return m_NumBones;
	}
	int getAnimationIndex(){
		return animationIndex;
	}
	void setAnimationIndex(int animationIndex){
		this->animationIndex = animationIndex;
	}

private:
	void readNodeHeirarchy(float animationTime, const aiScene* scene,
			const aiNode* pNode, const glm::mat4 & parentTransform);
	const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation,
			const std::string nodeName);

	unsigned int findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int findScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void calcInterpolatedPosition(aiVector3D& Out, float AnimationTime,
			const aiNodeAnim* pNodeAnim);
	void calcInterpolatedRotation(aiQuaternion& Out, float AnimationTime,
			const aiNodeAnim* pNodeAnim);
	void calcInterpolatedScaling(aiVector3D& Out, float AnimationTime,
			const aiNodeAnim* pNodeAnim);

private:

	struct VertexBoneData {
		unsigned int IDs[4];
		float Weights[4];
		void AddBoneData(unsigned int boneID, float weight) {
			for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
				if (Weights[i] == 0.0) {
					IDs[i] = boneID;
					Weights[i] = weight;
					return;
				}
			}
		}
	};

	struct BoneInfo {
		glm::mat4 boneOffset;
		glm::mat4 finalTransformation;
		BoneInfo() {
			boneOffset = glm::mat4(0.0);
			finalTransformation = glm::mat4(0.0);
		}
	};

	std::map<std::string, unsigned int> m_BoneMapping;
	std::vector<BoneInfo> m_BoneInfo;
	std::vector<VertexBoneData> bones;
	int m_NumBones;
	int animationIndex;
	GLuint VAO, VBO;
};

#endif /* BONES_H_ */
