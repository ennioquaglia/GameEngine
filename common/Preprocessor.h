#pragma once

#include <string>
#include <map>
#include "Debug.h"
/*

This class work like C/C++ preprocessor on files

It make possible for example to use the same shader file
under different enviroment (GLSL 1.2 / GLSL 4.0 ...) 


*/


class Preprocessor
{
		std::map<std::string,std::string> dictionary;
		bool isStringTrue(std::string comparision);
	public:
		Preprocessor(){}
		std::string parse(std::string source);
		void addVoiceToDictionary(std::string key,std::string value="");
		template <typename T> 
		void addVoiceToDictionary(std::string key,T value){
			addVoiceToDictionary(key,Engine::EString(value).data);
		}
		
		void printDictionary();
};

class GLSL_Preprocessor : public Preprocessor
{
	public:
		GLSL_Preprocessor(){}
		std::string parse(std::string source);

};
 