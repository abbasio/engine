#include "./AssetStore.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL_image.h>

using namespace std;

AssetStore::AssetStore(SDL_Renderer* renderer){
    this -> renderer = renderer;
    Logger::Log("Asset Store constructor called!");
}

AssetStore::~AssetStore(){
    ClearAssets();
    Logger::Log("Asset Store destructor called!");
}

void AssetStore::SetRenderer(SDL_Renderer* renderer){
    this -> renderer = renderer;
}

void AssetStore::ClearAssets(){
    for(auto texture: textures){
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

void AssetStore::AddTexture(const string& assetId, const string& filePath){
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Add texture to map
    textures.emplace(assetId, texture);
}

SDL_Texture* AssetStore::GetTexture(const string& assetId) {
    return textures[assetId];
}
