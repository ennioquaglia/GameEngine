#include "AssimpScene.h"

#define ASSIMP_STATIC
#include <assimp/cimport.h>


#include <assimp/Importer.hpp>

namespace Engine{

    AssimpScene::AssimpScene(){

    }

    AssimpScene::AssimpScene(PathName filename,unsigned int flag){
        scene=aiImportFile(filename.toStd().data(), flag);
    }

    void AssimpScene::destroy(){
        if(scene)aiReleaseImport(scene);
    }



}
