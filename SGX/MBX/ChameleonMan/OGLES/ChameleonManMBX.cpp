// Chameleon Man: PowerVR MBX / OpenGL ES 1.1 port.
// Skinning uses GL_OES_matrix_palette. Tangent-space light vectors are updated
// per vertex on the CPU, then GL_OES_texture_env_dot3 performs per-pixel DOT3.
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>

#include "PVRShell.h"
#include "OGLESTools.h"

namespace
{
const char kScene[] = "ChameleonScene.pod";
const char* const kTextures[] = {
    "FinalChameleonManHeadBody.pvr", "FinalChameleonManLegs.pvr",
    "Tang_space_BodyMap.pvr", "Tang_space_LegsMap.pvr",
    "Tang_space_BeltMap.pvr", "skyline.pvr", "Wall_diffuse_baked.pvr",
    "lamp.pvr", "ChameleonBelt.pvr"
};

enum TextureId
{
    TEX_BODY, TEX_LEGS, TEX_BODY_NORMAL, TEX_LEGS_NORMAL, TEX_BELT_NORMAL,
    TEX_SKY, TEX_WALL, TEX_LAMP, TEX_BELT, TEX_COUNT
};

enum MeshNode
{
    NODE_BODY = 0, NODE_LEGS = 1, NODE_BELT = 2,
    NODE_WALL = 3, NODE_BACKGROUND = 4, NODE_LIGHTS = 5
};

struct SkinVertex
{
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat uv[2];
    GLubyte matrixIndex[4];
    GLfloat weight[4];
    GLubyte color[4];
    GLuint sourceVertex;
};

struct SkinBatch
{
    std::vector<SkinVertex> vertices;
    std::vector<GLuint> boneNodes;
    unsigned int influenceCount;
};

static const GLfloat kNearPlane = 100.0f;
static const GLfloat kFarPlane = 1000.0f;
static const GLfloat kPi = 3.14159265358979323846f;

inline float Clamp(float v, float lo, float hi)
{
    return std::max(lo, std::min(hi, v));
}

inline float Dot(const GLfloat a[3], const GLfloat b[3])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline void Normalize(GLfloat v[3])
{
    const float length = std::sqrt(Dot(v, v));
    if(length > 1.0e-8f)
    {
        const float invLength = 1.0f / length;
        v[0] *= invLength;
        v[1] *= invLength;
        v[2] *= invLength;
    }
    else
    {
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 1.0f;
    }
}

inline void TransformPoint(const PVRTMat4& m, const GLfloat in[3], GLfloat out[3])
{
    out[0] = m.f[0] * in[0] + m.f[4] * in[1] + m.f[8]  * in[2] + m.f[12];
    out[1] = m.f[1] * in[0] + m.f[5] * in[1] + m.f[9]  * in[2] + m.f[13];
    out[2] = m.f[2] * in[0] + m.f[6] * in[1] + m.f[10] * in[2] + m.f[14];
}

inline void TransformVector(const PVRTMat4& m, const GLfloat in[3], GLfloat out[3])
{
    out[0] = m.f[0] * in[0] + m.f[4] * in[1] + m.f[8]  * in[2];
    out[1] = m.f[1] * in[0] + m.f[5] * in[1] + m.f[9]  * in[2];
    out[2] = m.f[2] * in[0] + m.f[6] * in[1] + m.f[10] * in[2];
}

inline void ReadVec3(const CPODData& data, unsigned int vertex, GLfloat out[3])
{
    const VERTTYPE* p = reinterpret_cast<const VERTTYPE*>(
        static_cast<const unsigned char*>(data.pData) + vertex * data.nStride);
    out[0] = vt2f(p[0]);
    out[1] = vt2f(p[1]);
    out[2] = vt2f(p[2]);
}

inline void ReadUV(const CPODData& data, unsigned int vertex, GLfloat out[2])
{
    const VERTTYPE* p = reinterpret_cast<const VERTTYPE*>(
        static_cast<const unsigned char*>(data.pData) + vertex * data.nStride);
    out[0] = vt2f(p[0]);
    out[1] = vt2f(p[1]);
}

inline GLubyte ToByte(float v)
{
    return static_cast<GLubyte>(Clamp(v * 255.0f + 0.5f, 0.0f, 255.0f));
}

static bool HasExtension(const char* extension)
{
    const char* all = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    if(!all || !extension || std::strchr(extension, ' ')) return false;
    const size_t length = std::strlen(extension);
    const char* current = all;
    while((current = std::strstr(current, extension)) != 0)
    {
        const bool leftBoundary = current == all || current[-1] == ' ';
        const bool rightBoundary = current[length] == '\0' || current[length] == ' ';
        if(leftBoundary && rightBoundary) return true;
        current += length;
    }
    return false;
}

static void SetBasicTextureEnv()
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}
}

class ChameleonManMBX : public PVRShell
{
    CPVRTModelPOD m_scene;
    CPVRTPrint3D m_print3D;
    CPVRTglesExt m_extensions;
    GLuint m_textures[TEX_COUNT];
    std::vector<SkinBatch> m_skinBatches[3];
    PVRTMat4 m_view;
    PVRTMat4 m_projection;
    float m_frame;
    float m_lightPhase;
    float m_wallPhase;
    float m_backgroundOffset;
    int m_previousTime;
    bool m_dot3Enabled;
    bool m_hasDot3;
    bool m_hasTextureCombine;
    GLint m_maxPaletteMatrices;

public:
    ChameleonManMBX() : m_frame(0.0f), m_lightPhase(0.0f), m_wallPhase(0.0f),
        m_backgroundOffset(0.0f), m_previousTime(0), m_dot3Enabled(true),
        m_hasDot3(false), m_hasTextureCombine(false), m_maxPaletteMatrices(0)
    {
        std::memset(m_textures, 0, sizeof(m_textures));
    }

    virtual bool InitApplication();
    virtual bool QuitApplication();
    virtual bool InitView();
    virtual bool ReleaseView();
    virtual bool RenderScene();

private:
    bool LoadTextures(CPVRTString* error);
    bool BuildSkinBatches(CPVRTString* error);
    bool BuildNodeBatches(unsigned int nodeIndex, CPVRTString* error);
    void CameraGetMatrix();
    void SetPalette(unsigned int nodeIndex, unsigned int batchIndex,
                    std::vector<PVRTMat4>& worldBones);
    void UpdateSkinColors(unsigned int nodeIndex, unsigned int batchIndex,
                          const std::vector<PVRTMat4>& worldBones,
                          const PVRTVec3& lightPosition);
    void DrawSkinnedNode(unsigned int nodeIndex, const PVRTVec3& lightPosition);
    void DrawStaticNodes();
    void SetupDot3(GLuint normalTexture, GLuint diffuseTexture);
    void SetupDiffuse(GLuint diffuseTexture);
};

bool ChameleonManMBX::InitApplication()
{
    CPVRTResourceFile::SetReadPath(const_cast<char*>(static_cast<const char*>(PVRShellGet(prefReadPath))));
    if(m_scene.ReadFromFile(kScene) != PVR_SUCCESS)
    {
        PVRShellSet(prefExitMessage, "Chameleon Man: could not load ChameleonScene.pod");
        return false;
    }
    return true;
}

bool ChameleonManMBX::QuitApplication()
{
    m_scene.Destroy();
    return true;
}

bool ChameleonManMBX::LoadTextures(CPVRTString* error)
{
    for(unsigned int i = 0; i < TEX_COUNT; ++i)
    {
        if(PVRTTextureLoadFromPVR(kTextures[i], &m_textures[i]) != PVR_SUCCESS)
        {
            *error = "Chameleon Man: failed to load texture ";
            *error += kTextures[i];
            return false;
        }
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    return true;
}

bool ChameleonManMBX::BuildNodeBatches(unsigned int nodeIndex, CPVRTString* error)
{
    if(nodeIndex >= m_scene.nNumNode)
    {
        *error = "Chameleon Man: scene is missing one of the expected character mesh nodes";
        return false;
    }

    SPODNode& node = m_scene.pNode[nodeIndex];
    SPODMesh& mesh = m_scene.pMesh[node.nIdx];
    if(!mesh.sBoneWeight.pData || !mesh.sBoneIdx.pData ||
       !mesh.sTangents.pData || !mesh.sBinormals.pData || !mesh.sNormals.pData ||
       !mesh.sFaces.pData || mesh.nNumStrips != 0 || mesh.sBoneBatches.nBatchCnt == 0)
    {
        *error = "Chameleon Man: expected triangle-list skinning, tangents and bone batches in POD";
        return false;
    }
    if(mesh.sBoneIdx.n == 0 || mesh.sBoneIdx.n > 4 ||
       mesh.sBoneWeight.n != mesh.sBoneIdx.n || mesh.psUVW[0].pData == 0)
    {
        *error = "Chameleon Man: unsupported skin influence count or missing texture coordinates";
        return false;
    }

    std::vector<SkinBatch>& batches = m_skinBatches[nodeIndex];
    batches.resize(static_cast<unsigned int>(mesh.sBoneBatches.nBatchCnt));
    const unsigned short* indices = reinterpret_cast<const unsigned short*>(mesh.sFaces.pData);

    for(unsigned int batchIndex = 0;
        batchIndex < static_cast<unsigned int>(mesh.sBoneBatches.nBatchCnt); ++batchIndex)
    {
        SkinBatch& batch = batches[batchIndex];
        batch.influenceCount = mesh.sBoneIdx.n;
        const unsigned int boneCount = mesh.sBoneBatches.pnBatchBoneCnt[batchIndex];
        if(boneCount == 0 || boneCount > static_cast<unsigned int>(mesh.sBoneBatches.nBatchBoneMax) ||
           boneCount > static_cast<unsigned int>(m_maxPaletteMatrices))
        {
            *error = "Chameleon Man: malformed bone batch in POD";
            return false;
        }
        batch.boneNodes.reserve(boneCount);
        for(unsigned int bone = 0; bone < boneCount; ++bone)
        {
            batch.boneNodes.push_back(static_cast<GLuint>(mesh.sBoneBatches.pnBatches[
                batchIndex * mesh.sBoneBatches.nBatchBoneMax + bone]));
        }

        const unsigned int firstFace = static_cast<unsigned int>(
            mesh.sBoneBatches.pnBatchOffset[batchIndex]);
        const unsigned int lastFace = batchIndex + 1 < static_cast<unsigned int>(mesh.sBoneBatches.nBatchCnt)
            ? static_cast<unsigned int>(mesh.sBoneBatches.pnBatchOffset[batchIndex + 1])
            : static_cast<unsigned int>(mesh.nNumFaces);
        if(lastFace < firstFace || lastFace > static_cast<unsigned int>(mesh.nNumFaces))
        {
            *error = "Chameleon Man: invalid triangle range in POD bone batch";
            return false;
        }
        batch.vertices.resize((lastFace - firstFace) * 3);

        for(unsigned int corner = 0; corner < batch.vertices.size(); ++corner)
        {
            const unsigned int vertexIndex = indices[firstFace * 3 + corner];
            if(vertexIndex >= mesh.nNumVertex)
            {
                *error = "Chameleon Man: invalid vertex index in POD";
                return false;
            }
            SkinVertex& out = batch.vertices[corner];
            ReadVec3(mesh.sVertex, vertexIndex, out.position);
            ReadVec3(mesh.sNormals, vertexIndex, out.normal);
            ReadUV(mesh.psUVW[0], vertexIndex, out.uv);
            out.sourceVertex = vertexIndex;
            std::memset(out.matrixIndex, 0, sizeof(out.matrixIndex));
            std::memset(out.weight, 0, sizeof(out.weight));
            const unsigned char* boneIndices = static_cast<const unsigned char*>(mesh.sBoneIdx.pData)
                + vertexIndex * mesh.sBoneIdx.nStride;
            const VERTTYPE* weights = reinterpret_cast<const VERTTYPE*>(
                static_cast<const unsigned char*>(mesh.sBoneWeight.pData)
                + vertexIndex * mesh.sBoneWeight.nStride);
            float weightSum = 0.0f;
            for(unsigned int influence = 0; influence < mesh.sBoneIdx.n; ++influence)
            {
                if(boneIndices[influence] >= boneCount)
                {
                    *error = "Chameleon Man: vertex references a bone outside its batch palette";
                    return false;
                }
                out.matrixIndex[influence] = boneIndices[influence];
                out.weight[influence] = vt2f(weights[influence]);
                weightSum += out.weight[influence];
            }
            if(weightSum <= 1.0e-6f)
            {
                *error = "Chameleon Man: zero total skin weight in POD";
                return false;
            }
            // Normalise small exporter rounding errors once when building the
            // expanded batch geometry.
            for(unsigned int influence = 0; influence < mesh.sBoneIdx.n; ++influence)
                out.weight[influence] /= weightSum;
            out.color[0] = out.color[1] = out.color[2] = 255;
            out.color[3] = 255;
        }
    }
    return true;
}

bool ChameleonManMBX::BuildSkinBatches(CPVRTString* error)
{
    for(unsigned int node = 0; node < 3; ++node)
        if(!BuildNodeBatches(node, error)) return false;
    return true;
}

bool ChameleonManMBX::InitView()
{
    CPVRTString error;
    SPVRTContext context;
    const bool rotated = PVRShellGet(prefIsRotated) && PVRShellGet(prefFullScreen);

    m_extensions.LoadExtensions();
    if(!CPVRTglesExt::IsGLExtensionSupported("GL_OES_matrix_palette"))
    {
        PVRShellSet(prefExitMessage, "Chameleon Man requires GL_OES_matrix_palette (MBX supports this extension)");
        return false;
    }
    glGetIntegerv(GL_MAX_PALETTE_MATRICES_OES, &m_maxPaletteMatrices);
    if(m_maxPaletteMatrices < 1)
    {
        PVRShellSet(prefExitMessage, "Chameleon Man: matrix palette reports no usable matrices");
        return false;
    }
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    const bool coreCombine = version && std::strstr(version, "1.1") != 0;
    const bool coreDot3 = version && std::strstr(version, "1.1") != 0;
    m_hasDot3 = coreDot3 || HasExtension("GL_OES_texture_env_dot3") ||
                HasExtension("GL_IMG_texture_env_enhanced_fixed_function");
    m_hasTextureCombine = coreCombine || HasExtension("GL_ARB_texture_env_combine") ||
                          HasExtension("GL_OES_texture_env_combine");
    if(!m_hasDot3) m_dot3Enabled = false;

    if(m_print3D.SetTextures(&context, PVRShellGet(prefWidth), PVRShellGet(prefHeight), rotated) != PVR_SUCCESS)
    {
        PVRShellSet(prefExitMessage, "Chameleon Man: failed to initialize Print3D");
        return false;
    }
    if(!LoadTextures(&error) || !BuildSkinBatches(&error))
    {
        m_print3D.ReleaseTextures();
        PVRShellSet(prefExitMessage, error.c_str());
        return false;
    }

    glViewport(0, 0, PVRShellGet(prefWidth), PVRShellGet(prefHeight));
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.02f, 0.02f, 0.04f, 1.0f);
    m_previousTime = static_cast<int>(PVRShellGetTime());
    return true;
}

bool ChameleonManMBX::ReleaseView()
{
    m_print3D.ReleaseTextures();
    glDeleteTextures(TEX_COUNT, m_textures);
    std::memset(m_textures, 0, sizeof(m_textures));
    return true;
}

void ChameleonManMBX::CameraGetMatrix()
{
    PVRTVec3 from, to, up(0.0f, 1.0f, 0.0f);
    float fov;
    const bool rotated = PVRShellGet(prefIsRotated) && PVRShellGet(prefFullScreen);
    if(m_scene.pCamera[0].nIdxTarget != -1)
        fov = m_scene.GetCameraPos(from, to, 0);
    else
        fov = m_scene.GetCamera(from, to, up, 0);
    fov *= rotated
        ? static_cast<float>(PVRShellGet(prefWidth)) / static_cast<float>(PVRShellGet(prefHeight))
        : static_cast<float>(PVRShellGet(prefHeight)) / static_cast<float>(PVRShellGet(prefWidth));
    m_view = PVRTMat4::LookAtRH(from, to, up);
    m_projection = PVRTMat4::PerspectiveFovRH(fov,
        static_cast<float>(PVRShellGet(prefWidth)) / static_cast<float>(PVRShellGet(prefHeight)),
        kNearPlane, kFarPlane, PVRTMat4::OGL, rotated);
}

void ChameleonManMBX::SetupDot3(GLuint normalTexture, GLuint diffuseTexture)
{
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    if(m_hasTextureCombine)
    {
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGB);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PRIMARY_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
    }
    else
    {
        // Legacy MBX fixed-function DOT3 path provided by IMG's enhanced-FF
        // extension; the incoming primary color is the second DOT3 operand.
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DOT3_RGBA);
    }

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    if(m_hasTextureCombine)
    {
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
    }
    else
    {
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    glActiveTexture(GL_TEXTURE0);
}

void ChameleonManMBX::SetupDiffuse(GLuint diffuseTexture)
{
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    SetBasicTextureEnv();
}

void ChameleonManMBX::SetPalette(unsigned int nodeIndex, unsigned int batchIndex,
                                  std::vector<PVRTMat4>& worldBones)
{
    SPODNode& node = m_scene.pNode[nodeIndex];
    SPODMesh& mesh = m_scene.pMesh[node.nIdx];
    SkinBatch& batch = m_skinBatches[nodeIndex][batchIndex];
    worldBones.resize(batch.boneNodes.size());

    glMatrixMode(GL_MATRIX_PALETTE_OES);
    for(unsigned int bone = 0; bone < batch.boneNodes.size(); ++bone)
    {
        m_extensions.glCurrentPaletteMatrixOES(bone);
        m_scene.GetBoneWorldMatrix(worldBones[bone], node, m_scene.pNode[batch.boneNodes[bone]]);
        PVRTMat4 viewBone = m_view * worldBones[bone];
        glLoadMatrixf(viewBone.f);
    }
    glMatrixMode(GL_MODELVIEW);
    (void)mesh;
}

void ChameleonManMBX::UpdateSkinColors(unsigned int nodeIndex, unsigned int batchIndex,
                                        const std::vector<PVRTMat4>& worldBones,
                                        const PVRTVec3& lightPosition)
{
    SPODNode& node = m_scene.pNode[nodeIndex];
    SPODMesh& mesh = m_scene.pMesh[node.nIdx];
    SkinBatch& batch = m_skinBatches[nodeIndex][batchIndex];
    const GLfloat light[3] = { lightPosition.x, lightPosition.y, lightPosition.z };

    for(unsigned int i = 0; i < batch.vertices.size(); ++i)
    {
        SkinVertex& v = batch.vertices[i];
        GLfloat sourcePosition[3], sourceNormal[3], sourceTangent[3], sourceBinormal[3];
        ReadVec3(mesh.sVertex, v.sourceVertex, sourcePosition);
        ReadVec3(mesh.sNormals, v.sourceVertex, sourceNormal);
        ReadVec3(mesh.sTangents, v.sourceVertex, sourceTangent);
        ReadVec3(mesh.sBinormals, v.sourceVertex, sourceBinormal);
        GLfloat p[3] = {0, 0, 0}, n[3] = {0, 0, 0}, t[3] = {0, 0, 0}, b[3] = {0, 0, 0};

        for(unsigned int influence = 0; influence < batch.influenceCount; ++influence)
        {
            const float weight = v.weight[influence];
            const unsigned int paletteIndex = v.matrixIndex[influence];
            if(weight <= 0.0f || paletteIndex >= worldBones.size()) continue;
            GLfloat transformed[3];
            TransformPoint(worldBones[paletteIndex], sourcePosition, transformed);
            for(unsigned int axis = 0; axis < 3; ++axis) p[axis] += transformed[axis] * weight;
            TransformVector(worldBones[paletteIndex], sourceNormal, transformed);
            for(unsigned int axis = 0; axis < 3; ++axis) n[axis] += transformed[axis] * weight;
            TransformVector(worldBones[paletteIndex], sourceTangent, transformed);
            for(unsigned int axis = 0; axis < 3; ++axis) t[axis] += transformed[axis] * weight;
            TransformVector(worldBones[paletteIndex], sourceBinormal, transformed);
            for(unsigned int axis = 0; axis < 3; ++axis) b[axis] += transformed[axis] * weight;
        }
        Normalize(n);
        // Re-orthogonalize the tangent frame after blending several bones.
        const float nDotT = Dot(n, t);
        for(unsigned int axis = 0; axis < 3; ++axis) t[axis] -= n[axis] * nDotT;
        Normalize(t);
        Normalize(b);
        GLfloat toLight[3] = { light[0] - p[0], light[1] - p[1], light[2] - p[2] };
        Normalize(toLight);

        if(m_dot3Enabled)
        {
            // The original Chameleon normal maps store y in red and x in green;
            // the fixed-function DOT3 input must therefore be G,R,B = T,B,N.
            const float tangentX = Clamp(Dot(t, toLight), -1.0f, 1.0f);
            const float tangentY = Clamp(Dot(b, toLight), -1.0f, 1.0f);
            const float tangentZ = Clamp(Dot(n, toLight), -1.0f, 1.0f);
            v.color[0] = ToByte(0.5f + 0.5f * tangentY);
            v.color[1] = ToByte(0.5f + 0.5f * tangentX);
            v.color[2] = ToByte(0.5f + 0.5f * tangentZ);
        }
        else
        {
            const float diffuse = Clamp(Dot(n, toLight), 0.0f, 1.0f);
            v.color[0] = v.color[1] = v.color[2] = ToByte(diffuse);
        }
        v.color[3] = 255;
    }
}

void ChameleonManMBX::DrawSkinnedNode(unsigned int nodeIndex, const PVRTVec3& lightPosition)
{
    SPODNode& node = m_scene.pNode[nodeIndex];
    SPODMesh& mesh = m_scene.pMesh[node.nIdx];
    std::vector<PVRTMat4> worldBones;

    glEnable(GL_MATRIX_PALETTE_OES);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_MATRIX_INDEX_ARRAY_OES);
    glEnableClientState(GL_WEIGHT_ARRAY_OES);

    for(unsigned int batchIndex = 0; batchIndex < m_skinBatches[nodeIndex].size(); ++batchIndex)
    {
        SkinBatch& batch = m_skinBatches[nodeIndex][batchIndex];
        if(batch.vertices.empty()) continue;
        SetPalette(nodeIndex, batchIndex, worldBones);
        UpdateSkinColors(nodeIndex, batchIndex, worldBones, lightPosition);

        PVRTMat4 world;
        m_scene.GetWorldMatrix(world, node);
        PVRTMat4 modelView = m_view * world;
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(modelView.f);

        const GLsizei stride = sizeof(SkinVertex);
        const SkinVertex* data = &batch.vertices[0];
        glVertexPointer(3, GL_FLOAT, stride, data->position);
        glNormalPointer(GL_FLOAT, stride, data->normal);
        glClientActiveTexture(GL_TEXTURE0);
        glTexCoordPointer(2, GL_FLOAT, stride, data->uv);
        glClientActiveTexture(GL_TEXTURE1);
        glTexCoordPointer(2, GL_FLOAT, stride, data->uv);
        glClientActiveTexture(GL_TEXTURE0);
        glColorPointer(4, GL_UNSIGNED_BYTE, stride, data->color);
        m_extensions.glMatrixIndexPointerOES(mesh.sBoneIdx.n, GL_UNSIGNED_BYTE,
                                             stride, data->matrixIndex);
        m_extensions.glWeightPointerOES(mesh.sBoneWeight.n, GL_FLOAT,
                                        stride, data->weight);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(batch.vertices.size()));
    }

    glDisableClientState(GL_MATRIX_INDEX_ARRAY_OES);
    glDisableClientState(GL_WEIGHT_ARRAY_OES);
    glDisable(GL_MATRIX_PALETTE_OES);
}

void ChameleonManMBX::DrawStaticNodes()
{
    for(unsigned int nodeIndex = 3; nodeIndex < m_scene.nNumMeshNode; ++nodeIndex)
    {
        SPODNode& node = m_scene.pNode[nodeIndex];
        SPODMesh& mesh = m_scene.pMesh[node.nIdx];
        PVRTMat4 world;
        m_scene.GetWorldMatrix(world, node);
        GLuint texture = m_textures[TEX_WALL];

        if(nodeIndex == NODE_WALL)
        {
            texture = m_textures[TEX_WALL];
            world *= PVRTMat4::RotationY(m_wallPhase);
        }
        else if(nodeIndex == NODE_BACKGROUND)
        {
            texture = m_textures[TEX_SKY];
        }
        else if(nodeIndex == NODE_LIGHTS)
        {
            texture = m_textures[TEX_LAMP];
            PVRTMat4 wallWorld;
            m_scene.GetWorldMatrix(wallWorld, m_scene.pNode[NODE_WALL]);
            world = wallWorld * PVRTMat4::RotationY(m_wallPhase) * wallWorld.inverse() * world;
        }

        glMatrixMode(GL_MODELVIEW);
        PVRTMat4 modelView = m_view * world;
        glLoadMatrixf(modelView.f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        SetBasicTextureEnv();
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(mesh.sVertex.n, VERTTYPEENUM, mesh.sVertex.nStride, mesh.sVertex.pData);
        glTexCoordPointer(mesh.psUVW[0].n, VERTTYPEENUM,
                          mesh.psUVW[0].nStride, mesh.psUVW[0].pData);
        if(nodeIndex == NODE_BACKGROUND)
        {
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glTranslatef(m_backgroundOffset, 0.0f, 0.0f);
            glMatrixMode(GL_MODELVIEW);
        }
        else
        {
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
        }
        glDrawElements(GL_TRIANGLES, mesh.nNumFaces * 3, GL_UNSIGNED_SHORT, mesh.sFaces.pData);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

bool ChameleonManMBX::RenderScene()
{
    const int currentTime = static_cast<int>(PVRShellGetTime());
    const int delta = currentTime - m_previousTime;
    m_previousTime = currentTime;

    if(PVRShellIsKeyPressed(PVRShellKeyNameACTION1) && m_hasDot3)
        m_dot3Enabled = !m_dot3Enabled;

    const float elapsedMs = static_cast<float>(std::max(0, delta));
    m_frame += elapsedMs * 0.02f;
    m_lightPhase += elapsedMs * 0.0034f;
    m_wallPhase += elapsedMs * 0.00027f;
    m_backgroundOffset += elapsedMs * -0.000027f;
    if(m_lightPhase >= 2.0f * kPi) m_lightPhase -= 2.0f * kPi;
    if(m_wallPhase >= 2.0f * kPi) m_wallPhase -= 2.0f * kPi;
    m_backgroundOffset -= std::floor(m_backgroundOffset);
    if(m_scene.nNumFrame > 0 && m_frame >= m_scene.nNumFrame) m_frame = 0.0f;
    m_scene.SetFrame(m_frame);
    CameraGetMatrix();

    PVRTVec3 lightPosition(200.0f, 350.0f,
        200.0f * static_cast<float>(std::fabs(std::sin(kPi / 4.0f + m_lightPhase))));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m_projection.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m_view.f);
    for(unsigned int node = 0; node < 3; ++node)
    {
        if(m_dot3Enabled)
        {
            const GLuint normalTexture = m_textures[node == NODE_BODY ? TEX_BODY_NORMAL :
                (node == NODE_LEGS ? TEX_LEGS_NORMAL : TEX_BELT_NORMAL)];
            const GLuint diffuseTexture = m_textures[node == NODE_BODY ? TEX_BODY :
                (node == NODE_LEGS ? TEX_LEGS : TEX_BELT)];
            SetupDot3(normalTexture, diffuseTexture);
        }
        else
        {
            SetupDiffuse(m_textures[node == NODE_BODY ? TEX_BODY :
                (node == NODE_LEGS ? TEX_LEGS : TEX_BELT)]);
        }
        DrawSkinnedNode(node, lightPosition);
    }

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glClientActiveTexture(GL_TEXTURE1);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    DrawStaticNodes();

    m_print3D.DisplayDefaultTitle("Chameleon Man",
        m_dot3Enabled ? "OGLES 1.1 / MBX: matrix-palette skinning + DOT3" :
                        "OGLES 1.1 / MBX: matrix-palette skinning + vertex light",
        ePVRTPrint3DSDKLogo);
    m_print3D.Flush();
    return true;
}

PVRShell* NewDemo()
{
    return new ChameleonManMBX();
}
