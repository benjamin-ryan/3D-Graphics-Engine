#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <iostream>
#include "fontRenderer.h"

FontRenderer::FontRenderer(SDL_Renderer* renderer, const std::string& fontFile)
{
    SDL_Surface* surface = SDL_LoadBMP("Textures/font.bmp");
    if (!surface) {
        std::cout << "Failed to open texture: " << "font.bmp" << std::endl;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);    

    initializeGlyphs();
}

void FontRenderer::renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, float scale)
{
    SDL_SetTextureColorMod(texture, 255, 255, 255);

    int startX = x;
    int adjustX = 0;
    int adjustY = 0;

    for (char c : text)
    {
        if (glyphs.find(c) != glyphs.end())
        {
            int adjustX = 0;
            int adjustY = 0;
            if (c == 'p' || c == 'q' || c == 'y' || c == 'g')
            {
                adjustY = static_cast<int>(4 * scale);
            }
            else if (c == 'j')
            {
                adjustY = static_cast<int>(3 * scale);
            }

            SDL_Rect srcRect = glyphs[c];
            SDL_Rect destRect = {x + adjustX, y + adjustY, static_cast<int>(srcRect.w * scale), static_cast<int>(15 * scale)};
            SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
            x += srcRect.w * scale;
        } else if (c == '\n')
        {
            y += static_cast<int>(20 * scale);
            x = startX;
        }
    }
}

void FontRenderer::renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, float scale, int r, int g, int b)
{
    SDL_SetTextureColorMod(texture, r, g, b);

    int startX = x;
    int adjustX = 0;
    int adjustY = 0;

    for (char c : text)
    {
        if (glyphs.find(c) != glyphs.end())
        {
            int adjustX = 0;
            int adjustY = 0;
            if (c == 'p' || c == 'q' || c == 'y' || c == 'g')
            {
                adjustY = static_cast<int>(4 * scale);
            }
            else if (c == 'j')
            {
                adjustY = static_cast<int>(3 * scale);
            }

            SDL_Rect srcRect = glyphs[c];
            SDL_Rect destRect = {x + adjustX, y + adjustY, static_cast<int>(srcRect.w * scale), static_cast<int>(15 * scale)};
            SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
            x += srcRect.w * scale;
        } else if (c == '\n')
        {
            y += static_cast<int>(20 * scale);
            x = startX;
        }
    }
}

void FontRenderer::renderTextCentered(SDL_Renderer* renderer, const std::string& text, int x, int y, float scale)
{
    SDL_SetTextureColorMod(texture, 255, 255, 255);

    int width = 0;
    for (char c : text) {
        if (c == '\n')
            break;
        width += static_cast<int>(glyphs[c].w * scale);
    }
    width = width / 2;
    int startX = x - width;
    x = x - width;
    int adjustX = 0;
    int adjustY = 0;

    for (char c : text)
    {
        if (glyphs.find(c) != glyphs.end())
        {
            int adjustX = 0;
            int adjustY = 0;
            if (c == 'p' || c == 'q' || c == 'y' || c == 'g')
            {
                adjustY = static_cast<int>(4 * scale);
            }
            else if (c == 'j')
            {
                adjustY = static_cast<int>(3 * scale);
            }

            SDL_Rect srcRect = glyphs[c];
            SDL_Rect destRect = {x + adjustX, y + adjustY, static_cast<int>(srcRect.w * scale), static_cast<int>(15 * scale)};
            SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
            x += srcRect.w * scale;
        } else if (c == '\n')
        {
            y += static_cast<int>(20 * scale);
            x = startX;
        }
    }
}

void FontRenderer::renderTextCentered(SDL_Renderer* renderer, const std::string& text, int x, int y, float scale, int r, int g, int b)
{
    SDL_SetTextureColorMod(texture, r, g, b);

    int width = 0;
    for (char c : text) {
        if (c == '\n')
            break;
        width += static_cast<int>(glyphs[c].w * scale);
    }
    width = width / 2;
    int startX = x - width;
    x = x - width;
    int adjustX = 0;
    int adjustY = 0;

    for (char c : text)
    {
        if (glyphs.find(c) != glyphs.end())
        {
            int adjustX = 0;
            int adjustY = 0;
            if (c == 'p' || c == 'q' || c == 'y' || c == 'g')
            {
                adjustY = static_cast<int>(4 * scale);
            }
            else if (c == 'j')
            {
                adjustY = static_cast<int>(3 * scale);
            }

            SDL_Rect srcRect = glyphs[c];
            SDL_Rect destRect = {x + adjustX, y + adjustY, static_cast<int>(srcRect.w * scale), static_cast<int>(15 * scale)};
            SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
            x += srcRect.w * scale;
        } else if (c == '\n')
        {
            y += static_cast<int>(20 * scale);
            x = startX;
        }
    }
}

void FontRenderer::initializeGlyphs()
{
    std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!.abcdefghijklmnopqrstuvwxyz";
    
    int x = 0;
    int y = 0;
    int width = 15;
    int height = 15;

    for (char c : characters)
    {
        if (c == 'J' || c == 'K' || c == 'L' || c == 'T' || c == 'U' || c == 'Y' || c == 'm')
        {
            width = 14;
        }
        else if (c == 'I')
        {
            width = 8;
        }
        else if (c == 'X' || c == '2' || c == '3' || c == '6' || c == '5' || c == '8' || c == '9' || c == '0')
        {
            width = 13;
        }
        else if (c == '1' || c == 't')
        {
            width = 9;
        }
        else if (c == '!' || c == '.' || c == 'i' || c == 'l')
        {
            width = 6;
        }
        else if (c == 'c' || c == 'g' || c == 'k' || c == 'n' || c == 'r' || c == 's' || c == 'u' || c == 'v' || c == 'x' || c == 'z')
        {
            width = 11;
        }
        else if (c == 'f' || c == 'j')
        {
            width = 10;
        }
        else if (c == 'a' || c == 'b' || c == 'd' || c == 'e' || c == 'o' || c == 'p' || c == 'q' || c == 'h' || c == 'y' || c == '7' || c == '4')
        {
            width = 12;
        }
        else
        {
            width = 15;
        }

        SDL_Rect rect = {x, y, width, height};
        glyphs[c] = rect;

        x += 16;
        if (x >= 128)
        {
            x = 0;
            y += 16;
        }
    }
    SDL_Rect rect = {85, 64, 7, 15};
    glyphs[' '] = rect;
}