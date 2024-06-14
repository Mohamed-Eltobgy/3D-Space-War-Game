#include "spaceShip.h"
#include "ammo.h"
#include "ammoController.h"
#include "enemy.h"
#include "flyWeightModelFactory.h"

Enemy::Enemy(string& modelPath, int width, int height, glm::vec3 position, glm::vec3 scale, glm::vec3 speed)
	:width(width), height(height), position(position), scale(scale),speed(speed)
{
	ammoController = AmmoController::getInstance();

	FlyWeightModelFactory* flyWeightModelFactory = FlyWeightModelFactory::getInstance();
	Model = flyWeightModelFactory->getModel("Enemy", modelPath);
}

void Enemy::update(GLFWwindow* window, Camera& camera,glm::vec3 playerPos) {
	
	glm::vec3 distToPLayer = playerPos - position;
    glm::vec3 newOrientation = glm::normalize(distToPLayer);
    //float reqAngle = calculateAngleBetweenVectors(orientation, newOrientation);
    //rotateVectorByAngle()
    orientation = newOrientation;

	position += speed * orientation;
}

void Enemy::draw(Shader shaderProgram, Camera camera) {
	Model->Draw(shaderProgram, camera, position, glm::quat(glm::radians(spaceShipRotation)), scale);
}


//float calculateAngleBetweenVectors(const glm::vec3& vec1, const glm::vec3& vec2) {
//    glm::vec3 normVec1 = glm::normalize(vec1);
//    glm::vec3 normVec2 = glm::normalize(vec2);
//    float dotProduct = glm::dot(normVec1, normVec2);
//    dotProduct = glm::clamp(dotProduct, -1.0f, 1.0f);
//    float angleRadians = glm::acos(dotProduct);
//    return angleRadians;
//}
//
//glm::vec3 rotateVectorByAngle(const glm::vec3& vec1, const glm::vec3& vec2, float angleRadians) {
//    // Calculate the rotation axis (cross product of vec1 and vec2)
//    glm::vec3 rotationAxis = glm::normalize(glm::cross(vec1, vec2));
//
//    // Create the rotation matrix
//    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleRadians, rotationAxis);
//
//    // Apply the rotation matrix to the original vector
//    glm::vec4 rotatedVec4 = rotationMatrix * glm::vec4(vec1, 1.0f);
//    glm::vec3 rotatedVec3 = glm::vec3(rotatedVec4);
//
//    return rotatedVec3;
//}
