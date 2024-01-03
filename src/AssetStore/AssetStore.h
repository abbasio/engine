#pragma once

#include <map>
#include <string>
#include <SDL2/SDL.h>

using namespace std;

class AssetStore{
    private:
        SDL_Renderer* renderer;
        map<string, SDL_Texture*> textures;
        // TODO: create a map for fonts 
        // TODO: create a map for audio 

    public:
        AssetStore(SDL_Renderer* renderer);
        ~AssetStore();
        void SetRenderer(SDL_Renderer* renderer);
        void ClearAssets();
        void AddTexture(const string& assetId, const string& filePath);
        SDL_Texture* GetTexture(const string& assetId);
};
