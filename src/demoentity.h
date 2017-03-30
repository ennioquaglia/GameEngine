#pragma once


#ifdef ENGINE_USE_GLFW

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include "Application.h"
#include "Debug.h"
#include "Scene.h"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "TransformComponent.h"
#include "InputManager.h"

#include "CharacterPhysicsComponent.h"
#include "physics.h"

#include "Primitive.h"

namespace Engine {





class PlayerAnimationManagerComponent : public AnimationManagerComponent
{
    bool isJumping;
    SkeletalMeshComponent* m=0;
public:
    COMPONENT_TYPE_INFORMATION(PlayerAnimationManagerComponent)
    PlayerAnimationManagerComponent(){
        isJumping=0;
    }

    enum AnimID{
        idle=SkeletalMeshComponent::usertype,
        walk,
        run,
        jump
    };
    virtual void setupAnimation(SkeletalMeshComponent* m){

        QENGINE_INFO("########################################### animation done!\n\n\n\n");
        //if(!m)return;
        //todo setup animation
        /*
            m->addAnimation(idle,90,110);
            m->addAnimation(walk,2,14);
            m->addAnimation(jump,42,54);
            */
        m->addAnimation(idle,"::Data/Model/alpha2/Alpha@idle.dae",0,200);
        //m->addAnimation(idle,0,200);
        m->addAnimation(jump,"::Data/Model/alpha2/Alpha@jump.dae",200,500);
        m->addAnimation(walk,"::Data/Model/alpha2/Alpha@walking.dae",0,200);
        m->addAnimation(run,"::Data/Model/alpha2/running.dae",0,200);

    }

    virtual void handleEvent(Event &msg) {

        AnimationManagerComponent::handleEvent(msg);

        if(!m){
            m=getSkeletalMesh();
            if(!m)return;
            m->setAnimVelocity(1.0f);
        }


        switch(msg.getType()) {
        case Hash("DoInverseKinematic"):{
//            //m->setBonePosition("Hips",glm::vec3());
//            TransformComponent* trans;
//            trans=getEntity()->getObjectByType<TransformComponent>();
//            MyCharacterDynamicPhysicsComponent* controller;
//            controller=getEntity()->getObjectByType<MyCharacterDynamicPhysicsComponent>();
//            if(trans && controller){

//                glm::vec3 globalRxFoot=glm::vec3(glm::inverse(trans->getMatrix()*m->getBoneMatrix("RightFoot"))*glm::vec4(controller->m_footPosition[0],1));//trans->getMatrix()*glm::vec3(get)
//                glm::vec3 globalLxFoot=glm::vec3(glm::inverse(trans->getMatrix()*m->getBoneMatrix("LeftFoot"))*glm::vec4(controller->m_footPosition[1],1));//trans->getMatrix()*glm::vec3(get)


//                m->setBonePosition("RightFoot",globalRxFoot);
//                m->setBonePosition("LeftFoot",globalLxFoot);

//            }

        }break;
        case Hash("AnimVelocity"):{
            //QENGINE_INFO(msg.getData<float>());
            m->setAnimVelocity(1.0f+msg.getData<float>()*2);
        }break;
        case EventType::shouldMove: {
            //vec3 dir=msg.getData<vec3>();

        }
            break;
        case EventType::shouldWalk: {
            if(!isJumping){
                glm::vec3 dir=msg.getData<glm::vec3>();

                float vel=glm::length(dir);
                if(vel<0.5f)m->setAnimation(walk);
                else {

                    m->setAnimation(run);
                   // m->setAnimVelocity(vel);
                }
            }
        }
            break;
        case EventType::stopWalking:{
            if(!isJumping)
                m->setAnimation(idle);
        }
            break;
        case EventType::shouldJump:{
            m->playAnimation(jump);
            isJumping=1;
        }
            break;
        case EventType::isOnGround:{
            isJumping=0;
            m->playAnimation(idle);

        }break;
        case EventType::Rotate:{

        }
            break;
        }
    }

};




class CameraComponent : public Component
{

    Camera camera;
    glm::vec3 dir;
    glm::quat rot;
    float cameraDist;
public:
    COMPONENT_TYPE_INFORMATION(CameraComponent)
    CameraComponent() {
        camera.Pos=glm::vec3(0,0,0);
        //camera.Target=vec3(0,0,0);
        camera.setMode(Camera::TargetRot);
        camera.setRotX(90);
        camera.setRotY(45);
        //camera.setDir(vec3(0,-1,0.1));
        cameraDist=8;
        m_Flag.add(Object::update);
    }

    virtual void Update(UpdateParameter &param){
        if(param.scene->isUsingDebugCamera())return;


        auto e=getEntity();
        if(!e)return;
        Scene *s=const_cast<Scene*>(param.scene);
        if(!s)return;
        const InputManager* input=&param.input;


        TransformComponent* trans;
        trans=e->getObjectByType<TransformComponent>();



        camera.Target=trans->getPosition()+glm::vec3(0,1.,0);

        glm::vec3 p=trans->getPosition();
        //RenderLine(p,p+vec3(0,10,0),1000,0);
        //RenderLine(p+vec3(5,0,5),p+vec3(0,10,0) );

        //camera.Pos=trans->getPos()-camera.getDir()*cameraDist+cameraDist*normalize(cross(camera.getDir(),vec3(0,1,0)))*0.3f+vec3(0,2,0);

        if(input->isMouseButtonDown(MouseState::LEFT_BUTTON)) {

            glm::vec2 delta=input->getMousePositionDelta();
            if(delta.x)camera.setRotX(camera.getRotX()-delta.x);
            if(delta.y)camera.setRotY(camera.getRotY()+delta.y);
        }
        float speed=0.15;

//        SkeletalMeshComponent* skm;
//        skm=e->getObjectByType<SkeletalMeshComponent>();
//        if(skm)skm->setAnimVelocity(1);

        if(input->isKeyDown(ENGINE_KEY_LEFT_SHIFT)){
            speed*=6;
            //if(skm)skm->setAnimVelocity(10);
        }//else if(skm)skm->setAnimVelocity(1);


        glm::vec3 dir=(input->getPositionMovementRotated(camera.getRotX())*glm::vec3(1,0,1))*speed ;///(deltaTimeMs/10.0f))*speed;

        static glm::vec3 old_dir=dir;
        //if(old_dir!=dir)
        if(dir!=glm::vec3(0)) {
            Event msg;
            msg.setData(dir);
            msg.setType(EventType::shouldWalk);
            sendEvent(msg);

        }
        else {
            Event msg;
            msg.setType(EventType::stopWalking);
            sendEvent(msg);
        }
        old_dir=dir;

        if(input->isKeyPressed(ENGINE_KEY_SPACE)) {
            Event msg;
            msg.setType(EventType::shouldJump);
            sendEvent(msg);
        }

        cameraDist+=input->getDeltaMouseScroll()/3.0;
        camera.distance = cameraDist;


        BulletPhysicsSystem* physicsSystem=s->getObjectByType<BulletPhysicsSystem>();
        if(physicsSystem){
            btCollisionWorld::ClosestRayResultCallback test(toBullet(camera.Target),toBullet(camera.getPos()));
            physicsSystem->dynamicsWorld->rayTest(toBullet(camera.Target-camera.getDir()),toBullet(camera.getPos()),test);
            if(test.hasHit()){
                camera.distance=glm::max(1.0f,glm::distance(fromBullet(test.m_hitPointWorld+test.m_hitNormalWorld*0.2),camera.Target));
            }
        }




        camera.setProjectionMatrix(s->getCamera().getProjectionMatrix());
        s->setCamera(camera);
    }
    void handleEvent(Event &msg){
        // QENGINE_INFO(msg.getType()+"   "+EventType::stopWalking);
        //	if(msg.getType()==Event::input)
        //Debug(msg.getType());

    }

};


class MainPlayer: public Entity{
    TransformComponent *m_Trans;
    SkeletalMeshComponent *m_MeshComponent;
public:
    MainPlayer(){
        m_Flag.add(Object::update);
        m_Trans= new TransformComponent();
        m_Trans->setLocalScale(Object::render,glm::vec3(0.03f));
        m_Trans->setLocalScale(Object::physics,glm::vec3(0.90f));
        m_Trans->setPosition(glm::vec3(10,10,10));

        m_Trans->setLocalPosition(Object::render, glm::vec3(0, -0.4, 0));

        m_Trans->setLocalScale(Object::render, glm::vec3(0.017));

        addComponent(m_Trans);


        PlayerAnimationManagerComponent *anim=new PlayerAnimationManagerComponent();
        addComponent(anim);


        //m_MeshComponent=new SkeletalMeshComponent("::Data/Model/beta.dae");
        //m_MeshComponent=new SkeletalMeshComponent("::Data/Model/remy/Remy.dae");
        //m_MeshComponent=new SkeletalMeshComponent("::Data/Model/paladin/idle.dae");
        //auto m_MeshComponent=new StaticMeshComponent("::Data/Model/sphere.obj");

        m_MeshComponent=new SkeletalMeshComponent("::Data/Model/alpha2/Alpha.dae");

        //Material mat("::Data/Texture/floor1_d.png","::Data/Texture/floor1_s.png","::Data/Texture/floor1_n.png");
        //Material mat("::Data/Model/paladin/textures/Paladin_diffuse.png","::Data/Model/paladin/textures/Paladin_specular.png","::Data/Model/paladin/textures/Paladin_normal.png");
        Material mat("::Data/Texture/default.png");
        mat.setShininess(200);
        m_MeshComponent->addMaterial(mat);
        addComponent(m_MeshComponent);


        addComponent(new CameraComponent());
        addComponent(new MyCharacterDynamicPhysicsComponent());


        //addComponent(new DebugCharacterComponent());



        //QENGINE_ERROR("MainPlayer::MainPlayer");

    }
    virtual void Update(UpdateParameter &param) {
        //m_Trans->setPosition(m_Trans->getPosition()+param.input.getPositionMovementRotated(param.scene->getCamera().getRotX())*(param.deltaTime)*10.0f);
    }

    virtual void Init(){

    }
    virtual void Destroy(){}

};



}
