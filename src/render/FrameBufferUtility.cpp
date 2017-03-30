#include "FrameBufferUtility.h"
#include "Event.h"
#include "Hash.h"
#include "Shader.h"
#include "Debug.h"


namespace Engine
{



FrameBuffer::FrameBuffer() {}
void FrameBuffer::bind() {}
void FrameBuffer::unbind() {}
void FrameBuffer::handleEvent(Event &msg) {
    if(msg.getType() == StringHash("windowResize").getHash()) {
        resetFrameBuffer(msg.getData<glm::vec2>());
    }
}

void FrameBuffer::DrawRectangle(float left,float up,float width,float height){
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(left,up);
    glTexCoord2f(1,0);
    glVertex2f(left+width,up);
    glTexCoord2f(1,1);
    glVertex2f(left+width,up+height);
    glTexCoord2f(0,1);
    glVertex2f(left,up+height);
    glEnd();
}





FrameBufferRGBA::FrameBufferRGBA() {
    fb = fb_tex = rb = 0;
    width = height = 0;
}
void FrameBufferRGBA::resetFrameBuffer(glm::vec2 size) {
    QENGINE_INFO("Creating framebuffer...");
    width = size.x;
    height = size.y;

    if(!width) {
        width = 256;
        height = 256;
    }

    if(fb_tex)glDeleteTextures(1, &fb_tex);
    if(fb)glDeleteFramebuffers(1, &fb);
    if(rb)glDeleteRenderbuffers(1, &rb);
    glGenFramebuffers (1, &fb);


    glGenTextures (1, &fb_tex);
    glBindTexture (GL_TEXTURE_2D, fb_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    glBindFramebuffer (GL_FRAMEBUFFER, fb);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex, 0);

    rb = 0;
    glGenRenderbuffers (1, &rb);
    glBindRenderbuffer (GL_RENDERBUFFER, rb);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers (1, draw_bufs);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        QENGINE_ERROR("error in framebuffer(class) creation");
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        return ;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
unsigned int FrameBufferRGBA::getTextureId() const{
    return fb_tex;
}
void FrameBufferRGBA::bind() {
    glBindFramebuffer (GL_FRAMEBUFFER, fb);
}
void FrameBufferRGBA::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
bool FrameBufferRGBA::ok() {
    return fb != 0;
}


void FrameBufferRGB::resetFrameBuffer(glm::vec2 size) {
   // QENGINE_INFO("Creating framebuffer...");
    width = size.x;
    height = size.y;

    if(!width) {
        width = 256;
        height = 256;
    }

    if(fb_tex)glDeleteTextures(1, &fb_tex);
    if(fb)glDeleteFramebuffers(1, &fb);
    if(rb)glDeleteRenderbuffers(1, &rb);
    glGenFramebuffers (1, &fb);


    glGenTextures (1, &fb_tex);
    glBindTexture (GL_TEXTURE_2D, fb_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    glBindFramebuffer (GL_FRAMEBUFFER, fb);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex, 0);

    rb = 0;
    glGenRenderbuffers (1, &rb);
    glBindRenderbuffer (GL_RENDERBUFFER, rb);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers (1, draw_bufs);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        QENGINE_ERROR("error in framebuffer(class) creation");
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        return ;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



FrameBufferDepth::FrameBufferDepth() {
    fb = fb_tex = rb = 0;
    width = height = 0;
}
void FrameBufferDepth::resetFrameBuffer(glm::vec2 size) {
    QENGINE_INFO("Creating framebuffer");
    width = size.x;
    height = size.y;

    if(!width) {
        width = 256;
        height = 256;
    }

    if(fb_tex)glDeleteTextures(1, &fb_tex);
    if(fb)glDeleteFramebuffers(1, &fb);
    if(rb)glDeleteRenderbuffers(1, &rb);
    glGenFramebuffers (1, &fb);


    glGenTextures (1, &fb_tex);
    glBindTexture (GL_TEXTURE_2D, fb_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer (GL_FRAMEBUFFER, fb);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex, 0);

    rb = 0;
    glGenRenderbuffers (1, &rb);
    glBindRenderbuffer (GL_RENDERBUFFER, rb);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers (1, draw_bufs);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        QENGINE_ERROR("error in framebuffer(class) creation");
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        return ;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
unsigned int FrameBufferDepth::getTextureId() const{
    return fb_tex;
}
void FrameBufferDepth::bind() {
    glBindFramebuffer (GL_FRAMEBUFFER, fb);
    //glViewport(0, 0, width, height);
}
void FrameBufferDepth::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
bool FrameBufferDepth::ok() {
    return fb != 0;
}



FrameBufferMultipleTargetRendering::FrameBufferMultipleTargetRendering() {
    for(unsigned int i=0;i<NUM;++i)fb_tex[i]=0;
    fb = rb = 0;
    width = height = 0;
}


void FrameBufferMultipleTargetRendering::resetFrameBuffer(glm::vec2 size) {
    //QENGINE_INFO("Creating framebuffer");
    width = size.x;
    height = size.y;

    if(!width) {
        width = 256;
        height = 256;
    }

    for(unsigned int i=0;i<NUM;++i)
        if(fb_tex[i])glDeleteTextures(1, &fb_tex[i]);
    if(fb)glDeleteFramebuffers(1, &fb);
    if(rb)glDeleteRenderbuffers(1, &rb);

    glGenFramebuffers (1, &fb);
    glBindFramebuffer (GL_FRAMEBUFFER, fb);

    glGenTextures (NUM, fb_tex);

    glBindTexture (GL_TEXTURE_2D, fb_tex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED , GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fb_tex[0], 0);

    for(unsigned int i=1;i<NUM;++i){
        glBindTexture (GL_TEXTURE_2D, fb_tex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i , GL_TEXTURE_2D, fb_tex[i], 0);
    }

    rb = 0;
    glGenRenderbuffers (1, &rb);
    glBindRenderbuffer (GL_RENDERBUFFER, rb);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);


    GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
    glDrawBuffers (NUM, draw_bufs);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        QENGINE_ERROR("error in framebuffer(class) creation");
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        QENGINE_PAUSE();
        return ;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

/*

void FrameBufferMultipleTargetRendering::resetFrameBuffer(glm::vec2 size) {
    QENGINE_INFO("Creating framebuffer");
    width = size.x;
    height = size.y;

    if(!width) {
        width = 256;
        height = 256;
    }

    for(unsigned int i=0;i<NUM;++i)
        if(fb_tex[i])glDeleteTextures(1, &fb_tex[i]);
    if(fb)glDeleteFramebuffers(1, &fb);
    if(rb)glDeleteRenderbuffers(1, &rb);

    glGenFramebuffers (1, &fb);
    glBindFramebuffer (GL_FRAMEBUFFER, fb);

    glGenTextures (NUM, fb_tex);


    for(unsigned int i=0;i<NUM-1;++i){
        glBindTexture (GL_TEXTURE_2D, fb_tex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_INT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i , GL_TEXTURE_2D, fb_tex[i], 0);
    }

    glBindTexture(GL_TEXTURE_2D, fb_tex[NUM-1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb_tex[NUM-1], 0);

    GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
    glDrawBuffers (NUM-1, draw_bufs);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        QENGINE_ERROR("error in framebuffer(class) creation");
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        QENGINE_PAUSE();
        return ;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}*/
unsigned int FrameBufferMultipleTargetRendering::getTextureId(unsigned int index) const{
    return fb_tex[index%NUM];
}
void FrameBufferMultipleTargetRendering::bind() {
    glBindFramebuffer (GL_FRAMEBUFFER, fb);
    //glViewport(0, 0, width, height);
}
void FrameBufferMultipleTargetRendering::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
bool FrameBufferMultipleTargetRendering::ok() {
    return fb != 0;
}





}



