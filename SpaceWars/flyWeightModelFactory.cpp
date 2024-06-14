#include "flyWeightModelFactory.h"

// Initialize static member variable
FlyWeightModelFactory* FlyWeightModelFactory::instance = nullptr;

FlyWeightModelFactory::FlyWeightModelFactory() {
    std::cout << "Ammocontroller instance created" << std::endl;
}

FlyWeightModelFactory* FlyWeightModelFactory::getInstance() {
    if (instance == nullptr) {
        instance = new FlyWeightModelFactory();
    }
    return instance;
}

AssimpModel* FlyWeightModelFactory::getModel(std::string key,std::string modelPath) {
    // Get an existing flyweight, or otherwise create a new
    // 'model' that we can retrieve/share multiple times
    auto iter = modelsMap.find(key);
    if (iter != modelsMap.end()) {
        return iter->second;
    }
    else {
        AssimpModel* model = new AssimpModel(modelPath);
        modelsMap[key] = model;
        return model;
    }
}