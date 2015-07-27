#pragma once

#include "../common/utility.h"
#include "../Physic/Physics.h"
#include "../common/Event.h"
#include <algorithm>
#include "../Render/RenderFlag.h" 
//#include "Engine.h"





typedef unsigned int ActorId;
typedef unsigned int ComponentClassType;
typedef unsigned int SystemID;
//typedef unsigned int ComponentId;

using namespace std;
using namespace Engine;

struct ComponentId {
    typedef enum {
        Transform,
        Render,
        Physic,
        UserInput,
        PhysicManager,
        CameraID,
        AnimationManager,
        Light,
        userType
    } Type;
};   

extern ComponentId::Type _getNewComponentId();
#define ENGINE_COMPONENT_MACRO public:  virtual ComponentId::Type getComponentId(void){static ComponentId::Type id=_getNewComponentId(); return id;} private:

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
    userTypeEvent
};


class LightComponent;

namespace Engine
{
    class SceneManager;
    class Scene;


    class ActorComponent;
    class ActorManager;
	class InputManager;


    class Actor
    {
        friend class ActorManager;
        friend class SubSystem;

        typedef map<ComponentClassType,ActorComponent*> Components;
        Components components;

        ActorId m_id;

        list<Engine::Event*> msgQueue;
        Engine::Scene* World;
    protected:
        Actor* getNextNode();
    public:
        Actor() {
            m_id=0;
            World=0;
        }
        ~Actor();

        virtual bool init();
        virtual void destroy();

        virtual void update(float deltaTimeMs,Engine::Scene* scene);
        virtual void handleEvent(Event &msg) {}
        template <class ComponentClass>
        ComponentClass* getComponent(ComponentClassType id) {
            if(components.find(id)==components.end()) {
                    Debug("no component "+id);
                    return 0;
                }
            return dynamic_cast<ComponentClass*>(components[id]);
        }

        ActorId getId() {
            return m_id;
        }
        //private:
        void addComponent(ActorComponent* c);
        void sendEvent(Event &msg);
        Engine::Scene* getWorld() {//obsolete
            return World;
        }


        Actor *parent;
        vector<Actor*> child;

        struct iterator {
            iterator(const iterator&);
            ~iterator();
            iterator& operator=(const iterator&);
            iterator& operator++(); //prefix increment
            Actor* operator*() const;

        };



        friend class Engine::Scene;
    };




	

    class ActorComponent
    {
    	
        ENGINE_COMPONENT_MACRO
        enum {
            none=0,
            NeedUpdate=1,
            ListenEvents=2
        };
    protected:
        
        Actor* m_pOwner;
        void sendLocalEvent(Event &msg);
        void sendBroadcastEvent(Event &msg);
		
	public:
		Flag flag;	
		
    public:
    	struct UpdateParameter{
			Actor* actor;
			Scene* scene;
			InputManager* input;
			float deltaTimeMs;
		};
    	 
        ActorComponent();
        virtual ~ActorComponent(void);

        virtual bool init();
        virtual void postActorSetInit() {}
        virtual void postWorldSetInit() {}
        virtual void update(UpdateParameter &param){}//float deltaTimeMs,Actor *actor,Engine::Scene* scene) {}

        virtual void handleEvent(Event &msg) {}
        virtual ComponentId::Type getComponentType(void) const = 0;

        static ComponentId::Type getNewId();

        virtual void destroy() {}
        Engine::Scene* getWorld() {
            if(!m_pOwner)return 0;
            return m_pOwner->getWorld();
        }

        Actor* getOwner() const {
            return m_pOwner;
        }
    private:

        void setOwner(Actor* pOwner) {
            if(!pOwner)return;
            m_pOwner = pOwner;
            postActorSetInit();
        }

        friend class Actor;
    };

    class GraphicComponent : public ActorComponent
    {
    public:
    	struct RenderParameter{
    		RenderParameter():flag(0),actor(0),scene(0){}
    		uint flag;
			Actor* actor;
			Scene* scene;
			//uint flag;
		};
    	GraphicComponent();
        virtual void draw(RenderParameter &param){//unitDraw(Actor *actor,Engine::Scene* scene) {
            bindRenderState(param);
            unitDraw(param);
            unbindRenderState(param);
        }
        virtual void bindRenderState(RenderParameter &param) {}
        virtual void unbindRenderState(RenderParameter &param) {}
        virtual void unitDraw(RenderParameter &param){}
        virtual ComponentId::Type getComponentType(void) const {
            return ComponentId::Render;
        }
    };


    class PhysicsSubSystem;

    class PhysicsComponent : public ActorComponent
    {
    public:
        virtual ComponentId::Type getComponentType(void) const {
            return ComponentId::Physic;
        }
        virtual void addToWorld(PhysicsSubSystem *sys) {};
        virtual void updateMatrix() {};
    };




    class SubSystem
    {
    protected:
        Engine::Scene* _Scene;
    public:
        SubSystem() {
            _Scene=0;
        }
        virtual void Init() {};
        virtual void update(float deltaTimeMs) {};
        virtual void addComponent(ActorComponent* c) {};
        virtual SystemID getSystemType(void) const =0;
        friend class SystemManager;
    };



    class Generic60fpsSubSystem : public SubSystem
    {
        typedef list<ActorComponent*> Components;
        Components components;
    public:
        Generic60fpsSubSystem() {}
        void Init() {}
        virtual void addComponent(ActorComponent* c) {
            if(!c)return;
            components.push_back(c);
        }
        virtual void update(float deltaTimeMs);
        virtual SystemID getSystemType(void) const {
            return StringHash("Generic60fpsSystem").getHash();
        }
    };

	class Generic30fpsSubSystem : public SubSystem
    {
        typedef list<ActorComponent*> Components;
        Components components;
    public: 
        Generic30fpsSubSystem() {}
        void Init() {}
        virtual void addComponent(ActorComponent* c) {
            if(!c)return;
            components.push_back(c);
        }
        virtual void update(float deltaTimeMs);
        virtual SystemID getSystemType(void) const {
            return StringHash("Generic30fpsSystem").getHash();
        }
    };


    class GenericSubSystem : public SubSystem
    {
        typedef list<ActorComponent*> Components;
        Components components;
    public:
        GenericSubSystem() {}
        void Init() {}
        virtual void addComponent(ActorComponent* c) {
            if(!c)return;
            components.push_back(c);
        }
        virtual void update(float deltaTimeMs);
        virtual SystemID getSystemType(void) const {
            return StringHash("GenericSystem").getHash();
        }
    };



    class RenderSubSystem : public SubSystem
    {
        typedef map<ComponentId::Type,vector<GraphicComponent*> > GraphicComponents;
        GraphicComponents components;
    public:
        RenderSubSystem() {}

        virtual void render(uint flag=RenderFlag::Default)=0;
    };



    class DefaultRenderSubSystem : public RenderSubSystem
    {
        typedef map<ComponentId::Type,vector<GraphicComponent*> > GraphicComponents;
        typedef list<LightComponent*> Lights;
        GraphicComponents components;
        Lights lights;
    public:
        DefaultRenderSubSystem() {}

        void Init();
        void render(uint flag=RenderFlag::Default);
        virtual void addComponent(ActorComponent* c);
        void update(float deltaTimeMs);
        virtual SystemID getSystemType(void) const {
            return StringHash("DefaultRenderSystem").getHash();
        }
    };




    class PhysicsSubSystem : public SubSystem
    {
        typedef map<ComponentId::Type,vector<PhysicsComponent*> > PhysicsComponents;
        PhysicsComponents components;
    public:
        PhysicsSubSystem() {
        }
        ~PhysicsSubSystem() {
            endPhysics();
        }

        void update(float deltaTimeMs);

        virtual void addComponent(ActorComponent* c) {
            if(!c)return;
            PhysicsComponent* pc=dynamic_cast<PhysicsComponent*>(c);
            if(pc) {
                    components[c->getComponentId()].push_back(pc);
                    pc->addToWorld(this);
                    //Debug("added physic object!!!");
                }
        }

        btDiscreteDynamicsWorld* dynamicsWorld;
        //btBroadphaseInterface*	m_overlappingPairCache;
        btBroadphaseInterface* broadphase;
        void Init();
        virtual SystemID getSystemType(void) const {
            return StringHash("DefaultPhysicSystem").getHash();
        }
    private:


        btDefaultCollisionConfiguration* collisionConfiguration ;
        btSequentialImpulseConstraintSolver* solver;
        btRigidBody* groundRigidBody;
        btCollisionDispatcher* dispatcher;

        void endPhysics() {
            delete dynamicsWorld;
            delete solver;
            delete collisionConfiguration;
            delete dispatcher;
            delete broadphase;
        }

    };



    class ActorManager
    {
        typedef map<ActorId,Actor*> Actors;
        Actors actors;
        Engine::Scene* _Scene;
    public:
        ActorManager() : _Scene(0) {}
        void setScene(Engine::Scene* scene) {
            _Scene=scene;

        }
        void Init() {

        }

        void update(float deltaTimeMs) {

            for(Actors::iterator it=actors.begin(); it!=actors.end(); it++) {
                    (it->second)->update(deltaTimeMs,_Scene);
                }

        };

        void addActor(Actor* actor);
    };



    class SystemManager
    {
    protected:
        typedef map<SystemID,SubSystem*> Systems;
        typedef vector<SubSystem*> SystemsList;
        typedef map<SystemID,RenderSubSystem*> RenderSystems;
        Systems systems;
        SystemsList systemsList;
        RenderSystems renderSystems;
        //RenderSubSystem *renderSystem;
        Engine::Scene* _Scene;
    public:
        SystemManager() {

        }
        ~SystemManager() {

        }
        virtual void Init() {
            //if(renderSystems.empty()){
            //	addRenderSystem(new DefaultRenderSubSystem());
            //}
        }
        virtual void render(uint flag=RenderFlag::Default) {
            static int a=0;
            if(!a)Debug("render render\n\n\n"),a++;

            for(RenderSystems::iterator it=renderSystems.begin(); it!=renderSystems.end(); it++) {
                    (it->second)->render(flag);
                }
        }
        virtual void addRenderSystem(RenderSubSystem* sys) {
            if(!sys)return;
            //renderSystem=sys;
            renderSystems[sys->getSystemType()]=sys;
            addSystem(sys);
        }
        virtual void update(float deltaTimeMs) {
            for(SystemsList::iterator it=systemsList.begin(); it!=systemsList.end(); it++) {
                    (*it)->update(deltaTimeMs);
                    glDebug((*it)->getSystemType() +"  -"+ StringHash("DefaultRenderSystem").getHash());
                    
                }
        }
        virtual void addSystem(SubSystem* sys) {
            if(!sys)return;
            Debug("NEW SYSTEM");
            systems[sys->getSystemType()]=sys;

            SystemsList::iterator find=std::find(systemsList.begin(), systemsList.end(), sys);
            if(find==systemsList.end())systemsList.push_back(sys);
            else(*find)=sys;


            sys->_Scene=_Scene;
            sys->Init();
        }
        virtual SubSystem* getSystem(SystemID id) {
            if(systems.find(id)!=systems.end()) {
                    return systems[id];
                }
            return 0;
        }

        void setScene(Engine::Scene* scene) {
            _Scene=scene;
        }
        virtual void addComponent(ActorComponent *c) {
            SubSystem *s=getSystem(StringHash("DefaultRenderSystem").getHash());
            if(s) {
                    s->addComponent(c);
                }
            s=getSystem(StringHash("DefaultPhysicSystem").getHash());
            if(s) {
                    s->addComponent(c);
                }
            else Debug("no physic");
        }
    };

}
