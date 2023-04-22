#include "texture.h"
#include <iostream>
#include <algorithm>
Texture::Texture(){}

Texture::~Texture()
{
    SDL_FreeSurface(mTextureSurface);
}

void flipSurfaceVertically(SDL_Surface* surface)
{
    int pitch = surface->pitch;
    int height = surface->h;
    void* pixels = surface->pixels;

    int rowSize = pitch * sizeof(Uint8);
    Uint8* rowBuffer = new Uint8[rowSize];

    for (int y = 0; y < height / 2; y++)
    {
        Uint8* row1 = static_cast<Uint8*>(pixels) + y * pitch;
        Uint8* row2 = static_cast<Uint8*>(pixels) + (height - y - 1) * pitch;

        std::memcpy(rowBuffer, row1, rowSize);
        std::memcpy(row1, row2, rowSize);
        std::memcpy(row2, rowBuffer, rowSize);
    }

    delete[] rowBuffer;
}

void Texture::LoadTexture(const std::string& filename)
{
	mTextureSurface = IMG_Load(filename.c_str());
    if (!mTextureSurface)
    {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return;
    }
    std::cout << "Loaded: " << filename << "\n";

    SDL_PixelFormat* pixelFormat = mTextureSurface->format;
    std::cout << "Pixel format for " << filename << ":\n";
    std::cout << "  Bits per pixel: " << static_cast<int>(pixelFormat->BitsPerPixel) << std::endl;
    std::cout << "  Bytes per pixel: " << static_cast<int>(pixelFormat->BytesPerPixel) << std::endl;
    std::cout << "  Format code: 0x" << std::hex << pixelFormat->format << std::endl;
    std::cout << "  Format name: " << SDL_GetPixelFormatName(pixelFormat->format) << std::endl;

    flipSurfaceVertically(mTextureSurface);
}

Uint32 GetPixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
    case 2:
        return *(Uint16*)p;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32*)p;
    default:
        return 0; // Should never happen, but avoids compiler warning
    }
}

//Use bilinear interpolation to make the output texture smoother
glm::vec3 Texture::SampleTexture(float u, float v) const
{
    // Wrap the UV coordinates using the modulo operator
    u = fmod(u, 1.0f);
    v = fmod(v, 1.0f);

    // If the UV coordinate is negative, add 1 to make it positive
    if (u < 0.0f) u += 1.0f;
    if (v < 0.0f) v += 1.0f;

    // Convert the wrapped UV coordinates to pixel coordinates
    float x = u * mTextureSurface->w;
    float y = v * mTextureSurface->h;

    // Calculate the integer and fractional parts of x and y
    int x1 = static_cast<int>(x) % mTextureSurface->w;
    int y1 = static_cast<int>(y) % mTextureSurface->h;
    int x2 = (x1 + 1) % mTextureSurface->w;
    int y2 = (y1 + 1) % mTextureSurface->h;

    float xFrac = x - floor(x);
    float yFrac = y - floor(y);

    // Fetch the texel colors at the surrounding pixel coordinates
    Uint8 r11, g11, b11;
    Uint8 r12, g12, b12;
    Uint8 r21, g21, b21;
    Uint8 r22, g22, b22;

    Uint32 pixel11 = GetPixel(mTextureSurface, x1, y1);
    Uint32 pixel12 = GetPixel(mTextureSurface, x1, y2);
    Uint32 pixel21 = GetPixel(mTextureSurface, x2, y1);
    Uint32 pixel22 = GetPixel(mTextureSurface, x2, y2);

    SDL_GetRGB(pixel11, mTextureSurface->format, &r11, &g11, &b11);
    SDL_GetRGB(pixel12, mTextureSurface->format, &r12, &g12, &b12);
    SDL_GetRGB(pixel21, mTextureSurface->format, &r21, &g21, &b21);
    SDL_GetRGB(pixel22, mTextureSurface->format, &r22, &g22, &b22);

    // Convert the color components from the range 0-255 to the range 0.0-1.0
    glm::vec3 color11(static_cast<float>(b11) / 255.0f, static_cast<float>(g11) / 255.0f, static_cast<float>(r11) / 255.0f);
    glm::vec3 color12(static_cast<float>(b12) / 255.0f, static_cast<float>(g12) / 255.0f, static_cast<float>(r12) / 255.0f);
    glm::vec3 color21(static_cast<float>(b21) / 255.0f, static_cast<float>(g21) / 255.0f, static_cast<float>(r21) / 255.0f);
    glm::vec3 color22(static_cast<float>(b22) / 255.0f, static_cast<float>(g22) / 255.0f, static_cast<float>(r22) / 255.0f);

    // Perform bilinear interpolation
    glm::vec3 colorTop = glm::mix(color11, color21, xFrac);
    glm::vec3 colorBottom = glm::mix(color12, color22, xFrac);
    glm::vec3 color = glm::mix(colorTop, colorBottom, yFrac);

    glm::vec3 linear_color = glm::pow(color, glm::vec3(2.2f, 2.2f, 2.2f));

    return linear_color;
}

