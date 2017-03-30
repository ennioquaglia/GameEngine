#include "Scene.h"
#include "Object.h"
#include "Context.h"
#include "Hash.h"
#include "Debug.h"
#include "InputManager.h"
#include "InputState.h"
#include "PostProcessEffect.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <fstream>
#include "pugixml.hpp"


#include <sstream>
#include "TransformComponent.h"
#include "StaticMeshComponent.h"
#include "LightComponent.h"
#include "PhysicsComponent.h"

namespace Engine {


Scene::Scene(Context * ctx):m_Context(ctx)
{
    m_Camera.Pos=glm::vec3(0,10,-10);
    m_Camera.Target=glm::vec3(0,0,0);
    m_Camera.setMode(Camera::LookAt);
    m_DebugCamera=m_Camera;
    m_CurrentCamera=&m_Camera;

    ctx->attachObserver(this);

    //useDebugCamera();

}

Scene::~Scene()
{
}

void Scene::Update(float deltaTime) {

    InputManager &input= m_Context->getInputManager();

    if(input.isKeyPressed(ENGINE_KEY_F1))useDebugCamera(!m_isUsingDebugCamera);

    if(m_isUsingDebugCamera) {
        if(input.isMouseButtonDown(MouseState::LEFT_BUTTON)) {
            glm::vec2 delta=input.getMousePositionDelta();
            if(delta.x)m_DebugCamera.setRotX(m_DebugCamera.getRotX()-delta.x);
            if(delta.y)m_DebugCamera.setRotY(m_DebugCamera.getRotY()+delta.y);
        }
        float speed=1;
        if(input.isKeyDown(ENGINE_KEY_LEFT_SHIFT))speed*=1.5;

        m_DebugCamera.Pos+=(input.getPositionMovementRotated(m_DebugCamera.getRotX())*(deltaTime*10))*speed;
    }

    Object::UpdateParameter p(this, input, deltaTime*m_Speed);
    for (ObjectsContainer::iterator it = m_Systems.begin(); it != m_Systems.end(); ++it) {
        (*it)->Update(p);
    }
}

unsigned int Scene::getNewUID(){
    //todo: store all objects in an array properly and use the ID as index
    static unsigned int number = 1;//1<< (sizeof(unsigned int)*8 - ENGINE_OBJECT_VERSION_BITS);
    return (number++ << (sizeof(unsigned int)*8 - ENGINE_OBJECT_VERSION_BITS));
}

void Scene::Render() {

    if(!m_Context){QENGINE_ERROR("Scene::Render()  m_Context is null");return;}
    float ratio =(float) m_Context->getWidth() / (float)m_Context->getHeight();


    m_CurrentCamera->setProjectionMatrix(glm::perspective(glm::radians(60.0f),ratio,0.1f,500.0f));
    glViewport(0, 0, m_Context->getWidth(), m_Context->getHeight());

    m_Context->ClearBuffer();
    // Setup a perspective projection

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(&m_CurrentCamera->getProjectionMatrix()[0][0]);
    //gluPerspective(60, ratio, 0.1, 500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(m_Effect)
        m_Effect->Render();
    else
        Draw(RenderFlag::Default);


}

void Scene::Draw(RenderFlag flag)  {
    glUseProgram(0);
    if(flag & RenderFlag::Default){
        glPushMatrix();
        m_CurrentCamera->MultMatrix();
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

    Object::RenderParameter p(this,(unsigned int)flag);
    for (ObjectsContainer::iterator it = m_RenderSystems.begin(); it != m_RenderSystems.end(); ++it) {
        (*it)->Draw(p);

    }

}

void Scene::handleEvent(Event &msg) {
    if(m_Effect){
        m_Effect->handleEvent(msg);
    }
}

void Scene::PostSetup(){
    QENGINE_INFO("Scene::PostSetup() begin");

    for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
        if ((*it)->m_Flag & Object::entity)(*it)->OnScene(*this);
    }
    for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
        if (!((*it)->m_Flag & Object::entity))(*it)->OnScene(*this);
    }
    setup_done=true;
    QENGINE_INFO("Scene::PostSetup() end");
}

void Scene::addObject(Object *obj) {
    if (!obj)return;

    obj->Init();


    m_Objects.push_back(obj);

    if(obj->getType()!=Entity::getTypeStatic()){
        m_Objects_type_vector[obj->getType()].push_back(obj);
        obj->m_UID=m_Objects_type_vector[obj->getType()].size()-1;
    }

    if (obj->m_Flag & Object::update ){QENGINE_INFO("ADDED UPDATE SYSTEM");m_Systems.push_back(obj);}
    if (obj->m_Flag & Object::render){ QENGINE_INFO("ADDED RENDER SYSTEM");m_RenderSystems.push_back(obj);}

    //QENGINE_INFO("Added object! "+obj->getType()+"  "+obj->getTypeStatic());

    if(setup_done)
        obj->OnScene(*this);

    if (obj->m_Flag & Object::entity)
        for(Object *o:((Entity*)obj)->m_Objects)
            addObject(o);



}


void Scene::removeObject(Object *obj) {//TODO
    if (!obj)return;

    //m_Objects.remove(obj);

    //if (obj->m_Flag & StringEnum("update")) m_Systems.remove(obj);
    //if (obj->m_Flag & StringEnum("render")) m_RenderSystems.remove(obj);
}





bool Scene::isUsingDebugCamera() const{
    return m_isUsingDebugCamera;
}

void Scene::useDebugCamera(bool use){
    QENGINE_INFO("UseDebugCamera="+use);

    if(!m_isUsingDebugCamera&&use){
        m_DebugCamera=m_Camera;
        m_DebugCamera.Pos=m_Camera.getPos();
        //debugCamera.rotX=-debugCamera.rotX;
        //debugCamera.rotY=-debugCamera.rotY;
        m_DebugCamera.setMode(Camera::Rot);
        m_CurrentCamera=&m_DebugCamera;
    }
    if(m_isUsingDebugCamera&&!use){
        m_CurrentCamera=&m_Camera;
    }
    m_isUsingDebugCamera=use;
}

Camera Scene::getCamera() const{
    return *m_CurrentCamera;
}

void Scene::setCamera(Camera cam){
    if(!m_isUsingDebugCamera)
        m_Camera=cam;
}


void Scene::setPostProcessorEffect(PostProcessor* effect){
    m_Effect=effect;
    if(m_Effect){
        //EventData<glm::vec2>
        Event e;
        e.setType(Hash("windowResize"));
        e.setData(glm::vec2(m_Context->getWidth(), m_Context->getHeight()));

        m_Effect->handleEvent(e);
    }
}




Object * Scene::getObjectByFlag(unsigned int flag) const
{
    for (ObjectsContainer::const_iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
        if (((*it)->m_Flag & flag)==flag)return *it;
    }
    return nullptr;
}

/*
    Object* Scene::getObjectByType(unsigned int type)
    {
        for (ObjectsContainer::iterator it = m_Objects.begin(); it != m_Objects.end(); ++it) {
            if ((*it)->getType() == type)return *it;
        }
        return nullptr;
    }*/

const Entity* Scene::getEntity(unsigned int uid) const{
    unsigned index=(uid<<ENGINE_OBJECT_VERSION_BITS)>>ENGINE_OBJECT_VERSION_BITS;
    if(index>m_Entities.size())return 0;
    return m_Entities[index];
}
const Entity* Scene::getEntity(std::string name) const{
    unsigned hash=Hash(name);
    auto it=m_Entities_hash_index.find(hash);
    if(it==m_Entities_hash_index.end())return nullptr;
    return getEntity(it->second);
}



void Scene::serializeXML(std::ostream &stream,unsigned level) {
    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"<scene>\n";


    auto entities=m_Entities;//getObjectsByType<Entity>();

    for( auto e: entities){
        e->serializeXML(stream,level+1);
    }

    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"</scene>\n";
}




glm::vec3 toVec3(std::string str){
    str=str.substr(5);
    std::istringstream ss(str);

    glm::vec3 v;
    for(int i = 0; i < 3; i++){ss >> v[i]; ss.get();}
    return v;
}
glm::vec4 toVec4(std::string str){
    str=str.substr(5);
    std::istringstream ss(str);

    glm::vec4 v;
    for(int i = 0; i < 4; i++){ss >> v[i]; ss.get();}
    return v;
}

glm::quat toQuat(std::string str){
    str=str.substr(11);
    std::istringstream ss(str);

    glm::quat v;
    for(int i = 0; i < 4; i++){ss >> v[i]; ss.get();}
    v=glm::normalize(v);
    return v;
}
glm::mat2 toMat2(std::string str){
    str=str.substr(5);
    std::istringstream ss(str);

    glm::mat2 v;
    for(int i = 0; i < 4; i++){ss >> v[i/2][i%2]; ss.get();}
    return v;
}

bool Scene::deserializeXML(PathName filename){
    std::ifstream s(filename.toStd());
    if(!s) {
        QENGINE_ERROR("Scene::deserializeXML -> file doesn't exist("+filename.original+")");
        return 0;
    }
    s.close();
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.toStd().data());

    if (result)
        QENGINE_INFO("XML [" + filename.original + "] parsed without errors, attr value: [" + doc.child("node").attribute("attr").value()+"]");
    else {
        QENGINE_ERROR( "XML [" + filename.original + "] parsed with errors, attr value: [" + doc.child("node").attribute("attr").value() + "]");
    }
    pugi::xml_node root = doc.child("scene");

    for (pugi::xml_node_iterator it = root.begin(); it != root.end(); ++it) {
        auto &node=*it;
        Entity* e=newEntity();
        std::string name=node.attribute("m_Name").value();
        if(name.length())e->setName(name,this);

        auto child = node.child("StaticMeshComponent");
        if(child) {
            StaticMeshComponent *c=new StaticMeshComponent(child.attribute("m_Filename").value());
            for(auto n:child.children("Material")){
                Material mat(n.attribute("m_Diffuse").value(),n.attribute("m_Specular").value(),n.attribute("m_Normal").value(),std::stof(n.attribute("m_Shininess").value()));
                c->addMaterial(mat);
            }
            c->setTextureMatrix(toMat2(child.attribute("m_TextureMatrix").value()));
            e->addComponent(c);
        }

        child = node.child("TransformComponent");
        if(child) {
            TransformComponent *c=new TransformComponent();

            c->setPosition(toVec3(child.attribute("m_origin").value()));
            c->setScale(toVec3(child.attribute("m_scale").value()));
            c->setRotation(toQuat(child.attribute("m_rotation").value()));

            e->addComponent(c);
        }

        child = node.child("StaticMeshPhysicsComponent");
        if(child) {
            StaticMeshPhysicsComponent *c=new StaticMeshPhysicsComponent(child.attribute("m_Filename").value());
            e->addComponent(c);
        }

        child = node.child("LightComponent");
        if(child) {
            LightComponent *c=new LightComponent();
            c->diffuse=toVec4(child.attribute("diffuse").value());
            c->specular=toVec4(child.attribute("specular").value());
            c->dir=toVec3(child.attribute("dir").value());
            c->constantAttenuation=std::stof(child.attribute("constantAttenuation").value());
            c->linearAttenuation=std::stof(child.attribute("linearAttenuation").value());
            c->quadraticAttenuation=std::stof(child.attribute("quadraticAttenuation").value());

            e->addComponent(c);
        }

        addObject(e);

    }
    return 1;

}


}
