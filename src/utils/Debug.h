#pragma once

#include "EString.h"


#define QENGINE_LOG_LEVEL_FATAL 0
#define QENGINE_LOG_LEVEL_ERROR 1
#define QENGINE_LOG_LEVEL_WARN  2
#define QENGINE_LOG_LEVEL_INFO  3



namespace Engine {


namespace internal{



	extern void log_message(int level, bool newline, EString &str);
	extern std::string approximate_time(float seconds);



	}
}


#ifdef _WIN32
#define QENGINE_PAUSE() system("pause")
#else
#define QENGINE_PAUSE() //todo
#endif


#ifndef QENGINE_LOG_LEVEL
#define QENGINE_LOG_LEVEL QENGINE_LOG_LEVEL_WARN //QENGINE_LOG_LEVEL_INFO
#endif

#if QENGINE_LOG_LEVEL >= QENGINE_LOG_LEVEL_FATAL
#define QENGINE_FATAL(a) Engine::internal::log_message(QENGINE_LOG_LEVEL_FATAL, true, Engine::EString()+a)
#define _QENGINE_FATAL(a) Engine::internal::log_message(QENGINE_LOG_LEVEL_FATAL, false, Engine::EString()+a)
#else
#define QENGINE_FATAL(a)
#endif

#if QENGINE_LOG_LEVEL >= QENGINE_LOG_LEVEL_ERROR
#define QENGINE_ERROR(a) Engine::internal::log_message(QENGINE_LOG_LEVEL_ERROR, true, Engine::EString()+a)
#else
#define QENGINE_ERROR(a)
#endif

#if QENGINE_LOG_LEVEL >= QENGINE_LOG_LEVEL_WARN
#define QENGINE_WARN(a) Engine::internal::log_message(QENGINE_LOG_LEVEL_WARN, true, Engine::EString()+a)
#else
#define QENGINE_WARN(a)
#endif


#if QENGINE_LOG_LEVEL >= QENGINE_LOG_LEVEL_INFO
#define QENGINE_INFO(a) Engine::internal::log_message(QENGINE_LOG_LEVEL_INFO, true, Engine::EString()+a)
#else
#define QENGINE_INFO(a)
#endif

#define QENGINE_DEBUG(a) QENGINE_INFO(a)


#if QENGINE_LOG_LEVEL >= QENGINE_LOG_LEVEL_ERROR

#ifndef GL_DEBUG
#define GL_DEBUG 1
#endif



#if GL_DEBUG

	#define glDebug0() {\
						int e=glGetError();\
						if(e)QENGINE_ERROR("OpenGL ERROR "+e+" in line number "+__LINE__+" in file "+__FILE__);\
			}

	#define glDebug1(s) {\
						int e=glGetError();\
						if(e)QENGINE_ERROR("OpenGL ERROR "+e+" in line number "+__LINE__+" in file "+__FILE__+"\n\tMessage:"+s);\
			}					

	#define GET_MACROd(_0,NAMEd,...) NAMEd
//#define glDebug(...) GET_MACROd(_0,##__VA_ARGS__,glDebug1, glDebug0)(__VA_ARGS__)

//#define glDebug(...) GET_MACROd(_0,__VA_ARGS__,glDebug1, glDebug0)(__VA_ARGS__)
//#define glDebug(...) (sizeof(#__VA_ARGS__) == sizeof("") ? glDebug0() : glDebug1(__VA_ARGS__))


#define glDebug(...) glDebug0()

#else
	#define glDebug(...)
#endif

#endif
