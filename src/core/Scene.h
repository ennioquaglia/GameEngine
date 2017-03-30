#pragma once
#include "Object.h"
#include "Camera.h"
#include <list>// todo use vector + cache( to delete)
#include <vector>
#include <algorithm>

#include "Debug.h"
#include "Container.h"


namespace pugi{
    class xml_node;
}
namespace Engine {
class DirectionalLightComponent;
//class Object;
//class Camera;
class Context;
class PostProcessor;
class PathName;

struct Options{
    Options():isShadowEnable(1),shadowLOD(9),useVSM(1),useParalax(1){}
    bool isShadowEnable;
    int shadowLOD;
    bool useVSM,useParalax;

};

typedef std::vector<Object*> ObjectsContainer;

class Scene
{
    friend class DirectionalLightComponent;
    Context *m_Context;


    //MultiComponentsContainer components;
    ObjectsContainer m_Objects, m_Systems, m_RenderSystems;
    bool setup_done=0;


    Camera m_DebugCamera;
    Camera m_Camera;
    Camera *m_CurrentCamera=&m_Camera;
    PostProcessor *m_Effect=0;
    bool m_isUsingDebugCamera=0;
    float m_Speed=1.0f;

    unsigned int getNewUID();

    std::vector<Entity*> m_Entities;
    //VectorContainer<Entity> m_Entities_data;
    std::map<unsigned,unsigned> m_Entities_hash_index;
    //std::vector<Entity>  m_Entities_type_vector;
    std::map<unsigned, std::vector<Object*> > m_Objects_type_vector;


public:
    Options options;

    Scene(Context *ctx);
    virtual ~Scene();

    void Update(float deltaTime);
    void Render();
    void Draw(RenderFlag flag=RenderFlag::Default);
    virtual void handleEvent(Event &msg);
    void PostSetup();

    Camera getCamera() const;
    void   setCamera(Camera cam);
    bool   isUsingDebugCamera() const;
    void   useDebugCamera(bool use=true);

    void  setVelocity(float s){ m_Speed=s;      }
    float getVelocity() const { return m_Speed; }

    Context* getContext() const {return m_Context;}
    void setPostProcessorEffect(PostProcessor* effect);

    void addObject(Object *obj);
    void removeObject(Object *obj);

    //QUERY
    //todo: store object in a proper way (cache friendly and/or ordered by type)
    Object* getObjectByFlag(unsigned int flag) const;

    template<class T=Object>
    std::vector<T*> getObjectsByTag(unsigned int tag) {
        std::vector<T*> objs;
        for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
            if (((*it)->getFlag() & tag)==tag)objs.push_back((T*)*it);
        }
        return objs;
    }

    template<class T>
    T* getObjectByType() const
    {
        auto it=m_Objects_type_vector.find(T::getTypeStatic());
        if(it==m_Objects_type_vector.end())return nullptr;
        return static_cast<T*>(*(it->second.begin()));
    }
    template<class T>
    std::vector<Object*> getObjectsByType() {
        auto it=m_Objects_type_vector.find(T::getTypeStatic());
        if(it==m_Objects_type_vector.end())return std::vector<Object*>();
        return (it->second);
    }

   Entity* newEntity(){


////        static std::vector<Entity> e;//(100);
////        static int num=0;
////        if(!num)e.resize(100);
////        //e.m_UID=getNewUID();
////        //m_Entities_type_vector.push_back(e);
//////        QENGINE_ERROR("\n\n\nAdress : "+(int)&m_Entities_type_vector[m_Entities_type_vector.size()-1]);
//////        Entity *t=&m_Entities_type_vector[m_Entities_type_vector.size()-1];
//////        QENGINE_ERROR((int)t);
//////        QENGINE_ERROR(t->getUID());
////        Entity *tt=&e[num++];//new Entity();//&e;//m_Entities_type_vector[e.m_UID];

//        //static int num=0;
//        //if(!num)m_Entities_data.resize(100);
//        Entity *tt=&m_Entities_data[num++];
//        tt->m_UID=getNewUID();
//        //QENGINE_ERROR(tt->getIndex());
////        T* t=new T();
////        t->m_UID = getNewUID();
//        m_Entities.push_back(tt);
//        return tt;//&m_Entities[m_Entities.size()-1];

       Entity *t=new Entity();//&m_Entities_data.insert();
       m_Entities.push_back(t);
       t->m_UID=getNewUID();

       return t;



    }
    void removeEntity(Entity *obj){//todo delete in m_Entities_data
        auto it=std::find(m_Entities.begin(),m_Entities.end(),obj);
        if(it==m_Entities.end())return;

        m_Entities.erase(it);
        //obj->Destroy();
        delete obj;
    }

//    template<class T=Entity>
//    T* newEntity(){
//        static_assert(std::is_base_of<Entity, T>::value, "T must inherit from Entity");



//        if(std::is_same<T,Entity>::value){
//            Entity e;
//            e.m_UID=getNewUID();
//            m_Entities_type_vector.push_back(e);
//            Entity *tt=&m_Entities_type_vector[e.m_UID];

//            m_Entities.push_back(tt);

//            QENGINE_ERROR(tt->getIndex());
//            //return m_Entities[m_Entities.size()-1];;
//        }



//        T* t=new T();
//        t->m_UID = getNewUID();


//        m_Entities.push_back(t);


//        //        T &t=m_Entities[m_Entities.size()-1];
//        //        t.m_UID=m_Entities.size()-1;


//        //t->Init();
//        //addObject(static_cast<Object*>(t));
//        return t;//&m_Entities[m_Entities.size()-1];
//    }





    const Entity* getEntity(unsigned int uid) const;
    const Entity* getEntity(std::string name) const;
    std::vector<Entity*> getEntities() {
        return m_Entities;
    }

    friend class Entity;

    template<class T>
    T* getObject(unsigned int uid) {
        static_assert(std::is_base_of<Object, T>::value, "T must inherit from Object");
        unsigned type=T::getTypeStatic();
        if(m_Objects_type_vector.find(type)!=m_Objects_type_vector.end()){
            auto vec=m_Objects_type_vector[type];
            unsigned index=(uid<<ENGINE_OBJECT_VERSION_BITS)>>ENGINE_OBJECT_VERSION_BITS;
            if(index>vec.size())return nullptr;
            return static_cast<T*>(vec[index]);
        }
        return nullptr;
    }

    template<class T>
    T* getComponent(unsigned int uid) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
        return getObject<T>(uid);
    }






    void serializeXML(std::ostream &stream,unsigned level=0) ;
    bool deserializeXML(PathName filename);
};

}
