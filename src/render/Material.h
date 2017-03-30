#pragma once

#include "Shader.h"
#include "texture.h"
#include "Pathname.h"

#include <string>
#include <list>
#include <vector>

namespace Engine{

    class Texture;
    class Shader;

    class Material//: public Resource<Shader>
    {

        float m_Shininess;
        Texture* m_Diffuse,*m_Specular,*m_Normal,*m_HeightMap;
    public:

        Material();
        Material(PathName diffuse,PathName specular="#666666FF",PathName normal="normal",float shininess=5.0f);
        void setDiffuse(PathName source);
        void setSpecular(PathName source);
        void setNormal(PathName source);
        void setHeightMap(PathName source);
        void setShininess(float value);
        void updateShader(Shader* s);//?
        void bind();

        static Material &getDefaultMaterial();
        static void setupShaderUniform(Shader *s);

        void serializeXML(std::ostream &stream,unsigned level) const;
    };

    typedef unsigned int MaterialId;
    typedef std::vector<Material> MaterialList;

}
