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

struct triangle
{
    vertex v[3];
};

struct mesh
{
    std::vector<triangle> triangles;
};

struct coord
{
    float x;
    float y;
};

class Renderer
{
    public:
        Renderer(SDL_Window* _window, SDL_Renderer* _render, mesh& _mesh);
        void renderFrame();
        bool loadObjFile(const std::string& filename);
    private:
        coord projection(vertex);
        vertex rotateX(vertex);
        vertex rotateY(vertex);

        SDL_Renderer* render;
        int windowWidth;
        int windowHeight;

        float FOV;
        float rotation;
        float time;

        mesh model;

        void fillTriangle(SDL_Renderer *renderer, coord v1, coord v2, coord v3);
};

#endif