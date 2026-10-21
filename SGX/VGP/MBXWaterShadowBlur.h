/* Small CPU separable Gaussian blur for a low-resolution water shadow mask. */
#ifndef MBX_WATER_SHADOW_BLUR_H
#define MBX_WATER_SHADOW_BLUR_H

#include <stddef.h>
#include <string.h>

/*
 * Blur only the alpha channel of an RGBA8 image with a 5-tap [1 4 6 4 1]/16
 * kernel. scratch must contain width*height*4 bytes. RGB is set to white so the
 * texture can be modulated by the black overlay colour in MBXWaterShadow.vp.
 * Repeat the call or blur a downsampled mask for a wider penumbra.
 */
static void MBXBlurWaterShadowAlpha(unsigned char *rgba,
                                    unsigned char *scratch,
                                    int width,
                                    int height)
{
    static const int weight[5] = { 1, 4, 6, 4, 1 };
    int x, y, k;

    if (!rgba || !scratch || width <= 0 || height <= 0)
        return;

    /* Horizontal pass. */
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            unsigned int sum = 0;
            for (k = -2; k <= 2; ++k)
            {
                int sx = x + k;
                const size_t index = ((size_t)y * (size_t)width + (size_t)(sx < 0 ? 0 : (sx >= width ? width - 1 : sx))) * 4u + 3u;
                sum += (unsigned int)rgba[index] * (unsigned int)weight[k + 2];
            }
            {
                const size_t out = ((size_t)y * (size_t)width + (size_t)x) * 4u;
                scratch[out + 0] = 255;
                scratch[out + 1] = 255;
                scratch[out + 2] = 255;
                scratch[out + 3] = (unsigned char)((sum + 8u) >> 4);
            }
        }
    }

    /* Vertical pass back into rgba. */
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            unsigned int sum = 0;
            for (k = -2; k <= 2; ++k)
            {
                int sy = y + k;
                const size_t index = ((size_t)(sy < 0 ? 0 : (sy >= height ? height - 1 : sy)) * (size_t)width + (size_t)x) * 4u + 3u;
                sum += (unsigned int)scratch[index] * (unsigned int)weight[k + 2];
            }
            {
                const size_t out = ((size_t)y * (size_t)width + (size_t)x) * 4u;
                rgba[out + 0] = 255;
                rgba[out + 1] = 255;
                rgba[out + 2] = 255;
                rgba[out + 3] = (unsigned char)((sum + 8u) >> 4);
            }
        }
    }
}

#endif /* MBX_WATER_SHADOW_BLUR_H */
