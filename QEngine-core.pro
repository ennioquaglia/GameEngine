
CONFIG += c++11

#CONFIG += console c++11
#CONFIG -= app_bundle
CONFIG -= qt




DEFINES += ENGINE_ENTITIES_HAVE_NAMES=1  ENGINE_OBJECT_VERSION_BITS=8

DEFINES += ENGINE_TEST






contains(DEFINES, ENGINE_TEST) {
    TEMPLATE = app
    CONFIG += console
    DEFINES += ENGINE_USE_GLFW
} else {
    TEMPLATE = lib
    CONFIG += staticlib
    DEFINES += ENGINE_USE_QT_WIDGET
}

QMAKE_CXXFLAGS += -Wno-enum-compare -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-comment -Wno-non-template-friend #-fno-exceptions





INCLUDEPATH +=   "$$_PRO_FILE_PWD_/../../../Dependencies/include"
LIBS +=   "$$_PRO_FILE_PWD_/../../../Dependencies/lib/glew32s.lib"  -lglfw3  -lassimp  -lglu32  -lSOIL -lopengl32 -L"$$_PRO_FILE_PWD_/../../../Dependencies/lib"
#LIBS += -L"$$_PRO_FILE_PWD_/../../Dependencies/lib"  -lopengl32 -lassimp -lSOIL -lglu32        "$$_PRO_FILE_PWD_/../../Dependencies/lib/glew32s.lib"     "$$_PRO_FILE_PWD_/../../Dependencies/lib/glfw3.lib"


#  Bullet physics
#LIBS+= "$$_PRO_FILE_PWD_/../../../Dependencies/lib/BulletDynamics.lib" "$$_PRO_FILE_PWD_/../../../Dependencies/lib/BulletCollision.lib" "$$_PRO_FILE_PWD_/../../../Dependencies/lib/linearMath.lib"
LIBS +=  -lBulletDynamics -lBulletCollision -lLinearMath
INCLUDEPATH += "$$_PRO_FILE_PWD_/../../../Dependencies/include/Bullet"


INCLUDEPATH += "src/" "src/utils/" "src/render/" "src/core/" "src/context/" "src/components/"

SOURCES += src/main.cpp \
    src/components/LightComponent.cpp \
    src/components/StaticMeshComponent.cpp \
    src/components/TransformComponent.cpp \
    src/context/Context.cpp \
    src/context/GlfwInputManager.cpp \
    src/context/GlfwWindow.cpp \
    src/context/InputManager.cpp \
    src/core/Object.cpp \
    src/core/Scene.cpp \
    src/render/Camera.cpp \
    src/render/FrameBufferUtility.cpp \
    src/render/Material.cpp \
    src/render/MeshData.cpp \
    src/render/PlatformRenderer.cpp \
    src/render/PostProcessEffect.cpp \
    src/render/Shader.cpp \
    src/render/Texture.cpp \
    src/render/VAO.cpp \
    src/utils/AssimpScene.cpp \
    src/utils/Debug.cpp \
    src/utils/EString.cpp \
    src/utils/Event.cpp \
    src/utils/Preprocessor.cpp \
    src/DemoApplication.cpp \
    src/render/FrameBufferUtility.tpp \
    src/components/PhysicsComponent.cpp \
    src/utils/physics.cpp \
    src/components/CharacterPhysicsComponent.cpp \
    src/utils/Hash.cpp \
    src/components/SkeletalMeshComponent.cpp \
    src/utils/pugixml.cpp \
    src/render/Primitive.cpp \
    src/utils/Shell.cpp

HEADERS += \
    src/components/LightComponent.h \
    src/components/StaticMeshComponent.h \
    src/components/TransformComponent.h \
    src/context/Application.h \
    src/context/Context.h \
    src/context/GlfwInputManager.h \
    src/context/GlfwWindow.h \
    src/context/InputManager.h \
    src/context/InputState.h \
    src/core/Object.h \
    src/core/Scene.h \
    src/render/Camera.h \
    src/render/FrameBufferUtility.h \
    src/render/Material.h \
    src/render/MeshData.h \
    src/render/PlatformRenderer.h \
    src/render/PostProcessEffect.h \
    src/render/Shader.h \
    src/render/Texture.h \
    src/render/VAO.h \
    src/utils/AssimpScene.h \
    src/utils/Debug.h \
    src/utils/enum_flags.h \
    src/utils/EString.h \
    src/utils/Event.h \
    src/utils/Flag.h \
    src/utils/Geometry.h \
    src/utils/Hash.h \
    src/utils/Pathname.h \
    src/utils/Preprocessor.h \
    src/utils/Resource.h \
    src/utils/Shell.h \
    src/DemoApplication.h \
    src/demoentity.h \
    src/components/PhysicsComponent.h \
    src/utils/physics.h \
    src/components/CharacterPhysicsComponent.h \
    src/components/SkeletalMeshComponent.h \
    src/utils/pugiconfig.hpp \
    src/utils/pugixml.hpp \
    src/render/Primitive.h \
    src/utils/Container.h

