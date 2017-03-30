#pragma once


#include "Object.h"
#include <glm/glm.hpp>
#include <string.h>
#include <vector>



namespace Engine{

class Shader;
class LightComponent;
class FrameBufferDepth;

class LightSystem : public System
{
    typedef std::vector<LightComponent*> LightComponents;
    LightComponents m_Components;
public:
    COMPONENT_TYPE_INFORMATION(LightSystem)

    LightSystem();
    virtual void addComponent(Object *obj);
    virtual void removeComponent(Object *obj);
    virtual void handleEvent(Event &msg);

    LightComponents getLights(){return m_Components;}
    virtual void Update(UpdateParameter &param);
    //virtual void Init();
    //virtual void Destroy() {}

    void OnScene(Scene &scene);

    static void updateUniforms(Shader *s, Scene* scene);
};


class LightComponent : public Component
{
public:
    COMPONENT_TYPE_INFORMATION(LightComponent)

    glm::mat4 depthMVP;
    unsigned int shadowMap, fb_shadow, rb;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constantAttenuation, linearAttenuation, quadraticAttenuation;
    float spotCutoff, spotExponent;
    glm::vec3 pos,dir;
    bool isDirectionalLight;


public:
    LightComponent();



    virtual void OnAttached(Entity& e);
    virtual void OnScene(Scene &scene);
    virtual void update(UpdateParameter &param);
    virtual void updateSingleUniforms(Shader *shader, unsigned int numLight);


    virtual void setDirectional(bool True = true) {
        isDirectionalLight = True;
    }

    virtual bool createFrameBuffer(const Scene *scene);
    virtual void updateFrameBuffer(Scene *scene);


    virtual void serializeXML(std::ostream &stream,unsigned level=0) const;

#if ENGINE_USE_QT_PROPERTY_BROWSER
    virtual void setupProperties(QtVariantPropertyManager *variantManager);
    virtual void updateProperties();
    virtual void updateFromProperties(std::string name,QVariant value,Scene *scene);
#endif
};



class DirectionalLightComponent : public LightComponent
{
public:
    COMPONENT_TYPE_INFORMATION(DirectionalLightComponent)
    std::vector<glm::mat4> depthMVP;
    std::vector<FrameBufferDepth> fbuffers;
    //vector<BlurKernel> blurs;
    Shader *blurShader=0;
    unsigned int frameBuffer, rb;
    int m_NumCascades;
    glm::vec4 splitDepths;

    DirectionalLightComponent();
    void setCascadesNumber(unsigned int n);
    unsigned int getCascadesNumber();
    virtual void updateSingleUniforms(Shader *shader, unsigned int numLight);
    virtual bool createFrameBuffer(const Scene *scene) ;
    virtual void updateFrameBuffer(Scene *scene) ;
    unsigned int getTextureId(int numCascade=0)const;
};
//*/
}
