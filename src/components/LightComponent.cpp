#include "Debug.h"
#include "Object.h"
#include "Scene.h"
#include "Hash.h"
#include "TransformComponent.h"
#include "Shader.h"

#include "core/Object.h"
#include "LightComponent.h"
#include "FrameBufferUtility.h"

#include <ostream>

#if ENGINE_USE_QT_WIDGET
#include <QOpenGLFramebufferObject>
#endif


#if ENGINE_USE_QT_PROPERTY_BROWSER
#include <QtCore/QVariant>
#include <QtCore/QString>
#include "src/qtpropertymanager.h"
#include "src/qtvariantproperty.h"
#include "src/qttreepropertybrowser.h"
#include <QColor>
#include <QVariant>
#endif


#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>


namespace Engine{

Shader DirectionalLightComponent::*blurShader =0;

LightSystem::LightSystem():System()
{
    QENGINE_INFO("LightSystem created! "+getType()+"  "+getTypeStatic());
    Init();
}

void LightSystem::addComponent(Object * obj)
{
    if (!obj)return;
    if (!(obj->getFlag() & LightComponent::light))return;
    LightComponent *l = (LightComponent*)obj;

    m_Components.push_back(l);
    obj->attachListener(this);
    QENGINE_INFO("Added a LightComponent!");
}

void LightSystem::removeComponent(Object *obj){
    QENGINE_DEBUG("LightSystem::removeComponent");
    if (!obj)return;

    LightComponent *c = (LightComponent*)obj;

    auto it=std::find(m_Components.begin(),m_Components.end(),obj);
    if(it!=m_Components.end()){
        m_Components.erase(it);
        QENGINE_DEBUG("Removed a light! ");
    }
}

void LightSystem::handleEvent(Event &msg){
    QENGINE_DEBUG("LightComponent:: received msg");
    if(msg.getType()==EventType::destroy){

        removeComponent(msg.getData<Object*>());
    }else QENGINE_DEBUG("LightComponent:: not a destroy msg("+msg.getType()+" - "+(int)EventType::destroy+")");
}


void LightSystem::OnScene(Scene &scene) {
    LightSystem* system=scene.getObjectByType<LightSystem>();
    if(!system){
        QENGINE_ERROR("LightSystem::updateUniforms  lightSystem not found!");
        return;
    }
}


void LightSystem::Update(UpdateParameter &param){
    //static unsigned int a=0;
    //if( !(++a%2)) //param.deltaTime &&
    for(LightComponent *l :m_Components){
        l->update(param);
    }
}


void LightSystem::updateUniforms(Shader *s,  Scene *scene){

    LightSystem* system=scene->getObjectByType<LightSystem>();
    if(!system){
        //QENGINE_ERROR("LightSystem::updateUniforms  lightSystem not found!");
        return;
    }



    int textureCount = 0;
    int numDirLights = 0;
    int p=s->getProgramId();




    for(std::vector<LightComponent*>::iterator it = system->m_Components.begin(); it != system->m_Components.end() && textureCount < 10; it++) {
        //for(int i=0; i<lights.size()&&textureCount<10; i++) {
        LightComponent *l = *it; //lights[i];
        if(l->getFlag() & Object::directional_light) {
            DirectionalLightComponent *d = (DirectionalLightComponent*)l;

            for(int c = 0; c < d->m_NumCascades; c++) {
                if(d->getTextureId(c) != 0) {

                    int textureId = textureCount + 8;
                    glActiveTexture(GL_TEXTURE0 + textureId);
                    glBindTexture(GL_TEXTURE_2D, d->getTextureId(c));

                    textureCount++;
                }
            }
        } else if(l->isDirectionalLight || l->spotCutoff <= 90) {
            if(l->shadowMap != 0) {
                int textureId = textureCount + 8;
                glActiveTexture(GL_TEXTURE0 + textureId);
                glBindTexture(GL_TEXTURE_2D, l->shadowMap);
                textureCount++;
            }
        }

    }

    unsigned int num=0;
    for(LightComponent *l : system->m_Components){
        l->updateSingleUniforms(s,num++);
    }

    int i = num;
    s->setUniform("numLights",num);
    s->setUniform("EyeWorldPos",scene->getCamera().getPos());
    s->setUniform("useVSM",scene->options.useVSM);


    textureCount = 0;
    for(std::vector<LightComponent*>::iterator it = system->m_Components.begin(); it != system->m_Components.end() && textureCount < 10; it++) {
        //for(int i=0; i<lights.size()&&textureCount<10; i++) {
        LightComponent *l = *it; //lights[i];

        if(l->getFlag() & Object::directional_light) {
            DirectionalLightComponent *d = (DirectionalLightComponent*)l;
            for(int c = 0; c < d->m_NumCascades; c++) {
                if(d->getTextureId(c) != 0) {
                    int loc = 23;
                    char s[40] = {0};

                    sprintf(s, "shadowMaps[%d]", textureCount);
                    loc = glGetUniformLocation(p, s);
                    if(loc > -1) {
                        int textureId = textureCount + 8;
                        glUniform1i(loc, textureId);
                    }//else QENGINE_ERROR("dir maps");
                    sprintf(s, "dirLights[%d].shadowMapID[%d]", numDirLights, c);
                    loc = glGetUniformLocation( p , s);
                    if(loc > -1) {
                        glUniform1i(loc, textureCount);
                    }//else QENGINE_ERROR("dir id");
                    sprintf(s, "dirLights[%d].DepthMVP[%d]", numDirLights, c);
                    loc = glGetUniformLocation(p, s);
                    if(loc > -1) {
                        glUniformMatrix4fv(loc, 1, GL_FALSE, &d->depthMVP[c][0][0]);
                    }//else QENGINE_ERROR("dir MVP");
                    textureCount++;
                }
            }
            ++numDirLights;
        } else if(l->isDirectionalLight || l->spotCutoff <= 90) {
            if(l->shadowMap != 0) {
                int loc = 23;
                char s[35] = {0};

                sprintf(s, "shadowMaps[%d]", textureCount);
                loc = glGetUniformLocation(p, s);
                if(loc > -1) {
                    int textureId = textureCount + 8;
                    glUniform1i(loc, textureId);
                }
                sprintf(s, "lights[%d].shadowMapID", i);
                loc = glGetUniformLocation(p, s);
                if(loc > -1) {
                    glUniform1i(loc, textureCount);
                }
                sprintf(s, "lights[%d].DepthMVP", i);
                loc = glGetUniformLocation(p, s);
                if(loc > -1) {
                    glUniformMatrix4fv(loc, 1, GL_FALSE, &l->depthMVP[0][0]);
                }
                textureCount++;
            } else QENGINE_ERROR("shadow map non inizializzata\n");
        }
    }
    glActiveTexture(GL_TEXTURE0);
}



LightComponent::LightComponent()
{

    shadowMap = 0;
    diffuse = glm::vec4(1);
    specular = glm::vec4(1);
    constantAttenuation = 0;
    linearAttenuation = 0.2;
    quadraticAttenuation = 0;
    spotCutoff = 180;
    spotExponent = 0;
    dir = glm::vec3(0);
    isDirectionalLight = 0;
    depthMVP = glm::mat4(1);

    m_Flag.add(LightComponent::light);
    //m_Flag.add(StringEnum("light"));

}







void LightComponent::OnAttached(Entity &e)
{
    if(!e.getObjectByType<TransformComponent>()){
        QENGINE_WARN("LightComponent::OnAttached entity requires TransformComponent ->added");
        e.addComponent(new TransformComponent());
    }
}

void LightComponent::OnScene(Scene &scene){
    Object* system = scene.getObjectByType<LightSystem>();
    if (system)system->addComponent(this);

    createFrameBuffer(&scene);
}

void LightComponent::update(UpdateParameter &param) {

    updateFrameBuffer(const_cast<Scene*>(param.scene));

    TransformComponent* trans=getEntity()->getObjectByType<TransformComponent>();
    if(trans)pos=trans->getLocalPosition(Object::light,1);
}

void LightComponent::updateSingleUniforms(Shader *shader, unsigned int numLight) {

    char s[35] = {0};
    int loc = 0;
    unsigned int l = numLight, p = shader->getProgramId();;
    sprintf(s, "lights[%d].position", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        float w = 1;
        if(isDirectionalLight)w = 0;
        glUniform4f(loc, pos[0], pos[1], pos[2], w);
    }

    sprintf(s, "lights[%d].diffuse", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform4fv(loc, 1, &diffuse[0]);
    }
    sprintf(s, "lights[%d].specular", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform4fv(loc, 1, &specular[0]);
    }
    sprintf(s, "lights[%d].constantAttenuation", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform1f(loc, constantAttenuation);
    }
    sprintf(s, "lights[%d].linearAttenuation", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform1f(loc, linearAttenuation);
    }
    sprintf(s, "lights[%d].quadraticAttenuation", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform1f(loc, quadraticAttenuation);
    }
    sprintf(s, "lights[%d].spotCutoff", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform1f(loc, spotCutoff);
    }
    sprintf(s, "lights[%d].spotExponent", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform1f(loc, spotExponent);
    }
    sprintf(s, "lights[%d].dir", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform3fv(loc, 1, &dir[0]);
    }
    sprintf(s, "lights[%d].DepthMVP", l);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniformMatrix4fv(loc, 1, 0, &depthMVP[0][0]);
    }
}

bool LightComponent::createFrameBuffer(const Scene *scene) {

    if(!scene)return 0;

    if(shadowMap)glDeleteTextures(1, &shadowMap);
    if(!(isDirectionalLight || spotCutoff <= 90))return 0;

    if(!scene->options.isShadowEnable)return 0;
    //QENGINE_INFO("creazione framebuffer");

    int shadow_size = (int)std::pow(2.0f, scene->options.shadowLOD);
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &fb_shadow);
    glBindFramebuffer(GL_FRAMEBUFFER, fb_shadow);


    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &shadowMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,shadow_size, shadow_size, 0,GL_RGB,GL_FLOAT,0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLfloat BorderColor[4] = {1, 1, 1, 0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

    //	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,shadowMap, 0);

    //glDrawBuffer(GL_NONE); // No color buffer is drawn to.
    //GLenum draw_bufs[] = { GL_NONE };
    GLenum draw_bufs[] = { GL_NONE };

    glDrawBuffers(1, draw_bufs);
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        QENGINE_ERROR("Light framebuffer error(normal light)");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDebug("light framebuffer");
    return 1;
}

void LightComponent::updateFrameBuffer(Scene* scene) {

    //    if(!scene)return;

    //    if(!scene->options.isShadowEnable || shadowMap <= 0)return;
    //    if(isDirectionalLight || spotCutoff < 90);
    //    else return;

    //    QENGINE_INFO("update framebuffer");
    //    Entity* e=getEntity();
    //    if(!e)return;
    //    TransformComponent* trans;
    //    trans = e->getObjectByType<TransformComponent>();
    //    if(!trans)return;

    //    QENGINE_INFO("update framebuffer 1");
    //    glm::vec3 pos = trans->getPosition();


    //    int shadow_resolution = (int)std::pow(2.0f, scene->options.shadowLOD);

    //    glm::mat4 depthViewMatrix;
    //    glm::mat4 depthProjectionMatrix;

    //    float shadowSize = 200;

    //    if(isDirectionalLight) {

    //        Camera cam=scene->getCamera();
    //        cam.getDir();

    //        glm::vec3 camPos = cam.getPos();
    //        //vec3 texelSize = vec3(1.0f)/vec3(shadow_resolution);
    //        //camPos = floor(camPos / texelSize) * texelSize;

    //        glm::vec3 center = glm::vec3(0);
    //        float distFromCentroid = 500;

    //        for(int i = 0; i < 8; i++)center += cam.corners[i];
    //        center /= 8.0f;

    //        glm::vec3 texelSize = glm::vec3(1.0f) / glm::vec3(shadow_resolution);
    //        center = floor(center / texelSize) * texelSize;



    //        glm::vec3 light_pos = center + dir * distFromCentroid ;


    //        //setPos(camPos+dir*200.0f);


    //        glm::vec3 light_target = center;
    //        glm::mat4 lightSpace = glm::lookAt(light_pos, light_target, glm::vec3(0, 0, -1));


    //        glm::vec3 max = scene->getCamera().corners[0], min = max;
    //        glm::vec3 cornersLightSpace[8];

    //        for(int i = 0; i < 8; i++) {
    //            cornersLightSpace[i] = glm::vec3(lightSpace * glm::vec4(scene->getCamera().corners[i], 1.0f));
    //            if(cornersLightSpace[i].x > max.x)
    //                max.x = cornersLightSpace[i].x;
    //            else if(cornersLightSpace[i].x < min.x)
    //                min.x = cornersLightSpace[i].x;
    //            if(cornersLightSpace[i].y > max.y)
    //                max.y = cornersLightSpace[i].y;
    //            else if(cornersLightSpace[i].y < min.y)
    //                min.y = cornersLightSpace[i].y;
    //            if(cornersLightSpace[i].z > max.z)
    //                max.z = cornersLightSpace[i].z;
    //            else if(cornersLightSpace[i].z < min.z)
    //                min.z = cornersLightSpace[i].z;

    //        }



    //        //center=scene->camera.Pos+scene->camera.Dir*200.0f;
    //        //cout<<"cam far clip="<<max.z<<" "<<min.z<<"\n";


    //        depthViewMatrix = glm::lookAt(light_pos, light_target, glm::vec3(0, 0, -1));
    //        depthProjectionMatrix = glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);
    //        //depthProjectionMatrix = glm::ortho(-shadowSize,shadowSize,-shadowSize,shadowSize,1.0f,1000.0f);
    //        //vec3 camPos = scene->camera.Pos;
    //        ////vec3 texelSize = vec3(1.0f)/vec3(shadow_resolution);
    //        ////camPos = floor(camPos / texelSize) * texelSize;

    //        //glm::vec3 light_pos =camPos+pos*500.0f ;//  +   pos;//scene->camera.Pos  + ldir*shadowSize/4.0f   +   pos;
    //        //glm::vec3 light_target=camPos;//  + ldir*shadowSize/10.0f;
    //        ////glm::vec3 light_pos =scene->camera.Pos+pos;
    //        ////glm::vec3 light_target=scene->camera.Pos;
    //        //glm::vec3 up_dir (0.0f, 0.0f, -1.0f);
    //        //depthViewMatrix = glm::lookAt (light_pos, light_target, up_dir);
    //        //glm::mat4 depthProjectionMatrix = glm::ortho(-shadowSize,shadowSize,-shadowSize,shadowSize,1.0f,1000.0f);//glm::frustum(-1.0f,1.0f,-1.0f,1.0f,1.0f,100.0f);//glm::perspective (fov, aspect, Near, Far);

    //        depthMVP = depthProjectionMatrix * depthViewMatrix;
    //    }
    //    if(spotCutoff <= 90) {
    //        //cout<<"spot..\n";
    //        depthViewMatrix = glm::lookAt (pos, pos + dir, glm::vec3(0, 1, 0));
    //        depthProjectionMatrix = glm::perspective(spotCutoff * 2, 1.0f, 1.0f, 100.0f); //glm::frustum(-1.0f,1.0f,-1.0f,1.0f,1.0f,100.0f);//glm::perspective (fov, aspect, Near, Far);
    //        depthMVP = depthProjectionMatrix * depthViewMatrix;
    //    }
    //    // create a projection matrix for the shadow caster



    //    // Send our transformation to the currently bound shader,
    //    // in the "MVP" uniform

    //    //scene->dephtShader->Use();
    //    //scene->depthMVP=depthMVP;

    //    //Camera cam;
    //    //cam.projection=depthProjectionMatrix;
    //    //cam.setMatrix(depthViewMatrix);
    //    //cam.mode=Camera::Matrix;
    //    //cam.UpdatePlanes();

    //    //scene->renderFlags=Scene::Shadow;
    //    //scene->ShadowCameras.clear();
    //    //scene->ShadowCameras.push_back(cam);

    //    //glUniformMatrix4fv(glGetUniformLocation(scene->dephtShader->getProgramId(),"depthMVP"), 1, GL_FALSE, &depthMVP[0][0]);


    //    Camera old = scene->getCamera();
    //    scene->getCamera().setMode(Camera::Matrix);
    //    scene->getCamera().setMatrix(glm::mat4(1));
    //    scene->getCamera().setProjectionMatrix(depthMVP); //mat4(1);



    //    glBindFramebuffer(GL_FRAMEBUFFER, fb_shadow);
    //    // set the viewport to the size of the shadow map
    //    glViewport(0, 0, shadow_resolution, shadow_resolution);
    //    // clear the shadow map to black (or white)
    //    glClearColor(0.0, 0.0, 0.0, 0.0);
    //    // no need to clear the colour buffer
    //    glClear(GL_DEPTH_BUFFER_BIT);

    //    //glEnable(GL_CULL_FACE);
    //    //glCullFace(GL_BACK);

    //    scene->Draw(RenderFlag::Shadow);

    //    // bind the default framebuffer again
    //    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //    //glDisable(GL_CULL_FACE);
    //    glActiveTexture(0);


    //    scene->setCamera(old);
    //    glDebug("light framebuffer");

}


DirectionalLightComponent::DirectionalLightComponent(): LightComponent() {

    m_NumCascades = 4;
    depthMVP.resize(m_NumCascades);
    fbuffers.resize(m_NumCascades+1);
    if(!blurShader){
        blurShader=new Shader("::Data/Shader/Blur/shader.vs", "::Data/Shader/Blur/shader.fs");
        Shader::Compile(blurShader);
    }
    m_Flag.add(Object::directional_light);
}
void DirectionalLightComponent::setCascadesNumber(unsigned int n){m_NumCascades=n;}
unsigned int DirectionalLightComponent::getCascadesNumber(){return m_NumCascades;}
void DirectionalLightComponent::updateSingleUniforms(Shader *shader, unsigned int numLight){

    char s[35] = {0};
    int loc = 0;
    //unsigned int l = numLight;
    unsigned int p=shader->getProgramId();
    sprintf(s, "dirLights[%d].dir", 0);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        float w = 1;
        if(isDirectionalLight)w = 0;
        glUniform4f(loc, dir[0], dir[1], dir[2], w);
    }

    sprintf(s, "dirLights[%d].diffuse", 0);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform4fv(loc, 1, &diffuse[0]);
    }
    sprintf(s, "dirLights[%d].specular", 0);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform4fv(loc, 1, &specular[0]);
    }

    sprintf(s, "dirLights[%d].split", 0);
    loc = glGetUniformLocation(p, s);
    if(loc > -1) {
        glUniform4fv(loc, 1, &splitDepths[0]);
    }

}
bool DirectionalLightComponent::createFrameBuffer(const Scene *scene) {

    for(unsigned int i = 0; i < fbuffers.size(); i++) {
        if(!scene->options.isShadowEnable)return 0;
        int shadow_size = (int)std::pow(2.0f, scene->options.shadowLOD);
        fbuffers[i].resetFrameBuffer(glm::vec2(shadow_size));
    }
    glBindFramebuffer (GL_FRAMEBUFFER, 0);
    glDebug("directional light framebuffer");
    return 1;
}
void DirectionalLightComponent::updateFrameBuffer(Scene *scene) {

    glDebug();
    if(!scene->options.isShadowEnable)return;
    int shadow_resolution = (int)std::pow(2.0f, scene->options.shadowLOD);

    //float level[4] = {0.05, 0.1, 0.3, 1};
    float level[4] = {0.03, 0.08, 0.2, 1};

    glm::mat4 depthViewMatrix;
    glm::mat4 depthProjectionMatrix;
    //float shadowSize = 200;
    //glm::vec3 camPos = scene->getCamera().getPos();

    //setPos(camPos + dir * 200.0f);
    //scene->getCamera().UpdatePlanes();

    Camera old = scene->getCamera();
    old.UpdatePlanes();

    scene->m_CurrentCamera->setMode(Camera::Matrix);
    scene->m_CurrentCamera->setMatrix(glm::mat4(1));

    glDebug();

    for(int c = 0; c < m_NumCascades; c++) {

        //glm::vec3 texelSize = glm::vec3(1.0f)/glm::vec3(shadow_resolution);
        //camPos = glm::floor(camPos / texelSize) * texelSize;

        glm::vec3 center = glm::vec3(0);
        float distFromCentroid = 500;


        glm::vec3 cornersCSM[8];
        for(int i = 0; i < 8; i++)cornersCSM[i] = old.corners[i];

        for(int i = 4; i < 8; i++)cornersCSM[i] = mix(old.corners[i - 4], old.corners[i], level[c]);

        if(c)
            for(int i = 0; i < 4; i++)cornersCSM[i] = mix(old.corners[i], old.corners[i + 4], level[c - 1]);


        center = glm::vec3(0);

        for(int i = 0; i < 8; i++)center += cornersCSM[i];
        center /= 8.0f;

        //glm::vec3 texelSize =glm:: vec3(1.0f)/glm::vec3(shadow_resolution);
        //center = glm::floor(center / texelSize) * texelSize;

        glm::vec3 light_pos = center + dir * distFromCentroid ;
        //light_pos= glm::floor(light_pos / texelSize) * texelSize;

        glm::vec3 light_target = center;
        glm::mat4 lightSpace = glm::lookAt (light_pos, light_target, glm::vec3(0, 0, -1));


        glm::vec3 max = glm::vec3(lightSpace * glm::vec4(cornersCSM[0], 1.0f)), min = max;
        //glm::vec3 max = cornersCSM[0], min = max;
        //glm::vec3 max = glm::vec3(-99999999999.0f), min = glm::vec3(99999999999.0f);


        glm::vec3 cornersLightSpace[8];

        for(int i = 0; i < 8; i++) {
            cornersLightSpace[i] = glm::vec3(lightSpace * glm::vec4(cornersCSM[i], 1.0f));
            if (cornersLightSpace[i].x > max.x)
                max.x = cornersLightSpace[i].x;
            else if (cornersLightSpace[i].x < min.x)
                min.x = cornersLightSpace[i].x;
            if (cornersLightSpace[i].y > max.y)
                max.y = cornersLightSpace[i].y;
            else if (cornersLightSpace[i].y < min.y)
                min.y = cornersLightSpace[i].y;
            if (cornersLightSpace[i].z > max.z)
                max.z = cornersLightSpace[i].z;
            else if (cornersLightSpace[i].z < min.z)
                min.z = cornersLightSpace[i].z;

        }
        min-=glm::vec3(20);
        max+=glm::vec3(20);

        depthViewMatrix = glm::lookAt (light_pos, light_target, glm::vec3(0, 0, -1));
        depthProjectionMatrix = glm::ortho(min.x, max.x, min.y, max.y, -max.z, -min.z);


        depthMVP[c] = depthProjectionMatrix * depthViewMatrix;


        //glEnable(GL_CULL_FACE);

        glDebug();


        //glViewport (0, 0, shadow_resolution, shadow_resolution);

        fbuffers[scene->options.useVSM ? 0 :(c+2)%fbuffers.size() ].bind();
        //fbuffers[0].bind();
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glViewport (0, 0, shadow_resolution, shadow_resolution);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        //glOrtho(0.0, 256.0, 0.0, 256.0, -1.0, 1.0);
        glOrtho(0.0, shadow_resolution, 0.0, shadow_resolution, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();


        scene->m_CurrentCamera->setProjectionMatrix( depthMVP[c] ); //mat4(1);


        glDebug();
        scene->Draw(RenderFlag::Shadow);
        glDebug();

        glDisable(GL_CULL_FACE);

        if(!scene->options.useVSM)continue;

        fbuffers[1].bind();
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        blurShader->Use();





        glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"inputTexture"),0);
        glUniform1f(glGetUniformLocation(blurShader->getProgramId(),"BlurFactor"),scene->options.useVSM ? 0.0023 : 0.0f);
        glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"axis"),1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,fbuffers[0].getTextureId());

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


        fbuffers[(c+2)%fbuffers.size()].bind();
        glClearColor (0.0, 0.0, 0.0, 0.0);
        glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUniform1i(glGetUniformLocation(blurShader->getProgramId(),"axis"),0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,fbuffers[1].getTextureId());


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



    }
    glDebug();


    // bind the default framebuffer again
#if ENGINE_USE_QT_WIDGET
    QOpenGLFramebufferObject::bindDefault();
#else
    glBindFramebuffer (GL_FRAMEBUFFER, 0);
#endif


    *(scene->m_CurrentCamera)=old;
    //scene->setCamera( old );
    glUseProgram(0);
    glDebug("directional light framebuffer");

}

unsigned int DirectionalLightComponent::getTextureId(int numCascade)const{

    return fbuffers[(numCascade+2)%fbuffers.size()].getTextureId();
}






void LightComponent::serializeXML(std::ostream &stream,unsigned level) const{
    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"<"
         <<getTypeName()<<" "
        <<SERIALIZE_VAR(diffuse)
       <<SERIALIZE_VAR(specular)
      <<SERIALIZE_VAR(constantAttenuation)
     <<SERIALIZE_VAR(constantAttenuation)
    <<SERIALIZE_VAR(linearAttenuation)
    <<SERIALIZE_VAR(quadraticAttenuation)
    <<SERIALIZE_VAR(spotCutoff)
    <<SERIALIZE_VAR(spotExponent)
    <<SERIALIZE_VAR(dir)
    <<" >\n";

    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"</"<<getTypeName()<<">\n";

}

#if ENGINE_USE_QT_PROPERTY_BROWSER

void LightComponent::setupProperties(QtVariantPropertyManager *variantManager){
    m_rootProperty=variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"LightComponent");

    QtVariantProperty *prop;
//    QtVariantProperty *prop=variantManager->addProperty(QVariant::String,"name");
//    m_Properties.push_back(prop);
//    m_rootProperty->addSubProperty(prop);

    prop=variantManager->addProperty(QVariant::Color,"diffuse");
    m_Properties.push_back(prop);
    m_rootProperty->addSubProperty(prop);


    prop=variantManager->addProperty(QVariant::Double,"constantAttenuation");
    m_Properties.push_back(prop);
    m_rootProperty->addSubProperty(prop);


}

void LightComponent::updateProperties(){
    for(QtVariantProperty *prop:m_Properties){
        if(!prop)continue;
        std::string name=prop->propertyName().toStdString();

        if(name=="constantAttenuation")prop->setValue(constantAttenuation);
        if(name=="diffuse")prop->setValue(QColor(diffuse.r*255,diffuse.g*255,diffuse.b*255));
//        if(name=="x")prop->setValue(getPosition().x);
//        if(name=="y")prop->setValue(getPosition().y);
//        if(name=="z")prop->setValue(getPosition().z);


//        if(name=="x ")prop->setValue(getScale().x);
//        if(name=="y ")prop->setValue(getScale().y);
//        if(name=="z ")prop->setValue(getScale().z);

//        glm::vec3 angle=glm::eulerAngles(getRotation());
//        if(name=="yaw")prop->setValue(angle.x);
//        if(name=="pitch")prop->setValue(angle.y);
//        if(name=="roll")prop->setValue(angle.z);

    }
}


void LightComponent::updateFromProperties(std::string name,QVariant value,Scene *scene){

    if(name=="constantAttenuation")constantAttenuation=value.toFloat();
    if(name=="diffuse"){
        auto c=value.value<QColor>();
        diffuse=glm::vec4(c.redF(),c.greenF(),c.blueF(),1);
    }

////    if(name=="name")setName(value.toString().toStdString(),scene);
//    if(name=="x")setPosition(glm::vec3(value.toFloat(),getPosition().y,getPosition().z));
//    if(name=="y")setPosition(glm::vec3(getPosition().x,value.toFloat(),getPosition().z));
//    if(name=="z")setPosition(glm::vec3(getPosition().x,getPosition().y,value.toFloat()));


//    if(name=="x ")setScale(glm::vec3(value.toFloat(),getScale().y,getScale().z));
//    if(name=="y ")setScale(glm::vec3(getScale().x,value.toFloat(),getScale().z));
//    if(name=="z ")setScale(glm::vec3(getScale().x,getScale().y,value.toFloat()));

//    glm::vec3 angle=glm::eulerAngles(getRotation());
//    if(name=="yaw"){angle.x=value.toFloat();setRotation(glm::quat(angle));}
//    if(name=="pitch"){angle.y=value.toFloat();setRotation(glm::quat(angle));}
//    if(name=="roll"){angle.z=value.toFloat();setRotation(glm::quat(angle));}

}

#endif



}
