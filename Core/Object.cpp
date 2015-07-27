#include "../common/AssetManager.h"

#include "Object.h"
#include "Engine.h"
#include "../Render/Shader.h"
#include "../Component/RenderComponent.h"
#include "../Core/Scene.h"

Actor::~Actor() {}

bool Actor::init() {}
void Actor::destroy() {}

void Actor::update(float deltaTimeMs, Engine::Scene* scene)
{
	// for(list<Event*>::iterator msg=msgQueue.begin(); msg!=msgQueue.end(); msg++) {
	//         for(Components::iterator it=components.begin(); it!=components.end(); it++) {
	//if(it->second!=(*msg).getSender())
	//                     (it->second)->handleEvent(*(*msg));
	//             }
	//
	//     }
	// msgQueue.clear();
}

// template <class ComponentType>
// ComponentType* Actor::getComponent(ComponentID id)
// {
// 	if(components.find(id)==components.end())return 0;
// 	return dynamic_cast<ComponentType*>(components[id]);
// }

void Actor::addComponent(ActorComponent* c)
{
	components[c->getComponentType()] = c;
	c->setOwner(this);
	//Debug("id : "+getId()+"   type : "+c->getComponentType());
	EventData<ActorComponent*> msg;
	msg.setType(componentAdded);
	msg.setData(c);
	sendEvent(msg);
}
void Actor::sendEvent(Event &msg)
{
	for(Components::iterator it = components.begin(); it != components.end(); it++)
		(it->second)->handleEvent(msg);
}
//SceneManager* Actor::getWorld(){return World;}

ActorComponent::ActorComponent()
{
	m_pOwner = 0;

}
ActorComponent::~ActorComponent() { }


bool ActorComponent::init() {}

void ActorComponent::sendLocalEvent(Event &msg)
{
	if(m_pOwner)m_pOwner->sendEvent(msg);
}
void ActorComponent::sendBroadcastEvent(Event &msg)
{
}

GraphicComponent::GraphicComponent()
{
	flag.add("renderable");
	flag.add("isVisible");
}



ComponentId::Type _getNewComponentId()
{
	static unsigned int id_counter = 0;
	//Debug("id :"+id_counter);
	return (ComponentId::Type)id_counter++;
}

void ActorManager::addActor(Actor* actor)
{
	actors[actor->getId()] = actor;
	actor->World = _Scene;
	if(_Scene)
		for(Actor::Components::iterator it = actor->components.begin(); it != actor->components.end(); it++) {
			_Scene->systemManager->addComponent(it->second);
		}
}

void DefaultRenderSubSystem::Init()
{
	glewExperimental = true; // Needed for core profile
	if(glewInit() != GLEW_OK) {
		Engine::error("Failed to initialize GLEW");
		return ;
	}
	if(glewIsSupported("GL_VERSION_2_1")) {
		Engine::debug("Ready for OpenGL 2.1");
	} else {
		Engine::warning("OpenGL 2.1 not supported");
		exit(1);
	}
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	float glsl_version = 0;
	sscanf((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION), "%f", &glsl_version);
	Debug("glsl-version: " + glsl_version + "/" + (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) + "/");
	GlobalsManager::Istance()->set("glsl_version", glsl_version); 
	Debug("GM:glsl-version:" + GlobalsManager::Istance()->get("glsl_version"));

	if(glewIsSupported("GL_EXT_texture_filter_anisotropic") || GLEW_EXT_texture_filter_anisotropic) {
		std::cout << "support anisotropic" << std::endl;
	}

	GLfloat max;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max);
	std::cout << max << std::endl;

	glGetFloatv(GL_MAX_VERTEX_ATTRIBS, &max);
	std::cout << "max attr:" << max << std::endl;
	glDebug();

}

class LightComponent;

void DefaultRenderSubSystem::update(float deltaTimeMs)
{
	/*
	for(Lights::iterator l = lights.begin(); l != lights.end(); l++) {
		(*l)->updateFrameBuffer(_Scene);
		//	cout<<"si";
	}*/
	glDebug();
}

void DefaultRenderSubSystem::render(uint flag)
{
	glDebug();


	list<Shader*> shader = Engine::AssetManager::getIstance()->shaders;
	if(flag & RenderFlag::Default) {
		int textureCount = 0;
		for(Lights::iterator it = lights.begin(); it != lights.end() && textureCount < 10; it++) {
			//for(int i=0; i<lights.size()&&textureCount<10; i++) {
			LightComponent *l = *it; //lights[i];
			if(l->flag == StringHash("directionalLight")) {
				DirectionalLightComponent *d = (DirectionalLightComponent*)l;
				for(int c = 0; c < d->numCascades; c++) {
					if(d->getTextureId(c) != 0) {
						int textureId = textureCount + 8;
						glActiveTexture(GL_TEXTURE0 + textureId);
						glBindTexture(GL_TEXTURE_2D, d->getTextureId(c));
						textureCount++;
					}
				}
			} else if(l->isDirectionalLight || l->spotCutoff <= 90) {
				if(l->shadowMap != 0) {
					int textureId = textureCount + 8;
					glActiveTexture(GL_TEXTURE0 + textureId);
					glBindTexture(GL_TEXTURE_2D, l->shadowMap);
					textureCount++;
				}
			}

		}
		glDebug();
		for(list<Shader*>::iterator s = shader.begin(); s != shader.end(); s++) {
			if(true) { //(*s)->useLight()) {
				int i = 0;
				(*s)->Use();
				glDebug();
				uint p = (*s)->getProgramId();

				for(Lights::iterator l = lights.begin(); l != lights.end(); l++, i++) {
					(*l)->updateUniforms(p, i);
					//	cout<<"si";
				}

				glUniform1i(glGetUniformLocation(p, "numLights"), i);
				{
					int textureCount = 0;
					int numDirLights = 0;
 
					for(Lights::iterator it = lights.begin(); it != lights.end() && textureCount < 10; it++) {
						//for(int i=0; i<lights.size()&&textureCount<10; i++) {
						LightComponent *l = *it; //lights[i];

						if(l->flag == StringHash("directionalLight")) {
							DirectionalLightComponent *d = (DirectionalLightComponent*)l;
							for(int c = 0; c < d->numCascades; c++) {
								if(d->getTextureId(c) != 0) {
									int loc = 23;
									char s[40] = {0};

									sprintf(s, "shadowMaps[%d]", textureCount);
									loc = glGetUniformLocation(p, s);
									if(loc > -1) {
										int textureId = textureCount + 8;
										glUniform1i(loc, textureId);
									}
									sprintf(s, "dirLights[%d].shadowMapID[%d]", numDirLights, c);
									loc = glGetUniformLocation(p, s);
									if(loc > -1) {
										glUniform1i(loc, textureCount);
									}
									sprintf(s, "dirLights[%d].DepthMVP[%d]", numDirLights, c);
									loc = glGetUniformLocation(p, s);
									if(loc > -1) {
										glUniformMatrix4fv(loc, 1, GL_FALSE, &d->depthMVP[c][0][0]);
									}
									textureCount++;
								}
							}
						} else if(l->isDirectionalLight || l->spotCutoff <= 90) {
							if(l->shadowMap != 0) {
								int loc = 23;
								char s[35] = {0};

								sprintf(s, "shadowMaps[%d]", textureCount);
								loc = glGetUniformLocation(p, s);
								if(loc > -1) {
									int textureId = textureCount + 8;
									glUniform1i(loc, textureId);
								}
								sprintf(s, "lights[%d].shadowMapID", i);
								loc = glGetUniformLocation(p, s);
								if(loc > -1) {
									glUniform1i(loc, textureCount);
								}
								sprintf(s, "lights[%d].DepthMVP", i);
								loc = glGetUniformLocation(p, s);
								if(loc > -1) {
									glUniformMatrix4fv(loc, 1, GL_FALSE, &l->depthMVP[0][0]);
								}
								textureCount++;
							} else Error("shadow map non inizializzata\n");
						}
					}
					glActiveTexture(GL_TEXTURE0);
				}
				glDebug();
				if(_Scene) {
					if(_Scene->current_camera) {
						glUniform3f(glGetUniformLocation(p, "EyeWorldPos"), _Scene->current_camera->Pos[0], _Scene->current_camera->Pos[1], _Scene->current_camera->Pos[2]);
						glUniformMatrix4fv(glGetUniformLocation(p, "CM"), 1, 0, &(_Scene->current_camera->getMatrix())[0][0]);
						glUniformMatrix4fv(glGetUniformLocation(p, "PM"), 1, 0, &(_Scene->current_camera->getProjectionMatrix())[0][0]);
						glUniform1i(glGetUniformLocation(p, "useVSM"),_Scene->options.useVSM);
	
					}
				}
				glDebug();
			}
		}
	} else {
		if(_Scene) {
			if(_Scene->current_camera) {
				for(list<Shader*>::iterator s = shader.begin(); s != shader.end(); s++) {
					(*s)->Use();

					uint p = (*s)->getProgramId();
					glUniform3f(glGetUniformLocation(p, "EyeWorldPos"), _Scene->current_camera->Pos[0], _Scene->current_camera->Pos[1], _Scene->current_camera->Pos[2]);
					glUniformMatrix4fv(glGetUniformLocation(p, "CM"), 1, 0, &(_Scene->current_camera->getMatrix())[0][0]);
					glUniformMatrix4fv(glGetUniformLocation(p, "PM"), 1, 0, &(_Scene->current_camera->getProjectionMatrix())[0][0]);
				}

			}
		}

	}


	glUseProgram(0);
	glDebug();
	GraphicComponent::RenderParameter param;
	param.flag = flag;
	param.scene = _Scene;

	if(flag & RenderFlag::Default) {
		for(GraphicComponents::iterator it = components.begin(); it != components.end(); it++) {
			glUseProgram(0);
			(it->second)[0]->bindRenderState(param);

			for(int i = 0; i < (it->second).size(); i++) {
				param.actor = (it->second)[i]->getOwner();
				(it->second)[i]->unitDraw(param);//(it->second)[i]->getOwner(),_Scene);
			}
			(it->second)[0]->unbindRenderState(param);
		}

	} else {
		for(GraphicComponents::iterator it = components.begin(); it != components.end(); it++) {
			glUseProgram(0);
			(it->second)[0]->bindRenderState(param);

			for(int i = 0; i < (it->second).size(); i++) {
				if((it->second)[i]->flag == StringHash("castShadow")) {
					param.actor = (it->second)[i]->getOwner();
					(it->second)[i]->unitDraw(param);//(it->second)[i]->getOwner(),_Scene);
				}
			}
			(it->second)[0]->unbindRenderState(param);
		}
	}
	glUseProgram(0);
	glDebug();
	
}

void DefaultRenderSubSystem::addComponent(ActorComponent* c)
{

	if(c->flag == StringHash("renderable")) {
		GraphicComponent* gra = (GraphicComponent*)(c);
		components[c->getComponentId()].push_back(gra);
	}


	if(c->flag == StringHash("light")) {
		LightComponent* l = (LightComponent*)(c);
		lights.push_back(l);
		l->createFrameBuffer(_Scene);
	}
	//Debug("added graphic component"+c->getComponentId());
}


void PhysicsSubSystem::Init()
{

	Debug("BULLET VERSION: " + btGetVersion());
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	/*btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
	m_overlappingPairCache = sweepBP;*/
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));


	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);
	btOverlappingPairCallback *ghostPairCallback = new btGhostPairCallback();
	dynamicsWorld->getPairCache()->setInternalGhostPairCallback(ghostPairCallback);
}




void Generic60fpsSubSystem::update(float deltaTimeMs)
{

	const double dt = 1 / 60.f;
	static float accumulator = 0;
	accumulator += deltaTimeMs / 1000.0f;

	ActorComponent::UpdateParameter param;
	param.scene = _Scene;
	param.deltaTimeMs = dt;
	param.input = _Scene->window->inputManager;


	while(accumulator >= dt) {
		for(Components::iterator it = components.begin(); it != components.end(); it++) {
			param.actor = (*it)->getOwner();
			(*it)->update(param);//dt,(*it)->getOwner(),_Scene);
		}
		accumulator -= dt;
	}
}

void Generic30fpsSubSystem::update(float deltaTimeMs)
{

	const double dt = 1 / 30.f;
	static float accumulator = 0;
	accumulator += deltaTimeMs / 1000.0f;

	ActorComponent::UpdateParameter param;
	param.scene = _Scene;
	param.deltaTimeMs = dt;
	param.input = _Scene->window->inputManager;


	while(accumulator >= dt) {
		for(Components::iterator it = components.begin(); it != components.end(); it++) {
			param.actor = (*it)->getOwner();
			(*it)->update(param);//dt,(*it)->getOwner(),_Scene);
		}
		accumulator -= dt;
	}
}


void GenericSubSystem::update(float deltaTimeMs)
{
	ActorComponent::UpdateParameter param;
	param.scene = _Scene;
	param.deltaTimeMs = deltaTimeMs;
	param.input = _Scene->window->inputManager;

	for(Components::iterator it = components.begin(); it != components.end(); it++) {
		param.actor = (*it)->getOwner();
		(*it)->update(param);
	}

}



void PhysicsSubSystem::update(float deltaTimeMs)
{

	static int a = 0;
	if(!a)Debug("physic update"), a++;



	const double dt = 1 / 60.f;
	static float accumulator = 0;
	accumulator += deltaTimeMs / 1000.0f;

	ActorComponent::UpdateParameter param;
	param.scene = _Scene;
	param.deltaTimeMs = dt;
	param.input = _Scene->window->inputManager;

	while(accumulator >= dt) {
		for(PhysicsComponents::iterator it = components.begin(); it != components.end(); it++)
			for(int i = 0; i < (it->second).size(); i++) {
				param.actor = (it->second)[i]->getOwner();
				(it->second)[i]->update(param);//dt,(it->second)[i]->getOwner(),_Scene);
			}
		//dynamicsWorld->stepSimulation(dt,5);
		accumulator -= dt;
	}
	dynamicsWorld->stepSimulation((deltaTimeMs / 1000.0f), 20); 
	for(PhysicsComponents::iterator it = components.begin(); it != components.end(); it++)
		for(int i = 0; i < (it->second).size(); i++)
			(it->second)[i]->updateMatrix();


}
