#ifndef TEXTURE_H
#define TEXTURE_H
#include "SDL.h"
#include "SDL_image.h"
#include <string>
#include "glm/glm.hpp"


class Texture
{
public:
	Texture();
	~Texture();

	void LoadTexture(const std::string& filename);
	void TileData();

	Uint32 GetPixel(int u, int v) const;
	glm::vec3 SampleTexture(float u, float v) const;

private:
	SDL_Surface* mTextureSurface;
	int width;
	int height;

	int tileW = 32;
	int tileH = 32;
};
#endif
