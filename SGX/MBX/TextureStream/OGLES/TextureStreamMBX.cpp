// Minimal OGLES 1.1 client for IMG_texture_stream, adapted to PVRShell/MBX.
// The SDK's bc_example kernel producer supplies a rotating test-pattern stream.
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "PVRShell.h"
#include "OGLESTools.h"

namespace
{
struct BCExampleIoctlPackage
{
    int inputparam;
    int outputparam;
};

#define BC_EXAMPLE_IOC_GID 'g'
#define BC_EXAMPLE_IOWR(index) _IOWR(BC_EXAMPLE_IOC_GID, (index), BCExampleIoctlPackage)
#define BC_EXAMPLE_IOCTL_FILL_BUFFER BC_EXAMPLE_IOWR(0)
#define BC_EXAMPLE_IOCTL_GET_BUFFER_COUNT BC_EXAMPLE_IOWR(1)

const char kBufferClassDevice[] = "/dev/bc_example";
const GLfloat kQuadVertices[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f
};
const GLfloat kQuadTexCoords[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
};
}

class TextureStreamMBX : public PVRShell
{
    CPVRTglesExt m_extensions;
    CPVRTPrint3D m_print3D;
    GLuint m_texture;
    GLint m_device;
    GLint m_streamWidth;
    GLint m_streamHeight;
    GLint m_streamFormat;
    GLint m_streamBufferCount;
    GLint m_displayBuffer;
    GLint m_captureBuffer;
    int m_bufferClassFd;
    unsigned long m_lastUpdate;
    bool m_ready;
    char m_status[256];

public:
    TextureStreamMBX() : m_texture(0), m_device(0), m_streamWidth(0),
        m_streamHeight(0), m_streamFormat(0), m_streamBufferCount(0),
        m_displayBuffer(0), m_captureBuffer(0), m_bufferClassFd(-1),
        m_lastUpdate(0), m_ready(false)
    {
        m_status[0] = '\0';
    }

    virtual bool InitApplication();
    virtual bool QuitApplication();
    virtual bool InitView();
    virtual bool ReleaseView();
    virtual bool RenderScene();

private:
    bool Fail(const char* message);
    bool FillBuffer(GLint index);
    void ResetProjection();
};

bool TextureStreamMBX::Fail(const char* message)
{
    PVRShellSet(prefExitMessage, message);
    return false;
}

bool TextureStreamMBX::InitApplication()
{
    return true;
}

bool TextureStreamMBX::QuitApplication()
{
    return true;
}

bool TextureStreamMBX::FillBuffer(GLint index)
{
    BCExampleIoctlPackage request;
    request.inputparam = index;
    request.outputparam = 0;
    if(m_bufferClassFd < 0 || ioctl(m_bufferClassFd, BC_EXAMPLE_IOCTL_FILL_BUFFER, &request) < 0)
        return false;
    return true;
}

bool TextureStreamMBX::InitView()
{
    SPVRTContext context;
    m_extensions.LoadExtensions();

    if(!CPVRTglesExt::IsGLExtensionSupported("GL_IMG_texture_stream"))
        return Fail("This driver does not expose GL_IMG_texture_stream");
    if(!m_extensions.glGetTexStreamDeviceAttribivIMG ||
       !m_extensions.glTexBindStreamIMG || !m_extensions.glGetTexStreamDeviceNameIMG)
        return Fail("IMG_texture_stream entry points are unavailable");

    GLint deviceCount = 0;
    glGetIntegerv(GL_TEXTURE_NUM_STREAM_DEVICES_IMG, &deviceCount);
    if(deviceCount < 1)
        return Fail("IMG_texture_stream is present, but the driver reports no stream devices");

    // The SDK's reference demo and buffer-class sample use stream device 0.
    m_device = 0;
    const GLubyte* deviceName = m_extensions.glGetTexStreamDeviceNameIMG(m_device);
    if(!deviceName)
        return Fail("Could not read the IMG texture-stream device name");

    m_extensions.glGetTexStreamDeviceAttribivIMG(
        m_device, GL_TEXTURE_STREAM_DEVICE_WIDTH_IMG, &m_streamWidth);
    m_extensions.glGetTexStreamDeviceAttribivIMG(
        m_device, GL_TEXTURE_STREAM_DEVICE_HEIGHT_IMG, &m_streamHeight);
    m_extensions.glGetTexStreamDeviceAttribivIMG(
        m_device, GL_TEXTURE_STREAM_DEVICE_FORMAT_IMG, &m_streamFormat);
    m_extensions.glGetTexStreamDeviceAttribivIMG(
        m_device, GL_TEXTURE_STREAM_DEVICE_NUM_BUFFERS_IMG, &m_streamBufferCount);
    if(m_streamWidth <= 0 || m_streamHeight <= 0 || m_streamBufferCount < 2)
        return Fail("Stream device reports invalid dimensions or fewer than two buffers");

    // This is the producer used by IMG's SDK texture-stream demo. It fills
    // buffer-class buffers with a moving test pattern; the GL extension then
    // samples those buffers directly as a texture.
    m_bufferClassFd = open(kBufferClassDevice, O_RDWR | O_NONBLOCK);
    if(m_bufferClassFd < 0)
        return Fail("Cannot open /dev/bc_example; load the SDK bc_example driver first");

    BCExampleIoctlPackage request;
    std::memset(&request, 0, sizeof(request));
    if(ioctl(m_bufferClassFd, BC_EXAMPLE_IOCTL_GET_BUFFER_COUNT, &request) < 0)
        return Fail("Could not query buffer count from /dev/bc_example");
    if(request.outputparam != m_streamBufferCount)
        return Fail("bc_example buffer count does not match the IMG stream device");

    for(GLint i = 0; i < m_streamBufferCount; ++i)
    {
        if(!FillBuffer(i))
            return Fail("Could not initialize a bc_example stream buffer");
    }
    m_displayBuffer = 0;
    m_captureBuffer = 1;

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_STREAM_IMG, m_texture);
    glTexParameteri(GL_TEXTURE_STREAM_IMG, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_STREAM_IMG, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const bool rotated = PVRShellGet(prefIsRotated) && PVRShellGet(prefFullScreen);
    if(m_print3D.SetTextures(&context, PVRShellGet(prefWidth),
                             PVRShellGet(prefHeight), rotated) != PVR_SUCCESS)
        return Fail("Could not initialize PVRShell Print3D");

    glViewport(0, 0, PVRShellGet(prefWidth), PVRShellGet(prefHeight));
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glClearColor(0.04f, 0.04f, 0.04f, 1.0f);
    ResetProjection();
    m_lastUpdate = PVRShellGetTime();
    m_ready = true;

    std::snprintf(m_status, sizeof(m_status), "Device: %s | %dx%d | format 0x%X | %d buffers",
        reinterpret_cast<const char*>(deviceName), m_streamWidth, m_streamHeight,
        static_cast<unsigned int>(m_streamFormat), m_streamBufferCount);
    return true;
}

void TextureStreamMBX::ResetProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

bool TextureStreamMBX::ReleaseView()
{
    m_ready = false;
    if(m_texture)
    {
        glDisable(GL_TEXTURE_STREAM_IMG);
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    m_print3D.ReleaseTextures();
    if(m_bufferClassFd >= 0)
    {
        close(m_bufferClassFd);
        m_bufferClassFd = -1;
    }
    return true;
}

bool TextureStreamMBX::RenderScene()
{
    if(!m_ready) return false;

    // Keep producer and display indices separate. The producer ioctl refuses
    // to overwrite a buffer still being read by the GPU; on that frame retain
    // the previous display buffer and retry on the next iteration.
    const unsigned long now = PVRShellGetTime();
    if(now - m_lastUpdate >= 16)
    {
        if(FillBuffer(m_captureBuffer))
        {
            m_displayBuffer = m_captureBuffer;
            m_captureBuffer = (m_captureBuffer + 1) % m_streamBufferCount;
        }
        m_lastUpdate = now;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_STREAM_IMG, m_texture);
    m_extensions.glTexBindStreamIMG(m_device, m_displayBuffer);
    glEnable(GL_TEXTURE_STREAM_IMG);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, kQuadVertices);
    glTexCoordPointer(2, GL_FLOAT, 0, kQuadTexCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_TEXTURE_STREAM_IMG);

    m_print3D.DisplayDefaultTitle("MBX Texture Stream", m_status, ePVRTPrint3DSDKLogo);
    m_print3D.Flush();
    return true;
}

PVRShell* NewDemo()
{
    return new TextureStreamMBX();
}
