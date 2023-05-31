// Class for processing the user input for the renderer

#ifndef INPUT_H
#define INPUT_H
#include "renderer.h"

class Input
{
public:
	Input();
	~Input();

	bool StartUp(Renderer* renderer);

	void ProcessEvents(bool& quit, Uint32 totalFrameTime);

	void ShutDown();

private:
	Renderer* mRenderer;

	float lastMouseX = 0;
	float lastMouseY = 0;
};

#endif

