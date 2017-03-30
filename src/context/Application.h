#pragma once

namespace Engine {



	class Application
	{
	public:
		Application() {}
        virtual ~Application() {}

		virtual bool Init() = 0;
		virtual bool Destroy() = 0;
		
		virtual bool exec() = 0;
	};

}
