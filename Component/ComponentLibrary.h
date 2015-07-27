#pragma once

#include "StandardComponent.h"
#include "PhysicComponent.h"
#include "RenderComponent.h"



class BasicUserInputComponent : public ActorComponent
{
	ENGINE_COMPONENT_MACRO
    vec3 dir;
    
public:
    BasicUserInputComponent() {}
    virtual ComponentId::Type getComponentType(void) const {
        return ComponentId::UserInput;
    }
    virtual void update(UpdateParameter &param){//float deltaTimeMs,Actor *actor,Engine::Scene* scene) {
        vec3 dir;//=param.input->getPositionMovementRotated(90);
        if(dir!=vec3(0)) {
        		float speed=10;
                dir/=param.deltaTimeMs/speed;
                Event msg;
                msg.setData(dir);
                msg.setType(shouldMove);
                sendLocalEvent(msg);
            }
    }
	

};

class AnimationManagerComponent : public ActorComponent
{
	ENGINE_COMPONENT_MACRO
	bool animSetupDone;
	public:
		AnimationManagerComponent(){animSetupDone=0;}
		
		virtual ComponentId::Type getComponentType(void) const {
        	return ComponentId::AnimationManager;
	    }
	    virtual void setupAnimation(SkeletalMeshManager* m){}
		virtual SkeletalMeshManager* getMeshManager(){
			
			SkeletalMeshGraphicComponent* comp=0;
        	comp=m_pOwner->getComponent<SkeletalMeshGraphicComponent>(ComponentId::Render);

        	if(!comp)return 0;
			return comp->mesh;
		}
	    virtual void handleEvent(Event &msg) {
			if(animSetupDone)return;
	    	if(!m_pOwner)return;
	    	if(msg.getType()==componentAdded){
	    		SkeletalMeshManager* m=getMeshManager();
				if(m){setupAnimation(m);
					animSetupDone=1;
				}
			}
	    }
	   	
};

class PlayerAnimationManagerComponent : public AnimationManagerComponent 
{
	ENGINE_COMPONENT_MACRO
	bool isJumping;
	public:
		PlayerAnimationManagerComponent(){
			isJumping=0;
		}
	    
	    enum AnimID{
	    	idle=SkeletalMeshManager::usertype,
	    	walk,
	    	run,
	    	jump
		};
	    virtual void setupAnimation(SkeletalMeshManager* m){
			
			Debug("########################################### animation done!\n\n\n\n");
			//if(!m)return;
			//todo setup animation
			/*
			m->addAnimation(idle,90,110);
			m->addAnimation(walk,2,14);
			m->addAnimation(jump,42,54);
			*/
			m->addAnimation(idle,AssetManager::getBasePath()+"Data/Model/alpha/Alpha@idle.dae",0,200);
			m->addAnimation(jump,AssetManager::getBasePath()+"Data/Model/alpha/Alpha@jump.dae",200,500);
			m->addAnimation(walk,0,200);
			

		}
 
	    virtual void handleEvent(Event &msg) {
			AnimationManagerComponent::handleEvent(msg);
			
	    	if(!m_pOwner)return;
	    	 
	    	
			//todo cache
  			SkeletalMeshGraphicComponent* comp=0;
        	comp=m_pOwner->getComponent<SkeletalMeshGraphicComponent>(ComponentId::Render);

        	if(!comp)return;

			SkeletalMeshManager* m=comp->mesh;

	        switch(msg.getType()) {
	                case shouldMove: {
	                        //vec3 dir=msg.getData<vec3>();
                        	
	                    }
	                    break;
     				case shouldWalk: {
     						if(!isJumping)
	                        //vec3 dir=msg.getData<vec3>();
                        	m->setAnimation(walk);
	                    }
	                    break;
	                case stopWalking:{
	                		if(!isJumping)
	                		m->setAnimation(idle);
						} 
					break;
					case shouldJump:{
	                		m->playAnimation(jump);
	                		isJumping=1;
						} 
					break;
					case isOnGround:{
						isJumping=0;
						m->playAnimation(idle);
						
					}break;
					case Rotate:{
		                		
					} 
					break;
	                default: break;
    		}
	    }
	   	
};

extern Actor* addSphereActor();
