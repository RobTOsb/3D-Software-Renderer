// Engine class, responsible for setting up everything in
// the renderer and has the main event loop

#ifndef ENGINE_H
#define ENGINE_H
#include "renderer.h"
#include "display.h"
#include "input.h"
class Engine
{
public:
	Engine();
	~Engine();

	bool StartUp();

	void ShutDown();

	void Run();

private:

	Display displayInstance;
	Input inputInstance;
	Renderer renderInstance;

	
};

#endif

