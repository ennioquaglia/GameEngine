#pragma once


#include "FrameBufferUtility.h"

namespace Engine{

class Scene;
class Shader;
class Texture;


class PostProcessor
{
protected:
    Scene *m_Scene;
public:
    PostProcessor(Scene *s);
    virtual void Render();
    virtual void handleEvent(Event &msg);

};


class DeferredPostProcessor : public PostProcessor
{
    FrameBufferRGBA framebuffer;
    Shader *shader;
public:
    DeferredPostProcessor(Scene *s);
    virtual void Render();
    virtual void handleEvent(Event &msg);

};


class MatrixModePostProcessor : public PostProcessor
{
    FrameBufferRGBA framebuffer;
    Shader *shader;
    Texture *charTex;
public:
    MatrixModePostProcessor(Scene *s);
    virtual void Render();
    virtual void handleEvent(Event &msg);

};



class RealDeferredPostProcessor : public PostProcessor
{

    FrameBufferMultipleTargetRendering m_GBuffer;
    FrameBufferRGB m_SSAO_Framebuffer;
    Shader *shader,*m_SSAO_Shader=0;
    BlurKernel<> m_BlurKernel;
    Texture *m_RnmTexture=0;
    std::vector<glm::vec3> ssaoKernel;
public:
    RealDeferredPostProcessor(Scene *s);
    virtual void Render();
    virtual void handleEvent(Event &msg);

};



}
