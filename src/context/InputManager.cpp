#include "InputManager.h"

#include <glm/glm.hpp>

namespace Engine {

    InputManager::InputManager()
    {
    }


    InputManager::~InputManager()
    {
    }



    bool DummyInputManager::isKeyDown(int key)const{return 0;}
    bool DummyInputManager::isKeyPressed(int key)const{return 0;}
    bool DummyInputManager::isKeyReleased(int key)const{return 0;}

    glm::vec2 DummyInputManager::getMousePos() const{return glm::vec2(0.f);}
    glm::vec2 DummyInputManager::getMousePositionDelta() const{return glm::vec2(0.f);}
    glm::vec2 DummyInputManager::getMousePositionDeltaAndCenter(float centerZone){return glm::vec2(0.f);}
    void DummyInputManager::setMousePos(glm::vec2 pos){}
    bool DummyInputManager::isMouseButtonDown(int button) const{return 0;}
    bool DummyInputManager::isMouseButtonPressed(int button) const{return 0;}
    bool DummyInputManager::isMouseButtonReleased(int button) const{return 0;}
    int DummyInputManager::getDeltaMouseScroll() const{return 0;}

    glm::vec3 DummyInputManager::getPositionMovement() const{return glm::vec3(0.f);}
    glm::vec3 DummyInputManager::getPositionMovementRotated(float ang) const{return glm::vec3(0.f);}


}
