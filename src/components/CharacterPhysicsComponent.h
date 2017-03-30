#pragma once

#include "PhysicsComponent.h"
#include <glm/gtx/quaternion.hpp>




namespace Engine{

class MyCharacterDynamicPhysicsComponent : public PhysicsComponent
{

protected:
public:
    glm::vec3 walkDirection;
    glm::quat orientation;

    btRigidBody * physicComponent;
    btDynamicsWorld * dynamicsWorld;


    //parameter
    float mass;
    float moveSpeed;
    float jumpHeight;
    float jumpSpeed;
    float capsuleHeight;
    float capsuleRadius;
    float springHeight;
    float yVelocity;
    int numRays;

    float feetDistance=0.5;


    //state
    bool isJumping;
    bool jump;
    bool onGround;
    bool isFalling;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    float m_strideWheelRotation;
    float m_strideWheelRadius;
    glm::vec3 m_footPosition[2];
    int m_currentFoot=0;


    glm::mat4 currentMatrix;
    //glm::vec3 position;
public:

    COMPONENT_TYPE_INFORMATION(MyCharacterDynamicPhysicsComponent)
    MyCharacterDynamicPhysicsComponent(float radius = 1.0f);


    virtual void updateMatrix();
    virtual btRigidBody * getPhysicsObject();

    virtual void handleEvent(Event &msg);   
    virtual void Update(UpdateParameter &param);


    //temporary
    virtual void Draw(RenderParameter & param);

protected:
    virtual btConvexShape * getCollisionShape();

    virtual void OnAttached(Entity& e);
    virtual void OnScene(Scene &scene);


    virtual void move(glm::vec3 dir);
    virtual glm::mat4 getTransform();

};



}
