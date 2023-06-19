#include "Headers/Model.h"
#include "Headers/Shader.h"
#include <GL/glew.h>
#include <string>

class GameObject
{
public:
	//Public Attributes:
	glm::mat4 ModelMatrix;
	glm::mat4 Transform;
	glm::mat4 ColliderTransform;
	std::string nametag;
	int animationIndex = 0;

	//Public
	GameObject(std::string tag,std::string pathToModel, Shader* shader);
	GameObject();
	~GameObject();

	void Draw();
	void Draw(Model* modelReference);
	//void DrawCollider();
	void Translate(glm::vec3 translation);
	void Rotate(float angle, glm::vec3 axis);
	void SetScale(glm::vec3 scale);
	glm::vec3 GetScale();
	void SetShader(Shader* shader);

	
	
	void UpdateColliderOBB(float angle, glm::vec3 rotation, glm::vec3 scale, glm::vec3 factor);
	AbstractModel::OBB GetOBB();

private:

	//Private Attributes:
	Model model;
	Model colliderModel;
	//Shader* shader;
	AbstractModel::SBB sphereAbstract;
	AbstractModel::OBB boxCollider;

	//Private Methods:
	void LoadModel(std::string pathToModel);


};

GameObject::GameObject(std::string tag,std::string pathToModel, Shader* shader)
{
	//Set Tag
	this->nametag = tag;
	
	//Load Model
	this->model.loadModel(pathToModel);
	//Set Shader
	this->model.setShader(shader);

	//Init Model Matrix
	this->Transform = glm::mat4(1.0f);
	this->ModelMatrix = glm::mat4(1.0f);

	//Put Collider
	
}

GameObject::GameObject() {
	this->ModelMatrix = glm::mat4(1.0f);
	this->Transform = glm::mat4(1.0f);
	this->ColliderTransform = glm::mat4(1.0f);
	
}


GameObject::~GameObject()
{
	this->model.destroy();
	this->colliderModel.destroy();
	
}

void GameObject::LoadModel(std::string pathToModel) {
	
}
void GameObject::Draw()
{
	this->model.setAnimationIndex(animationIndex);
	this->model.render(this->Transform);
}

void GameObject::Draw(Model* modelReference)
{
	this->model = *modelReference;
	Draw();
}

void GameObject::Translate(glm::vec3 translation)
{
	this->Transform = glm::translate(this->Transform, translation);
}

void GameObject::Rotate(float angle, glm::vec3 axis)
{
	this->Transform = glm::rotate(this->Transform, angle, axis);
}

void GameObject::SetScale(glm::vec3 scale)
{
	this->Transform = glm::scale(this->Transform, scale);
}

glm::vec3 GameObject::GetScale()
{
	return glm::vec3(this->Transform[0][0], this->Transform[1][1], this->Transform[2][2]);
}

void GameObject::UpdateColliderOBB(float angle, glm::vec3 rotation ,glm::vec3 scale, glm::vec3 factor) 
{
	this->ColliderTransform = glm::mat4(ModelMatrix);
	this->ColliderTransform = glm::rotate(ColliderTransform, glm::radians(angle), rotation);
	boxCollider.u = glm::quat_cast(this->ColliderTransform);
	this->ColliderTransform = glm::scale(ColliderTransform, scale);
	this->ColliderTransform = glm::translate(ColliderTransform, this->model.getObb().c);
	boxCollider.c = glm::vec3(ColliderTransform[3]);
	boxCollider.e = this->model.getObb().e * scale * factor;

}

void GameObject::SetShader(Shader* shader) {
	this->model.setShader(shader);
}

AbstractModel::OBB GameObject::GetOBB() {
	return this->boxCollider;
}
