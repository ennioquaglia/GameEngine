#pragma once
#include "StandardComponent.h"
#include "../Common/AssetManager.h"
#include "../Render/StaticMesh.h"
#include "../Render/SkeletalMesh.h"
#include "../Physic/Physics.h"
#include "../Physic/btCustomKinematicCharacterController.h"
#include "../Render/DebugRender.h"
#include "../Core/Scene.h"

class BasicPhysicsComponent : public PhysicsComponent
{
    ENGINE_COMPONENT_MACRO
public:
    BasicPhysicsComponent() {}

    virtual void handleEvent(Event &msg) {
        switch(msg.getType()) {
                case shouldMove: {
                        vec3 dir = msg.getData<vec3>();

                        TransformComponent * trans;
                        if(m_pOwner)trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                        if(trans)trans->addPos(dir);
                    }
                    break;
                default:
                    break;
            }
    }

};


class BulletPhysicsComponent : public PhysicsComponent
{
    ENGINE_COMPONENT_MACRO
protected:
    btScalar mass;
    btRigidBody * physicComponent;

public:
    BulletPhysicsComponent() {
        mass = 0;
        physicComponent = 0;
    }

	virtual void setMass(float value){
		mass=value;//>=0?value:0;
	}
    virtual void updateMatrix() {
        TransformComponent * trans;
        if(m_pOwner)trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
        if(trans)trans->setMatrix(getTransform());
    }
    virtual btRigidBody * getPhysicsObject() {
        return physicComponent;
    }

protected:
    virtual btCollisionShape * getCollisionShape() = 0;
    virtual void postActorSetInit() {
        //Debug("bullet post actor set");
        btCollisionShape * collisionShape = getCollisionShape();
        btVector3 Inertia;

        glm::vec3 p = vec3(0, 0, 0);
        btQuaternion q(0, 0, 0, 1);

        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        vec3 s = trans->getLocalScale(ComponentId::Physic);
                        collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));

                        p = trans->getLocalPos(ComponentId::Physic);
                        q = toBullet(trans->getLocalRotation(ComponentId::Physic).getQuaternion());

                        //q=Engine::toBullet(glm::quat_cast(trans->getMatrix()));
                    }
            }
        //glm::vec3 s=glm::vec3(matrix[0][0],matrix[1][1],matrix[2][2]);
        //glm::vec3(matrix[3][0],matrix[3][1],matrix[3][2]);
        //collisionShape->setLocalScaling(btVector3(s.x,s.y,s.z));

        btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(q, btVector3(p.x, p.y, p.z)));
        collisionShape->calculateLocalInertia(mass, Inertia);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, motionState, collisionShape, Inertia);
        physicComponent = new btRigidBody(fallRigidBodyCI);
        physicComponent->setUserPointer((void *)this);
        physicComponent->setRestitution(0.5);
    }

    virtual void addToWorld(PhysicsSubSystem * sys) {
        sys->dynamicsWorld->addRigidBody(physicComponent);
    }
    virtual void move(vec3 dir) {
        physicComponent->activate();
        btVector3 d = btVector3(dir.x, dir.y, dir.z);
        physicComponent->setLinearVelocity(d);
    }
    virtual mat4 getTransform() {
        btTransform trans;
        physicComponent->getMotionState()->getWorldTransform(trans);
        float m[16] = {0};
        trans.getOpenGLMatrix(m);

        mat4 mvm = glm::make_mat4(m);
        return mvm;
    }

};


class SpherePhysicsComponent : public BulletPhysicsComponent
{
    ENGINE_COMPONENT_MACRO
    float mRadius;
public:
    SpherePhysicsComponent(float radius = 1.0f) {
        mRadius = radius;
        mass = 0;
    }

private:
    btCollisionShape * getCollisionShape() {


        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        vec3 s = trans->getLocalScale(ComponentId::Physic);
                        return new btSphereShape(s.x);
                    }
            }
        return new btSphereShape(mRadius);
    }

};

class StaticMeshStaticPhysicsComponent : public BulletPhysicsComponent
{
    ENGINE_COMPONENT_MACRO
    string mFilename;
public:
    StaticMeshStaticPhysicsComponent(std::string filename) {
        mFilename = filename;
        mass = 0;
    }

private:
    void recursiveFoo(const struct aiScene * sc, const struct aiNode * nd, aiMatrix4x4 matrix) {
        unsigned int i;
        unsigned int n = 0, t;
        aiMatrix4x4 m = nd->mTransformation * matrix;
        // draw all meshes assigned to this node
        for(; n < nd->mNumMeshes; ++n) {
                const struct aiMesh * mesh = sc->mMeshes[nd->mMeshes[n]];
                for(t = 0; t < mesh->mNumFaces; ++t) {
                        const struct aiFace * face = & mesh->mFaces[t];
                        GLenum face_mode;

                        if(face->mNumIndices == 3)
                            for(i = 0; i < face->mNumIndices; i++) {
                                    int index = face->mIndices[i];
                                    indices.push_back(index + points.size() / 3);
                                }
                    }
                for(uint t = 0; t < mesh->mNumVertices; ++t) {
                        aiVector3D v = m * mesh->mVertices[t];
                        points.push_back(v.x);
                        points.push_back(v.y);
                        points.push_back(v.z);
                    }
            }
        // draw all children
        for(n = 0; n < nd->mNumChildren; ++n) {
                recursiveFoo(sc, nd->mChildren[n], m);
            }
    }
    btCollisionShape * getCollisionShape() {

        const aiScene * scene = aiImportFile(mFilename.data(), aiProcessPreset_TargetRealtime_Fast | aiProcess_Triangulate | aiProcess_OptimizeMeshes);

        if(!scene) { //delete this;
                return 0;
            }
        recursiveFoo(scene, scene->mRootNode, scene->mRootNode->mTransformation);

        //printf("\n%d\t%d\n", indices.size(), points.size());

        indexVertexArrays = new  btTriangleIndexVertexArray(indices.size() / 3,
                & indices[0],
                3 * sizeof(int),
                points.size() / 3, (btScalar *) & points[0], sizeof(float) * 3);

        btBvhTriangleMeshShape * shape = new btBvhTriangleMeshShape(indexVertexArrays, true);

        //btBvhTriangleMeshShape *shape=new btBvhTriangleMeshShape(mTriMesh,true);

        //shape->buildOptimizedBvh();

        aiReleaseImport(scene);

        return shape;
    }

    vector<float> points;
    vector<int> indices;
    btStridingMeshInterface * indexVertexArrays;
};



//######//Charachter//######//

//
// class CharacterControllerInterface {
//     public:
//     CharacterControllerInterface () {};
//     virtual ~CharacterControllerInterface () {};
//     virtual void setup (btDynamicsWorld * dynamicsWorld, btScalar height = 2.0, btScalar width = 0.25, btScalar stepHeight = 0.25) = 0;
//     virtual void destroy (btDynamicsWorld * dynamicsWorld) = 0;
//
//     virtual btCollisionObject * getCollisionObject () = 0;
//
//     virtual void reset () = 0;
//     virtual void warp (const btVector3 & origin) = 0;
//
//     virtual void preStep (btDynamicsWorld * dynamicsWorld) = 0;
//     virtual void playerStep (btDynamicsWorld * dynamicsWorld, btScalar dt,
//                              int forward,
//                              int backward,
//                              int left,
//                              int right,
//                              int jump) = 0;
//     virtual bool canJump () const = 0;
//     virtual void jump () = 0;
//
//     virtual bool onGround () const = 0;
// };
//
// class DynamicCharacterController : public CharacterControllerInterface {
//     protected:
//     btScalar m_halfHeight;
//     btCollisionShape * m_shape;
//     btRigidBody * m_rigidBody;
//
//     btVector3 m_raySource[2];
//     btVector3 m_rayTarget[2];
//     btScalar m_rayLambda[2];
//     btVector3 m_rayNormal[2];
//
//     btScalar m_turnAngle;
//
//     btScalar m_maxLinearVelocity;
//     btScalar m_walkVelocity;
//     btScalar m_turnVelocity;
//     public:
//     DynamicCharacterController ();
//     ~DynamicCharacterController ();
//     void setup (btDynamicsWorld * dynamicsWorld, btScalar height = 2.0, btScalar width = 0.25, btScalar stepHeight = 0.25);
//     void destroy (btDynamicsWorld * dynamicsWorld);
//
//     btCollisionObject * getCollisionObject ();
//
//     void preStep (btDynamicsWorld * dynamicsWorld);
//     void playerStep (btScalar dt,
//                      int forward,
//                      int backward,
//                      int left,
//                      int right,
//                      int jump);
//     bool canJump () const;
//     void jump ();
//
//     bool onGround () const;
// };
//

//##############//


class BulletPhysicManagerComponent : public ActorComponent
{
    ENGINE_COMPONENT_MACRO
    vec3 walkDirection;
    Rotation orientation;
    bool jump,jumping;
public:
    BulletPhysicManagerComponent() {
        jump=jumping=0;
        walkDirection=vec3(0);
    }

    virtual ComponentId::Type getComponentType(void) const {
        return ComponentId::PhysicManager;
    }
    virtual btRigidBody * getPhysicComponent() {
        BulletPhysicsComponent * comp = 0;
        comp = m_pOwner->getComponent<BulletPhysicsComponent>(ComponentId::Physic);

        if(!comp)return 0;

        btRigidBody * physicComponent = comp->getPhysicsObject();

        return physicComponent;
    }

    virtual void setRotation(Rotation rot) {
        // if(!m_pOwner)return;
        //
        //   	TransformComponent* trans=m_pOwner->getComponent<TransformComponent>(Transform);
        //   	trans->setRotation(rot);
        btRigidBody * physicComponent = getPhysicComponent();
        if(physicComponent) {
                physicComponent->activate();
                btTransform trans;
                physicComponent->getMotionState()->getWorldTransform(trans);
                btQuaternion q = toBullet(rot.q);
                q = q.normalize();
                physicComponent->setCenterOfMassTransform(btTransform(q, trans.getOrigin()));
            }
    }

    bool onGround() {

        return 1;
        return 0;

    }



    virtual void handleEvent(Event &msg) {

        switch(msg.getType()) {
                case shouldMove: {
                        walkDirection = msg.getData<vec3>();
                    }
                    break;
                case shouldWalk: {
                        walkDirection = msg.getData<vec3>();
                    } break;
                case shouldWarp: {
                        vec3 p= msg.getData<vec3>();
                        //m_character->warp(toBullet(p));
                    }
                    break;
                case stopWalking: {
                        walkDirection = vec3(0);
                    }
                    break;
                case Rotate: {
                        orientation=msg.getData<Rotation>();
                    }
                    break;
                case shouldJump: {
                        //if(m_character->canJump())
                        jump = 1;
                        //}
                    }
                    break;
            }
    }
    virtual void update(UpdateParameter &param){//float deltaTimeMs,Actor *actor,Engine::Scene* scene) {

        btRigidBody * physicComponent = getPhysicComponent();
        if(physicComponent) {
                float walkSpeed = 10;
                //physicComponent->activate();
                btVector3 d = btVector3(walkDirection.x, physicComponent->getLinearVelocity().y(), walkDirection.z);
                physicComponent->setLinearVelocity(d*walkSpeed);
                /*
                physicComponent->activate();
                btTransform trans;
                physicComponent->getMotionState()->getWorldTransform(trans);
                btQuaternion q = toBullet(orientation.q);
                q = q.normalize();
                physicComponent->setCenterOfMassTransform(btTransform(q, trans.getOrigin()));
                */
            }
        /*

        if(jump) {

        //m_character->jump();
        jump=0;
        jumping=1;
        }
        if(onGround()&&jumping) {
        jumping=0;
        Event msg;
        msg.type=isOnGround;
        this->sendLocalEvent(msg);
        }
        */

    }

};





/*

class CharacterPhysicsComponent : public PhysicsComponent
{
    ENGINE_COMPONENT_MACRO
    vec3 walkDirection;
    Rotation orientation;
    bool jump,jumping;
public:
    CharacterPhysicsComponent(float radius = 1.0f) {
        //mRadius=radius;
        //mass=1;
        dynamicsWorld = 0;
        walkDirection = vec3(0);
        jump = 0;
    }
    virtual void addToWorld(PhysicsSubSystem * sys) {
        dynamicsWorld = sys->dynamicsWorld;

        glm::vec3 p = vec3(0, 0, 0);
        btQuaternion q(0, 0, 0, 1);

        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        //vec3 s = trans->getLocalScale(ComponentId::Physic);
                        //collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));

                        p = trans->getLocalPos(ComponentId::Physic);
                        q = toBullet(trans->getLocalRotation(ComponentId::Physic).getQuaternion());

                        //q=Engine::toBullet(glm::quat_cast(trans->getMatrix()));
                    }
            }

        btTransform startTransform;
        startTransform.setIdentity();
        //startTransform.setOrigin (btVector3(0.0, 4.0, 0.0));
        startTransform.setOrigin(toBullet(p));


        m_ghostObject = new btPairCachingGhostObject();
        m_ghostObject->setWorldTransform(startTransform);
        sys->broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
        btScalar characterHeight = 0.8;
        btScalar characterWidth = 1.;
        btConvexShape * capsule = new btCapsuleShape(characterWidth, characterHeight);
        m_ghostObject->setCollisionShape(capsule);
        m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

        btScalar stepHeight = btScalar(0.35);
        m_character = new btKinematicCharacterController(m_ghostObject, capsule, stepHeight);

        dynamicsWorld->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

        dynamicsWorld->addAction(m_character);

        m_character->setJumpSpeed(15);

    }

    virtual void handleEvent(Event &msg) {

        switch(msg.getType()) {
                case shouldMove: {
                        walkDirection = msg.getData<vec3>();
                    }
                    break;
                case shouldWalk: {
                        walkDirection = msg.getData<vec3>();
                    } break;
                case shouldWarp: {
                        vec3 p= msg.getData<vec3>();
                        m_character->warp(toBullet(p));
                    }
                    break;
                case stopWalking: {
                        walkDirection = vec3(0);
                    }
                    break;
                case Rotate: {
                        orientation=msg.getData<Rotation>();
                    }
                    break;
                case shouldJump: {
                        if(m_character->canJump())
                            jump = 1;
                        //}
                    }
                    break;
            }
    }
    virtual void update(float deltaTimeMs,Actor *actor,Engine::DefaultSceneState* scene) {
        float walkSpeed = 1;
        m_character->setWalkDirection(toBullet(walkDirection) * walkSpeed);

        btQuaternion q = toBullet(orientation.q);
        q = q.normalize();
        btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
        orn = btMatrix3x3(q);
        m_ghostObject->getWorldTransform().setBasis(orn);


        if(jump) {

                m_character->jump();
                jump=0;
                jumping=1;
            }
        if(m_character->onGround()&&jumping) {
                jumping=0;
                Event msg;
                msg.setType(isOnGround);
                this->sendLocalEvent(msg);
            }


    }
    virtual void updateMatrix() {
        TransformComponent * trans;
        if(m_pOwner)trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
        if(trans)trans->setMatrix(getTransform());
 

    }


    virtual mat4 getTransform() {
        btTransform trans = m_ghostObject->getWorldTransform();
        //physicComponent->getMotionState()->getWorldTransform(trans);
        float m[16] = {0};
        trans.getOpenGLMatrix(m);

        mat4 mvm = glm::make_mat4(m);
        return mvm;
    }
private:

    btKinematicCharacterController * m_character;
    class	btPairCachingGhostObject * m_ghostObject;

    btDynamicsWorld * dynamicsWorld;
    btCollisionShape * getCollisionShape() {


        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        vec3 s = trans->getLocalScale(ComponentId::Physic);
                        return new btSphereShape(s.x);
                    }
            }
        return new btSphereShape(1);//mRadius);
    }

};
*/

class CharacterKinematicPhysicsComponent : public PhysicsComponent
{
    ENGINE_COMPONENT_MACRO
    vec3 walkDirection;
    Rotation orientation;
    bool jump,jumping;
public:
    CharacterKinematicPhysicsComponent(float radius = 1.0f) {
        //mRadius=radius;
        //mass=1;
        dynamicsWorld = 0;
        walkDirection = vec3(0);
        jump = 0;
    }
    virtual void addToWorld(PhysicsSubSystem * sys) {
        dynamicsWorld = sys->dynamicsWorld;

        glm::vec3 p = vec3(0, 0, 0);
        btQuaternion q(0, 0, 0, 1);

        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        //vec3 s = trans->getLocalScale(ComponentId::Physic);
                        //collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));

                        p = trans->getLocalPos(ComponentId::Physic);
                        q = toBullet(trans->getLocalRotation(ComponentId::Physic).getQuaternion());

                        //q=Engine::toBullet(glm::quat_cast(trans->getMatrix()));
                    }
            }

        btTransform startTransform;
        startTransform.setIdentity();
        //startTransform.setOrigin (btVector3(0.0, 4.0, 0.0));
        startTransform.setOrigin(toBullet(p));


        m_ghostObject = new btPairCachingGhostObject();
        m_ghostObject->setWorldTransform(startTransform);
        sys->broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
        btScalar characterHeight = 0.8;
        btScalar characterWidth = 1.;
        btConvexShape * capsule = new btCapsuleShape(characterWidth, characterHeight);
        m_ghostObject->setCollisionShape(capsule);
        m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

        btScalar stepHeight = btScalar(0.35);
        m_character = new btCustomKinematicCharacterController(m_ghostObject, capsule, stepHeight);

        dynamicsWorld->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

        dynamicsWorld->addAction(m_character);

        m_character->setJumpSpeed(15);

    }

    virtual void handleEvent(Event &msg) {

        switch(msg.getType()) {
                case shouldMove: {
                        walkDirection = msg.getData<vec3>();
                    }
                    break;
                case shouldWalk: {
                        walkDirection = msg.getData<vec3>();
                    } break;
                case shouldWarp: {
                        vec3 p= msg.getData<vec3>();
                        m_character->warp(toBullet(p));
                    }
                    break;
                case stopWalking: {
                        walkDirection = vec3(0);
                    }
                    break;
                case Rotate: {
                        orientation=msg.getData<Rotation>();
                    }
                    break;
                case shouldJump: {
                        if(m_character->canJump())
                            jump = 1;
                        //}
                    }
                    break;
            }
    }
    virtual void update(UpdateParameter &param){//float deltaTimeMs,Actor *actor,Engine::Scene* scene) {
        float walkSpeed = 1;
        m_character->setWalkDirection(toBullet(walkDirection) * walkSpeed);

        


        if(jump) {

                m_character->jump();
                jump=0;
                jumping=1;
            }
        if(m_character->onGround()&&jumping) {
                jumping=0;
                Event msg;
                msg.setType(isOnGround);
                this->sendLocalEvent(msg);
            }
		
			
		float rotOffset=0;	
		static float rotX=0;
		float newRotX=atan2(walkDirection.x,walkDirection.z)+3.14f;
		//if(rotX>4.71&&newRotX<1.57)rotX=rotX-6.28;
		//if(newRotX>4.71&&rotX<1.57)rotX=rotX+6.28;
		if(rotX>4.71&&newRotX<1.57)rotX=rotX-6.28;
		if(newRotX>4.71&&rotX<1.57)rotX=rotX+6.28;

		if(walkDirection!=vec3(0))
		rotX=glm::mix(rotX,newRotX,0.2f);
	
		btQuaternion q = btQuaternion(btVector3(0,1,0),rotX-3.14f);//toBullet(orientation.q);
	    q = q.normalize();
        btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
        orn = btMatrix3x3(q);
        m_ghostObject->getWorldTransform().setBasis(orn);

    }
    virtual void updateMatrix() {
        TransformComponent * trans;
        if(m_pOwner)trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
        if(trans)trans->setMatrix(getTransform());


    }


    virtual mat4 getTransform() {
        btTransform trans = m_ghostObject->getWorldTransform();
        //physicComponent->getMotionState()->getWorldTransform(trans);
        float m[16] = {0};
        trans.getOpenGLMatrix(m);

        mat4 mvm = glm::make_mat4(m);
        return mvm;
    }
private:

    btCustomKinematicCharacterController * m_character;
    class	btPairCachingGhostObject * m_ghostObject;

    btDynamicsWorld * dynamicsWorld;
    btCollisionShape * getCollisionShape() {


        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        vec3 s = trans->getLocalScale(ComponentId::Physic);
                        return new btSphereShape(s.x);
                    }
            }
        return new btSphereShape(1);//mRadius);
    }

};

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




class CharacterDynamicPhysicsComponent : public PhysicsComponent
{
    ENGINE_COMPONENT_MACRO
protected:
    vec3 walkDirection;
    Rotation orientation;
    bool jump,jumping;
    btScalar mass;
    btRigidBody * physicComponent;
    btDynamicsWorld * dynamicsWorld;
public:

	float speed;
	float jumpHeight;
	
    virtual void updateMatrix() {
        btTransform trans;
        physicComponent->getMotionState()->getWorldTransform(trans);
        //trans.setOrigin(toBullet(position));

        TransformComponent * t;
        if(m_pOwner)t = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
        if(t) {
                t->setMatrix(getTransform());
            }
    }
    virtual btRigidBody * getPhysicsObject() {
        return physicComponent;
    }

    CharacterDynamicPhysicsComponent(float radius = 1.0f) {
        //mRadius=radius;
        //mass=1;
        mass = 1;
        physicComponent = 0;
        //dynamicsWorld = 0;
        walkDirection = vec3(0);
        jump = 0;
        speed=30;
        jumpHeight=4;
        
    }

    virtual void handleEvent(Event &msg) {

        switch(msg.getType()) {
                case shouldMove: {
                        walkDirection = msg.getData<vec3>();
                    }
                    break;
                case shouldWalk: {
                        walkDirection = msg.getData<vec3>();
                    } break;
                case shouldWarp: {
                        vec3 p= msg.getData<vec3>();
                        //m_character->warp(toBullet(p));
                    }
                    break;
                case stopWalking: {
                        walkDirection = vec3(0);
                    }
                    break;
                case Rotate: {
                        orientation=msg.getData<Rotation>();
                    }
                    break;
                case shouldJump: {
                        //if(m_character->canJump())
                        jump = 1;
                        //}
                    }
                    break;
            }
    }
    vec3 position;
    virtual void update(UpdateParameter &param){//float deltaTimeMs,Actor *actor,Engine::Scene* scene) {
    	//physicComponent->set
		physicComponent->activate();
		btTransform trans,from,to;
        btMotionState *motionState=physicComponent->getMotionState();
        motionState->getWorldTransform(trans);
        from=trans;
        from.setOrigin(from.getOrigin()+btVector3(0,1,0));
        to=from;
        //to.setOrigin(to.getOrigin()+btVector3(0,-10,0));
        to.setOrigin(to.getOrigin()+btVector3(0,-2.5,0));

		vec3 pos=fromBullet(trans.getOrigin());//vec3(trans.getOrigin().getX(),test.m_hitPointWorld.getY(),trans.getOrigin().getZ());
        	
		RenderSphere(pos,1,1/60.0f);

		static float time=0;
        static float velocity=0;
        static btConvexShape *shape=getCollisionShape();
        static bool init=0;
        if(!init){
			init=1;
			//shape->setLocalScaling(btVector3(0.2,0.2,0.2));
		}
        
        //btClosestNotMeConvexResultCallback  test(physicComponent);//,from.getOrigin(),to.getOrigin(),dynamicsWorld->getPairCache(),dynamicsWorld->getDispatcher());
        //dynamicsWorld->convexSweepTest(shape,from,to,test);
//		btCollisionWorld::ClosestRayResultCallback test(from.getOrigin(),to.getOrigin());
		float maxH=-1;
		vec3 maxPos=pos;
		float timestep=param.deltaTimeMs/1000.0f;	
		time+=timestep;
		
		/*{
			btClosestNotMeRayResultCallback test(physicComponent,from.getOrigin(),to.getOrigin());
			dynamicsWorld->rayTest(from.getOrigin(),to.getOrigin(),test);
			if(test.hasHit())if(maxH<test.m_hitPointWorld.getY())maxH=test.m_hitPointWorld.getY();
		}*/
		for(float a=0;a<6.26 ;a+=0.392){
			float hM=-0.3,hm=-2.6;
			vec3 p=pos+vec3(sin(a),0,cos(a))/2.0f;
			btClosestNotMeRayResultCallback test(physicComponent,toBullet(p+vec3(0,hM,0)),toBullet(p+vec3(0,hm,0)));
			dynamicsWorld->rayTest(toBullet(p+vec3(0,hM,0)),toBullet(p+vec3(0,hm,0)),test);
			if(test.hasHit())if(maxH<test.m_hitPointWorld.getY()){maxH=test.m_hitPointWorld.getY();maxPos=fromBullet(test.m_hitPointWorld);}
			RenderLine(p+vec3(0,hM,0),p+vec3(0,hm,0),1/60.0f);
		}
		
		static float hTarget=pos.y;//trans.getOrigin().getY();
		static float maxH_last;
		
        if(maxH!=-1){ //on ground
        	if(maxH_last==-1){ //before was not on ground
        		Debug("on ground now");
				jumping=0;
				Event msg;
				msg.setType(isOnGround);
				this->sendLocalEvent(msg);
			}
        	RenderSphere(maxPos,0.1,1.0f);
        	//RenderLine(fromBullet(test.m_hitPointWorld),fromBullet(from.getOrigin()),10,0);	
			hTarget=glm::mix(maxH+2.f,hTarget,0.6f);
			time=0.7;
			velocity=0;
			if(!jumping){
				time=0.7;
				velocity=0;
			}
			if(jump&&!jumping) {
				time=0.7;
				velocity=36.1;
				jump=0;
				jumping=1;
			}	else{
				hTarget=glm::mix(maxH+2.f,hTarget,0.6f);	
			}
			
		}else{
			
			velocity-=timestep * 10.0f;
			//hTarget-=timestep * (velocity + timestep * 10.0f / 2.0f);
			/*
			time+=timestep;
			float temp = 10.0f * time;
			hTarget -= time*(velocity + temp/2.0f);
			velocity = velocity + temp;*/
		}
        maxH_last=maxH;
        
        
		
		
		
		hTarget+=timestep * velocity;//(velocity + timestep * 10.0f / 2.0f);
		
		
        
		float walkSpeed = speed;
        //physicComponent->setLinearVelocity(btVector3(0,0,0));
        physicComponent->setLinearVelocity(toBullet(walkDirection) * walkSpeed*btVector3(1,0,1));//+btVector3(0,velocity+physicComponent->getLinearVelocity().getY(),0));

		
		static float rotX=0;
		float newRotX=atan2(walkDirection.x,walkDirection.z)+3.14f;
		if(rotX>4.71&&newRotX<1.57)rotX=rotX-6.28;
		if(newRotX>4.71&&rotX<1.57)rotX=rotX+6.28;

		if(walkDirection!=vec3(0))
		rotX=glm::mix(rotX,newRotX,0.2f);
	
		btQuaternion q = btQuaternion(btVector3(0,1,0),rotX+3.14f);//toBullet(orientation.q);
	    q = q.normalize();
	    
	    
	    physicComponent->getMotionState()->getWorldTransform(trans);
	    
	    trans.setBasis(btMatrix3x3(q));
	    trans.setOrigin(btVector3(trans.getOrigin().getX(),hTarget,trans.getOrigin().getZ()));
	    
	    physicComponent->setWorldTransform(trans);
	    
	   // physicComponent->getWorldTransform().setBasis(orn);
		//physicComponent->getWorldTransform().setOrigin(btVector3(trans.getOrigin().getX(),hTarget,trans.getOrigin().getZ()));
		return;
  //       
		// physicComponent->activate();
// 
  //       
  //       btTransform trans,from,to;
  //       btMotionState *motionState=physicComponent->getMotionState();
  //       motionState->getWorldTransform(trans);
  //       from=trans;
  //       from.setOrigin(from.getOrigin()+btVector3(0,1,0));
  //       to=from;
  //       to.setOrigin(to.getOrigin()+btVector3(0,-2,0));
// 
// 
// 
// 
  //       btClosestNotMeConvexResultCallback  test(physicComponent);//,from.getOrigin(),to.getOrigin(),dynamicsWorld->getPairCache(),dynamicsWorld->getDispatcher());
  //       dynamicsWorld->convexSweepTest((btConvexShape*)(getCollisionShape()),from,to,test);
  //       /*
  //       while(test.hasHit()&&test.m_hitCollisionObject==physicComponent){
  //       from.setOrigin(test.m_hitPointWorld+btVector3(0,-0.1,0));
  //       to=from;
  //       to.setOrigin(to.getOrigin()+btVector3(0,-10,0));
  //       dynamicsWorld->convexSweepTest((btConvexShape*)(new btSphereShape(0.1)),from,to,test);
  //       }*/
// 
// 
  //       static float time=0;
  //       static float velocity=0;
		// 
  //       if(test.hasHit()) {
  //       		/*
		// 		if(test.m_hitNormalWorld.getY()>0.5){
		// 			btVector3 pos=to.getOrigin();
		// 			pos.setY(test.m_hitPointWorld.getY()+1.);//+btVector3(0,2.5,0)
					//pos+=(toBullet(walkDirection)*1.f*btVector3(1,0,1));
		// 			pos.setInterpolate3(trans.getOrigin(),pos,0.3);
		// 			position=fromBullet(pos);
					//to.setOrigin(pos);
					//motionState->setWorldTransform(to);
					//physicComponent->setMotionState(motionState);
		// 			time=0.9;
		// 			velocity=0;
		// 		}else{
		// 			getCollisionShape()->setLocalScaling(btVector3(0.2,0.2,0.2));
		// 			btClosestNotMeConvexResultCallback  test(physicComponent);//,from.getOrigin(),to.getOrigin(),dynamicsWorld->getPairCache(),dynamicsWorld->getDispatcher());
		// 	        dynamicsWorld->convexSweepTest((btConvexShape*)(getCollisionShape()),from,to,test);
		// 	        
		// 	        if(test.hasHit()) {
		// 					if(test.m_hitNormalWorld.getY()>0.5){
		// 						btVector3 pos=to.getOrigin();
		// 						pos.setY(test.m_hitPointWorld.getY()+1.);//+btVector3(0,2.5,0)
								//pos+=(toBullet(walkDirection)*1.f*btVector3(1,0,1));
		// 						pos.setInterpolate3(trans.getOrigin(),pos,0.3);
		// 						position=fromBullet(pos);
								//to.setOrigin(pos);
								//motionState->setWorldTransform(to);
								//physicComponent->setMotionState(motionState);
		// 						time=0.9;
		// 						velocity=0;
		// 					}
		// 	            }
		// 		}*/
  //           }
  //       else {
  //               time+=deltaTimeMs;
  //               velocity-=3*time*time;
// 
  //           }
  //       /*    
  //       if(jump) {
  //       		time=0.8;
  //       		velocity=10;
  //               jump=0;
  //               jumping=1;
  //           }*/
  //       if(jumping&&onGround()) {
  //           jumping=0;
  //           Event msg;
  //           msg.setType(isOnGround);
  //           this->sendLocalEvent(msg);
  //       }    
// 
        //*/
  //       float walkSpeed = speed;
        //physicComponent->setLinearVelocity(btVector3(0,0,0));
  //       physicComponent->setLinearVelocity(toBullet(walkDirection) * walkSpeed*btVector3(1,0,1)+btVector3(0,velocity+physicComponent->getLinearVelocity().getY(),0));
// 
		// 
		// static float rotX=0;
		// float newRotX=atan2(walkDirection.x,walkDirection.z)+3.14f;
		// if(rotX>4.71&&newRotX<1.57)rotX=rotX-6.28;
		// if(newRotX>4.71&&rotX<1.57)rotX=rotX+6.28;
// 
		// if(walkDirection!=vec3(0))
		// rotX=glm::mix(rotX,newRotX,0.2f);
	// 
		// btQuaternion q = btQuaternion(btVector3(0,1,0),rotX);//toBullet(orientation.q);
	 //    q = q.normalize();
	 //    btMatrix3x3 orn = physicComponent->getWorldTransform().getBasis();
	 //    orn = btMatrix3x3(q);
	 //    physicComponent->getWorldTransform().setBasis(orn);
// 
		// 
		// 
		

    }

protected:
    virtual btConvexShape * getCollisionShape() {
        static 	btConvexShape*s=new btCylinderShape(btVector3(1,1,1));//btSphereShape(1.0f)
        return s;

    }
    bool onGround(){
		btTransform trans,from,to;
        btMotionState *motionState=physicComponent->getMotionState();
        motionState->getWorldTransform(trans);
        from=trans;
        to=from;
        to.setOrigin(to.getOrigin()+btVector3(0,-1,0));




        btClosestNotMeConvexResultCallback  test(physicComponent);//,from.getOrigin(),to.getOrigin(),dynamicsWorld->getPairCache(),dynamicsWorld->getDispatcher());
        dynamicsWorld->convexSweepTest((btConvexShape*)(getCollisionShape()),from,to,test);

        return test.hasHit()?1:0;
		
	}
    virtual void postActorSetInit() {
        //Debug("bullet post actor set");
        btCollisionShape * collisionShape = getCollisionShape();
        btVector3 Inertia;

        glm::vec3 p = vec3(0, 0, 0);
        btQuaternion q(0, 0, 0, 1);

        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        vec3 s = trans->getLocalScale(ComponentId::Physic);
                        collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));

                        p = trans->getLocalPos(ComponentId::Physic);
                        q = toBullet(trans->getLocalRotation(ComponentId::Physic).getQuaternion());

                        //q=Engine::toBullet(glm::quat_cast(trans->getMatrix()));
                    }
            }
        //glm::vec3 s=glm::vec3(matrix[0][0],matrix[1][1],matrix[2][2]);
        //glm::vec3(matrix[3][0],matrix[3][1],matrix[3][2]);
        //collisionShape->setLocalScaling(btVector3(s.x,s.y,s.z));

        btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(q, btVector3(p.x, p.y, p.z)));
        collisionShape->calculateLocalInertia(mass, Inertia);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, motionState, collisionShape, Inertia);
        physicComponent = new btRigidBody(fallRigidBodyCI);
        physicComponent->setUserPointer((void *)this);
        physicComponent->setRestitution(0.0);
        physicComponent->setAngularFactor(0.0);
        physicComponent->setFlags(physicComponent->getFlags() | DISABLE_DEACTIVATION);
        
        
    }
    PhysicsSubSystem * sys;
    virtual void addToWorld(PhysicsSubSystem * sys) {
        this->sys=sys;
        sys->dynamicsWorld->addRigidBody(physicComponent);
        dynamicsWorld=sys->dynamicsWorld;
    }
    virtual void move(vec3 dir) {
        physicComponent->activate();
        btVector3 d = btVector3(dir.x, dir.y, dir.z);
        physicComponent->setLinearVelocity(d);
    }
    virtual mat4 getTransform() {
        btTransform trans;
        physicComponent->getMotionState()->getWorldTransform(trans);
        //trans.setOrigin(toBullet(position));

        float m[16] = {0};
        trans.getOpenGLMatrix(m);

        mat4 mvm = glm::make_mat4(m);

        return mvm;
    }





};

#include "../Physic/CharacterController.h"

class CharacterPhysicsComponent : public PhysicsComponent
{
    ENGINE_COMPONENT_MACRO
    vec3 walkDirection;
    Rotation orientation;
    bool jump,jumping;
public:
    CharacterPhysicsComponent(float radius = 1.0f) {
        //mRadius=radius;
        //mass=1;
        dynamicsWorld = 0;
        walkDirection = vec3(0);
        jump = 0;
    }
    virtual void addToWorld(PhysicsSubSystem * sys) {
        dynamicsWorld = sys->dynamicsWorld;

        glm::vec3 p = vec3(0, 0, 0);
        btQuaternion q(0, 0, 0, 1);

        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        //vec3 s = trans->getLocalScale(ComponentId::Physic);
                        //collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));

                        p = trans->getLocalPos(ComponentId::Physic);
                        q = toBullet(trans->getLocalRotation(ComponentId::Physic).getQuaternion());

                        //q=Engine::toBullet(glm::quat_cast(trans->getMatrix()));
                    }
            }

        btTransform startTransform;
        startTransform.setIdentity();
        //startTransform.setOrigin (btVector3(0.0, 4.0, 0.0));
        startTransform.setOrigin(toBullet(p));


        m_ghostObject = new btPairCachingGhostObject();
        m_ghostObject->setWorldTransform(startTransform);
        sys->broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
        btScalar characterHeight = 0.8;
        btScalar characterWidth = 1.;
        btConvexShape * capsule = new btCapsuleShape(characterWidth, characterHeight);
        m_ghostObject->setCollisionShape(capsule);
        m_ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

        btScalar stepHeight = btScalar(0.35);
        m_character = new CharacterController_Physics(m_ghostObject, capsule, stepHeight,dynamicsWorld);

        dynamicsWorld->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);

        dynamicsWorld->addAction(m_character);

        m_character->setJumpSpeed(15);

    }

    virtual void handleEvent(Event &msg) {

        switch(msg.getType()) {
                case shouldMove: {
                        walkDirection = msg.getData<vec3>();
                    }
                    break;
                case shouldWalk: {
                        walkDirection = msg.getData<vec3>();
                    } break;
                case shouldWarp: {
                        vec3 p= msg.getData<vec3>();
                        m_character->warp(toBullet(p));
                    }
                    break;
                case stopWalking: {
                        walkDirection = vec3(0);
                    }
                    break;
                case Rotate: {
                        orientation=msg.getData<Rotation>();
                    }
                    break;
                case shouldJump: {
                        if(m_character->canJump())
                            jump = 1;
                        //}
                    }
                    break;
            }
    }
    virtual void update(UpdateParameter &param){//float deltaTimeMs,Actor *actor,Engine::Scene* scene) {
        float walkSpeed = 1;
        m_character->setWalkDirection(toBullet(walkDirection) * walkSpeed);

        


        if(jump) {

                m_character->jump();
                jump=0;
                jumping=1;
            }
        if(m_character->onGround()&&jumping) {
                jumping=0;
                Event msg;
                msg.setType(isOnGround);
                this->sendLocalEvent(msg);
            }
		
			
		float rotOffset=0;	
		static float rotX=0;
		float newRotX=atan2(walkDirection.x,walkDirection.z)+3.14f;
		//if(rotX>4.71&&newRotX<1.57)rotX=rotX-6.28;
		//if(newRotX>4.71&&rotX<1.57)rotX=rotX+6.28;
		if(rotX>4.71&&newRotX<1.57)rotX=rotX-6.28;
		if(newRotX>4.71&&rotX<1.57)rotX=rotX+6.28;

		if(walkDirection!=vec3(0))
		rotX=glm::mix(rotX,newRotX,0.2f);
	
		btQuaternion q = btQuaternion(btVector3(0,1,0),rotX-3.14f);//toBullet(orientation.q);
	    q = q.normalize();
        btMatrix3x3 orn = m_ghostObject->getWorldTransform().getBasis();
        orn = btMatrix3x3(q);
        m_ghostObject->getWorldTransform().setBasis(orn);

    }
    virtual void updateMatrix() {
        TransformComponent * trans;
        if(m_pOwner)trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
        if(trans)trans->setMatrix(getTransform());


    }


    virtual mat4 getTransform() {
        btTransform trans = m_ghostObject->getWorldTransform();
        //physicComponent->getMotionState()->getWorldTransform(trans);
        float m[16] = {0};
        trans.getOpenGLMatrix(m);

        mat4 mvm = glm::make_mat4(m);
        return mvm;
    }
private:

    CharacterController_Physics * m_character;
    class	btPairCachingGhostObject * m_ghostObject;

    btDynamicsWorld * dynamicsWorld;
    btCollisionShape * getCollisionShape() {


        if(m_pOwner) {
                TransformComponent * trans;
                trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
                if(trans) {
                        vec3 s = trans->getLocalScale(ComponentId::Physic);
                        return new btSphereShape(s.x);
                    }
            }
        return new btSphereShape(1);//mRadius);
    }

};



/*

class BulletSpherePhysicsComponent : public PhysicsComponent
{
ENGINE_COMPONENT_MACRO
protected:
btScalar mass;
btRigidBody * physicComponent;
btDynamicsWorld * dynamicsWorld;
public:
BulletSpherePhysicsComponent() {
mass = 0;
physicComponent = 0;
}


virtual void updateMatrix() {
TransformComponent * trans;
if(m_pOwner)trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
if(trans)trans->setMatrix(getTransform());
}
virtual btRigidBody * getPhysicsObject() {
return physicComponent;
}

virtual void update(float deltaTimeMs){
btTransform from,to;
btMotionState *motionState=physicComponent->getMotionState();
motionState->getWorldTransform(from);
to=from;
to.setOrigin(to.getOrigin()+btVector3(0,-5,0));




btCollisionWorld::ClosestConvexResultCallback test(from.getOrigin(),to.getOrigin());
dynamicsWorld->convexSweepTest(getCollisionShape(),from,to,test);

if(test.hasHit()){

to.setOrigin(test.m_hitPointWorld);
motionState->setWorldTransform(to);
physicComponent->setMotionState(motionState);
}
}
protected:
virtual btConvexShape * getCollisionShape(){return new btSphereShape(1);}
virtual void postActorSetInit() {
//Debug("bullet post actor set");
btCollisionShape * collisionShape = getCollisionShape();
btVector3 Inertia;

glm::vec3 p = vec3(0, 0, 0);
btQuaternion q(0, 0, 0, 1);

if(m_pOwner) {
TransformComponent * trans;
trans = m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
if(trans) {
vec3 s = trans->getLocalScale(ComponentId::Physic);
collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));

p = trans->getLocalPos(ComponentId::Physic);
q = toBullet(trans->getLocalRotation(ComponentId::Physic).getQuaternion());

//q=Engine::toBullet(glm::quat_cast(trans->getMatrix()));
}
}
//glm::vec3 s=glm::vec3(matrix[0][0],matrix[1][1],matrix[2][2]);
//glm::vec3(matrix[3][0],matrix[3][1],matrix[3][2]);
//collisionShape->setLocalScaling(btVector3(s.x,s.y,s.z));

btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(q, btVector3(p.x, p.y, p.z)));
collisionShape->calculateLocalInertia(mass, Inertia);
btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, motionState, collisionShape, Inertia);
physicComponent = new btRigidBody(fallRigidBodyCI);
physicComponent->setUserPointer((void *)this);
physicComponent->setRestitution(0.5);
}

virtual void addToWorld(PhysicsSubSystem * sys) {
dynamicsWorld=sys->dynamicsWorld;
dynamicsWorld->addRigidBody(physicComponent);
}
virtual void move(vec3 dir) {
physicComponent->activate();
btVector3 d = btVector3(dir.x, dir.y, dir.z);
physicComponent->setLinearVelocity(d);
}
virtual mat4 getTransform() {
btTransform trans;
physicComponent->getMotionState()->getWorldTransform(trans);
float m[16] = {0};
trans.getOpenGLMatrix(m);

mat4 mvm = glm::make_mat4(m);
return mvm;
}

};
*/
