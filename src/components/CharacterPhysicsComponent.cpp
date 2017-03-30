#define GLEW_STATIC
#include <GL/glew.h>
#include "Primitive.h"

#include "CharacterPhysicsComponent.h"


#include <BulletCollision\CollisionDispatch\btGhostObject.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "physics.h"
#include "Debug.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "Hash.h"

#include "SkeletalMeshComponent.h"









//class btClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
//{
//public:
//    btClosestNotMeConvexResultCallback(btCollisionObject* me) : btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)) {
//        m_me = me;
//    }

//    virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace) {
//        if(convexResult.m_hitCollisionObject == m_me)
//            return 1.0;

//        return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
//    }
//protected:
//    btCollisionObject* m_me;
//};


//class btClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
//{
//public:
//    btClosestNotMeRayResultCallback (btCollisionObject* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
//    {
//        m_me = me;
//    }
//    btClosestNotMeRayResultCallback (btCollisionObject* me,btVector3 from,btVector3 to) : btCollisionWorld::ClosestRayResultCallback(from,to)
//    {
//        m_me = me;
//    }

//    virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,bool normalInWorldSpace)
//    {
//        if (rayResult.m_collisionObject == m_me)
//            return 1.0;

//        return ClosestRayResultCallback::addSingleResult (rayResult, normalInWorldSpace);
//    }
//protected:
//    btCollisionObject* m_me;
//};






namespace Engine{



MyCharacterDynamicPhysicsComponent::MyCharacterDynamicPhysicsComponent(float radius ) {
    mass = 1;
    physicComponent = 0;
    walkDirection = glm::vec3(0);
    isJumping=0;
    jump=0;
    onGround=0;
    isFalling=0;

    moveSpeed=30;
    jumpHeight=2;
    jumpSpeed=2;
    capsuleHeight=1.5;
    capsuleRadius=1;
    springHeight=1;
    numRays=10;
    yVelocity=0;


    m_strideWheelRadius=1;

    orientation=glm::quat();
    m_Flag.add(Object::render);
}


void MyCharacterDynamicPhysicsComponent::updateMatrix() {
    TransformComponent *trans;
    Entity *e=getEntity();
    if(e)trans=e->getObjectByType<TransformComponent>();
    if(trans){
        trans->setMatrix(getTransform());
        trans->setRotation(orientation);
    }
}
btRigidBody * MyCharacterDynamicPhysicsComponent::getPhysicsObject() {
    return physicComponent;
}



void MyCharacterDynamicPhysicsComponent::handleEvent(Event &msg) {

    switch(msg.getType()) {
    case EventType::shouldMove: {
        walkDirection = msg.getData<glm::vec3>();
        m_acceleration=walkDirection/10.0f;
    }
        break;
    case EventType::shouldWalk: {
        walkDirection = msg.getData<glm::vec3>();
    } break;
    case EventType::shouldWarp: {
        glm::vec3 p= msg.getData<glm::vec3>();
        //m_character->warp(toBullet(p));
    }
        break;
    case EventType::stopWalking: {
        walkDirection = glm::vec3(0);
    }
        break;
    case EventType::Rotate: {
        orientation=msg.getData<glm::quat>();
    }
        break;
    case EventType::shouldJump: {
        //if(m_character->canJump())
        jump = 1;
        //}
    }
        break;
    case Hash("DoInverseKinematic"):{
        break;//todo
        //m->setBonePosition("Hips",glm::vec3());
        TransformComponent* trans;


        Entity *e=getEntity();
        if(e)trans=e->getObjectByType<TransformComponent>();
        if(trans){

            SkeletalMeshComponent *m=e->getComponentByType<SkeletalMeshComponent>();
            if(m){
                m_footPosition[0] =glm::vec3(/*trans->getMatrix()**/m->getBoneMatrix("RightFoot")*glm::vec4(0,0,0,1));//trans->getMatrix()*glm::vec3(get)
                m_footPosition[1] =glm::vec3(trans->getMatrix()*m->getBoneMatrix("LeftFoot" )*glm::vec4(0,0,0,1));//trans->getMatrix()*glm::vec3(get)
                //QENGINE_INFO("bone:"+m_footPosition[0]+" "+m->getBonePosition("RightFoot"));

                for(int i=0;i<2;i++){
                    glm::vec3 target=m_footPosition[i]+glm::vec3(0,-1,0)*0.2f;
                    btClosestNotMeRayResultCallback test(physicComponent,toBullet(m_footPosition[i]),toBullet(target));
                    dynamicsWorld->rayTest(toBullet(m_footPosition[i]),toBullet(target),test);
                    if(test.hasHit())
                      ;//  m_footPosition[i]=fromBullet(test.m_hitPointWorld)+glm::vec3(0,-0.45,0);
                }

                glm::vec3 localRxFoot=glm::vec3(glm::inverse(trans->getMatrix()*m->getBoneMatrix("RightFoot"))*glm::vec4(m_footPosition[0],1));//trans->getMatrix()*glm::vec3(get)
                glm::vec3 localLxFoot=glm::vec3(glm::inverse(trans->getMatrix()*m->getBoneMatrix("LeftFoot"))*glm::vec4(m_footPosition[1],1));//trans->getMatrix()*glm::vec3(get)


                m->setBonePosition("RightFoot",localRxFoot);
                m->setBonePosition("LeftFoot",localLxFoot);

            }
        }



    }break;
    }
}
void MyCharacterDynamicPhysicsComponent::Update(UpdateParameter &param){//float deltaTimeMs,Actor *actor,Engine::Scene* scene) {


    physicComponent->activate();
    btTransform trans;
    btMotionState *motionState=physicComponent->getMotionState();
    motionState->getWorldTransform(trans);

    glm::vec3 pos=fromBullet(trans.getOrigin());
    static glm::vec3 old_pos;
    //	RenderSphere(pos,1,1/60.0f);


    float maxH=-1;
    glm::vec3 maxPos=pos;
    float timestep=param.deltaTime/1.0f;
    for(float a=0;a<6.26 ;a+=0.392){
        float hM=-0.3,hm=-2.6;
        glm::vec3 p=pos+glm::vec3(sin(a),0,cos(a))/2.0f;
        btClosestNotMeRayResultCallback test(physicComponent,toBullet(p+glm::vec3(0,hM,0)),toBullet(p+glm::vec3(0,hm,0)));
        dynamicsWorld->rayTest(toBullet(p+glm::vec3(0,hM,0)),toBullet(p+glm::vec3(0,hm,0)),test);
        if(test.hasHit())if(maxH<test.m_hitPointWorld.getY()){maxH=test.m_hitPointWorld.getY();maxPos=fromBullet(test.m_hitPointWorld);}
        //RenderLine(p+vec3(0,hM,0),p+vec3(0,hm,0),1/60.0f);
    }

    static float hTarget=pos.y;
    static float maxH_last;

    if(maxH!=-1){ //on ground
        onGround=1;
        if(maxH_last==-1){ //before was not on ground
            yVelocity=0;
            isJumping=0;
            Event msg;
            msg.setType(EventType::isOnGround);
            sendEvent(msg);
        }
        //  	RenderSphere(maxPos,0.1,1.0f);

        if(!isJumping){
            hTarget=glm::mix(maxH+2.f,hTarget,0.6f);
            yVelocity=0;
        }

    }else{
        onGround=0;
    }
    maxH_last=maxH;

    if(!onGround){ //apply gravity
        yVelocity-=timestep * 40.0f;
    }

    if(jump&&onGround&&!isJumping) {
        yVelocity=10;
        jump=0;
        isJumping=1;
    }

    hTarget+=timestep * yVelocity;//(velocity + timestep * 10.0f / 2.0f);



    float walkSpeed = moveSpeed;
    m_velocity=glm::mix(m_velocity,walkDirection*walkSpeed,0.2);
    //physicComponent->setLinearVelocity(btVector3(0,0,0));
    //physicComponent->setLinearVelocity(toBullet(walkDirection) * walkSpeed*btVector3(1,0,1));//+btVector3(0,velocity+physicComponent->getLinearVelocity().getY(),0));

    physicComponent->setLinearVelocity(toBullet(m_velocity) *btVector3(1,0,1));//+btVector3(0,velocity+physicComponent->getLinearVelocity().getY(),0));


    glm::vec3 dir=pos-old_pos;//fromBullet(physicComponent->getLinearVelocity());
    dir.y=0;
    //    //
    //    float dirVelocity=glm::length(dir);
    //    if(glm::length(dir)>0.02){
    //        static glm::vec3 old_dir;
    //        //std::swap(dir.x,dir.z);
    //        dir=glm::normalize(dir);
    //        dir.z=-dir.z;
    //        //dir=glm::mix(old_dir,dir,0.2);
    //        orientation= glm::mix(orientation,glm::normalize(glm::quat_cast(glm::lookAt(glm::vec3(0.0f),dir,glm::vec3(0,1,0)))),0.1f);
    //        old_dir=dir;
    //    }
    //    old_pos=pos;




    float dist = glm::length(dir);
    //if(dist>0.2f){
    {
        float turnAngle = glm::radians((dist / (2.0f * glm::pi<float>() * m_strideWheelRadius)) * 360.0f);

        //    //For visualization. Attach to Transform.
        //    surveyorWheel.Rotate (new Vector3 (0F, -turnAngle, 0F));

        //    angleCounter += turnAngle;

        //    if (angleCounter > stepDistance)
        //        angleCounter = 0F;

        //    animator.SetFloat("runstage", (angleCounter/stepDistance));

        //    if (animator.GetFloat ("runstage") > 1F)
        //        animator.SetFloat ("runstage", 0);

        //    lastPosition = currPosition;



        m_strideWheelRotation+=turnAngle;
        if(m_strideWheelRotation > 1.57){
            m_strideWheelRotation=0;
            m_currentFoot=!m_currentFoot;
        }



        //        glm::mat4 rot=glm::mat4_cast(orientation*glm::quat(glm::vec3(m_strideWheelRotation,0,0)));
        //        glm::vec3 target=pos+glm::vec3(rot*glm::vec4((m_currentFoot?1:-1)*feetDistance*0.5,0,1,1))*3.0f;
        //        btClosestNotMeRayResultCallback test(physicComponent,toBullet(pos),toBullet(target));
        //        dynamicsWorld->rayTest(toBullet(pos),toBullet(target),test);
        //        if(test.hasHit())
        //            m_footPosition[m_currentFoot]=fromBullet(test.m_hitPointWorld);




    }
    //if(m_strideWheelRotation>6.28319f)m_strideWheelRotation=0;
    old_pos=pos;


    //EventData<float>
    Event msg;
    msg.setType("AnimVelocity");
    msg.setData(dist);
    sendEvent(msg);



    static float rotX=0;
    float newRotX=atan2(walkDirection.x,walkDirection.z)+3.14f;
    if(rotX>4.71&&newRotX<1.57)rotX=rotX-6.28;
    if(newRotX>4.71&&rotX<1.57)rotX=rotX+6.28;

    if(walkDirection!=glm::vec3(0))
        rotX=glm::mix(rotX,newRotX,0.2f);

    btQuaternion q = btQuaternion(btVector3(0,1,0),rotX+3.14f);//toBullet(orientation.q);
    q = q.normalize();
    orientation=fromBullet(q);

    physicComponent->getMotionState()->getWorldTransform(trans);

    trans.setBasis(btMatrix3x3(q));
    trans.setOrigin(btVector3(trans.getOrigin().getX(),hTarget,trans.getOrigin().getZ()));

    physicComponent->setWorldTransform(trans);

    currentMatrix=translate(glm::mat4(1),glm::vec3(pos.x,hTarget,pos.z))*glm::mat4_cast(fromBullet(q));
    return;



}


btConvexShape * MyCharacterDynamicPhysicsComponent::getCollisionShape() {
    static 	btConvexShape*s=new btCylinderShape(btVector3(1,1,1));//btSphereShape(1.0f)
    return s;

}

void MyCharacterDynamicPhysicsComponent::OnAttached(Entity &e) {
    btCollisionShape * collisionShape = getCollisionShape();
    btVector3 Inertia;

    glm::vec3 p = glm::vec3(0, 0, 0);
    btQuaternion q(0, 0, 0, 1);


    TransformComponent * trans;
    trans = e.getObjectByType<TransformComponent>();
    if(trans) {
        glm::vec3 s = trans->getLocalScale(Object::physics);
        collisionShape->setLocalScaling(btVector3(s.x, s.y, s.z));

        p = trans->getLocalPosition(Object::physics);
        q = toBullet(trans->getLocalRotation(Object::physics));
    }



    btDefaultMotionState * motionState = new btDefaultMotionState(btTransform(q, btVector3(p.x, p.y, p.z)));
    collisionShape->calculateLocalInertia(mass, Inertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, motionState, collisionShape, Inertia);
    physicComponent = new btRigidBody(fallRigidBodyCI);
    physicComponent->setUserPointer((void *)this);
    physicComponent->setRestitution(0.0);
    physicComponent->setAngularFactor(0.0);
    physicComponent->setFlags(physicComponent->getFlags() | DISABLE_DEACTIVATION);


}
void MyCharacterDynamicPhysicsComponent::OnScene(Scene &s) {
    BulletPhysicsSystem* system = s.getObjectByType<BulletPhysicsSystem>();
    if (system){
        system->addComponent(this);
        system->dynamicsWorld->addRigidBody(physicComponent);
        dynamicsWorld=system->dynamicsWorld;
        QENGINE_INFO("MyCharacterDynamicPhysicsComponent::OnScene ok!");
    }
}
void MyCharacterDynamicPhysicsComponent::move(glm::vec3 dir) {
    physicComponent->activate();
    btVector3 d = btVector3(dir.x, dir.y, dir.z);
    physicComponent->setLinearVelocity(d);
}
glm::mat4 MyCharacterDynamicPhysicsComponent::getTransform() {
    return currentMatrix;

    btTransform trans;
    physicComponent->getMotionState()->getWorldTransform(trans);
    //trans.setOrigin(toBullet(position));

    float m[16] = {0};
    trans.getOpenGLMatrix(m);

    glm::mat4 mvm = glm::make_mat4(m);

    return mvm;
}






void MyCharacterDynamicPhysicsComponent::Draw(RenderParameter & param){
    return; //todo
    TransformComponent* trans;
    trans=getEntity()->getObjectByType<TransformComponent>();
    if(trans){
        glPushMatrix();
        param.scene->getCamera().MultMatrix();
        glPushMatrix();

        glm::vec3 pos=trans->getLocalPosition(Object::render,1);
        glTranslatef(pos[0],pos[1]-1.5f,pos[2]);

        //glTranslatef(0,m_strideWheelRadius,0);
        glPushMatrix();
        glTranslatef(0,m_strideWheelRadius,0);
        glm::mat4 rot=glm::mat4_cast(orientation*glm::quat(glm::vec3(m_strideWheelRotation,0,0)));
        glMultMatrixf(&rot[0][0]);

        float length=m_strideWheelRadius;;
        glBegin(GL_LINES);
        glVertex3f(0,0,0);
        glVertex3f(0,0,length);
        //        glVertex3f(0,0,0);
        //        glVertex3f(0,length,0);
        glEnd();

        glPopMatrix();
        glPushMatrix();
        glTranslatef(0,1,0);
        glMultMatrixf(&rot[0][0]);

        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        drawSphere(1,10,10);


        glPopMatrix();

        glPopMatrix();
        glPushMatrix();

        glTranslatef(m_footPosition[0][0],m_footPosition[0][1],m_footPosition[0][2]);
        drawSphere(0.5,10,10);
        glPopMatrix();

        glPushMatrix();

        glTranslatef(m_footPosition[1][0],m_footPosition[1][1],m_footPosition[1][2]);
        drawSphere(0.5,10,10);

        glPopMatrix();
        glPopMatrix();
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    }
}



}
