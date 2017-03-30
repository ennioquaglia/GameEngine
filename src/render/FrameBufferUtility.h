#pragma once
#include <glm/glm.hpp>



namespace Engine
{

    class Event;
    class Shader;
    class FrameBuffer
    {
        int m_Width, m_Height;
    public:
        FrameBuffer();
        virtual void resetFrameBuffer(glm::vec2 size) = 0;
        virtual void bind();
        virtual void unbind();
        virtual void handleEvent(Event &msg);

        static void DrawRectangle(float left=-1.,float up=-1.,float width=2.,float height=2.);
    };



    class FrameBufferRGBA : public FrameBuffer
    {
    protected:
        unsigned int fb, fb_tex, rb;

    public:
        int width, height;
        FrameBufferRGBA();
        virtual void resetFrameBuffer(glm::vec2 size);
        virtual unsigned int getTextureId() const;
        virtual void bind();
        virtual void unbind();
        virtual bool ok();

    };
    class FrameBufferRGB : public FrameBufferRGBA
    {
    public:
        virtual void resetFrameBuffer(glm::vec2 size);
    };

    class FrameBufferDepth : public FrameBuffer
    {

        int width, height;
        unsigned int fb, fb_tex, rb;
    public:

        FrameBufferDepth();
        virtual void resetFrameBuffer(glm::vec2 size);
        virtual unsigned int getTextureId() const;
        virtual void bind();
        virtual void unbind();
        virtual bool ok() ;

    };

    class FrameBufferMultipleTargetRendering : public FrameBuffer
    {
        enum{ NUM=5};
        //depth
        //normal
        //diffuse
        //specular

        int width, height;
        unsigned int fb, fb_tex[NUM], rb;
    public:

        FrameBufferMultipleTargetRendering();
        virtual void resetFrameBuffer(glm::vec2 size);
        virtual unsigned int getTextureId(unsigned int index=0) const;
        virtual void bind();
        virtual void unbind();
        virtual bool ok() ;
    };

    template <typename T=FrameBufferRGBA>
    class BlurKernel
    {
        static Shader *shader;
        T framebuffer[2];
    public:
        BlurKernel(glm::vec2 size=glm::vec2(0));
        void setSize(glm::vec2 size);
        unsigned int blurTexture(unsigned int TextureFrom,glm::vec2 blur);
        unsigned int getTextureId();


    };



}

#include "FrameBufferUtility.tpp"

