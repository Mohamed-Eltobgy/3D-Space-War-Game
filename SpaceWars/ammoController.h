#ifndef AMMOCONTROLLER_CLASS_H
#define AMMOCONTROLLER_CLASS_H

#include <iostream>
#include <filesystem>
#include "ammo.h"

class AmmoController {
private:
    static AmmoController* instance;
    // Private constructor to prevent instantiation from outside
    AmmoController();

    // Delete copy constructor and assignment operator to prevent copying
    AmmoController(const AmmoController&) = delete;
    AmmoController& operator=(const AmmoController&) = delete;

    std::string parentDir = (std::filesystem::current_path().std::filesystem::path::parent_path()).string();
public:
    vector<Ammo> ammoList;
    vector<Ammo> enemyAmmoList;
    // Static method to get the single instance of the class
    static AmmoController* getInstance();
    void addAmmo(int width, int height, glm::vec3 position, glm::vec3 orientation, glm::vec3 rotation,std::string type);
    void updateAmmos(GLFWwindow* window);
    void drawAmmos(Shader shaderProgram,Camera camera);
};

#endif // SINGLETON_H
