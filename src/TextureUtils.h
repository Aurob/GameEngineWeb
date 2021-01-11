
#ifndef TextureUtils_H
#define TextureUtils_H

#include <SDL2/SDL_image.h>
#include <emscripten.h>
#include <unordered_map>
#include <vector>
#include <string>

struct Texture
{
    std::string name;
    int w;
    int h;
    SDL_Texture *tex = NULL;
};

class TextureUtils {
    public:
        SDL_Renderer *renderer;
        std::unordered_map<std::string, std::vector<Texture>> Textures;

        bool texturesLoaded;
        bool loadTextures();
};

#endif