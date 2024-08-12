#include <SDL2/SDL.h>
#include "graphicsEngine.h"
#include <iostream>

int main()
{
    mesh cube;
    cube.triangles = {
        { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },
                              
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },
                                   
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },
                                            
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },
                                                
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },
                                                  
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f }
    };

    mesh prism;
    prism.triangles = {
        { 0.0f, 0.0f, 0.0f,    0.5f, 1.0f, 0.5f,    1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,    0.5f, 0.0f, 0.5f,    1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,    0.5f, 1.0f, 0.5f,    0.5f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f,    0.5f, 1.0f, 0.5f,    0.5f, 1.0f, 0.5f }
    };

    mesh m;
    m.triangles = {
    { -0.5f, 0.0f, -0.5f,  0.5f, 0.0f, -0.5f,  0.5f, 0.0f,  0.5f },
    {  -0.5f, 0.0f, -0.5f ,   0.5f, 0.0f,  0.5f ,  -0.5f, 0.0f,  0.5f },
    {  -0.5f, 0.0f, -0.5f ,   0.5f, 0.0f, -0.5f ,   0.0f, 1.0f,  0.0f },
    {   0.5f, 0.0f, -0.5f ,   0.5f, 0.0f,  0.5f ,   0.0f, 1.0f,  0.0f },
    {   0.5f, 0.0f,  0.5f ,  -0.5f, 0.0f,  0.5f ,   0.0f, 1.0f,  0.0f },
    {  -0.5f, 0.0f,  0.5f ,  -0.5f, 0.0f, -0.5f ,   0.0f, 1.0f,  0.0f }
    };

    std::cout << "Hello, World!" << std::endl;

    SDL_Window *window;
    SDL_Renderer *renderer;
    window = SDL_CreateWindow("3D Graphics Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Renderer frameRenderer(window, renderer, cube);
    frameRenderer.loadObjFile("Models/cube.obj");

    bool running = true;

    while (running)
    {
        if (SDL_QuitRequested())
        {
            running = false;
            break;
        }
        frameRenderer.renderFrame();
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}