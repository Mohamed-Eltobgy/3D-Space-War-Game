#ifndef AMMO_CLASS_H
#define AMMO_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "assimpModel.h"
#include "shaderClass.h"

class Ammo
{
public:
	// Stores the main vectors of the Ammo
	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, -90.0f, 0.0f);
	glm::vec3 scale;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the Ammo
	float speed = 3.0f;

	//Extrinsic Model
	AssimpModel* Model;

	// Ammo constructor to set up initial values
	Ammo(string& modelPath, int width, int height, glm::vec3 position, glm::vec3 orientation, glm::vec3 scale);

	// Comparison operator for std::set
	bool operator<(const Ammo& other) const {
		// Compare based on position.x
		return position.x < other.position.x;
	}

	// Handles inputs
	void update(GLFWwindow* window);

	//Draw thes Ammo
	void draw(Shader shaderProgram, Camera camera);
};
#endif