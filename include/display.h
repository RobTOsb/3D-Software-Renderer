//Display class, it holds the resolution variables, the SDL window
//and is responsible for swapping the buffers

#ifndef DISPLAY_H
#define DISPLAY_H

#include "SDL.h"

class Display
{
public:
	const static int SCREENWIDTH = 1280;
	const static int SCREENHEIGHT = 720;


	Display();
	~Display();
	bool StartUp();
	bool ShutDown();

	void SwapBuffers(Uint32* pixels);

private:
	
	SDL_Window* mWindow;
	SDL_Surface* mSurface;
};

#endif

