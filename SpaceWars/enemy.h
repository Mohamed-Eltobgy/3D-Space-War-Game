#ifndef ENEMY_CLASS_H
#define ENEMY_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "assimpModel.h"
#include "shaderClass.h"
#include "ammoController.h"
#include <string>
#include <functional>
#include <unordered_set>

class Enemy
{
public:
    // Stores the main vectors of the spaceShip
    glm::vec3 position;
    glm::vec3 orientation = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 spaceShipRotation = glm::vec3(0.0f, -90.0f, 0.0f);
    glm::vec3 scale = glm::vec3(3.0f);
    glm::vec3 speed = glm::vec3(1.0f);

    // Stores the width and height of the window
    int width;
    int height;

	//Control ammo
	AmmoController* ammoController;
	double ammoInterval = 4.0;
	double lastTime = glfwGetTime();
    // Control ammo
    AmmoController* ammoController;

    // Extrinsic Model
    AssimpModel* Model;

	// SpaceShip constructor to set up initial values
	Enemy(string& modelPath, int width, int height, glm::vec3 position, glm::vec3 scale, glm::vec3 speed);

	// Comparison operator for std::set
	bool operator<(const Enemy& other) const {
		// Compare based on position.x
		return position.x < other.position.x;
	}

    // Handles inputs
    void update(GLFWwindow* window, Camera& camera, glm::vec3 playerPos);

    // Draw the space ship
    void draw(Shader shaderProgram, Camera camera);

};
#endif