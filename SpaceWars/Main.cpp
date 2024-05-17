#include <filesystem>
#include "Model.h"
#include "assimpModel.h"

// Window dimensions
const unsigned int width = 1400;
const unsigned int height = 900;

// Rotation speed of the planets and sun
float rotationSpeed = 0.2f;

// Skybox vertices and indices
float skyboxVertices[] =
{
	-1.0f, -1.0f,  1.0f,	//        7--------6
	1.0f, -1.0f,  1.0f,		//       /|       /|
	1.0f, -1.0f, -1.0f,		//      4--------5 |
	-1.0f, -1.0f, -1.0f,	//      | |      | |
	-1.0f,  1.0f,  1.0f,	//      | 3------|-2
	1.0f,  1.0f,  1.0f,		//      |/       |/
	1.0f,  1.0f, -1.0f,		//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};

// Planets' initial positions
glm::vec3 sunPos = glm::vec3(0.0f);
glm::vec3 mercuryPos = glm::vec3(0.0f, 0.0f, 150.0f);
glm::vec3 venusPos = glm::vec3(0.0f, 0.0f, 300.0f);
glm::vec3 earthPos = glm::vec3(0.0f, 0.0f, 450.0f);
glm::vec3 marsPos = glm::vec3(0.0f, 0.0f, 600.0f);
glm::vec3 jupiterPos = glm::vec3(0.0f, 0.0f, 750.0f);
glm::vec3 saturnPos = glm::vec3(0.0f, 0.0f, 900.0f);
glm::vec3 uranusPos = glm::vec3(0.0f, 0.0f, 1050.0f);
glm::vec3 neptunePos = glm::vec3(0.0f, 0.0f, 1200.0f);

// Planets' initial rotations
glm::quat sunRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat mercuryRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat venusRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat earthRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat marsRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat jupiterRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat saturnRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat uranusRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
glm::quat neptuneRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);


//spaceShip pos
glm::vec3 spaceShipPos = glm::vec3(0.0f, 0.0f, 100.0f);
glm::quat spaceShipRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

// Function to generate random float between -1.0 and 1.0 for asteroid positions
float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

// Main function
int main()
{
	// Initialize GLFW
	glfwInit();

	// Specify what version of OpenGL we are using (3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Profile initialization for accessing OpenGL functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(width, height, "Solar System", NULL, NULL);

	// Position the window in the center of the screen
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);
	
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	// Load GLAD for OpenGL configuration
	gladLoadGL();

	// Specify the viewport of OpenGL in the window
	glViewport(0, 0, width, height);

	// Generate shader objects
	Shader shaderProgram("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");

	// Handle lighting
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// The position of the light is at the sun
	glm::vec3 lightPos = sunPos;

	// Activate the shader program
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

	// Enables the depth buffer for 3D rendering
	glEnable(GL_DEPTH_TEST);

	// Creates camera object with initial position: (-600, 0, 0)
	Camera camera(width, height, glm::vec3(-600.0f, 0.0f, 0.0f));

	// Adjust the camera to look at the sun
	camera.Orientation = glm::vec3(1.0f, 0.0f, 0.0f);
	
	// Get the parent directory of the current directory
	std::string parentDir = (std::filesystem::current_path().std::filesystem::path::parent_path()).string();
	
	// Paths to all the models
	std::string sunPath = "/Resources/models/sun/scene.gltf";
	std::string mercuryPath = "/Resources/models/mercury/scene.gltf";
	std::string venusPath = "/Resources/models/venus/scene.gltf";
	std::string earthPath = "/Resources/models/earth/scene.gltf";
	std::string marsPath = "/Resources/models/mars/scene.gltf";
	std::string jupiterPath = "/Resources/models/jupiter/scene.gltf";
	std::string saturnPath = "/Resources/models/saturn/scene.gltf";
	std::string asteroidPath = "/Resources/models/asteroid/scene.gltf";
	std::string uranusPath = "/Resources/models/uranus/scene.gltf";
	std::string neptunePath = "/Resources/models/neptune/scene.gltf";

	//std::string spaceShipPath = "/Resources/models/spaceship/spaceship.gltf";
	std::string spaceShipPath = "/Resources/models/spaceship/neghvar.obj";
	//std::string spaceShipPath = "/Resources/models/spaceship1/voyager.obj";
	//std::string spaceShipPath = "/Resources/models/backpack/backpack.obj";
	
	// Load in all models
	Model sun((parentDir + sunPath).c_str());
	Model mercury((parentDir + mercuryPath).c_str());
	Model venus((parentDir + venusPath).c_str());
	Model earth((parentDir + earthPath).c_str());
	Model mars((parentDir + marsPath).c_str());
	Model jupiter((parentDir + jupiterPath).c_str());
	Model saturn((parentDir + saturnPath).c_str());
	Model asteroid((parentDir + asteroidPath).c_str());
	Model uranus((parentDir + uranusPath).c_str());
	Model neptune((parentDir + neptunePath).c_str());

	//Model spaceShip((parentDir + spaceShipPath).c_str());
	string path = parentDir + spaceShipPath;
	AssimpModel spaceShip(path);
	  
	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Paths to all the faces of the cubemap
	std::string facesCubemap[6] =
	{
		parentDir + "/Resources/skybox/right.png",
		parentDir + "/Resources/skybox/left.png",
		parentDir + "/Resources/skybox/top.png",
		parentDir + "/Resources/skybox/bottom.png",
		parentDir + "/Resources/skybox/front.png",
		parentDir + "/Resources/skybox/back.png",
	};

	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	// The number of asteroids to be created
	const unsigned int number = 1500;

	// Radius of circle around which asteroids orbit
	float radius = 60.0f;

	// How much ateroids deviate from the radius
	float radiusDeviation = 25.0f;

	// Holds all transformations for the asteroids
	glm::vec3 translations[number];
	glm::quat rotations[number];
	glm::vec3 scales[number];

	for (unsigned int i = 0; i < number; i++)
	{
		// Generates x and y for the function x^2 + y^2 = radius^2 which is a circle
		float x = randf();
		float finalRadius = radius + randf() * radiusDeviation;
		float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x);

		// Makes the random distribution more even
		if (randf() > 0.5f)
		{
			// Generates a translation near a circle of radius "radius"
			translations[i] = glm::vec3(y * finalRadius, randf(), x * finalRadius + 900);
		}
		else
		{
			// Generates a translation near a circle of radius "radius"
			translations[i] = glm::vec3(x * finalRadius, randf(), y * finalRadius + 900);
		}

		// Generates random rotations
		rotations[i] = glm::quat(1.0f, randf(), randf(), randf());
		// Generates random scales
		scales[i] = 0.1f * glm::vec3(randf(), randf(), randf());
	}

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs
		camera.Inputs(window);

		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 2000.0f);

		// Draw the sun at the center
		sun.Draw(shaderProgram, camera, sunPos, sunRot, glm::vec3(3.0f));

		// Adjust lightings for the planets
		shaderProgram.Activate();
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		// Draw the planets around the sun
		mercury.Draw(shaderProgram, camera, mercuryPos, mercuryRot, glm::vec3(0.5f));
		venus.Draw(shaderProgram, camera, venusPos, venusRot, glm::vec3(0.7f));
		earth.Draw(shaderProgram, camera, earthPos, earthRot, glm::vec3(0.8f));
		mars.Draw(shaderProgram, camera, marsPos, marsRot, glm::vec3(0.6f));
		jupiter.Draw(shaderProgram, camera, jupiterPos, jupiterRot, glm::vec3(1.5f));
		saturn.Draw(shaderProgram, camera, saturnPos, saturnRot, glm::vec3(1.2f));
		uranus.Draw(shaderProgram, camera, uranusPos, uranusRot, glm::vec3(1.0f));
		neptune.Draw(shaderProgram, camera, neptunePos, neptuneRot, glm::vec3(1.0f));

		spaceShip.Draw(shaderProgram, camera, spaceShipPos, spaceShipRot, glm::vec3(4.0f));
		
		// Update the positions of the planets to orbit the sun with different speeds
		mercuryPos = glm::vec3(mercuryPos.x * cos(0.005f) - mercuryPos.z * sin(-0.005f), 0.0f, mercuryPos.x * sin(-0.005f) + mercuryPos.z * cos(0.005f));
		venusPos = glm::vec3(venusPos.x * cos(0.004f) - venusPos.z * sin(-0.004f), 0.0f, venusPos.x * sin(-0.004f) + venusPos.z * cos(0.004f));
		earthPos = glm::vec3(earthPos.x * cos(0.003f) - earthPos.z * sin(-0.003f), 0.0f, earthPos.x * sin(-0.003f) + earthPos.z * cos(0.003f));
		marsPos = glm::vec3(marsPos.x * cos(0.002f) - marsPos.z * sin(-0.002f), 0.0f, marsPos.x * sin(-0.002f) + marsPos.z * cos(0.002f));
		jupiterPos = glm::vec3(jupiterPos.x * cos(0.001f) - jupiterPos.z * sin(-0.001f), 0.0f, jupiterPos.x * sin(-0.001f) + jupiterPos.z * cos(0.001f));
		saturnPos = glm::vec3(saturnPos.x * cos(0.0005f) - saturnPos.z * sin(-0.0005f), 0.0f, saturnPos.x * sin(-0.0005f) + saturnPos.z * cos(0.0005f));
		uranusPos = glm::vec3(uranusPos.x * cos(0.0004f) - uranusPos.z * sin(-0.0004f), 0.0f, uranusPos.x * sin(-0.0004f) + uranusPos.z * cos(0.0004f));
		neptunePos = glm::vec3(neptunePos.x * cos(0.0003f) - neptunePos.z * sin(-0.0003f), 0.0f, neptunePos.x * sin(-0.0003f) + neptunePos.z * cos(0.0003f));

		// Update the sun's and planets' rotations
		sunRot = glm::rotate(sunRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		mercuryRot = glm::rotate(mercuryRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		venusRot = glm::rotate(venusRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		earthRot = glm::rotate(earthRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		marsRot = glm::rotate(marsRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		jupiterRot = glm::rotate(jupiterRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		saturnRot = glm::rotate(saturnRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		uranusRot = glm::rotate(uranusRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
		neptuneRot = glm::rotate(neptuneRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));

		// Draw the asteroids around saturn only
		for (unsigned int i = 0; i < number; i++)
		{
			asteroid.Draw(shaderProgram, camera, translations[i], rotations[i], scales[i]);
		}

		// Update the asteroid positions to orbit the sun with saturn
		for (unsigned int i = 0; i < number; i++)
		{
			translations[i] = glm::vec3(translations[i].x * cos(0.0005f) - translations[i].z * sin(-0.0005f), translations[i].y, translations[i].x * sin(-0.0005f) + translations[i].z * cos(0.0005f));
		}

		// Switch to the equal depth function to render the skybox as the last object since it always has to be at the back
		glDepthFunc(GL_LEQUAL);

		// Activate the skybox shader
		skyboxShader.Activate();

		// Set the view and projection matrix for the skybox
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// Remove the translation part of the view matrix
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));

		// Set the projection matrix for the skybox
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		// Set the view and projection matrix in the shader
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the skybox
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);

		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects
	shaderProgram.Delete();
	skyboxShader.Delete();

	// Delete window
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();
	return 0;
}