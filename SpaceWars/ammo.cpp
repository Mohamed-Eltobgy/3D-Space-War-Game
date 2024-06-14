#include "ammo.h"
#include "flyWeightModelFactory.h"

Ammo::Ammo(string& modelPath, int width, int height, glm::vec3 position,glm::vec3 orientation, glm::vec3 scale)
	:width(width), height(height), position(position), orientation(orientation), scale(scale)
{
	FlyWeightModelFactory* flyWeightModelFactory = FlyWeightModelFactory::getInstance();
	Model = flyWeightModelFactory->getModel("Ammo",modelPath);
}

void Ammo::update(GLFWwindow* window) {
	position += speed * orientation;
	/*std::cout << position.x << " " << position.y << " " << position.z << std::endl;*/
}

void Ammo::draw(Shader shaderProgram, Camera camera) {
	Model->Draw(shaderProgram, camera, position, glm::quat(glm::radians(rotation)), scale);
}
