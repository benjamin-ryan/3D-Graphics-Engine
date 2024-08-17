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

vertex addV(const vertex& a, const vertex& b)
{
    return vertex{a.x + b.x, a.y + b.y, a.z + b.z};
}

vertex multiplyV(const vertex& a, const vertex& b)
{
    return vertex{a.x * b.x, a.y * b.y, a.z * b.z};
}

vertex normalize(const vertex& v) {
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return vertex{v.x / length, v.y / length, v.z / length};
}

Renderer::Renderer(SDL_Window *_window, SDL_Renderer *_render, const std::vector<mesh> &_models)
{   
    window = _window;
    SDL_GetWindowSize(_window, &windowWidth, &windowHeight);

    texture = NULL;

    render = _render;
    models = _models;

    yaw = 0.0f;
    pitch = 0.0f;

    FOV = 100.0;
    rotation = 0.0f;
    time = 0.0f;

    cameraPos = {0, 0, 100.0f};
    lookDir = {0, 0, 0};
    nearPlane = 0.1f;
    farPlane = 1000.0f;
}

void Renderer::renderFrame()
{
    userInput();

    auto startTime = std::chrono::high_resolution_clock::now();
    rotation += time;

    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);

    std::vector<triangle> visibleTriangles;
    bool i = true;
    for (const auto &model : models) {
        i = !i;
    for (auto &tri : model.triangles)
    {
        vertex rotatedVertex1;
        vertex rotatedVertex2;
        vertex rotatedVertex3;

        if (controlCamera)
        {
            rotatedVertex1 = applyCameraTransform(tri.v[0]);
            rotatedVertex2 = applyCameraTransform(tri.v[1]);
            rotatedVertex3 = applyCameraTransform(tri.v[2]);
        } else {
            rotatedVertex1 = rotateX(rotateY(tri.v[0]));
            rotatedVertex2 = rotateX(rotateY(tri.v[1]));
            rotatedVertex3 = rotateX(rotateY(tri.v[2]));
        }

        rotatedVertex1 = subtractV(rotatedVertex1, cameraPos);
        rotatedVertex2 = subtractV(rotatedVertex2, cameraPos);
        rotatedVertex3 = subtractV(rotatedVertex3, cameraPos);

        // Offset into screen
        rotatedVertex1.z = rotatedVertex1.z - 100.0f;
        rotatedVertex2.z = rotatedVertex2.z - 100.0f;
        rotatedVertex3.z = rotatedVertex3.z - 100.0f;
        // Offset objects from each other
        if (i) {
            rotatedVertex1.x = rotatedVertex1.x + 3.0f;
            rotatedVertex2.x = rotatedVertex2.x + 3.0f;
            rotatedVertex3.x = rotatedVertex3.x + 3.0f;
        }

        vertex e1 = subtractV(rotatedVertex2, rotatedVertex1);
        vertex e2 = subtractV(rotatedVertex3, rotatedVertex1);
        vertex normal = crossProduct(e1, e2);

        float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
		normal.x /= l; normal.y /= l; normal.z /= l;

        vertex centroid = {
            (rotatedVertex1.x + rotatedVertex2.x + rotatedVertex3.x) / 3,
            (rotatedVertex1.y + rotatedVertex2.y + rotatedVertex3.y) / 3,
            (rotatedVertex1.z + rotatedVertex2.z + rotatedVertex3.z) / 3
        };
        vertex cameraRay = normalize(subtractV(centroid, cameraPos));
        float visibility = dotProduct(normal, cameraRay);

        if (normal.x * (rotatedVertex1.x - cameraPos.x) +
            normal.y * (rotatedVertex1.y - cameraPos.y) +
            normal.z * (rotatedVertex1.z - cameraPos.z) > 0)
        {
            coord edge1 = projection(rotatedVertex1);
            coord edge2 = projection(rotatedVertex2);
            coord edge3 = projection(rotatedVertex3);

            if (edge1.u == -1 || edge2.u == -1 || edge3.u == -1)
            {
                continue; // Skip rendering this triangle
            }

            triangle projectedTriangle = {
                edge1.u, edge1.v, rotatedVertex1.z * (farPlane / (farPlane - nearPlane)) - ((farPlane * nearPlane) / (farPlane - nearPlane)),
                edge2.u, edge2.v, rotatedVertex2.z * (farPlane / (farPlane - nearPlane)) - ((farPlane * nearPlane) / (farPlane - nearPlane)),
                edge3.u, edge3.v, rotatedVertex3.z * (farPlane / (farPlane - nearPlane)) - ((farPlane * nearPlane) / (farPlane - nearPlane))
            };
            projectedTriangle.lightIntensity = visibility;

            projectedTriangle.t[0] = tri.t[0];
            projectedTriangle.t[1] = tri.t[1];
            projectedTriangle.t[2] = tri.t[2];

            visibleTriangles.push_back(projectedTriangle);
        }
    }
    }

    sort(visibleTriangles.begin(), visibleTriangles.end(), [](triangle &t1, triangle &t2)
    {
        float z1 = (t1.v[0].z + t1.v[1].z + t1.v[2].z) / 3.0f;
		float z2 = (t2.v[0].z + t2.v[1].z + t2.v[2].z) / 3.0f;
		return z1 > z2;
    });

    for (auto &tri : visibleTriangles)
    {
        SDL_Color brightness = {
            static_cast<Uint8>(255 * tri.lightIntensity),
            static_cast<Uint8>(255 * tri.lightIntensity),
            static_cast<Uint8>(255 * tri.lightIntensity),
            0xFF};
        fillTriangle(render, {tri.v[0].x, tri.v[0].y}, {tri.v[1].x, tri.v[1].y}, {tri.v[2].x, tri.v[2].y}, tri.t[0], tri.t[1], tri.t[2], brightness);
        //SDL_RenderDrawLine(render, tri.v[0].x, tri.v[0].y, tri.v[1].x, tri.v[1].y);
        //SDL_RenderDrawLine(render, tri.v[1].x, tri.v[1].y, tri.v[2].x, tri.v[2].y);
        //SDL_RenderDrawLine(render, tri.v[2].x, tri.v[2].y, tri.v[0].x, tri.v[0].y);
    }

    SDL_RenderPresent(render);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    time = duration.count();
}

bool Renderer::loadObjFile(const std::string& filename, int index)
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

    //model = obj;
    if (index < models.size()) {
        models[index] = obj;
    } else {
        models.push_back(obj);
    }
    return true;
}

bool Renderer::loadObjTextureFile(const std::string &filename, const std::string &texturename, int index)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return false;
    }

    SDL_Surface* surface = SDL_LoadBMP(texturename.c_str());
    if (!surface) {
        std::cout << "Failed to open texture: " << texturename << std::endl;
        return false;
    }
    texture = SDL_CreateTextureFromSurface(render, surface);
    SDL_FreeSurface(surface);

    std::vector<vertex> vertices;
    std::vector<coord> tex;
    mesh m;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            vertex v;
            ss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (prefix == "vt") {
            coord t;
            ss >> t.u >> t.v;
            tex.push_back(t);
        } else if (prefix == "f") {
            triangle tri;
            for (int i = 0; i < 3; ++i) {
                std::string vertexData;
                ss >> vertexData;

                size_t firstSlash = vertexData.find('/');
                size_t secondSlash = vertexData.find('/', firstSlash + 1);

                int vIndex = std::stoi(vertexData.substr(0, firstSlash)) - 1;
                int tIndex = std::stoi(vertexData.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1;

                tri.v[i] = vertices[vIndex];
                tri.t[i] = tex[tIndex];
            }
            m.triangles.push_back(tri);
        }
    }
    //model = m;
    if (index < models.size()) {
        models[index] = m;
    } else {
        models.push_back(m);
    }
    return true;
}

coord Renderer::projection(vertex v)
{
    if (v.z + 100.0f >= nearPlane) {
        return coord{-1, -1}; // Special value to indicate clipping
    }
    return coord{(windowWidth / 2) + ((FOV * v.x) / (FOV + v.z)) * 100,
                 (windowHeight / 2) + ((FOV * v.y) / (FOV + v.z)) * -100};
}

vertex Renderer::rotateX(vertex v)
{
    if (!controlCamera)
    {
        vertex rotatedVertex;
        rotatedVertex.x = v.x;
        rotatedVertex.y = cos(rotation) * v.y - sin(rotation) * v.z;
        rotatedVertex.z = sin(rotation) * v.y + cos(rotation) * v.z;
        return rotatedVertex;
    } else {
        vertex rotatedVertex;
        rotatedVertex.x = v.x;
        rotatedVertex.y = cos(pitch) * v.y - sin(pitch) * v.z;
        rotatedVertex.z = sin(pitch) * v.y + cos(pitch) * v.z;
        return rotatedVertex;
    }
}

vertex Renderer::rotateY(vertex v)
{
    if (!controlCamera)
    {
        vertex rotatedVertex;
        rotatedVertex.x = cos(rotation) * v.x - sin(rotation) * v.z;
        rotatedVertex.y = v.y;
        rotatedVertex.z = sin(rotation) * v.x + cos(rotation) * v.z;
        return rotatedVertex;
    } else {
        vertex rotatedVertex;
        rotatedVertex.x = cos(yaw) * v.x - sin(yaw) * v.z;
        rotatedVertex.y = v.y;
        rotatedVertex.z = sin(yaw) * v.x + cos(yaw) * v.z;
        return rotatedVertex;
    }
}

vertex Renderer::applyCameraTransform(vertex v)
{
    // Apply yaw (rotation around x-axis)
    v = rotateY(v);

    // Apply pitch (rotation around y-axis)
    v = rotateX(v);

    return v;
}

void Renderer::fillTriangle(SDL_Renderer *renderer, coord v1, coord v2, coord v3, coord t1, coord t2, coord t3, SDL_Color color)
{
    SDL_Color tri_color={155,155,155,255};
    SDL_Vertex vertices[3] = {
        {
            { v1.u,v1.v },
            color, // 255, 0, 0, 0xFF
            { t1.u, 1.0f - t1.v }
        },
        {
            { v2.u, v2.v },
            color, // 0, 255, 0, 0xFF
            { t2.u, 1.0f - t2.v }
        },
        {
            { v3.u, v3.v },
            color, // 0, 0, 255, 0xFF 
            { t3.u, 1.0f - t3.v }
        }
    };
    if (texture == NULL) {
        SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
    }
    else {
        SDL_RenderGeometry(renderer, texture, vertices, 3, NULL, 0);
    }
}

void Renderer::userInput()
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    float cameraSpeed = 0.01f;
    float cameraSensitivity = 0.01f;

    int mouseX;
    int mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    float dX = (mouseX - centerX) * cameraSensitivity;
    float dY = (mouseY - centerY) * cameraSensitivity;

    yaw += dX;
    pitch -= dY;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    if (state[SDL_SCANCODE_W])
        cameraPos.z -= cameraSpeed * 100;
    if (state[SDL_SCANCODE_S])
        cameraPos.z += cameraSpeed * 100;
    if (state[SDL_SCANCODE_A])
        cameraPos.x -= cameraSpeed;
    if (state[SDL_SCANCODE_D])
        cameraPos.x += cameraSpeed;
    if (state[SDL_SCANCODE_Q])
        cameraPos.y -= cameraSpeed;
    if (state[SDL_SCANCODE_E])
        cameraPos.y += cameraSpeed;
    if (state[SDL_SCANCODE_ESCAPE])
        exit(0);

    if (controlCamera)
        SDL_WarpMouseInWindow(SDL_GetWindowFromID(SDL_GetWindowID(window)), centerX, centerY);
}

void Renderer::setControlCamera(bool _controlCamera)
{
    controlCamera = _controlCamera;
}