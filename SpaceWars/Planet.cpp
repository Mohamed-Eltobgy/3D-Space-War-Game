#include "Planet.h"


Planet::Planet(const char* file) :Model(file), planetPos(glm::vec3(0.0f)), planetRot(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
	planetScale(glm::vec3(1.0f)), movementSpeed(0.005f), rotationSpeed(0.2f) {}

Planet::Planet(const char* file, glm::vec3 planetPosition, glm::quat planetRotation, glm::vec3 planetScale, float movSpeed, float rotSpeed)
	:Model(file), planetPos(planetPosition), planetRot(planetRotation), planetScale(planetScale), movementSpeed(movSpeed), rotationSpeed(rotSpeed)
{}

void Planet::update()
{
	std::cout << "bef " << planetPos.x << " " << planetPos.y << " " << planetPos.z << std::endl;
	planetPos = glm::vec3(planetPos.x * cos(movementSpeed) - planetPos.z * sin(-1.0f*movementSpeed),0.0f,planetPos.x * sin(-1.0f * movementSpeed) + planetPos.z * cos(movementSpeed));
	std::cout << "aft " << planetPos.x << " " << planetPos.y << " " << planetPos.z << std::endl;
	planetRot = glm::rotate(planetRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Planet::draw(Shader shaderProgram, Camera camera)
{
	Draw(shaderProgram, camera, planetPos, planetRot, planetScale);
}

