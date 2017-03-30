#pragma once
#include <glm/glm.hpp>
#include <string>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

namespace Engine{


extern glm::vec3 getPosFromMatrix(glm::mat4 matrix);


extern btQuaternion toBullet(glm::quat quaternion);
extern glm::quat fromBullet(btQuaternion q);
extern btVector3 toBullet(glm::vec3 q);
extern glm::vec3 fromBullet(btVector3 q);






}







class btClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
    btClosestNotMeConvexResultCallback(btCollisionObject* me) : btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)) {
        m_me = me;
    }

    virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace) {
        if(convexResult.m_hitCollisionObject == m_me)
            return 1.0;

        return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
    }
protected:
    btCollisionObject* m_me;
};


class btClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
public:
    btClosestNotMeRayResultCallback (btCollisionObject* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
    {
        m_me = me;
    }
    btClosestNotMeRayResultCallback (btCollisionObject* me,btVector3 from,btVector3 to) : btCollisionWorld::ClosestRayResultCallback(from,to)
    {
        m_me = me;
    }

    virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
    {
        if (rayResult.m_collisionObject == m_me)
            return 1.0;

        return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
    }
protected:
    btCollisionObject* m_me;
};
