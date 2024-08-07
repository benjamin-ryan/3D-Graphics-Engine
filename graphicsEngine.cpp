#include <SDL2/SDL.h>
#include "graphicsEngine.h"
#include <iostream>
#include <chrono>
#include <sstream>

vertex crossProduct(const vertex& a, const vertex& b)
{
    return vertex{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float dotProduct(const vertex& a, const vertex& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vertex subtractV(const vertex& a, const vertex& b)
{
    return vertex{a.x - b.x, a.y - b.y, a.z - b.z};
}

vertex normalize(const vertex& v) {
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return vertex{v.x / length, v.y / length, v.z / length};
}

Renderer::Renderer(SDL_Window *_window, SDL_Renderer *_render, mesh &_mesh)
{
    SDL_GetWindowSize(_window, &windowWidth, &windowHeight);

    render = _render;
    model = _mesh;

    FOV = 100.0;
    rotation = 0.0f;
    time = 0.0f;
}

void Renderer::renderFrame()
{
    auto time1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration(0);

    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);

    rotation += 1 * time;

    vertex cameraPos = {0, 0, 100.0f};

    for (auto &tri : model.triangles)
    {
        vertex rotatedVertex1 = rotateX(rotateY(tri.v[0]));
        vertex rotatedVertex2 = rotateX(rotateY(tri.v[1]));
        vertex rotatedVertex3 = rotateX(rotateY(tri.v[2]));

        //coord edge1 = projection(tri.v[0]);
        //coord edge2 = projection(tri.v[1]);
        //coord edge3 = projection(tri.v[2]);

        // Calculate the normal vector of the triangle
        vertex e1 = subtractV(rotatedVertex2, rotatedVertex1);
        vertex e2 = subtractV(rotatedVertex3, rotatedVertex1);
        vertex normal = crossProduct(e1, e2);

        vertex centroid = {
            (rotatedVertex1.x + rotatedVertex2.x + rotatedVertex3.x) / 3,
            (rotatedVertex1.y + rotatedVertex2.y + rotatedVertex3.y) / 3,
            (rotatedVertex1.z + rotatedVertex2.z + rotatedVertex3.z) / 3
        };
        vertex cameraRay = subtractV(centroid, cameraPos);
        cameraRay = normalize(cameraRay);

        if (dotProduct(normal, cameraRay) > 0)
        {
            coord edge1 = projection(rotatedVertex1);
            coord edge2 = projection(rotatedVertex2);
            coord edge3 = projection(rotatedVertex3);

            fillTriangle(render, edge1, edge2, edge3);

            //SDL_RenderDrawLine(render, edge1.x, edge1.y, edge2.x, edge2.y);
            //SDL_RenderDrawLine(render, edge2.x, edge2.y, edge3.x, edge3.y);
            //SDL_RenderDrawLine(render, edge3.x, edge3.y, edge1.x, edge1.y);
        }
    }
    SDL_RenderPresent(render);

    auto time2 = std::chrono::high_resolution_clock::now();
    duration = time2 - time1;
    time = duration.count();
    time1 = time2;
}

bool Renderer::loadObjFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::vector<vertex> vertices;

    mesh obj;

    while (!file.eof())
    {
        char line[128];
        file.getline(line, 128);

        std::stringstream s;
        s << line;

        char junk;

        if (line[0] == 'v')
        {
            vertex v;
            s >> junk >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }

        if (line[0] == 'f')
        {
            int f[3];
            s >> junk >> f[0] >> f[1] >> f[2];
            obj.triangles.push_back({vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1]});
        }
    }

    model = obj;
    return true;
};

coord Renderer::projection(vertex v)
{
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    if (v.z < nearPlane)
        v.z = nearPlane;
    if (v.z > farPlane)
        v.z = farPlane;
    return coord{(windowWidth / 2) + ((FOV * v.x) / (FOV + v.z)) * 100,
                 (windowHeight / 2) + ((FOV * v.y) / (FOV + v.z)) * 100};
};

vertex Renderer::rotateX(vertex v)
{
    vertex rotatedVertex;
    rotatedVertex.x = v.x;
    rotatedVertex.y = cos(rotation) * v.y - sin(rotation) * v.z;
    rotatedVertex.z = sin(rotation) * v.y + cos(rotation) * v.z;
    return rotatedVertex;
}

vertex Renderer::rotateY(vertex v)
{
    vertex rotatedVertex;
    rotatedVertex.x = cos(rotation) * v.x - sin(rotation) * v.z;
    rotatedVertex.y = v.y;
    rotatedVertex.z = sin(rotation) * v.x + cos(rotation) * v.z;
    return rotatedVertex;
}

void Renderer::fillTriangle(SDL_Renderer *renderer, coord v1, coord v2, coord v3)
{
    SDL_Color tri_color={0,0,0,255};
    SDL_Vertex vertices[3] = {
        {
            { v1.x,v1.y },
            { 255, 255, 255, 0xFF }, // 255, 0, 0, 0xFF
            { 0.f, 0.f }
        },
        {
            { v2.x, v2.y },
            { 255, 255, 255, 0xFF }, // 0, 255, 0, 0xFF
            { 0.f, 0.f }
        },
        {
            { v3.x, v3.y },
            { 255, 255, 255, 0xFF }, // 0, 0, 255, 0xFF 
            { 0.f, 0.f }
        }
    };
  
    SDL_RenderGeometry(renderer,NULL,vertices,3,NULL,0);
}
