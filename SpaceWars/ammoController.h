#ifndef AMMOCONTROLLER_CLASS_H
#define AMMOCONTROLLER_CLASS_H

#include <iostream>
#include <filesystem>
#include "ammo.h"

class AmmoController {
private:
    static AmmoController* instance;
    vector<Ammo> ammoList;
    // Private constructor to prevent instantiation from outside
    AmmoController();

    // Delete copy constructor and assignment operator to prevent copying
    AmmoController(const AmmoController&) = delete;
    AmmoController& operator=(const AmmoController&) = delete;

    std::string parentDir = (std::filesystem::current_path().std::filesystem::path::parent_path()).string();
public:
    // Static method to get the single instance of the class
    static AmmoController* getInstance();
    void addAmmo(int width, int height, glm::vec3 position, glm::vec3 orientation);
    void updateAmmos(GLFWwindow* window);
    void drawAmmos(Shader shaderProgram,Camera camera);
};

#endif // SINGLETON_H
