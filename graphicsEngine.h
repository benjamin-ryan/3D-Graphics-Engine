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
        Renderer(SDL_Window* _window, SDL_Renderer* _render, mesh& _mesh);
        void renderFrame();
        bool loadObjFile(const std::string& filename);
        bool loadObjTextureFile(const std::string &filename, const std::string &texturename);
    private:
        coord projection(vertex);
        vertex rotateX(vertex);
        vertex rotateY(vertex);

        void userInput();

        SDL_Renderer* render;
        int windowWidth;
        int windowHeight;

        SDL_Texture* texture;

        vertex cameraPos;
        float nearPlane;
        float farPlane;

        float FOV;
        float rotation;
        float time;

        mesh model;

        void fillTriangle(SDL_Renderer *renderer, coord v1, coord v2, coord v3, coord t1, coord t2, coord t3, SDL_Color color);
};

#endif