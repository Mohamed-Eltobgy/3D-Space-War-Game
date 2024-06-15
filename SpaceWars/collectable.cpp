#include "collectable.h"
#include "flyWeightModelFactory.h"

Collectable::Collectable(string& modelPath,string moedlName, int width, int height, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	:width(width), height(height), position(position), rotation(rotation), scale(scale)
{
	FlyWeightModelFactory* flyWeightModelFactory = FlyWeightModelFactory::getInstance();
	Model = flyWeightModelFactory->getModel(moedlName, modelPath);
}

void Collectable::draw(Shader shaderProgram, Camera camera) {
	Model->Draw(shaderProgram, camera, position, glm::quat(glm::radians(rotation)), scale);
}
