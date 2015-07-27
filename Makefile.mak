CPP    = g++
WINDRES= windres
AR     = ar
RANLIB = ranlib
RM     = rm -f
OBJS   = Core/Engine.o \
         Core/Object.o \
         Core/Scene.o \
         Physic/btKinematicCharacterController.o \
         Physic/GlobalsManager.o \
         Physic/btCustomKinematicCharacterController.o \
         Physic/CharacterController.o \
         State/DefaultStateManager.o \
         State/DefaultSceneState.o \
         State/DefaultMenuState.o \
         common/ScriptLoader.o \
         common/AssetManager.o \
         common/utility.o \
         common/Loader.o \
         common/Preprocessor.o \
         common/Camera.o \
         Input/InputManager.o \
         Window/Window.o \
         Window/glfwWindow.o \
         Render/Shader.o \
         Render/Material.o \
         Render/StaticMesh.o \
         Render/SkeletalMesh.o \
         Render/vao.o \
         Render/RenderUtility.o \
         Component/ComponentLibrary.o \
         Component/RenderComponent.o \
         Component/PhysicComponent.o \
         AppResource.res

LIBS   = -lglfw32 -lglew32s -lsfml-main -lsfml-window -lsfml-system -lsfml-graphics -lglut32 -lglu32 -lopengl32 -lBulletDynamics -lBulletCollision -llinearMath -lassimp -lgdi32 -lEngine -lSOIL -Iinclude -Llib
CFLAGS = -Iinclude -Ilib

.PHONY: lib/libEngine.a clean clean-after

all: lib/libEngine.a

clean:
	$(RM) $(OBJS) lib/libEngine.a

clean-after:
	$(RM) $(OBJS)

lib/libEngine.a: $(OBJS)
	$(AR) rc $@ $(OBJS)
	$(RANLIB) $@

Core/Engine.o: Core/Engine.cpp common/utility.h Core/Engine.h Core/Object.h Component/ComponentLibrary.h common/ScriptLoader.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Core/Object.o: Core/Object.cpp common/AssetManager.h Core/Object.h Core/Engine.h Render/Shader.h Component/RenderComponent.h Core/Scene.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Core/Scene.o: Core/Scene.cpp Core/Scene.h common/utility.h Core/Object.h Component/ComponentLibrary.h common/ScriptLoader.h Input/InputEvent.h Window/Window.h Render/PostProcessor.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Physic/btKinematicCharacterController.o: Physic/btKinematicCharacterController.cpp include/LinearMath/btIDebugDraw.h include/BulletCollision/CollisionDispatch/btGhostObject.h include/BulletCollision/CollisionShapes/btMultiSphereShape.h include/BulletCollision/BroadphaseCollision/btOverlappingPairCache.h include/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h include/BulletCollision/CollisionDispatch/btCollisionWorld.h include/LinearMath/btDefaultMotionState.h Physic/btKinematicCharacterController.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Physic/GlobalsManager.o: Physic/GlobalsManager.cpp
	$(CPP)  -c $< -o $@ $(CFLAGS)

Physic/btCustomKinematicCharacterController.o: Physic/btCustomKinematicCharacterController.cpp Physic/btCustomKinematicCharacterController.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Physic/CharacterController.o: Physic/CharacterController.cpp Physic/CharacterController.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

State/DefaultStateManager.o: State/DefaultStateManager.cpp State/DefaultSceneState.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

State/DefaultSceneState.o: State/DefaultSceneState.cpp State/DefaultSceneState.h Core/Scene.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

State/DefaultMenuState.o: State/DefaultMenuState.cpp State/State.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

common/ScriptLoader.o: common/ScriptLoader.cpp common/ScriptLoader.h Core/Engine.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

common/AssetManager.o: common/AssetManager.cpp common/AssetManager.h common/utility.h Render/StaticMesh.h Render/SkeletalMesh.h Render/Shader.h Render/Texture.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

common/utility.o: common/utility.cpp common/utility.h Physic/Physics.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

common/Loader.o: common/Loader.cpp
	$(CPP)  -c $< -o $@ $(CFLAGS)

common/Preprocessor.o: common/Preprocessor.cpp common/Preprocessor.h common/GlobalsManager.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

common/Camera.o: common/Camera.cpp common/Camera.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Input/InputManager.o: Input/InputManager.cpp Core/Object.h Input/InputManager.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Window/Window.o: Window/Window.cpp Window/Window.h State/State.h Core/Application.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Window/glfwWindow.o: Window/glfwWindow.cpp Window/glfwWindow.h State/State.h Core/Application.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Render/Shader.o: Render/Shader.cpp Render/Shader.h common/utility.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Render/Material.o: Render/Material.cpp Render/Material.h Render/Texture.h common/AssetManager.h Render/Shader.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Render/StaticMesh.o: Render/StaticMesh.cpp Render/StaticMesh.h common/utility.h Render/Shader.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Render/SkeletalMesh.o: Render/SkeletalMesh.cpp common/AssetManager.h Render/SkeletalMesh.h common/utility.h Render/Shader.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Render/vao.o: Render/vao.cpp Render/vao.h common/Debug.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Render/RenderUtility.o: Render/RenderUtility.cpp Render/RenderUtility.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Component/ComponentLibrary.o: Component/ComponentLibrary.cpp Component/ComponentLibrary.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Component/RenderComponent.o: Component/RenderComponent.cpp Component/RenderComponent.h Render/RenderFlag.h common/Hash.h
	$(CPP)  -c $< -o $@ $(CFLAGS)

Component/PhysicComponent.o: Component/PhysicComponent.cpp
	$(CPP)  -c $< -o $@ $(CFLAGS)

AppResource.res: AppResource.rc
	$(WINDRES) -i AppResource.rc -J rc -o AppResource.res -O coff

