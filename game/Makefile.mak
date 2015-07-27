CPP    = g++
RM     = rm -f
OBJS   = main.o \
         ../common/Parser.o \
         ../common/pugixml.o \
         Source/character.o \
         Source/Camera.o

LIBS   = -I../include -L../lib -lEngine -lglfw3 -lglew32s -lSOIL -lglu32 -lglut32 -lBulletDynamics -lbulletCollision -llinearMath -lopengl32 -lassimp
CFLAGS = -I..\include -I..\lib

.PHONY: game.exe clean

all: game.exe

clean:
	$(RM) $(OBJS) game.exe

game.exe: $(OBJS)
	$(CPP) -O2 -o $@ $(OBJS) $(LIBS)

main.o: main.cpp ../State/DefaultSceneState.h ../Window/Window.h ../Window/glfwWindow.h ../Component/ComponentLibrary.h ../common/utility.h ../common/Event.h ../common/Parser.h ../Render/DebugRender.h Source/Camera.h Source/character.h Source/ParticleSystem.h ../Render/PostProcessor.h ../Core/Application.h
	$(CPP) -O2 -c $< -o $@ $(CFLAGS)

../common/Parser.o: ../common/Parser.cpp ../common/Parser.h ../common/pugixml.hpp ../Component/ComponentLibrary.h ../Core/Scene.h
	$(CPP) -O2 -c $< -o $@ $(CFLAGS)

../common/pugixml.o: ../common/pugixml.cpp ../common/pugixml.hpp
	$(CPP) -O2 -c $< -o $@ $(CFLAGS)

Source/character.o: Source/character.cpp Source/character.h
	$(CPP) -O2 -c $< -o $@ $(CFLAGS)

Source/Camera.o: Source/Camera.cpp
	$(CPP) -O2 -c $< -o $@ $(CFLAGS)

