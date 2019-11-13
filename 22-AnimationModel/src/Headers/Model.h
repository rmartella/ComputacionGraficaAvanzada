/*
 * Model.h
 *
 *  Created on: 13/09/2016
 *      Author: rey
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "Mesh.h"
#include "Bones.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model : public AbstractModel{
public:
	Model();
	virtual ~Model();
	void loadModel(const std::string & path);
	void render(glm::mat4 parentTrans = glm::mat4(1.0f));
	virtual bool rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection);
	std::vector<Mesh*> getMeshes() {
		return meshes;
	}
	int getAnimationIndex(){
		return animationIndex;
	}
	void setAnimationIndex(int animationIndex){
		this->animationIndex = animationIndex;
	}
private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh * processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture*> loadMaterialTextures(aiMaterial* mat,
			aiTextureType type, std::string typeName);

private:
	std::vector<Mesh*> meshes;
	std::vector<Texture*> textureLoaded;
	std::string directory;
	//std::vector<Bones> vecBones;
	glm::mat4 m_GlobalInverseTransform;
	const aiScene* scene;
	Assimp::Importer importer;
	int animationIndex;
	bool gammaCorrection;
};

#endif /* MODEL_H_ */
