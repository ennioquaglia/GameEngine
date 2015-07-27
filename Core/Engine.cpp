#include "../common/utility.h"
#include "Engine.h"
#include "Object.h"
#include <typeinfo>  //for 'typeid'

#include "../Component/ComponentLibrary.h"
#include "../common/ScriptLoader.h"

namespace Engine
{

    void debug(EString str)
    {
        cout<<"Debug: "<<str.data<<"\n";
        if(!current_window)return;
        current_window->appConsoleLine("Debug: "+str.data+"\n");
    }
    void error(EString str)
    {
        cout<<"ERROR: "<<str.data<<"\n";
        if(!current_window)return;
        current_window->appConsoleLine("ERROR: "+str.data+"\n");
    }
    void warning(EString str)
    {
        cout<<"Warning: "<<str.data<<"\n";
        if(!current_window)return;
        current_window->appConsoleLine("Warning: "+str.data+"\n");
    }


    bool isKeyDown(int key)
    {
        if(!current_window)return 0;
        return current_window->input.key[key];
    }
    bool isKeyPressed(int key)
    {
        if(!current_window)return 0;
        return current_window->input.key[key]&&!current_window->input_pre.key[key];
    }
    bool isKeyReleased(int key)
    {
        if(!current_window)return 0;
        return !current_window->input.key[key]&&current_window->input_pre.key[key];
    }


    vec2 getMousePos()
    {
        if(!current_window)return vec2(0);

        return vec2(current_window->input.mouse.x,current_window->input.mouse.y);
    }
    vec2 getMousePositionDelta()
    {
        if(!current_window)return vec2(0);
        return vec2(current_window->input_pre.mouse.x,current_window->input_pre.mouse.y)-vec2(current_window->input.mouse.x,current_window->input.mouse.y);
    }
    vec2 getMousePositionDeltaAndCenter(float centerZone)
    {
        if(!current_window)return vec2(0);

		vec2 size=vec2(current_window->Width,current_window->Height)/2.0f;
    	vec2 pos=getMousePos();

		float centro=centerZone;
		if(pos.x>(size.x+centro)||pos.x<(size.x-centro)||pos.y>(size.y+centro)||pos.y<(size.y-centro)){
			current_window->setMousePos(vec2(-1));
			float deltaX=(float)pos.x-(size.x);
			float deltaY=(float)pos.y-(size.y);
			return vec2(deltaX,deltaY);
		}	
        return vec2(0);
    }
    void setMousePos(vec2 pos)
    {
        if(!current_window)return;
        current_window->setMousePos(pos);
    }
    bool isMouseButtonDown(int button)
    {
        if(!current_window)return 0;
        return current_window->input.mouse.button[button];
    }
    bool isMouseButtonPressed(int button)
    {
        if(!current_window)return 0;
        return current_window->input.mouse.button[button]&&!current_window->input_pre.mouse.button[button];
    }
    bool isMouseButtonReleased(int button)
    {
        if(!current_window)return 0;
        return !current_window->input.mouse.button[button]&&current_window->input_pre.mouse.button[button];
    }
    int getDeltaMouseScroll()
    {
        if(!current_window)return 0;
        return current_window->input.mouse.wheelDelta;
    }
    vec3 getPositionMovement()
    {
        float x=0,y=0,z=0;
        float xrotrad = 0;

#ifdef ENGINE_USE_SFML

        if(isKeyDown(sf::Keyboard::W))debug("sfml input si");
        else debug("sfml input no");

        if(isKeyDown(sf::Keyboard::W))z++;
        if(isKeyDown(sf::Keyboard::A))x--;
        if(isKeyDown(sf::Keyboard::S))z--;
        if(isKeyDown(sf::Keyboard::D))x++;
        if(isKeyDown(sf::Keyboard::Q))y--;
        if(isKeyDown(sf::Keyboard::E))y++;

#endif
#ifdef ENGINE_USE_GLFW


        //if (isKeyDown(GLFW_KEY_W))debug("input si");
        //else debug("input no");

        if(isKeyDown(GLFW_KEY_W))z++;
        if(isKeyDown(GLFW_KEY_A))x--;
        if(isKeyDown(GLFW_KEY_S))z--;
        if(isKeyDown(GLFW_KEY_D))x++;
        if(isKeyDown(GLFW_KEY_Q))y--;
        if(isKeyDown(GLFW_KEY_E))y++;

#endif

        vec3 dir=vec3(x,y,z);
        //if(dir!=vec3(0))dir=normalize(dir);
        return dir;
    }


    vec3 getPositionMovementRotated(float ang)
    {
        ang=glm::radians(ang);

        vec3 p(0);
        vec3 pos=getPositionMovement();
        vec3 rot=vec3(cos(ang),0,sin(ang));
        p+=pos.z*rot;
        rot=vec3(cos(ang+1.57),0,sin(ang+1.57));
        p+=pos.x*rot;
        p.y=pos.y;
        return p;
    }

    vector<string> string_hash_table;
    unsigned int getHash(string str)
    {
        int i;
        for(i = 0; i < string_hash_table.size() ; ++i) {
                if(string_hash_table[i]==str)return i;
            }
        return i;
    }
    string getStringFromHash(unsigned int hash)
    {
        if(hash>string_hash_table.size())return "";
        return string_hash_table[hash];
    }
    void clearHashTable()
    {
        string_hash_table.clear();
    }
    int global_number_id=0;
    unsigned int getUnivocalId()
    {
        return global_number_id++;
    }








    SceneManager::SceneManager()
    {
        mAsset=new AssetManager;
        AssetManager::setIstance(mAsset);
		actorManager=0;
		systemManager=0;
        counter=0;
    }
    SceneManager::~SceneManager()
    {
        delete mAsset;
    }
    int SceneManager::Setup()
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
        //actorManager->setScene(this);

        //systemManager->setScene(this);
		systemManager->Init();
        //script.compileFromMakeFile("Data/Script/scriptMakefile.txt");
        //script.loadFromDll("Data/Script/main.dll",this);

    }

    void SceneManager::Render(float deltaTimeMs)
    {
    	//Debug("act");
        //act(deltaTimeMs);
        //Debug("draw");
        draw(deltaTimeMs);

    }
    void SceneManager::act(float deltaTimeMs)
    {
        fps=1.0f/(deltaTimeMs/1000.0f);
        //if(isKeyPressed(GLFW_KEY_F9))reload();
        //if(isKeyPressed(GLFW_KEY_F10))script.compileAndReload(this);
        if(isKeyPressed(GLFW_KEY_F1))useDebugCamera(!isUsingDebugCamera);
        if(isUsingDebugCamera) {
                if(isMouseButtonDown(MouseState::LEFT_BUTTON)) {
                        vec2 delta=getMousePositionDelta();
                        if(delta.x)debugCamera.setRotX(debugCamera.getRotX()-delta.x);
                        if(delta.y)debugCamera.setRotY(debugCamera.getRotY()+delta.y);
                    }
                float speed=1;
                if(isKeyDown(GLFW_KEY_LEFT_SHIFT))speed*=1.5;
                debugCamera.Pos+=(getPositionMovementRotated(debugCamera.getRotX())*(deltaTimeMs/100.0f))*speed;
            }

        actorManager->update(deltaTimeMs);
		systemManager->update(deltaTimeMs);
    }
    void SceneManager::draw(float deltaTimeMs)
    {
        current_camera->MultMatrix();

        //glutSolidTeapot(1);
		systemManager->render();
        //actorSystem.render();

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
    }

    void SceneManager::addActor(Actor* obj)
    {
        //obj->m_id=getUnivocalId();
        if(actorManager)actorManager->addActor(obj);
    }

    void SceneManager::setCamera(Camera cam)
    {
        camera=cam;
    }

    int SceneManager::reload()
    {
        //script.reload(this);
    }

}
