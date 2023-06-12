#include "Headers/Model.h"
#include "Headers/Shader.h"
#include <GL/glew.h>
#include <string>

class GameObject
{
public:
	//Public Attributes:
	glm::mat4 Transform;
	std::string nametag;

	//Public
	GameObject(std::string tag,std::string pathToModel);
	~GameObject();

	void Draw();
	void DrawCollider();
	void Translate(glm::vec3 translation);
	void Rotate(float angle, glm::vec3 axis);
	void Scale(glm::vec3 scale);


private:

	//Private Attributes:
	Model model;
	Model colliderModel;
	Shader shader;
	AbstractModel::SBB sphereAbstract;
	AbstractModel::OBB boxAbstract;

	//Private Methods:
	void LoadModel(std::string pathToModel);


};

GameObject::GameObject(std::string tag,std::string pathToModel)
{
	//Load Model
	LoadModel(pathToModel);
	this->nametag = tag;
	//Put Tag
	//Put Shader
	//Put Transform
	//Put Collider
	

}

GameObject::~GameObject()
{
}

void GameObject::LoadModel(std::string pathToModel) {
	this->model.loadModel(pathToModel);
}
void GameObject::Draw()
{
	//Draw Model
}
