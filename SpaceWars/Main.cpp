#include <filesystem>
#include "Model.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>
#include "Model.h"
#include "assimpModel.h"
#include "Planet.h"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"
#include "BulletDynamics/btBulletDynamicsCommon.h"
#include "BulletCollision/btBulletCollisionCommon.h"
#include "BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h"
#include "BulletCollision/NarrowPhaseCollision/btPointCollector.h"
#include "skyBox.h"
#include <random>

// Window dimensions
const unsigned int width = 1400;
const unsigned int height = 900;

// Rotation speed of the planets and sun
float rotationSpeed = 0.2f;

//sun initial positions
glm::vec3 sunPos = glm::vec3(0.0f);

// Function to generate random float between -1.0 and 1.0 for asteroid positions
float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

//btConvexHullShape* createConvexHull(const std::vector<Vertex>& vertices) {
//	btConvexHullShape* hull = new btConvexHullShape();
//	for (const Vertex& v : vertices) {
//		hull->addPoint(btVector3(v.position.x, v.position.y, v.position.z));
//	}
//	return hull;
//}
//
//bool checkCollision(btConvexHullShape* shape1, btConvexHullShape* shape2) {
//	btGjkPairDetector::ClosestPointInput input;
//	
//	btVoronoiSimplexSolver simplexSolver;
//	btGjkPairDetector gjkDetector(shape1, shape2, &simplexSolver, nullptr);
//
//	btPointCollector result;
//	gjkDetector.getClosestPoints(input, result, nullptr);
//
//	return result.m_hasResult && result.m_distance <= 0.0f;
//}

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
"}\n\0";

void menu(int width, int height) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::SetWindowFontScale(3.5f); // Increase font size
	// Calculate the position for the title to be centered
	ImVec2 titleSize = ImGui::CalcTextSize("Chicken Invaders EL-Ghalaba");


	ImVec2 titlePosition((width - titleSize.x) * 0.5f, 20.0f); // Centered horizontally, 20 pixels from top

	// Draw the title
	ImGui::SetCursorPos(titlePosition);
	ImGui::Text("Chicken Invaders EL-Ghalaba");
	ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Empty line of 20 units height
	ImGui::Dummy(ImVec2(0.0f, 20.0f));
	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	ImGui::SetWindowFontScale(2.0f); // adjust font size.
	ImGui::Text("Select Game Mode");
	ImGui::Dummy(ImVec2(0.0f, 20.0f));

}
void updateHp(float hp, GLuint& VAO, GLuint& VBO, GLuint rectProgram) {
	// Calculate width based on HP (e.g., max width is 1.0)
	float maxHpWidth = 0.9f;
	float hpWidth = maxHpWidth * (hp / 100.0f); // Assuming hp is a percentage (0 to 100)

	// Vertices coordinates based on hpWidth
	GLfloat vertices[] = {
		-0.9f, 0.85f, 0.0f,
		-0.9f, 0.93f, 0.0f,
		-0.9f + hpWidth, 0.85f, 0.0f,

		-0.9f, 0.93f, 0.0f,
		-0.9f + hpWidth, 0.85f, 0.0f,
		-0.9f + hpWidth, 0.93f, 0.0f
	};

	// Bind the VAO
	glBindVertexArray(VAO);

	// Bind the VBO and update the buffer data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configure the vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(rectProgram);
	// Bind the VAO so OpenGL knows to use it
	glBindVertexArray(VAO);
	// Draw the triangle using the GL_TRIANGLES primitive
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwSwapInterval(1); // Enable VSync
	// Load GLAD for OpenGL configuration
	gladLoadGL();

	// Specify the viewport of OpenGL in the window
	glViewport(0, 0, width, height);

	// Generate shader objects
	Shader shaderProgram("default.vert", "default.frag");

	// Handle lighting
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// The position of the light is at the sun
	glm::vec3 lightPos = sunPos;

	// Activate the shader program
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Enables the depth buffer for 3D rendering
	glEnable(GL_DEPTH_TEST);

	// Creates camera object with initial position: (-600, 0, 0)
	Camera camera(width, height, glm::vec3(-600.0f, 0.0f, 0.0f)); 

	// Adjust the camera to look at the sun
	camera.Orientation = glm::vec3(1.0f, 0.0f, 0.0f);
	
	// Static Camera
	Camera camera2(width, height, glm::vec3(0.0f, 1600.0f, 0.0f));
	camera2.Orientation = glm::vec3(0.01f, -1.0f, 0.0f);

	//spaceShip pos and Rot
	glm::vec3 spaceShipPos = camera.Position + camera.Orientation + glm::vec3(50.0,0.0,0.0);
	glm::quat spaceShipRot = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//potion pos and rot
	std::random_device rd;  // Obtain a random number from hardware
	std::mt19937 gen(rd()); // Seed the generator

	// Define distribution for x, y, z coordinates
	std::uniform_real_distribution<float> dist(-1000.0f, 1000.0f);

	// Generate random positions for each potion
	glm::vec3 potionPos1(dist(gen), dist(gen), dist(gen));	glm::vec3 potionPos2(dist(gen), dist(gen), dist(gen));
	glm::vec3 potionPos3(dist(gen), dist(gen), dist(gen));	glm::vec3 potionPos4(dist(gen), dist(gen), dist(gen));
	glm::vec3 potionPos5(dist(gen), dist(gen), dist(gen));
	glm::quat potionRot = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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

	// Planets' initial positions
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

	// Load in all models
	Planet sun((parentDir + sunPath).c_str(),sunPos,sunRot, glm::vec3(3.0f),0.0f,rotationSpeed);
	Planet mercury((parentDir + mercuryPath).c_str(), mercuryPos, mercuryRot, glm::vec3(0.5f), 0.005f ,rotationSpeed);
	Planet venus((parentDir + venusPath).c_str(), venusPos, venusRot, glm::vec3(0.7f), 0.004f, rotationSpeed);
	Planet earth((parentDir + earthPath).c_str(), earthPos, earthRot, glm::vec3(0.8f), 0.003f, rotationSpeed);
	Planet mars((parentDir + marsPath).c_str(), marsPos, marsRot, glm::vec3(0.6f), 0.002f, rotationSpeed);
	Planet jupiter((parentDir + jupiterPath).c_str(), jupiterPos, jupiterRot, glm::vec3(1.5f), 0.001f, rotationSpeed);
	Planet saturn((parentDir + saturnPath).c_str(), saturnPos, saturnRot, glm::vec3(1.2f), 0.0005f,rotationSpeed);
	Planet uranus((parentDir + uranusPath).c_str(), uranusPos, uranusRot, glm::vec3(1.0f), 0.0004f, rotationSpeed);
	Planet neptune((parentDir + neptunePath).c_str(), neptunePos, neptuneRot, glm::vec3(1.0f), 0.0003f, rotationSpeed);
	
	vector<Planet> planets = {sun,mercury,venus,earth,mars,jupiter,saturn,uranus,neptune};
	
	Model asteroid((parentDir + asteroidPath).c_str());

	//std::string spaceShipPath = "/Resources/models/spaceship/spaceship.gltf";
	std::string spaceShipPath = "/Resources/models/spaceship/neghvar.obj";
	//std::string spaceShipPath = "/Resources/models/spaceship1/voyager.obj";
	//std::string spaceShipPath = "/Resources/models/backpack/backpack.obj";

	//Model spaceShip((parentDir + spaceShipPath).c_str());
	string path = parentDir + spaceShipPath;
	//AssimpModel spaceShip(path);

	//potion
	std::string potionPath = "/Resources/models/potion/scene.gltf";
	string path1 = parentDir + potionPath;
	////Model potion(path1.c_str());
	AssimpModel potion1(path1);		AssimpModel potion2(path1);
	AssimpModel potion3(path1);		AssimpModel potion4(path1);
	AssimpModel potion5(path1);

	//std::vector<Vertex> sunVertices;
	//for (Mesh m : sun.meshes)
	//	for (Vertex v : m.vertices)
	//		sunVertices.push_back(v);

	//std::vector<Vertex> spaceShipVertices;
	//for (Mesh m : earth.meshes)
	//	for (Vertex v : m.vertices)
	//		spaceShipVertices.push_back(v);
	//
	//btConvexHullShape* hull1 = createConvexHull(sunVertices);
	//btConvexHullShape* hull2 = createConvexHull(spaceShipVertices);	
	
	// Paths to all the faces of the cubemap
	std::vector<std::string> facesCubemap =
	{
		parentDir + "/Resources/skybox/right.png",
		parentDir + "/Resources/skybox/left.png",
		parentDir + "/Resources/skybox/top.png",
		parentDir + "/Resources/skybox/bottom.png",
		parentDir + "/Resources/skybox/front.png",
		parentDir + "/Resources/skybox/back.png",
	};

	// Create Skybox object
	SkyBox skybox(facesCubemap, "skybox.vert", "skybox.frag");

	// The number of asteroids to be created
	const unsigned int number = 500;

	// Radius of circle around which asteroids orbit
	float radius = 60.0f;

	// How much asteroids deviate from the radius
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

	// Create Vertex Shader Object and get its reference
	GLuint rectShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(rectShader, 1, &vertexShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(rectShader);

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Create Shader Program Object and get its reference
	GLuint rectProgram = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(rectProgram, rectShader);
	glAttachShader(rectProgram, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(rectProgram);

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(rectShader);
	glDeleteShader(fragmentShader);

	float hp = 50.0f; // Initial HP
	GLuint VAO, VBO;

	// Generate the VAO and VBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Update HP initially
	//updateHp(hp, VAO, VBO);
	
	bool showMenu = true;
	short playMode = 0;
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glViewport(0, 0, width, height);
		glfwPollEvents();
		
		menu(width, height);
		
		ImGui::Bullet();
		ImGui::SameLine();
		if (ImGui::Button("Time Attack Mode")) {
			showMenu = !showMenu;
			playMode = 1;
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Empty line of 20 units height
		ImGui::Bullet();
		ImGui::SameLine();
		if (ImGui::Button("Survival Mode")) {
			showMenu = !showMenu;
			playMode = 2;
		}

		ImGui::SetWindowFontScale(1.0f); // return normal font size
		ImGui::End();
		ImGui::Render();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (!showMenu) {
			// Clean the back buffer and assign the new color to it
			glClear(GL_COLOR_BUFFER_BIT);
			
			updateHp(hp, VAO, VBO, rectProgram);

			// Define proximity threshold
			float proximityThreshold = 37.0f; // Adjust this as needed

			// Check if spaceship is near the potion
			if (glm::distance(camera.Position, potionPos1) < proximityThreshold) {
				hp += 10; // Adjust this value as needed
				if (hp > 100.0f)
					hp = 100.0f; // Cap HP to 100
				potionPos1 = glm::vec3(dist(gen), dist(gen), dist(gen));
			} else if (glm::distance(camera.Position, potionPos2) < proximityThreshold) {
				hp += 10; // Adjust this value as needed
				if (hp > 100.0f)
					hp = 100.0f; // Cap HP to 100
				potionPos2 = glm::vec3(dist(gen), dist(gen), dist(gen));
			} else if (glm::distance(camera.Position, potionPos3) < proximityThreshold) {
				hp += 10; // Adjust this value as needed
				if (hp > 100.0f)
					hp = 100.0f; // Cap HP to 100
				potionPos3 = glm::vec3(dist(gen), dist(gen), dist(gen));
			} else if (glm::distance(camera.Position, potionPos4) < proximityThreshold) {
				hp += 10; // Adjust this value as needed
				if (hp > 100.0f)
					hp = 100.0f; // Cap HP to 100
				potionPos4 = glm::vec3(dist(gen), dist(gen), dist(gen));
			} else if (glm::distance(camera.Position, potionPos5) < proximityThreshold) {
				hp += 10; // Adjust this value as needed
				if (hp > 100.0f)
					hp = 100.0f; // Cap HP to 100
				potionPos5 = glm::vec3(dist(gen), dist(gen), dist(gen));
			}
			glViewport(0, 0, width, height);
			// Handles camera inputs
			camera.Inputs(window);

			// Updates and exports the camera matrix to the Vertex Shader
			camera.updateMatrix(45.0f, 0.1f, 2000.0f);

			// Adjust lightings for the planets
			shaderProgram.Activate();
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

			// Draw the planets around the sun
			for (Planet &p : planets) {
				p.draw(shaderProgram, camera);
			}
		
			//spaceShip.Draw(shaderProgram, camera, spaceShipPos, spaceShipRot, glm::vec3(4.0f));
			potion1.Draw(shaderProgram, camera, potionPos1, potionRot, glm::vec3(18.0f));
			potion2.Draw(shaderProgram, camera, potionPos2, potionRot, glm::vec3(18.0f));
			potion3.Draw(shaderProgram, camera, potionPos3, potionRot, glm::vec3(18.0f));
			potion4.Draw(shaderProgram, camera, potionPos4, potionRot, glm::vec3(18.0f));
			potion5.Draw(shaderProgram, camera, potionPos5, potionRot, glm::vec3(18.0f));
			//spaceShipPos += glm::vec3(0.1f, 0.0f, 0.0f);
			
			// Update the sun's and planets' rotations
			for (Planet &p : planets) {
				p.update();
			}

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

			//Draw the skyBox
			skybox.draw(camera, width, height);

			//////////////////////////////////-----------------------------------------------------//////////////////////////////////
			// Set the viewport of additional camera to the right corner
			glClear(GL_DEPTH_BUFFER_BIT);
			int rightViewportWidth = width / 3.75;
			int rightViewportX = width - rightViewportWidth;
			glViewport(rightViewportX, 0, rightViewportWidth, height / 3.75);

			// Updates and exports the camera matrix to the Vertex Shader
			camera2.updateMatrix(45.0f, 0.1f, 2000.0f);

			// Adjust lightings for the planets
			shaderProgram.Activate();
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

			// Draw the planets around the sun
			for (Planet &p : planets) {
				p.draw(shaderProgram, camera2);
			}
			potion1.Draw(shaderProgram, camera2, potionPos1, potionRot, glm::vec3(18.0f));
			potion2.Draw(shaderProgram, camera2, potionPos2, potionRot, glm::vec3(18.0f));
			potion3.Draw(shaderProgram, camera2, potionPos3, potionRot, glm::vec3(18.0f));
			potion4.Draw(shaderProgram, camera2, potionPos4, potionRot, glm::vec3(18.0f));
			potion5.Draw(shaderProgram, camera2, potionPos5, potionRot, glm::vec3(18.0f));
			// Update the sun's and planets' rotations
			for (Planet &p : planets) {
				p.update();
			}

			// Draw the asteroids around saturn only
			for (unsigned int i = 0; i < number; i++)
			{
				asteroid.Draw(shaderProgram, camera2, translations[i], rotations[i], scales[i]);
			}

			// Update the asteroid positions to orbit the sun with saturn
			for (unsigned int i = 0; i < number; i++)
			{
				translations[i] = glm::vec3(translations[i].x * cos(0.0005f) - translations[i].z * sin(-0.0005f), translations[i].y, translations[i].x * sin(-0.0005f) + translations[i].z * cos(0.0005f));
			}

			// Activate the skybox shader
			//skyboxShader.Activate();
			skybox.draw(camera, width, height);
		}
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete all the objects
	shaderProgram.Delete();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(rectProgram);
	// Delete window
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();
	return 0;
}