#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <iostream>

class FontRenderer
{
    public:
        FontRenderer(SDL_Renderer* renderer, const std::string& fontFile);
        void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y);

    private:
        SDL_Texture* texture;
        std::unordered_map<char, SDL_Rect> glyphs;

        void initializeGlyphs();
};