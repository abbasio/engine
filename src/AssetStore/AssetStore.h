#pragma once

#include <map>
#include <string>
#include <SDL2/SDL.h>

using namespace std;

class AssetStore{
    private:
        map<string, SDL_Texture*> textures;
        // TODO: create a map for fonts 
        // TODO: create a map for audio 

    public:
        AssetStore();
        ~AssetStore();
        void ClearAssets();
        void AddTexture(SDL_Renderer* renderer, const string& assetId, const string& filePath);
        SDL_Texture* GetTexture(const string& assetId);
};
