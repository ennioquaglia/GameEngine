#pragma once

#include "../../Core/Scene.h"
#include "../../Window/Window.h"
#include "../../Window/glfwWindow.h"
#include "../../Component/ComponentLibrary.h"
#include "../../common/utility.h"
#include "../../common/Event.h"     
#include "../../Render/DebugRender.h"


class CameraComponent : public ActorComponent
{
    ENGINE_COMPONENT_MACRO
    Camera camera;
    vec3 dir; 
    Rotation rot; 
    float cameraDist;       
public: 
    CameraComponent() {
        camera.Pos=vec3(0,0,0);
        //camera.Target=vec3(0,0,0);
        camera.setMode(Camera::TargetRot);
        camera.setRotX(90);
        camera.setRotY(45); 
        //camera.setDir(vec3(0,-1,0.1));
        cameraDist=8;    
    }
    virtual ComponentId::Type getComponentType(void) const {
        return ComponentId::CameraID;
    }     
    virtual void update(UpdateParameter &param){//float deltaTimeMs,Actor *actor,Engine::Scene* scene) {
        static int a=0;
        
        if(param.scene->isUsingDebugCamera())return;
		if(!m_pOwner)return;
        Scene *s=m_pOwner->getWorld();
        
        if(!s)return;  
        InputManager* input=param.scene->window->inputManager;
        
        //if(s->IsUsingDebugCamera())return; 
                      
		//if(scene->window&&scene->window->inputManager)    
		if(!a){    
	    	param.scene->window->inputManager->attachObserver(this);
			a=1;
		}
		
        TransformComponent* trans;
        trans=m_pOwner->getComponent<TransformComponent>(ComponentId::Transform);
        //camera.Pos=trans->getPos()+ vec3(0,2,1);
     
        camera.Target=trans->getPos(0)+vec3(0,1.5,0);     

		vec3 p=trans->getPos(0);            
		//RenderLine(p,p+vec3(0,10,0),1000,0);         
		//RenderLine(p+vec3(5,0,5),p+vec3(0,10,0) );
		     
        //camera.Pos=trans->getPos()-camera.getDir()*cameraDist+cameraDist*normalize(cross(camera.getDir(),vec3(0,1,0)))*0.3f+vec3(0,2,0);

        if(input->isMouseButtonDown(MouseState::LEFT_BUTTON)) {
        		
                vec2 delta=input->getMousePositionDelta();
                if(delta.x)camera.setRotX(camera.getRotX()-delta.x);
                if(delta.y)camera.setRotY(camera.getRotY()+delta.y);
            }
        float speed=0.15;      
        if(input->isKeyDown(GLFW_KEY_LEFT_SHIFT))speed*=2;  

		 
        vec3 dir=(input->getPositionMovementRotated(camera.getRotX())*vec3(1,0,1))*speed ;///(deltaTimeMs/10.0f))*speed;

        static vec3 old_dir=dir;
        //if(old_dir!=dir)               
			if(dir!=vec3(0)) {
	                Event msg; 
	                msg.setData(dir);
	                msg.setType(shouldWalk);        
	                sendLocalEvent(msg);
	
	            }
	        else {
	                Event msg;
	                msg.setType(stopWalking);
	                sendLocalEvent(msg);
	            }
        old_dir=dir;     
  
        if(input->isKeyPressed(GLFW_KEY_SPACE)) { 
                Event msg;
                msg.setType(shouldJump);
                sendLocalEvent(msg); 
            }  
        /*
        static float cameraPre=0;
        if(cameraPre!=camera.getRotX()){
            Event msg;
            rot.setEulerAngles(vec3(0,radians(camera.getRotX())-3.14,0));
            msg.type=Rotate;
            msg.setData(rot);
            sendLocalEvent(msg);
        }
        cameraPre=camera.getRotX();
        */
        
        //cameraDist
        float ratio=GlobalsManager::Istance()->get("window_width")/GlobalsManager::Istance()->get("window_height");
    	camera.projection=glm::perspective(60.0f,ratio,0.1f,500.0f);
        camera.distance+=input->getDeltaMouseScroll()/3.0;
        
        
		param.scene->setCamera(camera);
    }
    void handleEvent(Event &msg){
    //	if(msg.getType()==Event::input)
			//Debug(msg.getType());
		 
	} 
   
};
