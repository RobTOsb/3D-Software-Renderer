#include "display.h"
#include <cstring>

Display::Display(){}

Display::~Display(){}

bool Display::StartUp()
{
    bool success = true;

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL2: %s", SDL_GetError());
        success = false;
        return success;
    }
    // Create a window
    mWindow = SDL_CreateWindow("3D Software Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREENWIDTH, SCREENHEIGHT,
        SDL_WINDOW_SHOWN);
    if (mWindow == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL2 window: %s", SDL_GetError());
        SDL_Quit();
        success = false;
        return success;
    }

    //Create surface
    mSurface = SDL_GetWindowSurface(mWindow);
    if (mSurface == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetWindowSurface failed: %s", SDL_GetError());
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
        success = false;
        return success;
    }

    return success;
}

//Double buffered, swaps the buffer here
void Display::SwapBuffers(Uint32* pixels)
{
    if (SDL_MUSTLOCK(mSurface)) 
    {
        SDL_LockSurface(mSurface);
    }

    memcpy(mSurface->pixels, pixels, SCREENWIDTH * SCREENHEIGHT * 4);

    if (SDL_MUSTLOCK(mSurface)) 
    {
        SDL_UnlockSurface(mSurface);
    }

    //Apply surface changes to window
    SDL_UpdateWindowSurface(mWindow);
}

bool Display::ShutDown()
{
    bool success = true;

    // Destroy the window and quit SDL2
    SDL_DestroyWindow(mWindow);
    mWindow = nullptr;
    SDL_Quit();

    if (mWindow != nullptr)
    {
        success = false;

        return success;
    }

    return success;

}


