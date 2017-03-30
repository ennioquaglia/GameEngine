#pragma once

#include <string>
#include <map>
#include "Debug.h"
/*
This class work like C/C++ preprocessor on files

It make possible for example to use the same shader file
under different enviroment (GLSL 1.2 / GLSL 4.0 ...)
*/

namespace Engine {


	class Preprocessor
	{
		std::map<std::string, std::string> dictionary;
        std::string m_Prefix;
		bool isStringTrue(std::string comparision);
	public:
		Preprocessor() {}
        Preprocessor(std::string prefix):m_Prefix(prefix) {}
		std::string parse(std::string source);
        void addVoiceToDictionary(std::string key, std::string value = "");
        void addVoiceToDictionary(std::string line);
        template <typename T>
		void addVoiceToDictionary(std::string key, T value) {
            addVoiceToDictionary(key, Engine::EString(value).toStd());
		}

		void printDictionary();
	};

	class GLSL_Preprocessor : public Preprocessor
	{
	public:
		GLSL_Preprocessor() {}
		std::string parse(std::string source);

	};

}
