#include <vector>
#include <string>
#include <fstream>

#ifndef GRAPHICSENGINE_H
#define GRAPHICESENGINE_H

struct vertex
{
    float x;
    float y;
    float z;
};

struct coord
{
    float u;
    float v;
};

struct triangle
{
    vertex v[3];
    coord t[3];
    float lightIntensity;
};

struct mesh
{
    std::vector<triangle> triangles;
};

class Renderer
{
    public:
        Renderer(SDL_Window *_window, SDL_Renderer *_render, const std::vector<mesh> &_models);
        void renderFrame();
        bool loadObjFile(const std::string& filename, int index);
        bool loadObjTextureFile(const std::string &filename, const std::string &texturename, int index);
        void setControlCamera(bool _controlCamera);
    private:
        coord projection(vertex);
        vertex rotateX(vertex);
        vertex rotateY(vertex);
        vertex applyCameraTransform(vertex);

        bool controlCamera;

        void userInput();

        SDL_Window* window;
        SDL_Renderer* render;
        int windowWidth;
        int windowHeight;

        SDL_Texture* texture;

        vertex cameraPos;
        vertex lookDir;
        float nearPlane;
        float farPlane;

        float yaw;
        float pitch;

        float FOV;
        float rotation;
        float time;

        std::vector<mesh> models;

        void fillTriangle(SDL_Renderer *renderer, coord v1, coord v2, coord v3, coord t1, coord t2, coord t3, SDL_Color color);
};

#endif