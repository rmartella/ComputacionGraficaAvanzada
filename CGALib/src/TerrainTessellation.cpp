#include "Headers/TerrainTessellation.h"

TerrainTessellation::TerrainTessellation(float yScale, float yShift, std::string heightMap, 
										 int rez, int num_patches) : Terrain(yScale, yShift, heightMap) {
	this->heightMap = heightMap;
	this->rez = rez;
	this->num_patches = num_patches;

	Texture textureHeightMap = Texture(heightMap);
	textureHeightMap.loadImage(true);
	unsigned char *data = textureHeightMap.getData();

	this->imageWidth = textureHeightMap.getWidth();
	this->imageHeight = textureHeightMap.getHeight();

	int count = rez * rez;

	for (unsigned i = 0; i < rez; i++)
	{
		for (unsigned j = 0; j < rez; j++)
		{
			vertexArray.push_back(
				Vertex(
					glm::vec3(-this->imageWidth / 2.0f + this->imageWidth * (float)j / ((float)rez), 0.0f,
							  -this->imageHeight / 2.0f + this->imageHeight * (float)i / ((float)rez)),
					glm::vec2((float)j / ((float)rez),
							  (float)i / ((float)rez)),
					glm::vec3(0, 1, 0)));
			vertexArray.push_back(
				Vertex(
					glm::vec3(-this->imageWidth / 2.0f + this->imageWidth * (float)(j + 1) / ((float)rez), 0.0f,
							  -this->imageHeight / 2.0f + this->imageHeight * (float)i / ((float)rez)),
					glm::vec2((float)(j + 1) / ((float)rez),
							  (float)i / ((float)rez)),
					glm::vec3(0, 1, 0)));
			vertexArray.push_back(
				Vertex(
					glm::vec3(-this->imageWidth / 2.0f + this->imageWidth * (float)j / ((float)rez), 0.0f,
							  -this->imageHeight / 2.0f + this->imageHeight * (float)(i + 1) / ((float)rez)),
					glm::vec2((float)j / ((float)rez),
							  (float)(i + 1) / ((float)rez)),
					glm::vec3(0, 1, 0)));
			vertexArray.push_back(
				Vertex(
					glm::vec3(-this->imageWidth / 2.0f + this->imageWidth * (float)(j + 1) / ((float)rez), 0.0f,
							  -this->imageHeight / 2.0f + this->imageHeight * (float)(i + 1) / ((float)rez)),
					glm::vec2((float)(j + 1) / ((float)rez),
							  (float)(i + 1) / ((float)rez)),
					glm::vec3(0, 1, 0)));
		}
	}

	glGenTextures(1, &this->textureHeightMapID);					  // Creando el id de la textura del
	glBindTexture(GL_TEXTURE_2D, this->textureHeightMapID);			  // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	  // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	  // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if (textureHeightMap.getData())
	{
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureHeightMap.getChannels() == 3 ? GL_RGB : GL_RGBA, textureHeightMap.getWidth(), textureHeightMap.getHeight(), 0,
					 textureHeightMap.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureHeightMap.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Fallo la carga de textura" << std::endl;

	textureHeightMap.freeImage();
}

void TerrainTessellation::render(glm::mat4 parentTrans)
{
	shader_ptr->turnOn();
	glBindVertexArray(VAO);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), this->scale);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), this->position);
	glm::quat oX = glm::angleAxis<float>(glm::radians(orientation.x), glm::vec3(1.0, 0.0, 0.0));
	glm::quat oY = glm::angleAxis<float>(glm::radians(orientation.y), glm::vec3(0.0, 1.0, 0.0));
	glm::quat oZ = glm::angleAxis<float>(glm::radians(orientation.z), glm::vec3(0.0, 0.0, 1.0));
	glm::quat ori = oZ * oY * oX;
	glm::mat4 modelMatrix = parentTrans * translate * glm::mat4_cast(ori) * scale;
	glActiveTexture(GL_TEXTURE20);
	glBindTexture(GL_TEXTURE_2D, textureHeightMapID);
	this->shader_ptr->setInt("heightMap", 20);
	this->shader_ptr->setMatrix4("model", 1, GL_FALSE, glm::value_ptr(modelMatrix));
	this->shader_ptr->setFloat("yScale", Terrain::yScale);
	this->shader_ptr->setFloat("yShift", Terrain::yShift);
	//this->shader_ptr->setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(this->rez, this->rez)));
	glDisable(GL_CULL_FACE);
	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	shader_ptr->setInt("texture1", 1);*/
	glDrawArrays(GL_PATCHES, 0, this->num_patches * this->rez * this->rez);
	glEnable(GL_CULL_FACE);
	glBindVertexArray(0);
	shader_ptr->turnOff();
}

void TerrainTessellation::init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexArray.size() * sizeof(vertexArray[0]), vertexArray.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertexArray[0]), (GLvoid *)(sizeof(vertexArray[0].m_pos)));
	glEnableVertexAttribArray(2);
	glPatchParameteri(GL_PATCH_VERTICES, this->num_patches);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

TerrainTessellation::~TerrainTessellation()
{
	this->destroy();
	glDeleteTextures(1, &textureHeightMapID);
}
