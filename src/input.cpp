#include "input.h"
#include "engine.h"
#include <iostream>


Input::Input(){}

Input::~Input(){}

bool Input::StartUp(Renderer* renderer)
{
    mRenderer = renderer;

    std::string defaultFilepath = "./assets/teapot/teapot.txt";

    mRenderer->loadScene(defaultFilepath);

    return true;
}

void Input::ProcessEvents(bool& quit, Uint32 totalFrameTime)
{
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    totalFrameTime;
    SDL_Event event;
    bool rightMouseButtonDown = false;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            quit = true;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                
                std::cout << "Right button\n";
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                std::cout << "Mouse released\n";
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
        }
        else if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            }
            if (event.key.keysym.sym == SDLK_1)
            {
                std::string filepath = "./assets/teapot/teapot.txt";
                mRenderer->clearScene();
                mRenderer->loadScene(filepath);
            }
            if (event.key.keysym.sym == SDLK_2)
            {
                std::string filepath = "./assets/firehydrant/firehydrant.txt";
                mRenderer->clearScene();
                mRenderer->loadScene(filepath);
            }
            if (event.key.keysym.sym == SDLK_3)
            {
                std::string filepath = "./assets/statue/statue.txt";
                mRenderer->clearScene();
                mRenderer->loadScene(filepath);
            }
            if (event.key.keysym.sym == SDLK_4)
            {
                std::string filepath = "./assets/cerberus/cerberus.txt";
                mRenderer->clearScene();
                mRenderer->loadScene(filepath);
            }
            if (event.key.keysym.sym == SDLK_5)
            {
                std::string filepath = "./assets/heating/heating.txt";
                mRenderer->clearScene();
                mRenderer->loadScene(filepath);
            }
            if (event.key.keysym.sym == SDLK_6)
            {
                std::string filepath = "./assets/chest/chest.txt";
                mRenderer->clearScene();
                mRenderer->loadScene(filepath);
            }
            if (event.key.keysym.sym == SDLK_7)
            {
                std::string filepath = "./assets/lantern/lantern.txt";
                mRenderer->clearScene();
                mRenderer->loadScene(filepath);
            }
            if (event.key.keysym.sym == SDLK_8)
            {
                std::string filepath = "./assets/material_test/material_test.txt";
                mRenderer->clearScene();
                mRenderer->loadScene(filepath);
            }
        }
    }

    // Process mouse movement outside the event loop
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
    {
        int deltaX, deltaY;
        SDL_GetRelativeMouseState(&deltaX, &deltaY);
        mRenderer->getSceneCamera()->RotateCamera(deltaX * 0.05f, deltaY * 0.05f); // Adjust the scaling factors as needed
    }

    // Process keyboard input outside the event loop
    if (keyboardState[SDL_SCANCODE_W])
    {
        mRenderer->getSceneCamera()->MoveForward(0.005f * totalFrameTime);
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        mRenderer->getSceneCamera()->MoveBackward(0.005f * totalFrameTime);
    }
    if (keyboardState[SDL_SCANCODE_A])
    {
        mRenderer->getSceneCamera()->StrafeLeft(0.005f * totalFrameTime);
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
        mRenderer->getSceneCamera()->StrafeRight(0.005f * totalFrameTime);
    }

    mRenderer->getScene().UpdateLights(totalFrameTime);
}



void Input::ShutDown()
{
    mRenderer = nullptr;
}