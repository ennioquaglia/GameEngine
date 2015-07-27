 
#include "ComponentLibrary.h"




 
Actor* addSphereActor()
{
	Actor* sphere=new Actor;
	SphereGraphicComponent* comp=new SphereGraphicComponent();
	TransformComponent* trans=new TransformComponent();
	
	sphere->addComponent(trans);
	sphere->addComponent(comp);
	sphere->addComponent(new SpherePhysicsComponent());
	sphere->addComponent(new BasicUserInputComponent());
	
	trans->setPos(vec3(5,5,0));
	//RenderSubSystem::instance()->addComponent(comp);
	return sphere;
}

