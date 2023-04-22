#include "engine.h"
#include <iostream>

//Dummy constructor and destructor
//The start up needs to be done in a specific order
Engine::Engine() {}
Engine::~Engine() {}

bool Engine::StartUp()
{
	bool success = true;

	if (!displayInstance.StartUp())
	{
		success = false;
		return success;
	}

    if (!renderInstance.StartUp(displayInstance.SCREENWIDTH, displayInstance.SCREENHEIGHT))
    {
        success = false;
        return success;
    }

    if (!inputInstance.StartUp(&renderInstance))
    {
        success = false;
        return success;
    }

    return success;
}

void Engine::Run()
{
    Uint32 startTime = SDL_GetTicks();
    Uint32 totalFrameTime = 0;
    Uint32 frameTime = 0;
    int frameCounter = 0;

    // Run the game loop
    bool quit = false;
    while (!quit) 
    {
        inputInstance.ProcessEvents(quit, frameTime);
        // Draw graphics
        renderInstance.render();
        // Present the back buffer to the screen
        displayInstance.SwapBuffers(renderInstance.getPixelBuffer());


        //Time keeping functions
        Uint32 endTime = SDL_GetTicks();
        frameTime = endTime - startTime;
        startTime = endTime;
        totalFrameTime += frameTime;
        frameCounter++;

        if (totalFrameTime >= 1000)
        {
            float averageFrameRate = frameCounter / (totalFrameTime / 1000.0f);
            std::cout << "Average Frame Rate: " << averageFrameRate << "    m/s Time: " << (int)frameTime << "\n\n";
            totalFrameTime = 0;
            frameCounter = 0;
        }
    }
}

//Shut down the instances in reverse to avoid dangling pointers
void Engine::ShutDown()
{
    inputInstance.ShutDown();

    renderInstance.ShutDown();

	displayInstance.ShutDown();
}