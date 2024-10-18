//Display class, it holds the resolution variables, the SDL window
//and is responsible for swapping the buffers

#ifndef DISPLAY_H
#define DISPLAY_H

#include "SDL.h"

class Display
{
public:
	constexpr static int SCREENWIDTH = 1920;
	constexpr static int SCREENHEIGHT = 1080;


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

