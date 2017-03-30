#pragma once

#include "utils/Event.h"
#include "Flag.h"
#include "Hash.h"
#include "enum_flags.h"
#include <vector>

#include <typeinfo>
#include <type_traits>
#include <list>




#ifndef ENGINE_ENTITIES_HAVE_NAMES
#define ENGINE_ENTITIES_HAVE_NAMES 1
#endif

#ifndef ENGINE_OBJECT_VERSION_BITS
#define ENGINE_OBJECT_VERSION_BITS 8
#endif

#ifndef ENGINE_USE_PICKING
#define ENGINE_USE_PICKING 0
#endif

#ifndef ENGINE_USE_QT_PROPERTY_BROWSER
#define ENGINE_USE_QT_PROPERTY_BROWSER 0
#endif


#if ENGINE_USE_QT_PROPERTY_BROWSER

#include <QtCore/QVariant>
class QtVariantProperty;
class QtVariantPropertyManager;
#endif



typedef unsigned int ComponentType;



#define COMPONENT_TYPE_INFORMATION(class_name)  \
    virtual ComponentType getType()           const{ return this->getTypeStatic(); }\
    static  ComponentType getTypeStatic()          { return Hash(#class_name); }\
    virtual std::string   getTypeName()       const{ return #class_name;}\
    static  std::string   getTypeNameStatic()      { return #class_name;}


namespace pugi{
    class xml_node;
}


namespace Engine {

class Entity;
class InputManager;
class Scene;

namespace EventType {
enum EventType {
    none=0,
    Rotate,
    shouldMove,
    shouldWalk,
    shouldJump,
    isOnGround,
    shouldWarp,
    stopWalking,
    shouldBeDestroyed,
    destroy,
    componentRemoved,
    componentUpdated,
    componentAdded,

};
#define last_event_id EventType::componentAdded+1


enum {
    newEvent = last_event_id
};

#ifdef last_event_id
#undef last_event_id
#endif
#define last_event_id EventType::newEvent+1

}
template<typename T> using Underlying = typename std::underlying_type<T>::type;
template<typename T> constexpr Underlying<T>
underlying(T t) { return Underlying<T>(t); }

template<typename T> struct TruthValue {
    T t;
    constexpr TruthValue(T t): t(t) { }
    constexpr operator T() const { return t; }
    constexpr explicit operator bool() const { return underlying(t); }
};

enum struct RenderFlag{
    Default=1,
    Shadow =2,
    Deferred=4
};

constexpr TruthValue<RenderFlag>
operator&(RenderFlag l, RenderFlag r) { return RenderFlag(underlying(l) & underlying(r)); }
template<typename T>
constexpr TruthValue<RenderFlag>
operator&( T l, RenderFlag r) { return RenderFlag(l & underlying(r)); }

enum struct Target{
    none    =   0,
    byFlag  =   1,
    self    =   2,
    entity  =   4,
    scene   =   8,
    sameType=   16
};

constexpr TruthValue<Target>
operator&(Target l, Target r) { return Target(underlying(l) & underlying(r)); }
template<typename T>
constexpr TruthValue<Target>
operator&( T l, Target r) { return Target(l & underlying(r)); }
constexpr TruthValue<Target>
operator|(Target l, Target r) { return Target(underlying(l) | underlying(r)); }
template<typename T>
constexpr TruthValue<Target>
operator|( T l, Target r) { return Target(l | underlying(r)); }





class Object
{
    virtual void Init(){}
    virtual void Destroy(){
        Event msg(EventType::destroy);
        msg.setData(this);
        sendEvent(msg);
    }

    unsigned int m_UID=0;

    using Listeners=std::vector<Object*>;
    Listeners listeners;

public:
    struct UpdateParameter {
        UpdateParameter(const Scene* s,const InputManager &i, float DeltaTime) :scene(s), input(i),deltaTime(DeltaTime){}
        const Scene* scene;
        const InputManager &input;
        const float deltaTime;
    };

    struct RenderParameter {
        RenderParameter(const Scene *s,unsigned int p_flag=(unsigned int)RenderFlag::Default, float DeltaTime=1.0f/60.0f) :flag(p_flag), scene(s),deltaTime(DeltaTime){}
        const unsigned int flag;
        const Scene* scene;
        const float deltaTime;
    };

protected:


    Flag m_Flag;  //or TAGs


    Object();
    virtual ~Object();

    virtual void Draw(RenderParameter &param) {}
    virtual void Update(UpdateParameter &param) {}

    virtual void OnAttached(Entity& e) {}
    virtual void OnScene(Scene &scene) {}
    friend class Scene;
    friend class Entity;


public:


    enum Type{
        none    =0,
        entity  =1,
        update  =2,
        render  =4,
        light   =8,
        directional_light= 16,
        physics =32,
        other   =64
    };

    virtual void sendEvent(Event &msg,Target target=Target::entity, int flag=0){
        for(auto l:listeners)l->handleEvent(msg);
    }

    virtual void handleEvent(Event &msg) {}


    static  ComponentType getTypeStatic()            { return 0; }
    virtual ComponentType getType()            const { return this->getTypeStatic(); }
    static  std::string   getTypeNameStatic()        { return "undefined_type"; }
    virtual std::string   getTypeName()        const { return "undefined_type"; }

    virtual Flag  getFlag()    const { return m_Flag; }
    unsigned int  getUID()     const { return m_UID;  }
    unsigned int  getVersion() const { return m_UID >> (sizeof(unsigned int)*8 - ENGINE_OBJECT_VERSION_BITS);}
    unsigned int  getIndex()   const { return (m_UID<<ENGINE_OBJECT_VERSION_BITS)>>ENGINE_OBJECT_VERSION_BITS ;}//- (getVersion()<<(sizeof(unsigned int)*8 - ENGINE_OBJECT_VERSION_BITS));}

    virtual void addComponent(Object *obj) {}
    virtual void removeComponent(Object *obj) {}

    void attachListener(Object *o){
        for(auto l:listeners)if(l==o)return;
        listeners.push_back(o);
    }
    void detachListener(Object *o){
        int i=0;
        for(auto l:listeners){
            if(l==o){listeners.erase(listeners.begin()+i);return;}
            ++i;
        }
    }


    virtual void serializeXML(std::ostream &stream,unsigned level=0) const =0;
    friend std::ostream& operator<< (std::ostream& stream, const Object* obj) {
        obj->serializeXML(stream);
        return stream;
    }


#if ENGINE_USE_QT_PROPERTY_BROWSER

    QtVariantProperty *m_rootProperty=0;
    std::list<QtVariantProperty*> m_Properties;
    virtual void setupProperties(QtVariantPropertyManager *variantManager){}
    virtual void updateProperties(){}
    virtual void updateFromProperties(std::string name,QVariant value,Scene *scene){}

#endif



};


class cache_reference{
    
};


class Component : public Object
{
    friend class Entity;
    friend class Scene;

    Entity *m_Entity=0;


    Component( const Component& other ); // non construction-copyable
    Component& operator=( const Component& ); // non copyable


public:
    Component(){}

    virtual inline Entity* getEntity() const{return m_Entity;}
    virtual void sendEvent(Event &msg,Target target=Target::entity, int flag=0);

    virtual void serializeXML(std::ostream &stream,unsigned level=0) const;

};






class Entity : public Object
{
    bool setup_done=0;
    typedef std::vector<Object*> ObjectsContainer;
    ObjectsContainer m_Objects;
    Entity* m_Parent=0;

    friend class Scene;
    friend class std::vector<Entity>;

protected:
public:

    Entity();

    virtual void OnScene(Scene &scene);
    virtual void Destroy();

#if ENGINE_ENTITIES_HAVE_NAMES
    std::string m_Name;
public:
    std::string getName() const          { return m_Name; }
    void        setName(std::string name,Scene* scene);
#endif


public:

    COMPONENT_TYPE_INFORMATION(Entity)

    virtual void sendEvent(Event &msg,Target target=Target::entity, int flag=0);

    virtual void addComponent(Component *obj);
    virtual void removeComponent(Component *obj);


    //####// Query //####//
    virtual ObjectsContainer& getComponents() {return m_Objects;}
    virtual Entity*           getParent()     {return m_Parent;}
    virtual Object*           getObjectByFlag(unsigned int flag);
    template<class T>
    T* getObjectByType()
    {
        for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
            if ( (*it)->getType() == T::getTypeStatic())return (T*)*it;
        }
        return nullptr;
    }
    virtual Component* getComponentByFlag(unsigned int flag);
    template<class T>
    T* getComponentByType()
    {
        for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
            if ( (*it)->getType() == T::getTypeStatic())return (T*)*it;
        }
        return nullptr;
    }


    virtual void serializeXML(std::ostream &stream,unsigned level=0) const ;



#if ENGINE_USE_QT_PROPERTY_BROWSER

    virtual void setupProperties(QtVariantPropertyManager *variantManager);
    virtual void updateProperties();
    virtual void updateFromProperties(std::string name,QVariant value,Scene *scene);


    friend class MainWindow;
#endif
};






class System : public Object
{
    friend class Scene;
public:
    System();
    virtual void addComponent(Object *obj){}
    virtual void removeComponent(Object *obj){}

    virtual void Update(UpdateParameter &param) {}

    virtual void Init(){}
    virtual void Destroy(){}

    virtual void serializeXML(std::ostream &stream,unsigned level=0) const {}
};






class RenderSystem : public System
{
    friend class Scene;
public:
    RenderSystem();
    virtual void Draw(RenderParameter &param) {}

    //        virtual void Init(){}
    //        virtual void Destroy(){}

};

}
