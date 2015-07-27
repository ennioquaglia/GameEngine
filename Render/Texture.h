#pragma once
#include "../common/utility.h"
#include <stdio.h>
#include <SOIL.h>

namespace Engine
{

    class Texture
    {
        friend class AssetManager;
    public:
        Texture() {
            texture=0;
        }
        Texture(std::string filename) {
            texture=0;
            load(filename);
        }

    private:

        void load(std::string filename);
    public:

        void destroy() {

        }

        bool bind() {
            if(!getID())return false;
            glBindTexture(GL_TEXTURE_2D, getID());
            return true;
        }

        GLuint getID() {
            if(!texture);//cout<<"text error";
            return texture;
        }

    private:
        string filename;
        GLuint texture;

    };
}
