#include "PostProcessEffect.h"
#include "FrameBufferUtility.h"
#include "FrameBufferUtility.tpp"
#include "Debug.h"
#include "Scene.h"
#include "Context.h"
#include "Event.h"
#include "Texture.h"
#include "LightComponent.h"

#include <random>

namespace Engine{


PostProcessor::PostProcessor(Scene *s):m_Scene(s){}
void PostProcessor::Render(){
    m_Scene->Draw();
}
void PostProcessor::handleEvent(Event &msg) {

}




DeferredPostProcessor::DeferredPostProcessor(Scene *s):PostProcessor(s){
    framebuffer.resetFrameBuffer(glm::vec2(s->getContext()->getWidth(),s->getContext()->getHeight()));//s->window->Width,s->window->Height));

    shader = new Shader("::Data/Shader/Debug/shader.vs", "::Data/Shader/Debug/shader.fs");
    Shader::Compile(shader);
}
void DeferredPostProcessor::Render(){
    framebuffer.bind();

    float ratio;

    float Width=m_Scene->getContext()->getWidth(), Height=m_Scene->getContext()->getHeight();
    ratio = Width / (float) Height;

    glViewport(0, 0, Width,Height);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,ratio,0.1,500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Scene->Draw(RenderFlag::Default);
    framebuffer.unbind();



    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->Use();
    glEnable(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"tex"),0);
    //glUniform1i(glGetUniformLocation(MatrixShader->getProgramId(),"char"),1);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D,framebuffer.getTextureId());
    //glActiveTexture (GL_TEXTURE1);
    //glBindTexture (GL_TEXTURE_2D,charSetTex->getID());

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
    glUseProgram(0);

}
void DeferredPostProcessor::handleEvent(Event &msg) {
    if(msg.getType()==StringHash("windowResize").getHash()){
        framebuffer.resetFrameBuffer(msg.getData<glm::vec2>());
    }
}



MatrixModePostProcessor::MatrixModePostProcessor(Scene *s):PostProcessor(s){
    framebuffer.resetFrameBuffer(glm::vec2(s->getContext()->getWidth(),s->getContext()->getHeight()));//s->window->Width,s->window->Height));

    shader = new Shader("::Data/Shader/MatrixMode/shader.vs", "::Data/Shader/MatrixMode/shader.fs");
    Shader::Compile(shader);

    charTex=Texture::load("::Data/Texture/char_sorted.bmp");
}
void MatrixModePostProcessor::Render(){
    framebuffer.bind();

    float Width=m_Scene->getContext()->getWidth(), Height=m_Scene->getContext()->getHeight();

    glViewport(0, 0, Width,Height);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(&(m_Scene->getCamera().getProjectionMatrix())[0][0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Scene->Draw(RenderFlag::Default);
    framebuffer.unbind();



    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->Use();
    glEnable(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"tex"),0);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"charTexture"),1);
    glUniform2f(glGetUniformLocation(shader->getProgramId(),"pixel"),Width/8.0f,Height/8.0f);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D,framebuffer.getTextureId());
    glActiveTexture (GL_TEXTURE1);
    glBindTexture (GL_TEXTURE_2D,charTex->getID());

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
    glUseProgram(0);

}
void MatrixModePostProcessor::handleEvent(Event &msg) {
    if(msg.getType()==StringHash("windowResize").getHash()){
        framebuffer.resetFrameBuffer(msg.getData<glm::vec2>());
    }
}



RealDeferredPostProcessor::RealDeferredPostProcessor(Scene *s):PostProcessor(s){
    glm::vec2 size=glm::vec2(s->getContext()->getWidth(),s->getContext()->getHeight());
    m_GBuffer.resetFrameBuffer(size);//s->window->Width,s->window->Height));
    m_SSAO_Framebuffer.resetFrameBuffer(size);

    //	blurKernel.setSize(glm::vec2(m_Scene->getContext()->getWidth(),m_Scene->getContext()->getHeight()));

    //shader = new Shader("::Data/Shader/Debug/shader.vs", "::Data/Shader/Debug/shader.fs");
    shader = new Shader("::Data/Shader/Deferred/shader.vs", "::Data/Shader/Deferred/shader.fs");
    Shader::Compile(shader);

    m_SSAO_Shader= new Shader("::Data/Shader/SSAO/shader.vs", "::Data/Shader/SSAO/shader.fs");
    Shader::Compile(m_SSAO_Shader);

    m_RnmTexture=Texture::load("::Data/Texture/noise.png");


    // Sample kernel
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;

        // Scale samples s.t. they're more aligned to center of kernel
        scale = glm::mix(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }


}
void RealDeferredPostProcessor::Render(){
    float Width=m_Scene->getContext()->getWidth(), Height=m_Scene->getContext()->getHeight();

    m_GBuffer.bind();

    glViewport(0, 0, Width,Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(&(m_Scene->getCamera().getProjectionMatrix())[0][0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Scene->Draw(RenderFlag::Deferred);

    m_GBuffer.unbind();



   /* static BlurKernel<FrameBufferRGBA> blur(glm::vec2(Width,Height));
    glDisable(GL_DEPTH_TEST);
    unsigned int tex= blur.blurTexture(framebuffer.getTextureId(),glm::vec2(0.01));
    glEnable(GL_DEPTH_TEST);

*/

    m_SSAO_Framebuffer.bind();
    glViewport(0, 0, Width,Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(&(m_Scene->getCamera().getProjectionMatrix())[0][0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_SSAO_Shader->Use();
    m_SSAO_Shader->setUniform("depthMap",0);
    m_SSAO_Shader->setUniform("normalMap",1);
    m_SSAO_Shader->setUniform("rnm",2);
    m_SSAO_Shader->setUniform("PM",1,0,m_Scene->getCamera().getProjectionMatrix());
    m_SSAO_Shader->setUniform("CM",1,0,m_Scene->getCamera().getMatrix());

    for (GLuint i = 0; i < 64; ++i)
         glUniform3fv(glGetUniformLocation(m_SSAO_Shader->getProgramId(), ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);


    for(int i=0;i<2;++i){
        glActiveTexture (GL_TEXTURE0+i);
        glBindTexture (GL_TEXTURE_2D,m_GBuffer.getTextureId(i));
    }
    glActiveTexture (GL_TEXTURE2);
    glBindTexture (GL_TEXTURE_2D,m_RnmTexture->getID());


    FrameBuffer::DrawRectangle();


    int SSAO_tex = m_SSAO_Framebuffer.getTextureId();//m_BlurKernel.blurTexture(m_SSAO_Framebuffer.getTextureId(),1000.f/glm::vec2(Width,Height));

    glBindFramebuffer (GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Width,Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(&(m_Scene->getCamera().getProjectionMatrix())[0][0]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->Use();
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"depth"),0);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"normal"),1);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"diffuse"),2);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"specular"),3);
    glUniform1i(glGetUniformLocation(shader->getProgramId(),"SSAO_map"),4);

    shader->setUniform("PM",1,0,m_Scene->getCamera().getProjectionMatrix());
    shader->setUniform("CM",1,0,m_Scene->getCamera().getMatrix());
    //shader->setUniform("parallax",param.m_Scene->options.useParalax);

    LightSystem::updateUniforms(shader,m_Scene);

    for(int i=0;i<4;++i){
        glActiveTexture (GL_TEXTURE0+i);
        glBindTexture (GL_TEXTURE_2D,m_GBuffer.getTextureId(i));
    }
    glActiveTexture (GL_TEXTURE4);
    glBindTexture (GL_TEXTURE_2D,SSAO_tex);

    //glUniform1i(glGetUniformLocation(MatrixShader->getProgramId(),"char"),1);
    //glActiveTexture (GL_TEXTURE0);
    //glBindTexture (GL_TEXTURE_2D,tex);
    //glBindTexture (GL_TEXTURE_2D,framebuffer.getTextureId(0));
    //glActiveTexture (GL_TEXTURE1);
    //glBindTexture (GL_TEXTURE_2D,charSetTex->getID());


    FrameBuffer::DrawRectangle();
    glUseProgram(0);

}
void RealDeferredPostProcessor::handleEvent(Event &msg) {
    if(msg.getType()==Hash("windowResize")){
        glm::vec2 size=msg.getData<glm::vec2>();
        m_GBuffer.resetFrameBuffer(size);
        m_SSAO_Framebuffer.resetFrameBuffer(size);
        m_BlurKernel.setSize(size);
    }
}




}
