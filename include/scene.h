//Class for loading the complete scene into the program

#ifndef SCENE_H
#define SCENE_H

#include "model.h"
#include "camera.h"
#include <memory>
#include <string>
#include "lights.h"

class Scene
{
public:
    Scene();
    ~Scene();

    void loadScene(std::string& sceneFile);

    Camera& getCamera() { return mCamera; }

    std::vector<Model*> getScene();
    std::vector<Lights*> getLights();

    void UpdateLights(int delta);
    void UpdateModel(int delta);

    void clearScene();

private:

    //Use smart pointers because I wanted to learn more about them
    //If I had a chance to redo, I think I would manage my own memory
    //As I think there is a bit of overhead involved.
    std::vector<std::unique_ptr<Model>> modelsInScene;
    std::vector<std::unique_ptr<Lights>> lightsInScene;

    int numModels = 0;
    int mNumLights = 0;

    std::string name;

    Camera mCamera;

};



#endif
