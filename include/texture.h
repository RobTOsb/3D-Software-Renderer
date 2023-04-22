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

	glm::vec3 getPixel(float u, float v);
	glm::vec3 SampleTexture(float u, float v) const;

private:
	SDL_Surface* mTextureSurface;
};
#endif
