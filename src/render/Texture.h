#pragma once
#include "Resource.h"

#include <string>
#include <stdio.h>


namespace Engine
{

    class Texture : public Resource<Texture,PathName>
    {
        friend class Resource<Texture,PathName>;
        friend class ResourceManager<Texture,PathName>;
        friend class AssetManager;
        friend class Material;
        friend class std::vector<Texture>;

        Texture();
        Texture(PathName filename);


    public:
        ~Texture();
        void destroy();
        bool bind();
        const unsigned int getID() const;

    private:
        void import(PathName _filename);

        PathName filename;
        unsigned int texture;

    };


}
