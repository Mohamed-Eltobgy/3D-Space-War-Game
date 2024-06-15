#ifndef Collectable_CLASS_H
#define Collectable_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "assimpModel.h"
#include "shaderClass.h"

class Collectable
{
public:
	// Stores the main vectors of the collectable
	glm::vec3 position;
	glm::vec3 rotation = glm::vec3(0.0f, -80.0f, 0.0f);
	glm::vec3 scale = glm::vec3(3.0f);

	// Stores the width and height of the window
	int width;
	int height;

	//Extrinsic Model
	AssimpModel* Model;

	// collectable constructor to set up initial values
	Collectable(string& modelPath,string moedlName, int width, int height, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	// Comparison operator for std::set
	bool operator<(const Collectable& other) const {
		// Compare based on position.x
		return position.x < other.position.x;
	}

	//Draw thes collectable
	void draw(Shader shaderProgram, Camera camera);
};
#endif