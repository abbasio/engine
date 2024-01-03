#pragma once

#include <string>

struct SpriteComponent{
    std::string assetId;
    int width;
    int height;

    SpriteComponent(std::string assetId = "", int width = 0, int height = 0){
        this -> assetId = assetId;
        this -> width = width;
        this -> height = height;
    }
};
