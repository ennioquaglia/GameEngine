#pragma once
#include <string>

namespace Engine{
	
	class Application
	{	
		public:
		
			Application(){}
			virtual bool setup() = 0;
			virtual bool exec() = 0;	
			virtual void update() = 0;
	};
	
}