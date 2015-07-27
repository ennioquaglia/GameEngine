#pragma once

#include "Camera.h"
#include "../../core/object.h"



class MyCharacterDynamicPhysicsComponent : public PhysicsComponent
{
    ENGINE_COMPONENT_MACRO
protected:
    vec3 walkDirection;
    Rotation orientation;
    
    btRigidBody * physicComponent;
    btDynamicsWorld * dynamicsWorld;
    
    
 	//parameter
    btScalar mass;	
	float moveSpeed;
	float jumpHeight;
	float jumpSpeed;
	float capsuleHeight;
	float capsuleRadius;
	float springHeight;
	float velocity;
	int numRays;
	
	
	//state
	bool isJumping;
	bool jump;
	bool onGround;
	bool isFalling;
	
	mat4 currentMatrix;
public:

	
   MyCharacterDynamicPhysicsComponent(float radius = 1.0f) {
        mass = 1;
        physicComponent = 0;
        walkDirection = vec3(0);
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
		velocity=0;
		
   
    }
	
	
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

		physicComponent->activate();
		btTransform trans;
        btMotionState *motionState=physicComponent->getMotionState();
        motionState->getWorldTransform(trans);

		vec3 pos=fromBullet(trans.getOrigin());
        	
	//	RenderSphere(pos,1,1/60.0f);


		float maxH=-1;
		vec3 maxPos=pos;
		float timestep=param.deltaTimeMs/1.0f;	
		for(float a=0;a<6.26 ;a+=0.392){
			float hM=-0.3,hm=-2.6;
			vec3 p=pos+vec3(sin(a),0,cos(a))/2.0f;
			btClosestNotMeRayResultCallback test(physicComponent,toBullet(p+vec3(0,hM,0)),toBullet(p+vec3(0,hm,0)));
			dynamicsWorld->rayTest(toBullet(p+vec3(0,hM,0)),toBullet(p+vec3(0,hm,0)),test);
			if(test.hasHit())if(maxH<test.m_hitPointWorld.getY()){maxH=test.m_hitPointWorld.getY();maxPos=fromBullet(test.m_hitPointWorld);}
			//RenderLine(p+vec3(0,hM,0),p+vec3(0,hm,0),1/60.0f);
		}
		
		static float hTarget=pos.y;
		static float maxH_last;
		
        if(maxH!=-1){ //on ground
        	onGround=1;
        	if(maxH_last==-1){ //before was not on ground 
        		velocity=0;
				isJumping=0;
				Event msg;
				msg.setType(isOnGround);
				param.actor->sendEvent(msg);
			}
      //  	RenderSphere(maxPos,0.1,1.0f);

			if(!isJumping){
				hTarget=glm::mix(maxH+2.f,hTarget,0.6f);
				velocity=0;
			}
			
		}else{
			onGround=0;
		}
   	    maxH_last=maxH;
   	   
		if(!onGround){ //apply gravity
			velocity-=timestep * 40.0f; 
		}
		
		if(jump&&onGround&&!isJumping) {
				velocity=10;
				jump=0;
				isJumping=1;
		}
		
		hTarget+=timestep * velocity;//(velocity + timestep * 10.0f / 2.0f);
		
		
        
		float walkSpeed = moveSpeed;
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
		
		currentMatrix=translate(mat4(1),vec3(pos.x,hTarget,pos.z))*glm::mat4_cast(fromBullet(q));
		return;

		

    }

protected:
    virtual btConvexShape * getCollisionShape() {
        static 	btConvexShape*s=new btCylinderShape(btVector3(1,1,1));//btSphereShape(1.0f)
        return s;

    }

    virtual void postActorSetInit() {
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
                    }
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
    	return currentMatrix;
    	
        btTransform trans;
        physicComponent->getMotionState()->getWorldTransform(trans);
        //trans.setOrigin(toBullet(position));

        float m[16] = {0};
        trans.getOpenGLMatrix(m);

        mat4 mvm = glm::make_mat4(m);

        return mvm;
    }



};