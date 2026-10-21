/* CPU projection of opaque caster triangles onto the mean water plane. */
#ifndef MBX_WATER_SHADOW_PROJECTION_H
#define MBX_WATER_SHADOW_PROJECTION_H

#include <stddef.h>

struct MBXWaterVec3
{
    float x, y, z;
};

/*
 * Project a point-light caster vertex onto y=planeY along the ray from the
 * light through the vertex. This variant assumes Y is up, with the light and
 * caster above the receiver plane. Returns 0 when the vertex cannot cast onto
 * that plane. Set epsilon to a small positive offset to avoid mask z-fighting.
 */
static int MBXProjectShadowPointLight(const MBXWaterVec3 *point,
                                      const MBXWaterVec3 *light,
                                      float planeY,
                                      float epsilon,
                                      MBXWaterVec3 *projected)
{
    float dy;
    float t;

    if (!point || !light || !projected)
        return 0;
    if (light->y <= planeY || point->y <= planeY || point->y >= light->y)
        return 0;

    dy = point->y - light->y;
    if (dy > -0.000001f && dy < 0.000001f)
        return 0;

    t = (planeY - light->y) / dy;
    if (t < 1.0f)
        return 0;

    projected->x = light->x + t * (point->x - light->x);
    projected->y = planeY + epsilon;
    projected->z = light->z + t * (point->z - light->z);
    return 1;
}

/*
 * Project along a directional light ray (rayDirection points from the light
 * toward the scene). This variant assumes Y is up and rayDirection.y < 0.
 */
static int MBXProjectShadowDirectional(const MBXWaterVec3 *point,
                                       const MBXWaterVec3 *rayDirection,
                                       float planeY,
                                       float epsilon,
                                       MBXWaterVec3 *projected)
{
    float t;

    if (!point || !rayDirection || !projected)
        return 0;
    if (point->y <= planeY || rayDirection->y >= -0.000001f)
        return 0;

    t = (planeY - point->y) / rayDirection->y;
    if (t < 0.0f)
        return 0;

    projected->x = point->x + t * rayDirection->x;
    projected->y = planeY + epsilon;
    projected->z = point->z + t * rayDirection->z;
    return 1;
}

/*
 * For every indexed caster triangle, project all three vertices and append
 * valid triangles to outTriangles. Capacity is measured in MBXWaterVec3
 * vertices (three per triangle); the returned count is also in vertices.
 * Triangles crossing the receiver plane are skipped as a whole.
 */
static size_t MBXProjectShadowTriangles(const MBXWaterVec3 *vertices,
                                        size_t vertexCount,
                                        const unsigned short *indices,
                                        size_t indexCount,
                                        const MBXWaterVec3 *lightOrDirection,
                                        int pointLight,
                                        float planeY,
                                        float epsilon,
                                        MBXWaterVec3 *outTriangles,
                                        size_t outCapacityVertices)
{
    size_t index;
    size_t outCount = 0;

    if (!vertices || !indices || !lightOrDirection || !outTriangles)
        return 0;

    for (index = 0; index + 2 < indexCount; index += 3)
    {
        MBXWaterVec3 projected[3];
        unsigned int i0 = indices[index + 0];
        unsigned int i1 = indices[index + 1];
        unsigned int i2 = indices[index + 2];
        int valid0, valid1, valid2;

        if (i0 >= vertexCount || i1 >= vertexCount || i2 >= vertexCount)
            continue;

        if (pointLight)
        {
            valid0 = MBXProjectShadowPointLight(&vertices[i0], lightOrDirection, planeY, epsilon, &projected[0]);
            valid1 = MBXProjectShadowPointLight(&vertices[i1], lightOrDirection, planeY, epsilon, &projected[1]);
            valid2 = MBXProjectShadowPointLight(&vertices[i2], lightOrDirection, planeY, epsilon, &projected[2]);
        }
        else
        {
            valid0 = MBXProjectShadowDirectional(&vertices[i0], lightOrDirection, planeY, epsilon, &projected[0]);
            valid1 = MBXProjectShadowDirectional(&vertices[i1], lightOrDirection, planeY, epsilon, &projected[1]);
            valid2 = MBXProjectShadowDirectional(&vertices[i2], lightOrDirection, planeY, epsilon, &projected[2]);
        }

        if (valid0 && valid1 && valid2)
        {
            if (outCount + 3 > outCapacityVertices)
                break;
            outTriangles[outCount++] = projected[0];
            outTriangles[outCount++] = projected[1];
            outTriangles[outCount++] = projected[2];
        }
    }

    return outCount;
}

#endif /* MBX_WATER_SHADOW_PROJECTION_H */
