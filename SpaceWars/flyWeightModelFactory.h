#ifndef FlyWeightModelFactory_CLASS_H
#define FlyWeightModelFactory_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "assimpModel.h"
#include "shaderClass.h"

class FlyWeightModelFactory
{
private:
    static FlyWeightModelFactory* instance;
    map<std::string,AssimpModel*> modelsMap;
    // Private constructor to prevent instantiation from outside
    FlyWeightModelFactory();

    // Delete copy constructor and assignment operator to prevent copying
    FlyWeightModelFactory(const FlyWeightModelFactory&) = delete;
    FlyWeightModelFactory& operator=(const FlyWeightModelFactory&) = delete;

public:
    // Static method to get the single instance of the class
    static FlyWeightModelFactory* getInstance();
    AssimpModel* getModel(std::string key,std::string modelPath);
};
#endif

