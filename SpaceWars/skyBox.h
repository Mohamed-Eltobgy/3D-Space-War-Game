#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include<string>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"
#include"Texture.h"

class SkyBox
{
public:
	GLuint VAO_ID, VBO_ID, EBO_ID, cubemapTexture;
	Shader skyboxShader;

	// Initializes the skyBox
	SkyBox(const std::vector<std::string>& faces, const std::string& vertexPath, const std::string& fragmentPath);

	~SkyBox() {
		glDeleteVertexArrays(1, &VAO_ID);
		glDeleteBuffers(1, &VBO_ID);
		glDeleteBuffers(1, &EBO_ID);
		skyboxShader.Delete();
	}

	// Draws the skyBox
	void draw(Camera& camera, unsigned int width, unsigned int height);
};
#endif