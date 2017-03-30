#pragma once
#include "Object.h"
#include "VAO.h"
#include "Pathname.h"
#include "Resource.h"
#include "MeshData.h"
#include "Material.h"
#include "Container.h"

#include <string>
#include <vector>

namespace Engine {

    class Shader;
    class StaticMeshComponent : public Component
	{
		friend class StaticMeshSystem;
        PathName m_Filename;
        StaticMeshData *m_MeshData;
        MaterialList m_MaterialList;
        glm::mat2 m_TextureMatrix=glm::mat2(1.0f);
	public:
        COMPONENT_TYPE_INFORMATION(StaticMeshComponent)

		
        StaticMeshComponent(PathName filename);
		~StaticMeshComponent();
		virtual void Init();
        virtual void Destroy();
		virtual void OnScene(Scene &scene);
        virtual void OnAttached(Entity& e);

        void setTextureMatrix(glm::mat2 matrix);

        void addMaterial(Material mat);
        void setMaterial(Material mat,unsigned int index);
        void removeMaterial(unsigned int index);
        Material &getMaterial(unsigned int index=0);

        void serializeXML(std::ostream &stream,unsigned level) const;


#if ENGINE_USE_QT_PROPERTY_BROWSER
    virtual void setupProperties(QtVariantPropertyManager *variantManager);
    virtual void updateProperties();
    virtual void updateFromProperties(std::string name,QVariant value,Scene *scene);
#endif
	};


	class StaticMeshSystem : public RenderSystem
	{
		//typedef std::vector<StaticMeshComponent*> MeshComponents;
        using MeshComponents=std::vector<StaticMeshComponent*>;
		MeshComponents m_Components;
        Shader *m_DefaultShader=0,*m_ShadowShader=0,*m_DeferredShader=0;
	public:
        COMPONENT_TYPE_INFORMATION(StaticMeshSystem)

        StaticMeshSystem():RenderSystem(){Init();}
		virtual void Draw(RenderParameter &param);
		virtual void addComponent(Object *obj);
        virtual void removeComponent(Object *obj);
        virtual void handleEvent(Event &msg);

        virtual void Init();
        virtual void Destroy() {}

	};



}
