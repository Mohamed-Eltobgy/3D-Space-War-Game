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

    // Control ammo
    AmmoController* ammoController;

    // Extrinsic Model
    AssimpModel* Model;

    // SpaceShip constructor to set up initial values
    Enemy(std::string& modelPath, int width, int height, glm::vec3 position, glm::vec3 scale, glm::vec3 speed)
        : width(width), height(height), position(position), scale(scale), speed(speed)
    {
        // Initialize model and ammoController
        Model = new AssimpModel(modelPath);
        ammoController = new AmmoController();
    }

    // Define equality operator
    bool operator==(const Enemy& other) const {
        return position == other.position &&
            orientation == other.orientation &&
            up == other.up &&
            spaceShipRotation == other.spaceShipRotation &&
            scale == other.scale &&
            speed == other.speed &&
            width == other.width &&
            height == other.height;
    }

    // Handles inputs
    void update(GLFWwindow* window, Camera& camera, glm::vec3 playerPos);

    // Draw the space ship
    void draw(Shader shaderProgram, Camera camera);

    ~Enemy() {
        delete Model;
        delete ammoController;
    }
};

namespace std {
    template <>
    struct hash<Enemy> {
        std::size_t operator()(const Enemy& obj) const {
            // Combine hash values of the data members
            std::size_t h1 = std::hash<glm::vec3>()(obj.position);
            std::size_t h2 = std::hash<glm::vec3>()(obj.orientation);
            std::size_t h3 = std::hash<glm::vec3>()(obj.up);
            std::size_t h4 = std::hash<glm::vec3>()(obj.spaceShipRotation);
            std::size_t h5 = std::hash<glm::vec3>()(obj.scale);
            std::size_t h6 = std::hash<glm::vec3>()(obj.speed);
            std::size_t h7 = std::hash<int>()(obj.width);
            std::size_t h8 = std::hash<int>()(obj.height);

            // Combine the hash values using XOR and bit shifting
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5) ^ (h7 << 6) ^ (h8 << 7);
        }
    };
}

#endif
