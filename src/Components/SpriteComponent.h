#pragma once

#include <string>
#include <SDL2/SDL.h>

struct SpriteComponent{
    std::string assetId;
    int zIndex;
    int width;
    int height;
    bool isFixed;
    SDL_Rect srcRect;

    SpriteComponent(std::string assetId = "", int zIndex = 0, int width = 0, int height = 0, bool isFixed = false, int srcRectX = 0, int srcRectY = 0){
        this -> assetId = assetId;
        this -> zIndex = zIndex;
        this -> width = width;
        this -> height = height;
        this -> isFixed = isFixed;
        this -> srcRect = {srcRectX, srcRectY, width, height};
    }
};


// TODO: Replace int zIndex with a layer parameter that groups sprites in the z axis by a named layer
