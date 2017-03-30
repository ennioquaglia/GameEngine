#include "Debug.h"
#include <iostream>
#include <sstream>

namespace Engine {
	namespace internal{

        void log_message(int level, bool newline, EString &str)
		{
            switch(level){
                case QENGINE_LOG_LEVEL_WARN:
                std::cout <<"WARNING : ";
                break;
                case QENGINE_LOG_LEVEL_ERROR:
                std::cout <<"ERROR : ";
                break;
                case QENGINE_LOG_LEVEL_FATAL:
                std::cout <<"FATAL ERROR!!! : ";
                break;
            }

            std::cout << str.toStd();
			if (newline)std::cout << std::endl;
		}

		std::string approximate_time(float seconds) {
			if (seconds == 0)return "0s";

            std::string SUFFIXEX[] = { "s","ms","\x0e6s","ns" }; // s - ms - µs - ns
			float time = seconds;
			std::string a;
			for (int i = 0; i<4; i++) {
				if (time >= 1) {
					a = std::to_string(time);
					a += SUFFIXEX[i];
					return a;
				}
				time = time*1000.0f;
			}
			a = std::to_string(time);
			a += SUFFIXEX[3];
			return a;
		}

	}
}
