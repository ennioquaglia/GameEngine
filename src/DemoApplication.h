#pragma once
#include "Application.h"
#include "Debug.h"
#include "Scene.h"

namespace Engine {
    class TransformComponent;

    class DirectionalLightComponent;
	class DemoApplication : public Application
	{
	protected:
		Context* m_Context;
		Scene *m_Scene;


        TransformComponent *trans;
        DirectionalLightComponent *m_Sun;
        PostProcessor* m_Effect;
	public:
		DemoApplication(Context *context);
		~DemoApplication();

		virtual bool Init();
		virtual bool Destroy();

		virtual bool exec();
        virtual void Update(float deltaTime);
	};







}



