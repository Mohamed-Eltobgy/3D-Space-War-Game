#ifndef SPACESHIP_CLASS_H
#define SPACESHIP_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "assimpModel.h"
#include "shaderClass.h"

class SpaceShip:AssimpModel
{
public:
	// Stores the main vectors of the spaceShip
	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 spaceShipRotation = glm::vec3(0.0f, -90.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(3.0f);

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 1.5f;
	float rotationSpeed = 1.0f;
	float sensitivity = 100.0f;

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// SpaceShip constructor to set up initial values
	SpaceShip(string& modelPath,int width, int height, glm::vec3 position, glm::vec3 scale);

	// Handles inputs
	void update(GLFWwindow* window, Camera &camera);

	//Draw thes space ship
	void draw(Shader shaderProgram, Camera camera);
};
#endif