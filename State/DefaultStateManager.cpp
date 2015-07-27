#include "DefaultSceneState.h"

using namespace Engine;
    

void DefaultGameStateManager::Update(float deltaTimeMs){
	GameState *s=StateStack.top();
	if(!s)return;
	s->Update(deltaTimeMs);
}

void DefaultGameStateManager::Draw(){
	GameState *s=StateStack.top();
	if(!s)return;
	s->Draw();
}

void DefaultGameStateManager::handleEvent(Event &msg){
	GameState *s=StateStack.top();
	if(!s)return;
	
	s->handleEvent(msg);
}

void DefaultGameStateManager::Init() {
	menuState=new DefaultMenuState();
	sceneState=new DefaultSceneState();
	sceneState->setWindow(window);
	menuState->Init();
	sceneState->Init();
	PushState(sceneState);
	Debug("manager init");
}
void DefaultGameStateManager::Cleanup() {
	delete menuState;
	delete sceneState;
}
void DefaultGameStateManager::Pause() {}
void DefaultGameStateManager::Resume() {}

void DefaultGameStateManager::ChangeState(GameState* newState) {
	if(!newState)return;
	StateStack.pop();
	StateStack.push(newState);
}
void DefaultGameStateManager::PushState(GameState* newState) {
	if(!newState)return;
	StateStack.push(newState);
}
void DefaultGameStateManager::PopState() {
	StateStack.pop();
}





