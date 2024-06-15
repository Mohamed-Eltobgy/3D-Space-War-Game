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

void AmmoController::addAmmo(int width, int height, glm::vec3 position, glm::vec3 orientation,glm::vec3 rotation,std::string type) {

    std::string ammoPath = parentDir + "/Resources/models/bullet/bullet.obj";
    //std::string ammoPath = parentDir + "/Resources/models/sun/scene.gltf";
    Ammo ammo(ammoPath, width, height, position, orientation,rotation, glm::vec3(0.01f));

    if (type == "Enemy") {
        enemyAmmoList.push_back(ammo);
    }
    else {
        ammoList.push_back(ammo);
    }
    
    cout << ammoList.size() << endl;
}

void AmmoController::updateAmmos(GLFWwindow* window) {
    for (Ammo& ammo : ammoList) {
        ammo.update(window);
    }
    for (Ammo& ammo : enemyAmmoList) {
        ammo.update(window);
    }
}

void AmmoController::drawAmmos(Shader shaderProgram, Camera camera) {
    for (Ammo& ammo : ammoList) {
        ammo.draw(shaderProgram, camera);
    }
    for (Ammo& ammo : enemyAmmoList) {
        ammo.draw(shaderProgram, camera);
    }
}