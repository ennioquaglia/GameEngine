#include "StaticMeshComponent.h"
#include "Scene.h"
#include "Hash.h"
#include "Debug.h"
#include "TransformComponent.h"
#include "Preprocessor.h"
#include "Shader.h"
#include "Material.h"
#include "AssimpScene.h"
#include "LightComponent.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>

#include <ostream>


#if ENGINE_USE_QT_PROPERTY_BROWSER
#include <QtCore/QVariant>
#include <QtCore/QString>
#include "src/qtpropertymanager.h"
#include "src/qtvariantproperty.h"
#include "src/qttreepropertybrowser.h"
#include <fstream>
#endif


//#include <GL\glut.h>
namespace Engine {

    StaticMeshComponent::StaticMeshComponent(PathName filename) :m_Filename(filename)
    {
        m_MeshData= StaticMeshData::load(filename);//StaticMeshData::load(filename);
	}

	StaticMeshComponent::~StaticMeshComponent()
	{
        Destroy();
	}

	void StaticMeshComponent::Init()
	{
	}

    void StaticMeshComponent::Destroy()
    {
//        Object* system = scene.getObjectByType<StaticMeshSystem>();
//        if (system)system->removeComponent(this);

        //TODO
//        QENGINE_DEBUG("StaticMeshComponent::Destroy()");
//        Event msg(EventType::destroy);
//        QENGINE_DEBUG("StaticMeshComponent::Destroy ("+msg.getType()+" - "+(int)EventType::destroy+")");

//        msg.setType(EventType::destroy);
//        QENGINE_DEBUG("StaticMeshComponent::Destroy ("+msg.getType()+" - "+(int)EventType::destroy+")");

//        msg.setData((Object*)this);
//        QENGINE_DEBUG("StaticMeshComponent::Destroy ("+msg.getType()+" - "+(int)EventType::destroy+")");

//        sendEvent(msg);
    }
	void StaticMeshComponent::OnScene(Scene &scene)
	{
        Object* system = scene.getObjectByType<StaticMeshSystem>();
		if (system)system->addComponent(this);
        //QENGINE_INFO("StaticMeshComponent::OnScene");
	}
    void StaticMeshComponent::OnAttached(Engine::Entity &e)
    {
        //QENGINE_INFO("StaticMeshComponent::onAttached");
        if(!e.getObjectByType<TransformComponent>()){
            QENGINE_WARN("StaticMeshComponent::OnAttached entity requires TransformComponent ->added");
            e.addComponent(new TransformComponent());
        }
    }

    void StaticMeshComponent::setTextureMatrix(glm::mat2 matrix){
        m_TextureMatrix=matrix;
    }

    void StaticMeshComponent::addMaterial(Material mat){
        m_MaterialList.push_back(mat);
    }
    void StaticMeshComponent::setMaterial(Material mat,unsigned int index){
        if(index<m_MaterialList.size())
            m_MaterialList.assign(index,mat);
    }
    void StaticMeshComponent::removeMaterial(unsigned int index){
        MaterialList::iterator it = std::next(m_MaterialList.begin(), index);
        m_MaterialList.erase(it);
    }
    Material& StaticMeshComponent::getMaterial(unsigned int index){
        if(index<m_MaterialList.size()){
            MaterialList::iterator it = std::next(m_MaterialList.begin(), index);
            return *it;
        }
        if(m_MaterialList.size())return *m_MaterialList.rbegin();
        return Material::getDefaultMaterial();
    }

    void StaticMeshComponent::serializeXML(std::ostream &stream,unsigned level) const{
        for(unsigned i=0;i<level;++i)stream<<"\t";
        stream<<"<"
              <<getTypeName()<<" "
              <<SERIALIZE_VAR_PATH(m_Filename)
              <<SERIALIZE_VAR(m_TextureMatrix)
              <<" >\n";

        for(auto m: m_MaterialList)m.serializeXML(stream,level+1);

        for(unsigned i=0;i<level;++i)stream<<"\t";
        stream<<"</"<<getTypeName()<<">\n";

    }
#if ENGINE_USE_QT_PROPERTY_BROWSER
    void StaticMeshComponent::setupProperties(QtVariantPropertyManager *variantManager){
        m_rootProperty=variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),"StaticMeshComponent");

        QtVariantProperty *prop=variantManager->addProperty(QVariant::String,"Filename");
        m_Properties.push_back(prop);
        m_rootProperty->addSubProperty(prop);


    }

    void StaticMeshComponent::updateProperties(){
        for(QtVariantProperty *prop:m_Properties){
            if(!prop)continue;
            std::string name=prop->propertyName().toStdString();

            if(name=="Filename")prop->setValue(QVariant(QString(m_Filename.original.c_str())));

        }
    }


    void StaticMeshComponent::updateFromProperties(std::string name,QVariant value,Scene *scene){
    //    if(name=="name")setName(value.toString().toStdString(),scene);
        if(name=="Filename"){


            PathName newPath(value.toString().toStdString());
            if(m_Filename.original!=newPath.original){
                std::ifstream file(newPath.data);
                if(file){
                    file.close();
                    m_Filename=newPath;
                    //if(m_MeshData)StaticMeshData::remove(m_MeshData);
                    m_MeshData=StaticMeshData::load(m_Filename);//new StaticMeshData(m_Filename);
                }
                else file.close();
            }
        }
    }

#endif





    void StaticMeshSystem::Init(){

        //QENGINE_INFO("StaticMeshSystem::Init()");

        m_DefaultShader=new Shader("::Data/Shader/StaticMesh/Default/shader.vs","::Data/Shader/NormalMap.fs",EString()+"#define GLSL_VERSION "+330+"\n#define MAX_LIGHTS "+ 16);
        Shader::Compile(m_DefaultShader);
        if(!*m_DefaultShader)QENGINE_ERROR("m_DefaultShader shader error");

        m_ShadowShader=new Shader("::Data/Shader/StaticMesh/Shadow/shader.vs", "::Data/Shader/Shadow.fs",EString()+"#define GLSL_VERSION "+330+"\n#define MAX_LIGHTS "+ 16);
        Shader::Compile(m_ShadowShader);
        if(!*m_ShadowShader)QENGINE_ERROR("m_ShadowShader shader error");

        m_DeferredShader=new Shader("::Data/Shader/StaticMesh/Default/shader.vs", "::Data/Shader/Deferred/GPass/shader.fs",EString()+"#define GLSL_VERSION "+330+"\n#define MAX_LIGHTS "+ 16);
        Shader::Compile(m_DeferredShader);
        if(!*m_DeferredShader)QENGINE_ERROR("m_DeferredShader shader error");
    }

	void StaticMeshSystem::Draw(RenderParameter &param) {

        int loc;
        Camera camera=param.scene->getCamera();


        Shader *shader=0;

        if(param.flag & RenderFlag::Default){
            shader=m_DefaultShader;
        }
        if(param.flag & RenderFlag::Shadow){
            shader=m_ShadowShader;
        }
        if(param.flag & RenderFlag::Deferred){
            shader=m_DeferredShader;
        }


        if(shader){
            shader->Use();

            shader->setUniform("PM",1,0,camera.getProjectionMatrix());
            shader->setUniform("CM",1,0,camera.getMatrix());
            shader->setUniform("parallax",param.scene->options.useParalax);

            if(param.flag & RenderFlag::Default){
                LightSystem::updateUniforms(shader,const_cast<Scene*>(param.scene));
            }

            loc=shader->getUniformLocation("MVM");
            if(loc<0)QENGINE_ERROR("MVM uniform not found");
            else
            for (StaticMeshComponent *c : m_Components) {

                TransformComponent *trans=c->getEntity()->getObjectByType<TransformComponent>();

                glUniformMatrix4fv(loc, 1, 0, &trans->getMatrix()[0][0]);

                shader->setUniform("textureMatrix",1,0,c->m_TextureMatrix);

                Material::setupShaderUniform(shader);


#if ENGINE_USE_PICKING
                glPushName(getIndex());
#endif


                if(c->m_MaterialList.empty()){
                    Material::getDefaultMaterial().updateShader(shader);
                    Material::getDefaultMaterial().bind();
                    for( auto vao : c->m_MeshData->VAOs){
                        vao.draw();
                    }
                }else{

                    MaterialList::iterator it=c->m_MaterialList.begin();

                    for( auto vao : c->m_MeshData->VAOs){
                        if(it!=c->m_MaterialList.end()){
                            (*it).updateShader(shader);
                            (*it).bind();
                            ++it;
                        }
                        vao.draw();

                    }
                }

#if ENGINE_USE_PICKING
                glPopName();
#endif
            }
            glUseProgram(0);
        }

	}
	void StaticMeshSystem::addComponent(Object * obj)
	{
		if (!obj)return;

        //QENGINE_INFO("Trying to add a Static Mesh");
        if (obj->getType() != StaticMeshComponent::getTypeStatic())return;
		StaticMeshComponent *c = (StaticMeshComponent*)obj;
		
		m_Components.push_back(c);
        obj->attachListener(this);

        QENGINE_DEBUG("Added a Static Mesh! "+c->m_Filename);
	}

    void StaticMeshSystem::removeComponent(Object *obj){
        QENGINE_DEBUG("StaticMeshSystem::removeComponent");
        if (!obj)return;

        StaticMeshComponent *c = (StaticMeshComponent*)obj;

        auto it=std::find(m_Components.begin(),m_Components.end(),c);
        if(it!=m_Components.end()){
            m_Components.erase(it);
            QENGINE_DEBUG("Removed a Static Mesh! "+c->m_Filename);
        }
    }

    void StaticMeshSystem::handleEvent(Event &msg){
        QENGINE_DEBUG("StaticMeshSystem:: received msg");
        if(msg.getType()==EventType::destroy){

            removeComponent(msg.getData<Object*>());
        }else QENGINE_DEBUG("StaticMeshSystem:: not a destroy msg("+msg.getType()+" - "+(int)EventType::destroy+")");
    }
}
