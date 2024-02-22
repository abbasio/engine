#pragma once

#include <string>
#include <SDL2/SDL.h>

struct SpriteComponent{
    std::string assetId;
    int width;
    int height;
    int zIndex;
    SDL_RendererFlip flip;
    bool isFixed;
    SDL_Rect srcRect;
    std::string direction; 

    SpriteComponent(std::string assetId = "", int width = 0, int height = 0, int zIndex = 0, bool isFixed = false, int srcRectX = 0, int srcRectY = 0, std::string direction = "Right"){
        this -> assetId = assetId;
        this -> width = width;
        this -> height = height;
        this -> zIndex = zIndex;
        this -> flip = SDL_FLIP_NONE;
        this -> isFixed = isFixed;
        this -> srcRect = {srcRectX, srcRectY, width, height};
        this -> direction = direction;
    }
};


// TODO: Replace int zIndex with a layer parameter that groups sprites in the z axis by a named layer
