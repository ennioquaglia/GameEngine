#include "DefaultSceneState.h"

#include "../Core/Scene.h"


using namespace Engine;


DefaultSceneState::DefaultSceneState()
{
    scene=0;
    window=0;
}


void DefaultSceneState::setWindow(Window *w)
{
    window=w;
}
void DefaultSceneState::makeCurrentContext()
{
    if(!window)return;
    window->makeCurrent();
}
void DefaultSceneState::Update(float deltaTimeMs)
{
    if(scene)scene->Update(deltaTimeMs);
}

void DefaultSceneState::Draw()
{
    if(scene)scene->Render();
}

void DefaultSceneState::handleEvent(Event &msg){
	if(scene)scene->handleEvent(msg);
}

void DefaultSceneState::Init()
{
    scene=new Scene();
    scene->state=this;
    scene->window=window;
	scene->Init();
}
void DefaultSceneState::Cleanup()
{
    delete scene;
}
void DefaultSceneState::Pause() {}
void DefaultSceneState::Resume() {}








