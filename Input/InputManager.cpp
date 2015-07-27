#include "../Core/Object.h"
#include "InputManager.h"
#include <algorithm>


using namespace Engine;

void InputManager::attachObserver(ActorComponent* a){
	if(find(components.begin(),components.end(),a)==components.end()){
		components.push_back(a);
	}
}
void InputManager::detachObserver(ActorComponent* a){
	list<ActorComponent*>::iterator it=find(components.begin(),components.end(),a);
	if(it!=components.end()){
		components.erase(it);
	}
}

void InputManager::send(Event &e){
	for(list<ActorComponent*>::iterator it=components.begin();it!=components.end();it++)
		(*it)->handleEvent(e);
	for(list<Actor*>::iterator it=actors.begin();it!=actors.end();it++)
		(*it)->handleEvent(e);
}


void InputManager::attachObserver(Actor* a){
	if(find(actors.begin(),actors.end(),a)==actors.end()){
		actors.push_back(a);
	}
}
void InputManager::detachObserver(Actor* a){
	list<Actor*>::iterator it=find(actors.begin(),actors.end(),a);
	if(it!=actors.end()){
		actors.erase(it);
	}
}

