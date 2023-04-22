//Renderer class, holds most of the rendering logic

#ifndef RENDERER_H
#define RENDERER_H

#include "display.h"
#include "drawing.h"
#include "shader.h"
#include "scene.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool StartUp(const int w, const int h);
	bool ShutDown();


	//Mian rendering loop
	//First sets up the shader variables from the models in the scene
	//Second determines whether the face can be seen by the camera, if not culls the triangle
	//Third transforms the vertices to world and camera space
	//Fourth sends the vertices off to get renderered in the drawing class
	void render();

	//Packs the vertices into triangles for the renderer
	void createTriangles(glm::ivec3& index, glm::vec3* primitive, std::vector<glm::vec3>& vals);

	Uint32* getPixelBuffer();
	Camera* getSceneCamera() { return &mCurrentScene.getCamera(); }
	Scene& getScene() { return mCurrentScene; }

	//Clears pixel buffer and z-buffer
	void bufferClear();

	void loadScene(std::string& filePath);



	bool BackfaceCulling(glm::vec3& faceNorm, glm::vec3& vert, glm::mat4& worldToObject);

	void setCurrentScene(std::string& filePath);
	void clearScene();


private:

	Uint32* mPixelBuffer;
	float* mZbuffer;

	Scene mCurrentScene;

	
};

#endif


