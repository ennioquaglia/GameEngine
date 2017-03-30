

#include "PhysicsComponent.h"
#include "TransformComponent.h"

#include <BulletCollision\CollisionDispatch\btGhostObject.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "physics.h"
#include "Debug.h"
#include "Scene.h"
#include "AssimpScene.h"

#include <ostream>

namespace Engine{


PhysicsComponent::PhysicsComponent()
{
    m_Flag.add(Object::physics);
}



BulletPhysicsSystem::BulletPhysicsSystem():System(){Init();}
BulletPhysicsSystem::~BulletPhysicsSystem(){
    Destroy();
}

void BulletPhysicsSystem::Init(){

    QENGINE_INFO("BULLET VERSION: " + btGetVersion());
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver;
    /*btVector3 worldMin(-1000,-1000,-1000);
        btVector3 worldMax(1000,1000,1000);
        btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
        m_overlappingPairCache = sweepBP;*/
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));


    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
    btOverlappingPairCallback *ghostPairCallback = new btGhostPairCallback();
    dynamicsWorld->getPairCache()->setInternalGhostPairCallback(ghostPairCallback);


}

void BulletPhysicsSystem::addComponent(Object * obj)
{
    if (!obj)return;

    if (!(obj->getFlag().contains(Object::physics)))return;
    PhysicsComponent *c = static_cast<PhysicsComponent*>(obj);

    m_Components.push_back(c);

    QENGINE_INFO("Added a PhysicsComponent!");
}


void BulletPhysicsSystem::Update(UpdateParameter &param)
{

    const double dt = 1 / 60.f;
    static float accumulator = 0;
    accumulator += param.deltaTime ;

    Object::UpdateParameter param1(param.scene,param.input,dt);//=param;
    //	param1.scene = _Scene;
    //    param1.deltaTime = dt;
    //	param1.input = _Scene->window->inputManager;

    while(accumulator >= dt) {
        for(Components::iterator it = m_Components.begin(); it != m_Components.end(); it++)
            (*it)->Update(param1);
        //            for(int i = 0; i < (it->second).size(); i++) {
        //                param1.actor = (it->second)[i]->getOwner();
        //                (it->second)[i]->Update(param1);//dt,(it->second)[i]->getOwner(),_Scene);
        //            }
        //dynamicsWorld->stepSimulation(dt,5);
        accumulator -= dt;
    }
    dynamicsWorld->stepSimulation((param.deltaTime ), 0);//20);
    for(Components::iterator it = m_Components.begin(); it != m_Components.end(); it++)
        (*it)->updateMatrix();
    //        for(int i = 0; i < (it->second).size(); i++)
    //            (it->second)[i]->updateMatrix();


}



void BulletPhysicsSystem::Destroy(){
    delete dynamicsWorld;
    delete solver;
    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
}






void BulletPhysicsComponent::update_cache(){
    Entity *e=getEntity();
    if(e){
        trans=e->getObjectByType<TransformComponent>();
        if(trans)dirty=0;
    }
}

void BulletPhysicsComponent::setMass(float value){
    mass=value;//>=0?value:0;
}
void BulletPhysicsComponent::updateMatrix() {
    TransformComponent * trans=0;
    Entity *e=getEntity();
    if(e)trans=e->getObjectByType<TransformComponent>();
    if(trans){
        //        trans->setMatrix(getTransform());
        btTransform tr;
        physicComponent->getMotionState()->getWorldTransform(tr);
        trans->setPosition(fromBullet(tr.getOrigin()));
        trans->setRotation(fromBullet(tr.getRotation()));
    }
}
btRigidBody * BulletPhysicsComponent::getPhysicsObject() {
    return physicComponent;
}

void BulletPhysicsComponent::OnAttached(Entity &e) {

    btCollisionShape * collisionShape = getCollisionShape();
    btVector3 Inertia;

    glm::vec3 p = glm::vec3(0, 0, 0);
    btQuaternion q(0, 0, 0, 1);


//    TransformComponent * trans;
//    trans = e.getObjectByType<TransformComponent>();
    update_cache();

    if(!dirty) {
        glm::vec3 s = trans->getLocalScale(Object::physics,1);
        collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));

        p = trans->getLocalPosition(Object::physics,1);
        q = toBullet(trans->getLocalRotation(Object::physics,1));

        //q=Engine::toBullet(glm::quat_cast(trans->getMatrix()));
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

void BulletPhysicsComponent::OnScene(Scene &scene) {
    BulletPhysicsSystem* system = scene.getObjectByType<BulletPhysicsSystem>();
    if (system){
        system->addComponent(this);
        system->dynamicsWorld->addRigidBody(physicComponent);
        QENGINE_INFO("BulletPhysicsComponent::OnScene ok!");
    }
}
void BulletPhysicsComponent::move(glm::vec3 dir) {
    physicComponent->activate();
    btVector3 d = btVector3(dir.x, dir.y, dir.z);
    physicComponent->setLinearVelocity(d);
}
glm::mat4 BulletPhysicsComponent::getTransform() {
    btTransform trans;
    physicComponent->getMotionState()->getWorldTransform(trans);
    float m[16] = {0};
    trans.getOpenGLMatrix(m);

    glm::mat4 mvm = glm::make_mat4(m);
    return mvm;
}


btCollisionShape * SpherePhysicsComponent::getCollisionShape() {

    Entity *e=getEntity();
    if(e) {
        TransformComponent * trans;
        trans = e->getObjectByType<TransformComponent>();
        if(trans) {
            glm::vec3 s = trans->getScale();
            return new btSphereShape(s.x);
        }
    }
    return new btSphereShape(mRadius);
}



void StaticMeshPhysicsComponent::serializeXML(std::ostream &stream,unsigned level) const{
    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"<"
         <<getTypeName()<<" "
        <<SERIALIZE_VAR_PATH(m_Filename)
       <<" >\n";

    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"</"<<getTypeName()<<">\n";
}

void StaticMeshPhysicsComponent::recursiveFoo(const struct aiScene * sc, const struct aiNode * nd, aiMatrix4x4 matrix) {
    unsigned int i;
    unsigned int n = 0, t;
    aiMatrix4x4 m = nd->mTransformation * matrix;
    // draw all meshes assigned to this node
    for(; n < nd->mNumMeshes; ++n) {
        const struct aiMesh * mesh = sc->mMeshes[nd->mMeshes[n]];
        for(t = 0; t < mesh->mNumFaces; ++t) {
            const struct aiFace * face = & mesh->mFaces[t];
            //            int face_mode;

            if(face->mNumIndices == 3)
                for(i = 0; i < face->mNumIndices; i++) {
                    int index = face->mIndices[i];
                    indices.push_back(index + points.size() / 3);
                }
        }
        for(unsigned int t = 0; t < mesh->mNumVertices; ++t) {
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


btCollisionShape * StaticMeshPhysicsComponent::getCollisionShape() {

    const aiScene * scene = (AssimpScene::load(m_Filename.toStd().data(), aiProcessPreset_TargetRealtime_Fast | aiProcess_Triangulate | aiProcess_OptimizeMeshes))->scene;

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

    //aiReleaseImport(scene);

    return shape;
}




}
