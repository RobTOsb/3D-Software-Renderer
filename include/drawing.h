//This holds all the drawing functions
//All static so that the renderer can freely access
//them in it's own class.

#ifndef DRAWING_H
#define DRAWING_H

#include "renderer.h"
#include "glm/glm.hpp"
#include "shader.h"

class Drawing
{
public:
	Drawing();
	~Drawing();

	static void PutPixel(Uint32* pixels, int x, int y, Uint32 colour);

	static void DrawTriangle(Uint32* pixels, glm::vec4* vertices, PBRShader& shader, float* zBuffer);

	static void ViewPortTransform(glm::vec3* vertices);

	static int gammaAdjust(float n);

	//Pixel format to set final colour to
	static const SDL_PixelFormat* const pixelFormat;

private:
	
};

#endif
