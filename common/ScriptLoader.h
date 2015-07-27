#pragma once

#include "utility.h"
#include <windows.h>
using namespace std;

namespace Engine{
	class SceneManager;
}
class ScriptLoader
{
		string lastCmd,lastDll;
		HINSTANCE hGetProcIDDLL;
	public:
		ScriptLoader(){}
		
		void compileFromMakeFile(string filename){
			compileFromString(Engine::loadTextFile(filename));
		}
		void compileFromString(string str){
			system(str.data());
			lastCmd=str;
		}
		int loadFromDll(string filename,Engine::SceneManager *scene);
		
		void release(){
			FreeLibrary(hGetProcIDDLL); 
		}
		void compileAndReload(Engine::SceneManager *scene);
		void reload(Engine::SceneManager *scene);
};