#include "spaceShip.h"
#include "ammo.h"
#include "ammoController.h"
#include "SoundSource.h"
#include "SoundBuffer.h"

SpaceShip::SpaceShip(string& modelPath,int width, int height, glm::vec3 position, glm::vec3 scale)
	:AssimpModel(modelPath),width(width),height(height),position(position),scale(scale)
{
	ammoController = AmmoController::getInstance();
}

void SpaceShip::update(GLFWwindow* window,Camera &camera,SoundSource &speaker) {

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += speed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position += speed * -glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position += speed * -orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += speed * glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position += speed * -up;
	}

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
			spaceShipRotation.x -= rotX;
		}
		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}

	//Control the rotation
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		spaceShipRotation -= glm::vec3(0.0f, rotationSpeed, 0.0f);
		orientation = glm::rotate(orientation, glm::radians(-rotationSpeed), up);
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		spaceShipRotation += glm::vec3(0.0f, rotationSpeed, 0.0f);
		orientation = glm::rotate(orientation, glm::radians(rotationSpeed), up);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 3.5f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 2.0f;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !mouse_button_held)
	{
		speaker.Play(shootingSound);
		ammoController->addAmmo(width,height,position,orientation);
		mouse_button_held = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		mouse_button_held = false;
	}
	
	camera.Orientation = orientation + glm::vec3(0.0f, -0.2f, 0.0f);
	camera.Position = position - orientation * 70.0f + glm::vec3(0.0f, 30.0f, 0.0f);
}

void SpaceShip::draw(Shader shaderProgram, Camera camera) {
	Draw(shaderProgram, camera, position, glm::quat(glm::radians(spaceShipRotation)), scale);
}


// Handles mouse inputs
//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
//{
//	// Hides mouse cursor
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
//
//	// Prevents camera from jumping on the first click
//	if (firstClick)
//	{
//		glfwSetCursorPos(window, (width / 2), (height / 2));
//		firstClick = false;
//	}
//
//	// Stores the coordinates of the cursor
//	double mouseX;
//	double mouseY;
//	// Fetches the coordinates of the cursor
//	glfwGetCursorPos(window, &mouseX, &mouseY);
//
//	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
//	// and then "transforms" them into degrees 
//	float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
//	float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
//
//	// Calculates upcoming vertical change in the Orientation
//	glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));
//
//	// Decides whether or not the next vertical Orientation is legal or not
//	if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
//	{
//		orientation = newOrientation;
//		spaceShipRotation.x -= rotX;
//	}
//
//	// Rotates the Orientation left and right
//	orientation = glm::rotate(orientation, glm::radians(-rotY), up);
//	spaceShipRotation.y -= rotY;
//
//	camera.Orientation = orientation + glm::vec3(0.0f, -0.5f, 0.0f);
//	camera.Position = position - orientation * 70.0f + glm::vec3(0.0f, 5.0f, 0.0f);
//
//	// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
//	glfwSetCursorPos(window, (width / 2), (height / 2));
//}
//else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
//{
//	// Unhides cursor since camera is not looking around anymore
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//	// Makes sure the next time the camera looks around it doesn't jump
//	firstClick = true;
//}