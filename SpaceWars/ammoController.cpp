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
}

void AmmoController::updateAmmos(GLFWwindow* window) {
    for (Ammo& ammo : ammoList) {
        ammo.update(window);
    }
    for (Ammo& ammo : enemyAmmoList) {
        ammo.update(window);
    }

    // Define the condition for removal
    auto condition = [](const Ammo& ammo) {
        return (ammo.position.x * ammo.position.x + ammo.position.y * ammo.position.y + ammo.position.z * ammo.position.z) > 2500*2500;
    };

    ammoList.erase(std::remove_if(ammoList.begin(), ammoList.end(), condition), ammoList.end());
    enemyAmmoList.erase(std::remove_if(enemyAmmoList.begin(), enemyAmmoList.end(), condition), enemyAmmoList.end());

    //std::cout << ammoList.size() << " " << enemyAmmoList.size() << std::endl;
}

void AmmoController::drawAmmos(Shader shaderProgram, Camera camera) {
    for (Ammo& ammo : ammoList) {
        ammo.draw(shaderProgram, camera);
    }
    for (Ammo& ammo : enemyAmmoList) {
        ammo.draw(shaderProgram, camera);
    }
}


