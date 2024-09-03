#include <SDL2/SDL.h>
#include "graphicsEngine.h"
#include <iostream>

// g++ -o main main.cpp graphicsEngine.cpp fontRenderer.cpp -std=c++17 `sdl2-config --cflags --libs`

int main()
{
    mesh cube;
    cube.triangles = {
        { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,    1.0f, 0.0f,    1.0f, 1.0f },
                              
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f,   0.0f, 1.0f,    1.0f, 0.0f,    1.0f, 1.0f },
                                   
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 1.0f,    1.0f, 0.0f,    1.0f, 1.0f },
                                            
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f,   0.0f, 1.0f,    1.0f, 0.0f,    1.0f, 1.0f },
                                                
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f,    1.0f, 0.0f,    1.0f, 1.0f },
                                                  
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 1.0f,    0.0f, 0.0f,    1.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,    1.0f, 0.0f,    1.0f, 1.0f },
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
    {  -0.5f, 0.0f, -0.5f,    0.5f, 0.0f, -0.5f,    0.5f, 0.0f,  0.5f },
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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    std::vector<mesh> models(2);

    Renderer frameRenderer(window, renderer, models);
    frameRenderer.loadObjFile("Models/cube.obj", 0);
    frameRenderer.loadObjTextureFile("Models/skeletonBigger.obj", "Textures/skeleton.bmp", 1);
    //frameRenderer.loadObjTextureFile("./Models/snorkelWithTextures.obj", "./Textures/snorkel.bmp", 0);
    frameRenderer.setControlCamera(false);

    bool running = true;
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frames = 0;

    while (running)
    {
        if (SDL_QuitRequested())
        {
            running = false;
            break;
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;

        frames++;
        if (elapsed.count() >= 1.0f) {
            frameRenderer.setFps(frames);
            frames = 0;
            lastTime = currentTime;
        }

        //frameRenderer.renderFrame();
        frameRenderer.frameRender();
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}