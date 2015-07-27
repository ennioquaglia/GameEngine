#include "Scene.h"

#include "../common/utility.h"

#include "../Core/Object.h"
#include "../Component/ComponentLibrary.h"
#include "../common/ScriptLoader.h"
#include "../Input/InputEvent.h"
#include "../Window/Window.h"
#include "../Render/PostProcessor.h"
using namespace Engine;
 

Scene::Scene()
{
    mAsset=new AssetManager;
    AssetManager::setIstance(mAsset);
    actorManager=0;
    systemManager=0;
    counter=0;
    effect=0;
} 

void Scene::addActor(Actor* obj)
{
    obj->m_id=getUnivocalId();
    if(actorManager)actorManager->addActor(obj);
}

void Scene::setCamera(Camera cam)
{
    camera=cam;
}

void Scene::setWindow(Window *w)
{
    window=w;
}



bool Scene::isUsingDebugCamera()
{
    return _isUsingDebugCamera;
}

void Scene::useDebugCamera(bool use)
{
    Debug("UseDebugCamera="+use);
    _isUsingDebugCamera=use;
    if(use) {
            debugCamera=camera;
            debugCamera.Pos=debugCamera.getPos();
            //debugCamera.rotX=-debugCamera.rotX;
            //debugCamera.rotY=-debugCamera.rotY;
            debugCamera.setMode(Camera::Rot);
            current_camera=&debugCamera;
        }
    else {
            current_camera=&camera;
        }
}

void Scene::Update(float deltaTimeMs)
{ 
	
    //fps=1.0f/(deltaTimeMs/1000.0f);
    //if(isKeyPressed(GLFW_KEY_F9))reload(); 
    //if(isKeyPressed(GLFW_KEY_F10))script.compileAndReload(this);
    if(window->inputManager->isKeyPressed(GLFW_KEY_F1))useDebugCamera(!_isUsingDebugCamera);
    if(_isUsingDebugCamera) {
            if(window->inputManager->isMouseButtonDown(MouseState::LEFT_BUTTON)) {
                    vec2 delta=window->inputManager->getMousePositionDelta();
                    if(delta.x)debugCamera.setRotX(debugCamera.getRotX()-delta.x);
                    if(delta.y)debugCamera.setRotY(debugCamera.getRotY()+delta.y);
                }
            float speed=1;
            if(window->inputManager->isKeyDown(GLFW_KEY_LEFT_SHIFT))speed*=1.5;
            debugCamera.Pos+=(window->inputManager->getPositionMovementRotated(debugCamera.getRotX())*(deltaTimeMs/100.0f))*speed;
        }
	glDebug();	
    actorManager->update(deltaTimeMs);
    glDebug();
    systemManager->update(deltaTimeMs);
    glDebug();
    float ratio=GlobalsManager::Istance()->get("window_width")/GlobalsManager::Istance()->get("window_height");
	current_camera->projection=glm::perspective(60.0f,ratio,0.1f,500.0f);

}

void Scene::Draw(uint flag)
{
	//glDisable(GL_CULL_FACE);
	
	
	if(flag&RenderFlag::Default){ //grid
		glPushMatrix();   	
	    current_camera->MultMatrix();
	    //glutSolidTeapot(1);
	    
	    //actorSystem.render();
		glColor3f(0,1,0);
	    glBegin(GL_LINES);
	    for(int x=-10; x<=10; x++) {
	            glVertex3f(x,0,-10);
	            glVertex3f(x,0,10);
	        }
	    for(int y=-10; y<=10; y++) {
	            glVertex3f(-10,0,y);
	            glVertex3f(10,0,y);
	        }
	    glEnd(); 
	    glPopMatrix();
	}
    
    systemManager->render(flag);
    
    
    
	//glEnable(GL_CULL_FACE);
}
void Scene::handleEvent(Event &msg) {
	   if(effect){
		   effect->handleEvent(msg);
	   }	
}

void Scene::Init()
{
    //mAsset=new AssetManager();
    camera.Pos=vec3(0,20,-20);
    camera.Target=vec3(0,0,0);
    camera.setMode(Camera::LookAt);
    debugCamera=camera;
    current_camera=&camera;
    if(!actorManager)actorManager=new ActorManager();
    if(!systemManager)systemManager=new SystemManager();


    actorManager->Init();
	actorManager->setScene(this);
	systemManager->setScene(this);
    systemManager->Init();
    
    Debug("Scene init");
    //script.compileFromMakeFile("Data/Script/scriptMakefile.txt");
    //script.loadFromDll("Data/Script/main.dll",this);

}
void Scene::setEffect(PostProcessor *e){
	effect=e;
}

void Scene::Cleanup()
{
    delete mAsset;
}

void Scene::Render(){
	if(effect){

		
		effect->Render();
	}else{
		Draw();
	}
	
}










