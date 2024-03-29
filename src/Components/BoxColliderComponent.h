#pragma once

#include <glm/glm.hpp>

struct BoxColliderComponent{
    int width;
    int height;
    int damageLayer; 
    glm::vec2 offset;
    bool isColliding;

    BoxColliderComponent(int width = 0, int height = 0, int damageLayer = 0, glm::vec2 offset = glm::vec2(0)){
        this -> width = width;
        this -> height = height;
        this -> damageLayer = damageLayer;
        this -> offset = offset;
        this -> isColliding = false;
    }
};
