#ifndef PLANET_CLASS_H
#define PLANET_CLASS_H

#include "Model.h"

class Planet:Model
{
public:
	glm::vec3 planetPos;
	glm::quat planetRot;
	glm::vec3 planetScale;
	float movementSpeed;
	float rotationSpeed;
	
	Planet(const char* file);
	Planet(const char* file,glm::vec3 planetPos, glm::quat planetRot, glm::vec3 planetScale, float movementSpeed, float rotationSpeed);
		
	void update();
	void draw(Shader shaderProgram, Camera camera);
};
#endif