#define GLEW_STATIC
#include <GL/glew.h>

#include "TransformComponent.h"
#include "Hash.h"
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>

#include "EString.h"
#include <ostream>
#include "Debug.h"


#if ENGINE_USE_QT_PROPERTY_BROWSER
#include <QtCore/QVariant>
#include <QtCore/QString>
#include "src/qtpropertymanager.h"
#include "src/qtvariantproperty.h"
#include "src/qttreepropertybrowser.h"
#endif




namespace Engine {
using namespace glm;


TransformComponent::TransformComponent(){
    m_scale=glm::vec3(1);
}

TransformComponent::~TransformComponent()
{
}

glm::mat4 TransformComponent::TransformData::getMatrix(){
    return glm::translate(glm::mat4(1),m_origin)*glm::mat4_cast(m_rotation)*glm::scale(glm::mat4(1),m_scale);
}


glm::mat4 Engine::TransformComponent::getLocalMatrix(const unsigned int id) {
    if (localMatrices.find(id) == localMatrices.end())localMatrices[id] = TransformData();
    return localMatrices[id].getMatrix();
}

void Engine::TransformComponent::setLocalMatrix(const unsigned int id, glm::mat4 matrix) {
    //localMatrices[id] = matrix;

}

glm::vec3 Engine::TransformComponent::getLocalScale(const unsigned int id, bool global) {
    glm::mat4 m;
    if (global)m = glm::scale(glm::mat4(1), m_scale)*getLocalMatrix(id);
    else m = getLocalMatrix(id);
    return glm::vec3(m[0][0], m[1][1], m[2][2]);
}

glm::vec3 Engine::TransformComponent::getLocalPosition(const unsigned int id, bool global) {
    glm::mat4 m;
    if (global)m = getMatrix(id);
    else m = getLocalMatrix(id);

    return glm::vec3(m[3][0], m[3][1], m[3][2]);
}

void Engine::TransformComponent::setLocalPosition(const unsigned int id, glm::vec3 v) {
    if (localMatrices.find(id) == localMatrices.end())localMatrices[id] = TransformData();
    localMatrices[id].m_origin=v;
}

void Engine::TransformComponent::setLocalScale(const unsigned int id, glm::vec3 v) {
    if (localMatrices.find(id) == localMatrices.end())localMatrices[id] = TransformData();
    localMatrices[id].m_scale=v;
}


glm::quat TransformComponent::getLocalRotation(const unsigned int id, bool global ){
    if (localMatrices.find(id) == localMatrices.end())localMatrices[id] = TransformData();
    if(global) return m_rotation*localMatrices[id].m_rotation;
    return localMatrices[id].m_rotation;
}

void TransformComponent::setLocalRotation(const unsigned int id, glm::quat q){
    if (localMatrices.find(id) == localMatrices.end())localMatrices[id] = TransformData();
    localMatrices[id].m_rotation=q;
}

void TransformComponent::MultMatrix() {
    glMultMatrixf(&(getMatrix())[0][0]);
}

inline void TransformComponent::setScale(const glm::vec3 v) { m_scale = v; }
inline void TransformComponent::setPosition(const glm::vec3 v){m_origin=v;}
inline void TransformComponent::setRotation(const glm::quat r) { m_rotation = r; }

inline glm::vec3 TransformComponent::getPosition()const{return m_origin;}
inline glm::vec3 TransformComponent::getScale()const{return m_scale;}
inline glm::quat TransformComponent::getRotation()const{return m_rotation;}

inline glm::mat4 TransformComponent::getMatrix(const unsigned int id){
    return glm::translate(glm::mat4(1),m_origin)*glm::mat4_cast(m_rotation)*glm::scale(glm::mat4(1),m_scale)*getLocalMatrix(id);//todo
}
void TransformComponent::setMatrix(glm::mat4 m, bool use_scale){
    m_origin=glm::vec3(m*glm::vec4(0,0,0,1));

    m_rotation=normalize(glm::quat_cast(m));

}



void TransformComponent::serializeXML(std::ostream &stream,unsigned level) const{
    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"<"
         <<getTypeName()<<" "
        <<SERIALIZE_VAR(m_origin)
       <<SERIALIZE_VAR(m_scale)
      <<SERIALIZE_VAR(m_rotation)
     <<" >\n";

    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"</"<<getTypeName()<<">\n";
}

#if ENGINE_USE_QT_PROPERTY_BROWSER
void TransformComponent::setupProperties(QtVariantPropertyManager *variantManager){
    m_rootProperty=variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"TransformComponent");

    QtVariantProperty *prop;
//    QtVariantProperty *prop=variantManager->addProperty(QVariant::String,"name");
//    m_Properties.push_back(prop);
//    m_rootProperty->addSubProperty(prop);

    QtVariantProperty *Pos=variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"Position");
    m_Properties.push_back(Pos);
    m_rootProperty->addSubProperty(Pos);

    {
        prop=variantManager->addProperty(QVariant::Double,"x");
        Pos->addSubProperty(prop);
        m_Properties.push_back(prop);

        prop=variantManager->addProperty(QVariant::Double,"y");
        Pos->addSubProperty(prop);
        m_Properties.push_back(prop);

        prop=variantManager->addProperty(QVariant::Double,"z");
        Pos->addSubProperty(prop);
        m_Properties.push_back(prop);
    }


    QtVariantProperty *Scale=variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"Scale");
    m_Properties.push_back(Scale);
    m_rootProperty->addSubProperty(Scale);

    {
        prop=variantManager->addProperty(QVariant::Double,"x ");
        Scale->addSubProperty(prop);
        m_Properties.push_back(prop);

        prop=variantManager->addProperty(QVariant::Double,"y ");
        Scale->addSubProperty(prop);
        m_Properties.push_back(prop);

        prop=variantManager->addProperty(QVariant::Double,"z ");
        Scale->addSubProperty(prop);
        m_Properties.push_back(prop);
    }


    QtVariantProperty *Rotation=variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"Rotation");
    m_Properties.push_back(prop);
    m_rootProperty->addSubProperty(Rotation);

    {
        prop=variantManager->addProperty(QVariant::Double,"yaw");
        Rotation->addSubProperty(prop);
        m_Properties.push_back(prop);

        prop=variantManager->addProperty(QVariant::Double,"pitch");
        Rotation->addSubProperty(prop);
        m_Properties.push_back(prop);

        prop=variantManager->addProperty(QVariant::Double,"roll");
        Rotation->addSubProperty(prop);
        m_Properties.push_back(prop);
    }

}

void TransformComponent::updateProperties(){
    for(QtVariantProperty *prop:m_Properties){
        if(!prop)continue;
        std::string name=prop->propertyName().toStdString();

        if(name=="x")prop->setValue(getPosition().x);
        if(name=="y")prop->setValue(getPosition().y);
        if(name=="z")prop->setValue(getPosition().z);


        if(name=="x ")prop->setValue(getScale().x);
        if(name=="y ")prop->setValue(getScale().y);
        if(name=="z ")prop->setValue(getScale().z);

        glm::vec3 angle=glm::eulerAngles(getRotation());
        if(name=="yaw")prop->setValue(angle.x);
        if(name=="pitch")prop->setValue(angle.y);
        if(name=="roll")prop->setValue(angle.z);

    }
}


void TransformComponent::updateFromProperties(std::string name,QVariant value,Scene *scene){
//    if(name=="name")setName(value.toString().toStdString(),scene);
    if(name=="x")setPosition(glm::vec3(value.toFloat(),getPosition().y,getPosition().z));
    if(name=="y")setPosition(glm::vec3(getPosition().x,value.toFloat(),getPosition().z));
    if(name=="z")setPosition(glm::vec3(getPosition().x,getPosition().y,value.toFloat()));


    if(name=="x ")setScale(glm::vec3(value.toFloat(),getScale().y,getScale().z));
    if(name=="y ")setScale(glm::vec3(getScale().x,value.toFloat(),getScale().z));
    if(name=="z ")setScale(glm::vec3(getScale().x,getScale().y,value.toFloat()));

    glm::vec3 angle=glm::eulerAngles(getRotation());
    if(name=="yaw"){angle.x=value.toFloat();setRotation(glm::quat(angle));}
    if(name=="pitch"){angle.y=value.toFloat();setRotation(glm::quat(angle));}
    if(name=="roll"){angle.z=value.toFloat();setRotation(glm::quat(angle));}

}

#endif

}
