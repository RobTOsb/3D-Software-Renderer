#include "drawing.h"
#include <algorithm>

Drawing::Drawing(){}

Drawing::~Drawing(){}

const SDL_PixelFormat* const Drawing::pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGB24);

//Precalculated gamma table, I got it from Angelo Ortiz's software renderer
const int gammaTable[256] = { 0, 21, 28, 34, 39, 43, 46,
        50, 53, 56, 59, 61, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84,
        85, 87, 89, 90, 92, 93, 95, 96, 98, 99, 101, 102, 103, 105, 106,
        107, 109, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 122,
        123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
        136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145, 146, 147,
        148, 149, 150, 151, 151, 152, 153, 154, 155, 156, 156, 157, 158,
        159, 160, 160, 161, 162, 163, 164, 164, 165, 166, 167, 167, 168,
        169, 170, 170, 171, 172, 173, 173, 174, 175, 175, 176, 177, 178,
        178, 179, 180, 180, 181, 182, 182, 183, 184, 184, 185, 186, 186,
        187, 188, 188, 189, 190, 190, 191, 192, 192, 193, 194, 194, 195,
        195, 196, 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203,
        203, 204, 205, 205, 206, 206, 207, 207, 208, 209, 209, 210, 210,
        211, 212, 212, 213, 213, 214, 214, 215, 215, 216, 217, 217, 218,
        218, 219, 219, 220, 220, 221, 221, 222, 223, 223, 224, 224, 225,
        225, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231,
        232, 232, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238,
        238, 239, 239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244,
        245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 249, 250, 250,
        251, 251, 252, 252, 253, 253, 254, 254, 255, 255 };

//Calculate the barycentric coordinates of a point in a triangle.
//Derived from Christer Ericson's "Real-Time Collision Detection"
glm::vec3 barycentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c) 
{
    glm::vec3 v0 = b - a;
    glm::vec3 v1 = c - a;
    glm::vec3 v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    return glm::vec3(u, v, w);
}

void Drawing::PutPixel(Uint32* pixels, int x, int y, Uint32 colour)
{
    pixels[y * Display::SCREENWIDTH + x] = colour;
}

void Drawing::DrawTriangle(Uint32* pixels, glm::vec4* vertices, PBRShader& shader, float* zBuffer)
{
    glm::vec3 finalColor;

    glm::vec3 ndcVert[3];

    glm::vec3 hW = glm::vec3(1 / vertices[0].w, 1 / vertices[1].w, 1 / vertices[2].w);

    //Perspective divide
    for (int i = 0; i < 3; i++)
    {
        ndcVert[i] = vertices[i] / vertices[i].w;
    }

    Drawing::ViewPortTransform(ndcVert);

    // Calculate the bounding box of the triangle
    int minX = std::min({ ndcVert[0].x, ndcVert[1].x, ndcVert[2].x });
    int maxX = std::max({ ndcVert[0].x, ndcVert[1].x, ndcVert[2].x });
    int minY = std::min({ ndcVert[0].y, ndcVert[1].y, ndcVert[2].y });
    int maxY = std::max({ ndcVert[0].y, ndcVert[1].y, ndcVert[2].y });

    // Clamp the bounding box to the screen dimensions
    minX = std::max(minX, 0);
    maxX = std::min(maxX, Display::SCREENWIDTH - 1);
    minY = std::max(minY, 0);
    maxY = std::min(maxY, Display::SCREENHEIGHT - 1);

    glm::vec3 zVals = { ndcVert[0].z, ndcVert[1].z, ndcVert[2].z };


    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            //Add 0.5 to get pixel center
            glm::vec3 P = { x + 0.5f, y + 0.5f, 0.0f };

            glm::vec3 bc_screen = barycentric(P, ndcVert[0], ndcVert[1], ndcVert[2]);

            if (bc_screen.x >= 0.0f && bc_screen.y >= 0.0f && bc_screen.z >= 0.0f)
            {

                //Interpolate z values for z-buffer
                for (int i = 0; i < 3; i++)
				{
					P.z += ndcVert[i][2] * bc_screen[i];
				}

                if (zBuffer[int(P.x + P.y * Display::SCREENWIDTH)] > P.z)
                {  
                    zBuffer[int(P.x + P.y * Display::SCREENWIDTH)] = P.z;

                    //Calculate perspective correct barycentric coordinates
                    glm::vec3 f = bc_screen * hW;
                    float areaPers = 1.0f / (f.x + f.y + f.z);
                    glm::vec3 bc_persp = f * areaPers;

                    finalColor = shader.fragment(bc_persp);

                    PutPixel(pixels, P.x, P.y, SDL_MapRGB(pixelFormat, 
                                                gammaAdjust(finalColor.r), 
                                                gammaAdjust(finalColor.g), 
                                                gammaAdjust(finalColor.b)));
                }
            }
        }
    }
}

//Transforms the ndc vertices to screen coordinates
void Drawing::ViewPortTransform(glm::vec3* vertices)
{
    for (int i = 0; i < 3; i++)
    {
        vertices[i].x = ((vertices[i].x + 1) * Display::SCREENWIDTH * 0.5f);
        vertices[i].y = ((vertices[i].y + 1) * Display::SCREENHEIGHT * 0.5f);
    }
}

//Adjusts gamma to 2.2, derived from Angelo Ortiz's software renderer
int Drawing::gammaAdjust(float n) 
{
    int val = static_cast<int>(glm::clamp(n * 255.0f, 0.0f, 255.0f) + 0.5f);

    if (val < 0)
    {
        val = 0;
    }

    return gammaTable[val];
}


