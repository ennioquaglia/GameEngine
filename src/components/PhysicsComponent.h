#pragma once

#include "Object.h"
#include "AssimpScene.h"

#include <glm/glm.hpp>
#include <string>

class btDiscreteDynamicsWorld;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;
class btRigidBody;
class btCollisionDispatcher;

class btCollisionShape;
class btStridingMeshInterface;
class btDynamicsWorld;
class btConvexShape;

namespace Engine{

class TransformComponent;

class PhysicsComponent : public Component
{
public:
    COMPONENT_TYPE_INFORMATION(PhysicsComponent)
    PhysicsComponent();
    virtual void updateMatrix(){}
    virtual void Update(UpdateParameter &param){}
};




class BulletPhysicsSystem : public System
{
    typedef std::vector<PhysicsComponent*> Components;
    Components m_Components;





    btDefaultCollisionConfiguration* collisionConfiguration ;
    btSequentialImpulseConstraintSolver* solver;
    btRigidBody* groundRigidBody;
    btCollisionDispatcher* dispatcher;
public:
    COMPONENT_TYPE_INFORMATION(BulletPhysicsSystem)

    BulletPhysicsSystem();
    virtual ~BulletPhysicsSystem();

    virtual void addComponent(Object *obj);
    virtual void Update(UpdateParameter &param);

    virtual void Init();
    virtual void Destroy();

    btDiscreteDynamicsWorld* dynamicsWorld;
    //btBroadphaseInterface*	m_overlappingPairCache;
    btBroadphaseInterface* broadphase;

};






class BulletPhysicsComponent : public PhysicsComponent
{

protected:
    float mass;
    btRigidBody * physicComponent;

    TransformComponent * trans=0;
    bool dirty=1;

public:
    COMPONENT_TYPE_INFORMATION(BulletPhysicsComponent)
    BulletPhysicsComponent() {
        mass = 0;
        physicComponent = 0;
    }

    void update_cache();

    virtual void setMass(float value);
    virtual void updateMatrix();
    virtual btRigidBody * getPhysicsObject();

//protected:
    virtual btCollisionShape * getCollisionShape() = 0;
    virtual void OnAttached(Entity& e);
    virtual void OnScene(Scene &scene);
    virtual void move(glm::vec3 dir) ;
    virtual glm::mat4 getTransform();
};


class SpherePhysicsComponent : public BulletPhysicsComponent
{

    float mRadius;
public:
    COMPONENT_TYPE_INFORMATION(SpherePhysicsComponent)
    SpherePhysicsComponent(float radius = 1.0f) {
        mRadius = radius;
        mass = 1;
    }

private:
    btCollisionShape * getCollisionShape();

};



class StaticMeshPhysicsComponent : public BulletPhysicsComponent
{

    PathName m_Filename;
public:
    COMPONENT_TYPE_INFORMATION(StaticMeshPhysicsComponent)
    StaticMeshPhysicsComponent(PathName filename) :m_Filename(filename) {
        mass = 0;
    }

    void serializeXML(std::ostream &stream,unsigned level) const;
private:
    void recursiveFoo(const struct aiScene * sc, const struct aiNode * nd, aiMatrix4x4 matrix);
    btCollisionShape * getCollisionShape();

    std::vector<float> points;
    std::vector<int> indices;
    btStridingMeshInterface * indexVertexArrays;
};










}
