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

vertex scaleV(const vertex& v, float s)
{
    return vertex{v.x * s, v.y * s, v.z * s};
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

void multiplyVM(vertex &v, vertex &product, matrix4 &m)
{
    product.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
    product.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
    product.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
    float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
    if (w != 0.0f)
    {
        product.x = product.x / w;
        product.y = product.y / w;
        product.z = product.z / w;
    }
}

matrix4 multiplyM(const matrix4 &a, const matrix4 &b)
{
    matrix4 matrix;
    for (int i = 0; i < 4; i++) 
        for (int j = 0; j < 4; j++)
            matrix.m[j][i] = a.m[j][0] * b.m[0][i] + a.m[j][1] * b.m[1][i] + a.m[j][2] * b.m[2][i] + a.m[j][3] * b.m[3][i];
	return matrix;
}

vertex normalize(const vertex& v) {
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return vertex{v.x / length, v.y / length, v.z / length};
}

matrix4 pointAtMatrix(vertex &pos, vertex &target, vertex &up)
{
    // Forward
    vertex forward = normalize(subtractV(target, pos));

    // Up
    vertex newUp = normalize(subtractV(up, scaleV(forward, dotProduct(up, forward))));

    // Right
    vertex right = crossProduct(newUp, forward);
    
    matrix4 pointAt;
    pointAt.m[0][0] = right.x;
    pointAt.m[0][1] = right.y;
    pointAt.m[0][2] = right.z;
    pointAt.m[1][0] = newUp.x;
    pointAt.m[1][1] = newUp.y;
    pointAt.m[1][2] = newUp.z;
    pointAt.m[2][0] = forward.x;
    pointAt.m[2][1] = forward.y;
    pointAt.m[2][2] = forward.z;
    pointAt.m[3][0] = pos.x;
    pointAt.m[3][1] = pos.y;
    pointAt.m[3][2] = pos.z;
    pointAt.m[3][3] = 1.0f;
    return pointAt;
}

matrix4 matrixRotateX(float _pitch)
{
		matrix4 matrix;
		matrix.m[0][0] = 1.0f;
		matrix.m[1][1] = cosf(_pitch);
		matrix.m[1][2] = sinf(_pitch);
		matrix.m[2][1] = -sinf(_pitch);
		matrix.m[2][2] = cosf(_pitch);
		matrix.m[3][3] = 1.0f;
		return matrix;
}

matrix4 matrixRotateY(float _yaw)
{
    matrix4 matrix;
	matrix.m[0][0] = cosf(_yaw);
	matrix.m[0][2] = sinf(_yaw);
	matrix.m[2][0] = -sinf(_yaw);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(_yaw);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

matrix4 inverseMatrix4(matrix4& m)
{
    matrix4 matrix;
    matrix.m[0][0] = m.m[0][0];
    matrix.m[0][1] = m.m[1][0];
    matrix.m[0][2] = m.m[2][0];
    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
    return matrix;
}

Renderer::Renderer(SDL_Window *_window, SDL_Renderer *_render, const std::vector<mesh> &_models)
{   
    window = _window;
    SDL_GetWindowSize(_window, &windowWidth, &windowHeight);

    texture = NULL;

    render = _render;
    models = _models;

    rYaw = 0.0f;
    rPitch = 0.0f;

    FOV = 100.0f;
    rotation = 0.0f;
    time = 0.0f;

    //cameraPos = {0, 0, 100.0f}; // Camera Position for original projection method
    cameraPos = {0, 0, 0.0f};
    cameraDir = {0, 0, 1};
    nearPlane = 0.1f;
    farPlane = 1000.0f;

    projectionMatrix.m[0][0] = (float(windowHeight) / float(windowWidth)) * (1.0f / tanf(FOV * 0.5f / 180.0f * 3.14159f));
    projectionMatrix.m[1][1] = 1.0f / tanf(FOV * 0.5f / 180.0f * 3.14159f);
    projectionMatrix.m[2][2] = farPlane / (farPlane - nearPlane);
    projectionMatrix.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
    projectionMatrix.m[2][3] = 1.0f;

    lowResWidth = 320;
    lowResHeight = 180;
    lowResTexture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, lowResWidth, lowResHeight);

    fontRenderer = new FontRenderer(render, "Textures/font.bmp");
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

        rotatedVertex1 = applyRotation(tri.v[0]);
        rotatedVertex2 = applyRotation(tri.v[1]);
        rotatedVertex3 = applyRotation(tri.v[2]);
        
        rotatedVertex1 = subtractV(rotatedVertex1, cameraPos);
        rotatedVertex2 = subtractV(rotatedVertex2, cameraPos);
        rotatedVertex3 = subtractV(rotatedVertex3, cameraPos);

        // Offset into screen
        rotatedVertex1.z = rotatedVertex1.z + 100.0f;
        rotatedVertex2.z = rotatedVertex2.z + 100.0f;
        rotatedVertex3.z = rotatedVertex3.z + 100.0f;
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
    //if (v.z + 100.0f >= nearPlane) {
    //    return coord{-1, -1}; // Special value to indicate clipping
    //}
    return coord{(windowWidth / 2) + ((FOV * v.x) / (FOV + v.z)) * 100,
                ((windowHeight / 2) + ((FOV * v.y) / (FOV + v.z)) * 100)};
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
        rotatedVertex.y = cos(rPitch) * v.y - sin(rPitch) * v.z;
        rotatedVertex.z = sin(rPitch) * v.y + cos(rPitch) * v.z;
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
        rotatedVertex.x = cos(rYaw) * v.x - sin(rYaw) * v.z;
        rotatedVertex.y = v.y;
        rotatedVertex.z = sin(rYaw) * v.x + cos(rYaw) * v.z;
        return rotatedVertex;
    }
}

vertex Renderer::applyRotation(vertex v)
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

    float cameraSpeed = 0.001f;
    float cameraSensitivity = 0.01f;

    int mouseX;
    int mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    float dX = (mouseX - centerX) * cameraSensitivity;
    float dY = (mouseY - centerY) * cameraSensitivity;

    rYaw += dX;
    rPitch -= dY;

    //yaw += dX;
    //pitch += dY;

    if (rPitch > 89.0f) rPitch = 89.0f;
    if (rPitch < -89.0f) rPitch = -89.0f;

    vertex forward = scaleV(cameraDir, time * 8.0f);
    vertex right = crossProduct({0.0f, 8.0f, 0.0f}, cameraDir);

    if (state[SDL_SCANCODE_W])
        cameraPos = addV(cameraPos, forward);
    if (state[SDL_SCANCODE_S])
        cameraPos = subtractV(cameraPos, forward);
    if (state[SDL_SCANCODE_A])
        cameraPos = subtractV(cameraPos, scaleV(right, time)); //cameraPos.x -= cameraSpeed;
    if (state[SDL_SCANCODE_D])
        cameraPos = addV(cameraPos, scaleV(right, time)); //cameraPos.x += cameraSpeed;
    if (state[SDL_SCANCODE_Q])
        cameraPos.y -= cameraSpeed * 8.0f;
    if (state[SDL_SCANCODE_E])
        cameraPos.y += cameraSpeed * 8.0f;
    if (state[SDL_SCANCODE_LEFT])
        yaw += 0.01f;
    if (state[SDL_SCANCODE_RIGHT])
        yaw -= 0.01f;
    if (state[SDL_SCANCODE_UP])
        pitch += 0.01f;
    if (state[SDL_SCANCODE_DOWN])
        pitch -= 0.01f;
    if (state[SDL_SCANCODE_ESCAPE])
        exit(0);

    if (controlCamera)
        SDL_WarpMouseInWindow(SDL_GetWindowFromID(SDL_GetWindowID(window)), centerX, centerY);
}

void Renderer::setControlCamera(bool _controlCamera)
{
    controlCamera = _controlCamera;
}

void Renderer::setFps(int _fps)
{
    fps = _fps;
}

void Renderer::frameRender()
{
    userInput();

    auto startTime = std::chrono::high_resolution_clock::now();
    //rotation += time; // Comment this line out to turn off rotating

    //SDL_SetRenderTarget(render, lowResTexture);
    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(render);
    SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);

    // Camera Calculations
    vertex up = {0, 1, 0};
    vertex target = {0, 0, 1};
    matrix4 cameraRotationMatrix = multiplyM(matrixRotateY(yaw), matrixRotateX(pitch));
    multiplyVM(target, cameraDir, cameraRotationMatrix);
    target = addV(cameraPos, cameraDir);
    matrix4 cameraMatrix = pointAtMatrix(cameraPos, target, up);
    matrix4 viewMatrix = inverseMatrix4(cameraMatrix);

    std::vector<triangle> visibleTriangles;

    bool i = true;

    for (const auto &model : models) {
        i = !i;
    for (auto &tri : model.triangles)
    {
        vertex rotatedVertex1;
        vertex rotatedVertex2;
        vertex rotatedVertex3;

        // Rotation
        rotatedVertex1 = applyRotation(tri.v[0]);
        rotatedVertex2 = applyRotation(tri.v[1]);
        rotatedVertex3 = applyRotation(tri.v[2]);

        // Offset into z axis
        rotatedVertex1.z = rotatedVertex1.z + 20.0f;
        rotatedVertex2.z = rotatedVertex2.z + 20.0f;
        rotatedVertex3.z = rotatedVertex3.z + 20.0f;

        // Offset from other mesh
        if (i) {
            rotatedVertex1.z = rotatedVertex1.z + 2.0f;
            rotatedVertex2.z = rotatedVertex2.z + 2.0f;
            rotatedVertex3.z = rotatedVertex3.z + 2.0f;
            rotatedVertex1.y = rotatedVertex1.y - 9.0f;
            rotatedVertex2.y = rotatedVertex2.y - 9.0f;
            rotatedVertex3.y = rotatedVertex3.y - 9.0f;
        }

        // Calculate normal by cross product
        vertex e1 = subtractV(rotatedVertex2, rotatedVertex1);
        vertex e2 = subtractV(rotatedVertex3, rotatedVertex1);
        vertex normal = crossProduct(e1, e2);
        normal = normalize(normal);

        if (normal.x * (rotatedVertex1.x - cameraPos.x) +
            normal.y * (rotatedVertex1.y - cameraPos.y) +
            normal.z * (rotatedVertex1.z - cameraPos.z) < 0)
        {
            vertex viewedVertex1;
            vertex viewedVertex2;
            vertex viewedVertex3;

            // View object through camera position and diretion
            multiplyVM(rotatedVertex1, viewedVertex1, viewMatrix);
            multiplyVM(rotatedVertex2, viewedVertex2, viewMatrix);
            multiplyVM(rotatedVertex3, viewedVertex3, viewMatrix);

            vertex projectedVertex1;
            vertex projectedVertex2;
            vertex projectedVertex3;

            // Project 3D -> 3D
            multiplyVM(viewedVertex1, projectedVertex1, projectionMatrix);
            multiplyVM(viewedVertex2, projectedVertex2, projectionMatrix);
            multiplyVM(viewedVertex3, projectedVertex3, projectionMatrix);

            // Convert Normalized Coordinates (-1, 1) to SDL Window Coordinates
            convertToWindowCoordinates(projectedVertex1);
            convertToWindowCoordinates(projectedVertex2);
            convertToWindowCoordinates(projectedVertex3);

            triangle projectedTriangle = {
                projectedVertex1,
                projectedVertex2,
                projectedVertex3
            };

            // Copy over texture U V coordinates
            projectedTriangle.t[0] = tri.t[0];
            projectedTriangle.t[1] = tri.t[1];
            projectedTriangle.t[2] = tri.t[2];

            // Calculate light level from dot product
            vertex light = { 0.0f, 0.0f, -1.0f };
            projectedTriangle.lightIntensity = dotProduct(normal, light);

            // Add triangle to list
            visibleTriangles.push_back(projectedTriangle);
        }
    }
    }

    // Sort Triangles by depth from back to front
    sort(visibleTriangles.begin(), visibleTriangles.end(), [](triangle &t1, triangle &t2)
    {
        float z1 = (t1.v[0].z + t1.v[1].z + t1.v[2].z) / 3.0f;
		float z2 = (t2.v[0].z + t2.v[1].z + t2.v[2].z) / 3.0f;
		return z1 > z2;
    });

    // Rasterize Triangles (now sorted from back to front)
    for (auto &tri : visibleTriangles)
    {
        tri.lightIntensity = 1.0f; // This line disables lighting
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

    // Render text to the screen
    fontRenderer->renderTextCentered(render, "Benjamin Ryan!", windowWidth / 2, 5, 1.0f);
    fontRenderer->renderText(render, std::to_string(fps) + " FPS", 0, 5, 1.0f, 255, 0, 0);

    SDL_RenderPresent(render);

    /*
    SDL_SetRenderTarget(render, NULL);
    SDL_RenderCopy(render, lowResTexture, nullptr, nullptr);
    SDL_RenderPresent(render);
    */

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;
    time = duration.count();
}

void Renderer::convertToWindowCoordinates(vertex &v)
{
    /* SDL Window has a coordinate system with (0, 0) in Top Left */
    v.x = (v.x + 1.0f) * (0.5f * windowWidth);
    v.y = (1.0f - v.y) * (0.5f * windowHeight);
    //v.x = (v.x + 1.0f) * (0.5f * lowResWidth);
    //v.y = (1.0f - v.y) * (0.5f * lowResHeight);
}