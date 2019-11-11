/*
 * Bone.h
 *
 *  Created on: 02/12/2016
 *      Author: rey
 */

#ifndef BONES_H_
#define BONES_H_

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

class Bones {
public:
	Bones(GLuint VAO, unsigned int numVertex);
	virtual ~Bones();
	void loadBones(uint meshIndex, const aiMesh* pMesh);
	void bonesTransform(float timeInSeconds, std::vector<glm::mat4>& transforms,
			const aiScene* scene);
	int getNumBones(){
		return m_NumBones;
	}

private:
	void readNodeHeirarchy(float animationTime, const aiScene* scene,
			const aiNode* pNode, const glm::mat4 & parentTransform);
	const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation,
			const std::string nodeName);

	uint findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint findScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void calcInterpolatedPosition(aiVector3D& Out, float AnimationTime,
			const aiNodeAnim* pNodeAnim);
	void calcInterpolatedRotation(aiQuaternion& Out, float AnimationTime,
			const aiNodeAnim* pNodeAnim);
	void calcInterpolatedScaling(aiVector3D& Out, float AnimationTime,
			const aiNodeAnim* pNodeAnim);

private:

	struct VertexBoneData {
		uint IDs[4];
		float Weights[4];
		void AddBoneData(uint boneID, float weight) {
			for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
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

	std::map<std::string, uint> m_BoneMapping;
	std::vector<BoneInfo> m_BoneInfo;
	std::vector<VertexBoneData> bones;
	int m_NumBones;
	GLuint VAO, VBO;
};

#endif /* BONES_H_ */
