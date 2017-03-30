#pragma once
#include "FrameBufferUtility.h"
#include "Event.h"
#include "Hash.h"
#include "Shader.h"
#include "Debug.h"

namespace Engine {

template <typename T>
Shader *BlurKernel<T>::shader=0;

template <typename T>
BlurKernel<T>::BlurKernel(glm::vec2 size){
    if(size!=glm::vec2(0)){
        framebuffer[0].resetFrameBuffer(size);
        framebuffer[1].resetFrameBuffer(size);
    }
    if(!shader){
        shader=new Shader("::Data/Shader/Blur/shader.vs", "::Data/Shader/Blur/shader.fs");
        Shader::Compile(shader);
    }
}

template <typename T>
void BlurKernel<T>::setSize(glm::vec2 size){
    framebuffer[0].resetFrameBuffer(size);
    framebuffer[1].resetFrameBuffer(size);
}

template <typename T>
unsigned int BlurKernel<T>::blurTexture(unsigned int TextureFrom,glm::vec2 blur){

    if(blur==glm::vec2(0)||!framebuffer[0].ok()){QENGINE_WARN("BlurKernel::blurTexture call with blur=vec2(0)");return TextureFrom;}
    framebuffer[0].bind();
    shader->Use();
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"inputTexture"),0);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"inputTexture"),0);
    glUniform1f(glGetUniformLocation(shader->getProgramId(),"BlurFactor"),blur.x);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"axis"),1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,TextureFrom);


    glViewport (0, 0, framebuffer[0].width, framebuffer[0].height);

    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,0.7,0.1,500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(-1,-1);
    glTexCoord2f(1,0);
    glVertex2f(1,-1);
    glTexCoord2f(1,1);
    glVertex2f(1,1);
    glTexCoord2f(0,1);
    glVertex2f(-1,1);
    glEnd();

    framebuffer[0].unbind();
    return framebuffer[0].getTextureId();

    glUniform1f(glGetUniformLocation(shader->getProgramId(),"BlurFactor"),blur.y);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"axis"),0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,framebuffer[0].getTextureId());


    glViewport (0, 0, framebuffer[1].width, framebuffer[1].height);
    framebuffer[1].bind();
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(-1,-1);
    glTexCoord2f(1,0);
    glVertex2f(1,-1);
    glTexCoord2f(1,1);
    glVertex2f(1,1);
    glTexCoord2f(0,1);
    glVertex2f(-1,1);
    glEnd();
    framebuffer[1].unbind();
    return framebuffer[1].getTextureId();

}
template <typename T>
unsigned int BlurKernel<T>::getTextureId(){return framebuffer[0].getTextureId();}


}
