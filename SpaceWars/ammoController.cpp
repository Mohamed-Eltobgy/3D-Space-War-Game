#include "ammoController.h"

// Initialize static member variable
AmmoController* AmmoController::instance = nullptr;

AmmoController::AmmoController() {
    std::cout << "Ammocontroller instance created" << std::endl;
}

AmmoController* AmmoController::getInstance() {
    if (instance == nullptr) {
        instance = new AmmoController();
    }
    return instance;
}

void AmmoController::addAmmo(int width, int height, glm::vec3 position, glm::vec3 orientation) {

    std::string ammoPath = parentDir + "/Resources/models/bullet/bullet.obj";
    //std::string ammoPath = parentDir + "/Resources/models/sun/scene.gltf";
    Ammo ammo(ammoPath, width, height, position, orientation, glm::vec3(0.01f));

    ammoList.push_back(ammo);
    cout << ammoList.size() << endl;
}

void AmmoController::updateAmmos(GLFWwindow* window) {
    for (Ammo& ammo : ammoList) {
        ammo.update(window);
    }
}

void AmmoController::drawAmmos(Shader shaderProgram, Camera camera) {
    for (Ammo& ammo : ammoList) {
        ammo.draw(shaderProgram, camera);
    }
}