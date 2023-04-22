#include "scene.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>


Scene::Scene(){}

Scene::~Scene(){}

void Scene::loadScene(std::string& sceneFile)
{
    std::ifstream scene(sceneFile);
    if (!scene.is_open())
    {
        std::cerr << "Error opening scene file: " << sceneFile << std::endl;
        return;
    }

    std::string line;
    std::unique_ptr<Model> model;

    while (std::getline(scene, line))
    {
        std::istringstream lineStream(line);
        std::string keyword;

        lineStream >> keyword;

        if (keyword == "s") 
        {
            lineStream >> name;
        }
        else if (keyword == "m") 
        {
            lineStream >> numModels;
        }
        else if (keyword == "MODEL") 
        {
            std::string path;
            lineStream >> path;

            model = std::make_unique<Model>();

            model->loadModel(path);

        }
        else if (keyword == "ALB")
        {
            std::string path;
            lineStream >> path;

            //ALB
            model->mAlbedo.LoadTexture(path);
        }
        else if (keyword == "AO")
        {
            std::string path;
            lineStream >> path;

            //AO
            model->mAmbientO.LoadTexture(path);
        }
        else if (keyword == "METAL")
        {
            std::string path;
            lineStream >> path;

            //METAL
            model->mMetallic.LoadTexture(path);
        }
        else if (keyword == "NORM")
        {
            std::string path;
            lineStream >> path;

            //NORM
            model->mNormal.LoadTexture(path);
        }
        else if (keyword == "ROUGH")
        {
            std::string path;
            lineStream >> path;

            //ROUGH
            model->mRoughness.LoadTexture(path);
        }
        else if (keyword == "HEIGHT")
        {
            std::string path;
            lineStream >> path;

            //HEIGHT
            model->mHeight.LoadTexture(path);
        }
        else if (keyword == "TRANSLATE")
        {
            glm::vec3 translation;

            lineStream >> translation.x >> translation.y >> translation.z;

            model->SetTranslate(translation);
        }
        else if (keyword == "ROTATION")
        {
            glm::vec3 rotation;

            lineStream >> rotation.x >> rotation.y >> rotation.z;   
            model->SetRotation((rotation));
            
        }
        else if (keyword == "SCALE")
        {
            glm::vec3 scale;

            lineStream >> scale.x >> scale.y >> scale.z;

            model->SetScale(scale);
        }
        else if (keyword == "END_MODEL")
        {
            if (model) 
            {
                model->SetModelMatrix();
                modelsInScene.push_back(std::move(model));
                model.reset();
            }
        }
        else if (keyword == "PLIGHT")
        {
            
            glm::vec3 position;
            glm::vec3 color;
            float intensity;

            lineStream >> position.x >> position.y >> position.z >> intensity >> color.r >> color.g >> color.b;

            mNumLights++;

            lightsInScene.push_back(std::make_unique<Lights>(position, intensity, color ));

        }
        else if (keyword == "CAMERA")
        {
            Camera camera;

            mCamera = camera;
        }
    }

    std::cout << this->numModels << " " << this->name << "\n";
}

void Scene::clearScene()
{
    // Clear the models and lights vectors
    modelsInScene.clear();
    lightsInScene.clear();

    // Reset the model and light counts
    numModels = 0;
    mNumLights = 0;
    
    //Resets the camera
    mCamera = Camera(); 
}


//GETTERS AND SETTERS

std::vector<Model*> Scene::getScene()
{
    std::vector<Model*> modelPtrs;
    for (const auto& model : modelsInScene) 
    {
        modelPtrs.push_back(model.get());
    }
    return modelPtrs;
}

std::vector<Lights*> Scene::getLights()
{
    std::vector<Lights*> lightPtrs;
    for (const auto& light : lightsInScene)
    {
        lightPtrs.push_back(light.get());
    }

    return lightPtrs;
}

void Scene::UpdateLights(int delta)
{
    for (int i = 0; i < lightsInScene.size(); i++)
    {        
        lightsInScene[i]->Update(delta);
    }
}

void Scene::UpdateModel(int delta)
{
    for (int i = 0; i < modelsInScene.size(); i++)
    {
        modelsInScene[i]->Update(delta);
    }
}
