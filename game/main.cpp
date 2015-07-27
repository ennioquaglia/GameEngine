//############//Game Engine//###########//
/*
Author: Ennio Quaglia
email:  ennio96@live.com

Last update: 07/2015
*/




#define GLEW_STATIC
#include "../State/DefaultSceneState.h"
#include "../Window/Window.h"
#include "../Window/glfwWindow.h"
#include "../Component/ComponentLibrary.h"
#include "../common/utility.h"
#include "../common/Event.h"
#include "../common/Parser.h"
#include "../Render/DebugRender.h"

#include <string.h>

#include "Source/Camera.h"
#include "Source/character.h"
#include "Source/ParticleSystem.h"
#include "../Render/PostProcessor.h"
#include "../Core/Application.h"





Actor* addMainActor(Scene *scene)
{
	Actor* actor = new Actor;

	TransformComponent* trans = new TransformComponent();
	actor->addComponent(trans);

	CameraComponent* cam = new CameraComponent();
	actor->addComponent(cam);

	actor->addComponent(new MyCharacterDynamicPhysicsComponent());

	PlayerAnimationManagerComponent *anim = new PlayerAnimationManagerComponent();
	actor->addComponent(anim);

	trans->setLocalPos(ComponentId::Render, vec3(0, -0.4, 0));
	trans->setPos(vec3(0, 5, 0.5));
	trans->setLocalPos(ComponentId::Light, vec3(0, 3, 0));
	trans->setLocalScale(ComponentId::Render, vec3(0.017));
	SkeletalMeshGraphicComponent * mesh = new SkeletalMeshGraphicComponent(AssetManager::getBasePath() + "Data/Model/mixamoWalkBlender.dae");

	mesh->velocity = 1.4;
	
	Material *mat = new Material();
	mat->setDiffuse(AssetManager::getBasePath() + "Data/Texture/default.png");
	mat->setShininess(200);
	mesh->addMaterial(mat);
	actor->addComponent(mesh);

	scene->systemManager->getSystem(StringHash("GenericSystem").getHash())->addComponent(cam);
	scene->systemManager->getSystem(StringHash("GenericSystem").getHash())->addComponent(mesh);
	
	scene->addActor(actor);

	return actor;
}

 

Actor* addMap()
{
	Actor* sphere = new Actor;

	TransformComponent* trans = new TransformComponent();

	trans->setPos(vec3(0, -0.01, 0));
	Rotation rot;
	rot.setEulerAngles(vec3(0, 0.785, 0));
	trans->setRotation(rot);
	sphere->addComponent(trans);
	glDebug();
	sphere->addComponent(new StaticMeshStaticPhysicsComponent(AssetManager::getBasePath() + "Data/Model/mappa4.obj"));
	glDebug();
	MeshGraphicComponent* mesh = new MeshGraphicComponent(AssetManager::getBasePath() + "Data/Model/mappa4.obj");
	glDebug();
	mesh->setTextureMatrix(mat2(scale(mat4(1), vec3(10))));
	glDebug();
	Material *mat = new Material();
	glDebug();
	mat->setDiffuse(AssetManager::getBasePath() + "Data/Texture/floor1_d.png");
	mat->setNormal(AssetManager::getBasePath() + "Data/Texture/floor1_n.png");
	mat->setSpecular(AssetManager::getBasePath() + "Data/Texture/floor1_s.png");
	mat->setShininess(20);
	glDebug();
	mesh->addMaterial(mat);
	glDebug();
	sphere->addComponent(mesh);
	glDebug();
	return sphere;
}

Actor* addModel(string filename, vec3 pos = vec3(0), vec3 s = vec3(1))
{

	Actor* sphere = new Actor;

	TransformComponent* trans = new TransformComponent();
	trans->setPos(pos);

	trans->setScale(s);
	sphere->addComponent(trans);
	glDebug();
	sphere->addComponent(new StaticMeshStaticPhysicsComponent(AssetManager::getBasePath() + filename));
	glDebug();
	MeshGraphicComponent* mesh = new MeshGraphicComponent(AssetManager::getBasePath() + filename);
	glDebug();
	mesh->setTextureMatrix(mat2(scale(mat4(1), vec3(10))));
	glDebug();
	Material *mat = new Material();
	glDebug();
	mat->setDiffuse(AssetManager::getBasePath() + "Data/Texture/floor1_d.png");
	mat->setNormal(AssetManager::getBasePath() + "Data/Texture/floor1_n.png");
	mat->setSpecular(AssetManager::getBasePath() + "Data/Texture/floor1_s.png");
	mat->setShininess(20);
	glDebug();
	mesh->addMaterial(mat);
	glDebug();
	sphere->addComponent(mesh);
	glDebug();
	return sphere;
}
Actor* addLucy(Scene* scene)
{
	string tex[] = {"#666666", "#666667", "#666668", "#666669"};
	for(int i = 0; i < 5; i++) {
		Actor* sphere = new Actor;

		vec3 pos(i * 3, 0, -25);
		TransformComponent* trans = new TransformComponent();
		trans->setScale(vec3(0.2));
		trans->setPos(pos);
		sphere->addComponent(trans);
		glDebug();
		sphere->addComponent(new SpherePhysicsComponent(1));
		glDebug();
		MeshGraphicComponent* mesh = new MeshGraphicComponent(AssetManager::getBasePath() + "Data/Model/lucy_low.obj");
		glDebug();
		mesh->setTextureMatrix(mat2(scale(mat4(1), vec3(10))));
		glDebug();
		Material *mat = new Material();
		glDebug();

		if(i) {
			mat->setDiffuse(tex[i - 1]);
			mat->setShininess(i * i * i * i);
		} else {
			mat->setDiffuse(AssetManager::getBasePath() + "Data/Texture/floor1_d.png");
			mat->setNormal(AssetManager::getBasePath() + "Data/Texture/floor1_n.png");
			mat->setSpecular(AssetManager::getBasePath() + "Data/Texture/floor1_s.png");
			mat->setShininess(20);
		}
		glDebug();
		mesh->addMaterial(mat);
		glDebug();
		sphere->addComponent(mesh);
		glDebug();


		scene->addActor(sphere);
	}
	return 0;
}

Actor* addAxis1()
{
	Actor* actor = new Actor;

	TransformComponent* trans = new TransformComponent();
	trans->setPos(vec3(5, 0, 5));

	Rotation rot;
	rot.setQuaternion(quat(vec3(0, 1.57, 0)));
	trans->setRotation(rot);

	actor->addComponent(trans);
	MeshGraphicComponent* mesh = new MeshGraphicComponent(AssetManager::getBasePath() + "Data/Model/axis.obj");
	Material *mat = new Material();
	mat->setDiffuse("#FF0000");
	mesh->addMaterial(mat);
	Material *mat1 = new Material();
	mat1->setDiffuse("#00FF00");
	mesh->addMaterial(mat1);
	mesh->addMaterial(mat1);
	Material *mat2 = new Material();
	mat2->setDiffuse("#0000FF");
	mesh->addMaterial(mat2);
	actor->addComponent(mesh);
	return actor;
}
Actor* addAxis2()
{
	Actor* actor = new Actor;

	TransformComponent* trans = new TransformComponent();
	trans->setPos(vec3(0, 0, 5));

	Rotation rot;
	rot.setEulerAngles(vec3(0, 1.57, 0));
	trans->setRotation(rot);

	actor->addComponent(trans);
	MeshGraphicComponent* mesh = new MeshGraphicComponent(AssetManager::getBasePath() + "Data/Model/axis.obj");
	Material *mat = new Material();
	mat->setDiffuse("#FF0000");
	mesh->addMaterial(mat);
	Material *mat1 = new Material();
	mat1->setDiffuse("#00FF00");
	mesh->addMaterial(mat1);
	mesh->addMaterial(mat1);
	Material *mat2 = new Material();
	mat2->setDiffuse("#0000FF");
	mesh->addMaterial(mat2);
	actor->addComponent(mesh);
	return actor;
}
Actor* addAxis3()
{
	Actor* actor = new Actor;

	TransformComponent* trans = new TransformComponent();
	trans->setPos(vec3(-5, 0, 5));

	Rotation rot;
	rot.setNormalDirection(vec3(1, 0, 0));
	trans->setRotation(rot);

	actor->addComponent(trans);
	MeshGraphicComponent* mesh = new MeshGraphicComponent(AssetManager::getBasePath() + "Data/Model/axis.obj");
	Material *mat = new Material();
	mat->setDiffuse("#FF0000");
	mesh->addMaterial(mat);
	Material *mat1 = new Material();
	mat1->setDiffuse("#00FF00");
	mesh->addMaterial(mat1);
	mesh->addMaterial(mat1);
	Material *mat2 = new Material();
	mat2->setDiffuse("#0000FF");
	mesh->addMaterial(mat2);
	actor->addComponent(mesh);
	return actor;
}


Actor* addSphereLight()
{
	Actor* sphere = new Actor;
	SphereGraphicComponent* comp = new SphereGraphicComponent();
	TransformComponent* trans = new TransformComponent();
	trans->setPos(vec3(0, 10, 0));


	sphere->addComponent(trans);
	sphere->addComponent(comp);
	SpherePhysicsComponent *p = new SpherePhysicsComponent(1);
	p->setMass(1);
	sphere->addComponent(p);
	LightComponent* l = new LightComponent();

	sphere->addComponent(l);

	return sphere;
}



Actor* addParticleSystem(Scene *scene, ParticleSystem* ps, vec3 pos)
{
	Actor* actor = new Actor;

	TransformComponent* trans = new TransformComponent();
	trans->setPos(pos);
	Rotation rot;

	actor->addComponent(trans);

	ActorComponent *c = ps;
	ps->setup();
	scene->systemManager->getSystem(StringHash("GenericSystem").getHash())->addComponent(c);

	actor->addComponent(c);
	scene->addActor(actor);

	return actor;
}


Actor* addSun(Scene *scene)
{
	Actor* actor = new Actor;
	
	TransformComponent* trans = new TransformComponent();
	trans->setPos(vec3(0, 20, 0));
	Rotation rot;
	actor->addComponent(trans);
	
	LightComponent* l = new DirectionalLightComponent();
	l->setDirectional(1);
	l->dir = normalize(vec3(0.5, 1, 0.2));
	actor->addComponent(l);
	scene->systemManager->getSystem(StringHash("GenericSystem").getHash())->addComponent(l);
	
	scene->addActor(actor);
	return actor;
}

  

class DemoApplication : public Application
{
	Actor* mainchar;
	Engine::glfwWindow *win;
	Scene* scene;

public:
	DemoApplication(): mainchar(0), win(0), scene(0) {}

	bool setup() {
		win = new Engine::glfwWindow;
		DefaultGameStateManager *manager = new DefaultGameStateManager(win);

		win->setApplication(this);
		win->setState(manager);
		win->setup();
		//AssetManager::setBasePath("");

		scene = manager->sceneState->scene;

		scene->systemManager->addSystem(new PhysicsSubSystem());
		scene->systemManager->addSystem(new GenericSubSystem());
		scene->systemManager->addSystem(new Generic30fpsSubSystem());
		scene->systemManager->addSystem(new Generic60fpsSubSystem());
		scene->systemManager->addRenderSystem(new DefaultRenderSubSystem());

		DebugRenderSubSystem *dr = new DebugRenderSubSystem();
		scene->systemManager->addRenderSystem(dr);
		scene->setEffect(new PostProcessor(scene));
		mainchar = addMainActor(scene);

		scene->addActor(addModel("Data/Model/mappa4.obj", vec3(-10, 0, -10), vec3(1.5)));
		scene->addActor(addModel("Data/Model/mappa5.obj", vec3(0), vec3(1.1)));
		scene->addActor(addAxis1());
		//	addLucy(scene);
		addParticleSystem(scene,new ParticleSystem(),vec3(10,0,20));
		addParticleSystem(scene,new ParticleSystem1(),vec3(-20,0,20));
		addSun(scene);

  
		loadScene(AssetManager::getBasePath() + "Data/Level/prova/", scene);
		loadScene(AssetManager::getBasePath() + "Data/Level/handmade/", scene);
		Debug("Setup done!\n\n\n\n\nPress F1 to switch to FreeCam\nPress F2 to spawn a light sphere\nPress F3 to get the position\n\n\n\nFor any question contact me at: ennio96@live.com\n\n\n");
	}
	bool exec() {
		win->exec();
	}
	void update() {

		if(win->inputManager->isKeyPressed(GLFW_KEY_F2)) {
			scene->addActor(addSphereLight());
		}
		if(win->inputManager->isKeyPressed(GLFW_KEY_F3)) {
			TransformComponent* trans = mainchar->getComponent<TransformComponent>(ComponentId::Transform);
			Debug(trans->getPos());
		}

	}


};

int main(int argc, char** argv)
{

	DemoApplication app;

	app.setup();

	return app.exec();
}
