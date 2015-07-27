#include "ScriptLoader.h"
#include "../Core/Engine.h"

typedef void (__stdcall *voidFunctor)();
typedef void (__stdcall *gameFunctor)(Engine::SceneManager*);

int ScriptLoader::loadFromDll(string filename,Engine::SceneManager *scene)
{
    lastDll=filename;
    hGetProcIDDLL = LoadLibrary(filename.data());

    if(!hGetProcIDDLL) {
            Engine::error("could not load the dynamic library");
            return EXIT_FAILURE;
        }

    //# resolve function address here

    gameFunctor setup=(gameFunctor)GetProcAddress(hGetProcIDDLL, "GameSetup");
    if(!setup) {
            Engine::error("could not locate the function");
            return 0;
        }
    else {
            setup(scene);
        }

    // voidFunctor funci = (voidFunctor)GetProcAddress(hGetProcIDDLL, "HelloWorld");
    // if(!funci) {
    //         Engine::error("could not locate the function");
    //         return 0;
    //     }
    // else {
    //         funci();
    //     }

    return 1;
}

void ScriptLoader::compileAndReload(Engine::SceneManager *scene)
{
    release();
    compileFromString(lastCmd);
    loadFromDll(lastDll,scene);
}

void ScriptLoader::reload(Engine::SceneManager *scene)
{
    loadFromDll(lastDll,scene);
}
