#include "Object.h"
#include "Hash.h"
#include "Debug.h"
#include "Scene.h"
#include <iostream>



#if ENGINE_USE_QT_PROPERTY_BROWSER
#include <QtCore/QVariant>
#include <QtCore/QString>
#include "src/qtpropertymanager.h"
#include "src/qtvariantproperty.h"
#include "src/qttreepropertybrowser.h"
#endif


namespace Engine {

Object::Object():m_Flag(0)
{
    //QENGINE_INFO("Object construction");

}
Object::~Object()
{
    Destroy();//TODO

#if ENGINE_USE_QT_PROPERTY_BROWSER
    for(auto p:m_Properties)delete p;
#endif
}







System::System():Object()
{
    m_Flag.add(Object::update);
    // m_Flag.add(StringEnum("update"));
}


void Component::sendEvent(Event &msg,Target target, int flag) {
    Object::sendEvent(msg,target,flag);
    if(m_Entity)m_Entity->sendEvent(msg,target,flag);
}



Entity::Entity():m_Parent(0)
{
    m_Flag.add(Object::entity);
    // m_Flag.add(StringEnum("entity"));
}


#if ENGINE_ENTITIES_HAVE_NAMES

void Entity::setName(std::string name,Scene* scene){  //todo

    if(name.find(' ')!=std::string::npos){
        QENGINE_WARN("Entity::setName() -> name contains forbidden characters (\' \')");
        EString::replaceAll(name," ","_");
    }

    if(!scene){m_Name=name;return;}

    unsigned hash=Hash(m_Name);
    auto it=scene->m_Entities_hash_index.find(hash);
    if(it!=scene->m_Entities_hash_index.end() && it->second==getIndex()){
        scene->m_Entities_hash_index.erase(it);
    }
    m_Name=name;
    hash=Hash(m_Name);
    it=scene->m_Entities_hash_index.find(hash);
    if(it!=scene->m_Entities_hash_index.end())
        QENGINE_WARN("Entity::setName() -> name alredy exists : overwrite (error prone!)");

    scene->m_Entities_hash_index[hash]=getIndex();
}

#endif


void Entity::sendEvent(Event &msg,Target target, int flag) {
    if(target & Target::entity){
        for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it)
            (*it)->handleEvent(msg);
    }
}


void Entity::addComponent(Component *obj) {
    if (!obj)return;
    obj->m_Entity=this;
    m_Objects.push_back(obj);
    if(setup_done){
        obj->OnAttached(*this);
    }

    //EventData<Component*>
    Event msg;
    msg.setType(EventType::componentAdded);
    msg.setData(obj);
    sendEvent(msg);
}

void Entity::removeComponent(Component *obj) {//TODO
    if (!obj)return;
    //obj->m_Entity=0;
    //m_Objects.remove(obj);


    auto it=std::find(m_Objects.begin(),m_Objects.end(),obj);
    if(it==m_Objects.end())return;

    m_Objects.erase(it);
    //obj->Destroy();
    delete obj;
}
Object * Entity::getObjectByFlag(unsigned int flag)
{
    for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
        if ((*it)->m_Flag & flag)return *it;
    }
    return nullptr;
}
Component * Entity::getComponentByFlag(unsigned int flag)
{
    for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
        if ((*it)->m_Flag & flag)return (Component*)*it;
    }
    return nullptr;
}

void Entity::OnScene(Scene &scene)
{
    for(auto o:m_Objects)
        o->OnAttached(*this);

    setup_done=true;
}

void Entity::Destroy(){
    for(auto o:m_Objects){
        delete o;
        //o->Destroy();
    }
}



RenderSystem::RenderSystem():System()
{
    m_Flag.add(Object::render);
    //m_Flag.add(StringEnum("render"));
}




void Entity::serializeXML(std::ostream &stream,unsigned level) const{

    QENGINE_ERROR("entity serialize");
    for(unsigned i=0;i<level;++i)stream<<"\t";

    stream<<"<"
         <<getTypeName()<<" "
        <<SERIALIZE_VAR_STR(m_Name)
       <<" >\n";

    for(auto o:m_Objects)o->serializeXML(stream,level+1);

    //stream<<o->getType()<<" "<<o->getIndex()<<" ";

    for(unsigned i=0;i<level;++i)stream<<"\t";

    stream<<"</"<<getTypeName()<<">\n";
}



void Component::serializeXML(std::ostream &stream,unsigned level) const{
    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"<"
         <<getTypeName()<<" "
        <<" >\n";
    // <<m_Objects.size()<<" ";
    //for(auto o:m_Objects)o->serialize(stream);

    //stream<<o->getType()<<" "<<o->getIndex()<<" ";

    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"</"<<getTypeName()<<">\n";
}

#if ENGINE_USE_QT_PROPERTY_BROWSER

void Entity::setupProperties(QtVariantPropertyManager *variantManager){
    m_rootProperty=variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"Object");


    QtVariantProperty *prop=variantManager->addProperty(QVariant::String,"name");
    m_Properties.push_back(prop);
    m_rootProperty->addSubProperty(prop);

    for(auto o:m_Objects){
        o->setupProperties(variantManager);
        m_rootProperty->addSubProperty(o->m_rootProperty);
    }

}
void Entity::updateProperties(){
    for(QtVariantProperty *prop:m_Properties){
        if(!prop)continue;
        std::string name=prop->propertyName().toStdString();

        if(name=="name")prop->setValue(QVariant(QString(getName().c_str())));
    }
    for(auto o:m_Objects)o->updateProperties();
}
void Entity::updateFromProperties(std::string name, QVariant value, Scene *scene){
    if(name=="name")setName(value.toString().toStdString(),scene);
    for(auto o:m_Objects)o->updateFromProperties(name,value,scene);
}

#endif




}
