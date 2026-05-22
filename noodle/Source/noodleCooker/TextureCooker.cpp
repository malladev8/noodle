#include "TextureCooker.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb/stb_image.h"

bool TextureCooker::CookTexture(const char* assetPath, std::ofstream& out)
{
    int width = -1;
    int height = -1;
    int channels = -1;
    int req_comp = 4; // Force RGBA8 (4 channels) for now. Determine SRGB based on usage
    unsigned char* pixels = stbi_load(assetPath, &width, &height, &channels, req_comp);

    if (width == -1 || height == -1 || channels == -1)
    {
        printf("Failed to load texture from: %s\n", assetPath);
        return false;
    }

    out.write(reinterpret_cast<const char*>(&width), sizeof(width));
    out.write(reinterpret_cast<const char*>(&height), sizeof(height));

    uint32 pixelCount = width * height * req_comp;
    out.write(reinterpret_cast<const char*>(&pixelCount), sizeof(pixelCount));

    out.write(reinterpret_cast<const char*>(&pixels), pixelCount);

    stbi_image_free(pixels);
    return true;
}
