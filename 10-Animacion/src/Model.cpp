/*
 * Model.cpp
 *
 *  Created on: 13/09/2016
 *      Author: rey
 */

#include "Headers/Model.h"

Model::Model() {
}

Model::~Model() {
	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].destroy();
	for (int i = 0; i < this->textureLoaded.size(); i++)
		delete this->textureLoaded[i];
}

void Model::render(glm::mat4 parentTrans) {
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].setShader(this->getShader());
		this->meshes[i].setPosition(this->getPosition());
		this->meshes[i].setScale(this->getScale());
		this->meshes[i].setOrientation(this->getOrientation());
		this->meshes[i].render(parentTrans);
	}
}

void Model::loadModel(const std::string & path) {
	// Lee el archivo via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(),
			aiProcess_Triangulate | aiProcess_FlipUVs
					| aiProcess_CalcTangentSpace);
	// Revisa errores
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE
			|| !scene->mRootNode) // if is Not Zero
			{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString()
				<< std::endl;
		return;
	}
	// Recupera el path del directorio del archivo.
	this->directory = path.substr(0, path.find_last_of('/'));

	// Se procesa el nodo raiz recursivamente.
	this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// Procesa cada maya del nodo actual
	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	for (GLuint i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<AbstractModel::Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture*> textures;

	// Recorre los vertices de cada maya
	for (GLuint i = 0; i < mesh->mNumVertices; i++) {
		AbstractModel::Vertex vertex;
		glm::vec3 vector;
		// Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.m_pos = vector;
		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.m_normal = vector;
		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Esto se ejecuta si la maya contiene texturas.
		{
			glm::vec2 vec;
			// Un vertice puede contener hasta 8 diferentes coordenadas de textura, unicamente se considera 
			// que los modelos tiene una coordenada de textura por vertice, y corresponde a la primera en el arreglo.
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.m_tex = vec;
		} else
			vertex.m_tex = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// Se recorre cada cara de la maya (una cara es un triangulo en la maya) y recupera su correspondiente indice del vertice.
	for (GLuint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		// Recupera todos los indices de la cara y los almacena en el vector de indices
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// Process materials
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		std::vector<Texture*> diffuseMaps = this->loadMaterialTextures(material,
				aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		std::vector<Texture*> specularMaps = this->loadMaterialTextures(material,
				aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(),
				specularMaps.end());
		// 3. Normal maps
		std::vector<Texture*> normalMaps = this->loadMaterialTextures(material,
				aiTextureType_NORMALS, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. Height maps
		std::vector<Texture*> heightMaps = this->loadMaterialTextures(material,
				aiTextureType_HEIGHT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	// Regresa la maya de un objeto creado de los datos extraidos.
	return Mesh(vertices, indices, textures);
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial* mat,
		aiTextureType type, std::string typeName) {
	std::vector<Texture*> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// Verifica si la textura fue cargada antes y si es as�, continua con la siguiente iteracion: en caso controaio se salta la carga
		GLboolean skip = false;
		for (GLuint j = 0; j < textureLoaded.size(); j++) {
			if (textureLoaded[j]->getFileName() == str.C_Str()) {
				textures.push_back(textureLoaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			std::string filename = std::string(str.C_Str());
			filename = this->directory + '/' + filename;
			Texture * texture = new Texture(GL_TEXTURE_2D, filename);
			texture->load();
			texture->setType(typeName);
			textures.push_back(texture);
			this->textureLoaded.push_back(texture); // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

bool Model::rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection) {
	return false;
}
