#include <filesystem>
#include <random>
#include "Model.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Model.h"
#include "assimpModel.h"
#include "SoundDevice.h"
#include "SoundBuffer.h"
#include "SoundSource.h"
#include "spaceShip.h"
#include "skyBox.h"
#include "planet.h"
#include "ammoController.h"
#include "flyWeightModelFactory.h"
#include "enemy.h"

// Window dimensions
const unsigned int width = 1400;
const unsigned int height = 900;

// Rotation speed of the planets and sun
float rotationSpeed = 0.2f;

// sun initial positions
glm::vec3 sunPos = glm::vec3(0.0f);

// Function to generate random float between -1.0 and 1.0 for asteroid positions
float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

void menu(int width, int height)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

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
void updateHp(float hp, GLuint &VAO, GLuint &VBO, GLuint rectProgram)
{
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
		-0.9f + hpWidth, 0.93f, 0.0f};

	// Bind the VAO
	glBindVertexArray(VAO);

	// Bind the VBO and update the buffer data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Configure the vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
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

void gameOver(int width, int height)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("End", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::Dummy(ImVec2(0, 100));
	ImGui::SetWindowFontScale(5.0f);
	ImVec2 titleSize = ImGui::CalcTextSize("Game Over");

	ImVec2 titlePosition((width - titleSize.x) * 0.5f, 120.0f); // Centered horizontally, 120 pixels from top

	// Draw the title
	ImGui::SetCursorPos(titlePosition);
	ImGui::Text("Game Over");
	ImGui::End();
	ImGui::Render();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
	GLFWwindow *window = glfwCreateWindow(width, height, "Solar System", NULL, NULL);

	// Position the window in the center of the screen
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);

	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwSwapInterval(1);
	// Enable VSync
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
	Camera camera(width, height, glm::vec3(-600.0f, 20.0f, 0.0f));

	// Adjust the camera to look at the sun
	camera.Orientation = glm::vec3(1.0f, 0.0f, 0.0f);

	// Static Camera
	Camera camera2(width, height, glm::vec3(0.0f, 1600.0f, 0.0f));
	camera2.Orientation = glm::vec3(0.01f, -1.0f, 0.0f);
	// spaceShip pos and Rot
	glm::vec3 spaceShipPos = glm::vec3(-530.0f, 0.0f, 0.0f);
	glm::vec3 spaceShipScale = glm::vec3(3.0f);
	// glm::quat spaceShipRot = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat spaceShipRot = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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
	Planet sun((parentDir + sunPath).c_str(), sunPos, sunRot, glm::vec3(3.0f), 0.0f, rotationSpeed);
	Planet mercury((parentDir + mercuryPath).c_str(), mercuryPos, mercuryRot, glm::vec3(0.5f), 0.005f, rotationSpeed);
	Planet venus((parentDir + venusPath).c_str(), venusPos, venusRot, glm::vec3(0.7f), 0.004f, rotationSpeed);
	Planet earth((parentDir + earthPath).c_str(), earthPos, earthRot, glm::vec3(0.8f), 0.003f, rotationSpeed);
	Planet mars((parentDir + marsPath).c_str(), marsPos, marsRot, glm::vec3(0.6f), 0.002f, rotationSpeed);
	Planet jupiter((parentDir + jupiterPath).c_str(), jupiterPos, jupiterRot, glm::vec3(1.5f), 0.001f, rotationSpeed);
	Planet saturn((parentDir + saturnPath).c_str(), saturnPos, saturnRot, glm::vec3(1.2f), 0.0005f, rotationSpeed);
	Planet uranus((parentDir + uranusPath).c_str(), uranusPos, uranusRot, glm::vec3(1.0f), 0.0004f, rotationSpeed);
	Planet neptune((parentDir + neptunePath).c_str(), neptunePos, neptuneRot, glm::vec3(1.0f), 0.0003f, rotationSpeed);

	vector<Planet> planets = {sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune};

	Model asteroid((parentDir + asteroidPath).c_str());
	std::string spaceShipPath = parentDir + "/Resources/models/spaceship/neghvar.obj";
	//std::string spaceShipPath = parentDir + "/Resources/models/spaceship1/voyager.obj";
	std::string enemySpaceShipPath = parentDir + "/Resources/models/spaceship2/scene.gltf";
	//std::string spaceShipPath = parentDir + "/Resources/models/bullet/bullet.obj";

	//Ammo path
	std::string ammoPath = parentDir + "/Resources/models/bullet/bullet.obj";

	// Model spaceShip((parentDir + spaceShipPath).c_str());
	SpaceShip spaceShip(spaceShipPath, width, height, spaceShipPos, spaceShipScale);
	// AssimpModel spaceShip(spaceShipPath);

	//smoke
	std::string smokePath = "/Resources/smoke/smoke.obj";
	string path2 = parentDir + smokePath;
	AssimpModel smoke1(path2);

	// potion
	std::string potionPath = "/Resources/models/potion/scene.gltf";
	string path1 = parentDir + potionPath;
	AssimpModel potion1(path1);
	AssimpModel potion2(path1);
	AssimpModel potion3(path1);
	AssimpModel potion4(path1);
	AssimpModel potion5(path1);

	vector<AssimpModel> potions = { potion1, potion2, potion3, potion4, potion5 };

	// potion pos and rot
	glm::quat potionRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	//potionRot = glm::rotate(potionRot, glm::radians(rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));

	std::random_device rd;	// Obtain a random number from hardware
	std::mt19937 gen(rd()); // Seed the generator

	// Define distribution for x, y, z coordinates
	std::uniform_real_distribution<float> dist(-1000.0f, 1000.0f);
	
	std::uniform_real_distribution<float> speed(0.5f, 2.0f);

	for (AssimpModel& pot : potions)
	{
		pot.position = glm::vec3(dist(gen), dist(gen), dist(gen));
	}
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
	const unsigned int number = 200;

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

	Shader hpShader("hp.vert", "hp.frag");

	float hp = 50.0f; // Initial HP
	GLuint VAO, VBO;

	// Generate the VAO and VBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Update HP initially
	// updateHp(hp, VAO, VBO);

	// Initialize the sound device and effects
	SoundDevice *soundDevice = SoundDevice::get();
	uint32_t healSound = SoundBuffer::get()->addSoundEffect((parentDir + "/Resources/sounds/heal.ogg").c_str());
	uint32_t mainMenuSound = SoundBuffer::get()->addSoundEffect((parentDir + "/Resources/sounds/mainmenu.ogg").c_str());
	uint32_t crashSound = SoundBuffer::get()->addSoundEffect((parentDir + "/Resources/sounds/shipcrash.ogg").c_str());
	uint32_t shootingSound = SoundBuffer::get()->addSoundEffect((parentDir + "/Resources/sounds/shooting.ogg").c_str());
	SoundSource speaker;

	spaceShip.shootingSound = shootingSound;

	// Play the main menu sound
	speaker.Play(mainMenuSound);

	bool showMenu = true;
	short playMode = 0;
	double startTime;
	double countdownDuration = 10.0;
	double elapsedTime, remainingTime = 10;
	float lastCollisionTime = 0.0f;

	const double targetFPS = 60.0;
	const double frameTime = 1.0 / targetFPS;
	double lastFrameTime = glfwGetTime();
	double deltaTime = 0.0;


	// Get the AmmoController instance and use it
	AmmoController* ammoController = AmmoController::getInstance();
	// Get the FlyWeightModelFactory instance and save models in it
	FlyWeightModelFactory* flyWeightModelFactory = FlyWeightModelFactory::getInstance();
	flyWeightModelFactory->getModel("Ammo", ammoPath);
	//flyWeightModelFactory->getModel("Enemy", enemySpaceShipPath);

	vector<Enemy> enemyList;
	for (int i = 0; i < 5; i++) {
		enemyList.push_back(Enemy(enemySpaceShipPath, width, height, glm::vec3(dist(gen), dist(gen), dist(gen)), 
													glm::vec3(3.0f), glm::vec3(speed(gen), speed(gen), speed(gen))));
	}
		
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		double currentFrameTime = glfwGetTime();
		deltaTime += (currentFrameTime - lastFrameTime);
		lastFrameTime = currentFrameTime;
          
		// to control 60 frame per second
		if (deltaTime >= frameTime)
		{
			deltaTime = 0.0;
			// Clear the screen
			glViewport(0, 0, width, height);
			glfwPollEvents();

			menu(width, height);

			ImGui::Bullet();
			ImGui::SameLine();
			if (ImGui::Button("Time Attack Mode"))
			{
				// Stop the main menu sound
				speaker.Stop();
				showMenu = !showMenu;
				playMode = 1;
				startTime = glfwGetTime();
			}

			ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Empty line of 20 units height
			ImGui::Bullet();
			ImGui::SameLine();
			if (ImGui::Button("Survival Mode"))
			{
				speaker.Stop();
				showMenu = !showMenu;
				playMode = 2;
				startTime = glfwGetTime();
			}

			ImGui::SetWindowFontScale(1.0f); // return normal font size
			ImGui::End();
			ImGui::Render();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (!showMenu)
			{
				// Clean the back buffer and assign the new color to it
				glClear(GL_COLOR_BUFFER_BIT);

				updateHp(hp, VAO, VBO, hpShader.ID);
				float currentTime = glfwGetTime();
				// Define proximity threshold
				float proximityThreshold = 37.0f; // Adjust this as needed

				// Check if spaceship is near the potion
				for (AssimpModel& pot : potions)
				{
					if (glm::distance(spaceShip.position, pot.position) < proximityThreshold)
					{
						speaker.Play(healSound);
						hp += 10; // Subject to change
						if (hp > 100.0f)
							hp = 100.0f; // Cap HP to 100
						pot.position = glm::vec3(dist(gen), dist(gen), dist(gen));
					}
				}

				glViewport(0, 0, width, height);
				// Handles camera inputs
				// camera.Inputs(window);

				// Adjust lightings for the planets
				shaderProgram.Activate();
				glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
				glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

				// Draw the planets around the sun
				for (Planet& p : planets)
				{
					p.draw(shaderProgram, camera);
				}
				// spaceShip.Draw(shaderProgram, camera, spaceShipPos, spaceShipRot, glm::vec3(4.0f));

				for (AssimpModel& pot : potions)
				{
					pot.Draw(shaderProgram, camera, pot.position, potionRot, glm::vec3(18.0f));
				}

			// update spaceship position and rotation
			spaceShip.update(window, camera, speaker);
			// Updates and exports the camera matrix to the Vertex Shader
			camera.updateMatrix(45.0f, 0.1f, 2000.0f);
			// draw the space ship
			spaceShip.draw(shaderProgram, camera);

			//update Enemies
			for (Enemy& enemy : enemyList) {
				enemy.update(window,camera, spaceShip.position);
			}
			//draw Enemies
			for (Enemy& enemy : enemyList) {
				enemy.draw(shaderProgram,camera);
			}
			// spaceShip.Draw(shaderProgram, camera, spaceShipPos, spaceShipRot, glm::vec3(4.0f));
			// spaceShipPos += glm::vec3(0.1f, 0.0f, 0.0f);

				// Update the sun's and planets' rotations
				for (Planet& p : planets)
				{
					p.update();
				}

				// Draw the asteroids around saturn only and update their postion
				for (unsigned int i = 0; i < number; i++)
				{
					asteroid.Draw(shaderProgram, camera, translations[i], rotations[i], scales[i]);
					translations[i] = glm::vec3(translations[i].x * cos(0.0005f) - translations[i].z * sin(-0.0005f), translations[i].y, translations[i].x * sin(-0.0005f) + translations[i].z * cos(0.0005f));
				}

				for (int i = 0; i < planets.size(); i++)
				{
					float distanceToPlanet = glm::distance(spaceShip.position, planets[i].planetPos);
					float pr;
					switch (i)
					{
					case 0: // sun
						pr = 93;	break;
					case 1: // mercury
						pr = 23;	break;
					case 2: // venus
						pr = 38;	break;
					case 3: // earth
						pr = 33;	break;
					case 4: // mars
						pr = 33;	break;
					case 5: // jupiter
						pr = 53;	break;
					case 6: // saturn
						pr = 45;	break;
					case 7: // uranus
						pr = 43;	break;
					case 8: // neptune
						pr = 43;
					}

					// Ensure the camera doesn't penetrate the planet
					if (distanceToPlanet < pr)
					{
						glm::vec3 directionToPlanet = glm::normalize(spaceShip.position - planets[i].planetPos);
						spaceShip.position = planets[i].planetPos + directionToPlanet * pr;
						if (currentTime - lastCollisionTime >= 1)
						{
							hp -= 10;
							lastCollisionTime = currentTime;
							speaker.Play(crashSound);
							smoke1.Draw(shaderProgram, camera, spaceShip.position, glm::quat(glm::radians(glm::vec3(0.0f))), glm::vec3(2.0f));

						}
					}
				}

			ammoController->updateAmmos(window);
			ammoController->drawAmmos(shaderProgram, camera);

			// Draw the skyBox
			skybox.draw(camera, width, height);

				//////////////////////////////////-----------------Second view port-------------------//////////////////////////////////
				// Set the viewport of additional camera to the lower right corner
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

				// Draw the planets around the sun and update rotations
				for (Planet& p : planets)
				{
					p.draw(shaderProgram, camera2);
					p.update();
				}
				spaceShip.draw(shaderProgram, camera2);

				for (AssimpModel& pot : potions)
				{
					pot.Draw(shaderProgram, camera2, pot.position, potionRot, glm::vec3(18.0f));
				}

				// Draw the asteroids around saturn only and update their postion
				for (unsigned int i = 0; i < number; i++)
				{
					asteroid.Draw(shaderProgram, camera2, translations[i], rotations[i], scales[i]);
					translations[i] = glm::vec3(translations[i].x * cos(0.0005f) - translations[i].z * sin(-0.0005f), translations[i].y, translations[i].x * sin(-0.0005f) + translations[i].z * cos(0.0005f));
				}

				skybox.draw(camera2, width, height);

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				ImGui::SetNextWindowPos(ImVec2(width - 420, 0));
				ImGui::SetNextWindowSize(ImVec2(width, 40));
				ImGui::Begin("Time", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
				ImGui::SetWindowFontScale(2.0f);
				if (playMode == 1)
				{
					elapsedTime = glfwGetTime() - startTime;
					remainingTime = countdownDuration - elapsedTime;
					ImGui::Text("Time Remaining: %.0f seconds", remainingTime);
				}
				else
				{
					elapsedTime = glfwGetTime() - startTime;
					ImGui::Text("Time Elapsed: %.0f seconds", elapsedTime);
				}
				ImGui::End();
				ImGui::Render();
				glClear(GL_DEPTH_BUFFER_BIT);
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				if (remainingTime <= 0 || hp <= 0)
				{
					gameOver(width, height);
					//speaker.Stop();
				}
			}
			// Swap the back buffer with the front buffer
			glfwSwapBuffers(window);
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete all the objects
	shaderProgram.Delete();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	hpShader.Delete();
	// Delete window
	glfwDestroyWindow(window);

	// Terminate GLFW
	glfwTerminate();

	return 0;
}