#pragma once
#include "Resource.h"
#include "Pathname.h"
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/anim.h>


namespace Engine{

    class AssimpScene: public Resource<AssimpScene,PathName,unsigned int>
        {
            friend class Resource<AssimpScene,PathName,unsigned int>;
            friend class ResourceManager<AssimpScene,PathName,unsigned int>;
            friend class AssetManager;

            AssimpScene();
            AssimpScene(PathName filename, unsigned int flag);

        public:

            const aiScene* operator*(){
                return scene;
            }

            void destroy();
            const aiScene* scene=0;

        };






}
