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

    width = mTextureSurface->w;
    height = mTextureSurface->h;

    flipSurfaceVertically(mTextureSurface);

    
}

void Texture::TileData() 
{
    // Create a new surface with the same pixel format but adjusted dimensions
    SDL_Surface* tiledSurface = SDL_CreateRGBSurface(0, width * tileW, height * tileH, mTextureSurface->format->BitsPerPixel, mTextureSurface->format->Rmask, mTextureSurface->format->Gmask, mTextureSurface->format->Bmask, mTextureSurface->format->Amask);
    if (!tiledSurface)
    {
        SDL_Log("Failed to create surface: %s", SDL_GetError());
        return;
    }

    // Access the pixel data
    Uint32* pixelData = static_cast<Uint32*>(mTextureSurface->pixels);
    Uint32* tiledPixelData = static_cast<Uint32*>(tiledSurface->pixels);

    int tileNumW = width / tileW;
    int tileNumH = height / tileH;

    for (int tileRow = 0; tileRow < tileNumW; ++tileRow) {
        for (int tileCol = 0; tileCol < tileNumH; ++tileCol) {
            for (int tilePixelHeight = 0; tilePixelHeight < tileH; ++tilePixelHeight) {
                int linearIndex = (tilePixelHeight * width + tileCol * tileW + tileRow * width * tileH);
                for (int tilePixelWidth = 0; tilePixelWidth < tileW; ++tilePixelWidth) {
                    int tiledIndex = ((tileRow * tileH + tilePixelHeight) * width * tileNumW + tileCol * tileW + tilePixelWidth);
                    tiledPixelData[tiledIndex] = pixelData[linearIndex++];
                }
            }
        }
    }

    // Replace the original surface with the new one
    SDL_FreeSurface(mTextureSurface);
    mTextureSurface = tiledSurface;
}


// Function from https://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
Uint32 Texture::GetPixel(int u, int v) const
{
    if (u < 0 || v < 0)
    {
        u = 0;
        v = 0;
    }

    int bpp = mTextureSurface->format->BytesPerPixel;
    Uint8* p = (Uint8*)mTextureSurface->pixels + v * mTextureSurface->pitch + u * bpp;

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
        return 0;
    }
}

// Nearest neighbour sampling
// Used bilinear but it caused a massive slow down
// Probably because I used getpixel four times to get the information required for bilinear interpolation

glm::vec3 Texture::SampleTexture(float u, float v) const
{
    // Wrap the UV coordinates using the modulo operator
    u = fmod(u, 1.0f);
    v = fmod(v, 1.0f);

    // If the UV coordinate is negative, add 1 to make it positive
    if (u < 0.0f) u += 1.0f;
    if (v < 0.0f) v += 1.0f;

    float x = u * mTextureSurface->w;
    float y = v * mTextureSurface->h;

    Uint32 color;

    color = GetPixel(x, y);

    // Needed to use exception for when pixel value outside of texture range
    // Maybe can be solved with proper frustum culling so it doesn't render the model at all?
    //try 
    //{
    //    
    //}
    //catch (const std::out_of_range& e) 
    //{
    //    std::cerr << "Error: " << e.what() << std::endl;
    //    color = 0;  // Default pixel color.
    //}
    //catch (const std::runtime_error& e) 
    //{
    //    std::cerr << "Error: " << e.what() << std::endl;
    //}

    Uint8 r, g, b;

    SDL_GetRGB(color, mTextureSurface->format, &r, &g, &b);

    glm::vec3 colorVec(static_cast<float>(b) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(r) / 255.0f);

    return colorVec;

}


