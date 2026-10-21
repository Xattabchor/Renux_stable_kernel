/*
 * CPU-side two-axis wave evaluation for MBXWater.vp.
 *
 * Store the returned normalAndHeight array as vertex.texcoord[1] (four floats)
 * and update it once per frame. The VGP applies the height and transforms the
 * normal; the fixed-function texture combiner then mixes the two render targets.
 */
#ifndef MBX_WATER_WAVES_H
#define MBX_WATER_WAVES_H

#include <math.h>
#include <stddef.h>

struct MBXWaterWave
{
    float dirX;       /* normalized direction in object-space XZ */
    float dirZ;
    float amplitude;  /* same units as vertex positions */
    float wavelength; /* > 0 */
    float speed;      /* radians per second */
    float phase;
};

/* Output layout: normal.x, normal.y, normal.z, vertical displacement. */
struct MBXWaterVertexWaveData
{
    float normalAndHeight[4];
};

/*
 * xz contains count pairs: x0,z0,x1,z1,... .
 * Directions in waves[] should be normalized. Three or four gentle waves
 * usually give a better MBX result than a very dense high-frequency spectrum.
 */
static void MBXUpdateWaterWaves(const float *xz,
                                size_t count,
                                const MBXWaterWave *waves,
                                size_t waveCount,
                                float timeSeconds,
                                MBXWaterVertexWaveData *outData)
{
    const float twoPi = 6.2831853071795864769f;
    size_t vertexIndex;

    for (vertexIndex = 0; vertexIndex < count; ++vertexIndex)
    {
        const float x = xz[vertexIndex * 2 + 0];
        const float z = xz[vertexIndex * 2 + 1];
        float height = 0.0f;
        float slopeX = 0.0f;
        float slopeZ = 0.0f;
        size_t waveIndex;

        for (waveIndex = 0; waveIndex < waveCount; ++waveIndex)
        {
            const MBXWaterWave *wave = &waves[waveIndex];
            const float wavelength = (wave->wavelength > 0.0001f)
                                   ? wave->wavelength : 0.0001f;
            const float k = twoPi / wavelength;
            const float phase = k * (wave->dirX * x + wave->dirZ * z)
                              - wave->speed * timeSeconds + wave->phase;
            const float s = (float)sin((double)phase);
            const float c = (float)cos((double)phase);
            const float slope = wave->amplitude * k * c;

            height += wave->amplitude * s;
            slopeX += slope * wave->dirX;
            slopeZ += slope * wave->dirZ;
        }

        /* Normal of y = height(x,z). */
        {
            float nx = -slopeX;
            float ny = 1.0f;
            float nz = -slopeZ;
            const float length = (float)sqrt((double)(nx * nx + ny * ny + nz * nz));
            const float invLength = 1.0f / ((length > 0.0001f) ? length : 0.0001f);

            outData[vertexIndex].normalAndHeight[0] = nx * invLength;
            outData[vertexIndex].normalAndHeight[1] = ny * invLength;
            outData[vertexIndex].normalAndHeight[2] = nz * invLength;
            outData[vertexIndex].normalAndHeight[3] = height;
        }
    }
}

#endif /* MBX_WATER_WAVES_H */
