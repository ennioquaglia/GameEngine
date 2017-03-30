#include "Material.h"
#include "Texture.h"

#include "Shader.h"

#include "EString.h"
#include <ostream>

namespace Engine{

Material& Material::getDefaultMaterial()
{
    static Material mat;
    return mat;
}
void Material::setupShaderUniform(Shader *s)
{
    glDebug();
    glUniform1i(glGetUniformLocation(s->getProgramId(), "diffuseMap"), 0);
    glUniform1i(glGetUniformLocation(s->getProgramId(), "specularMap"), 1);
    glUniform1i(glGetUniformLocation(s->getProgramId(), "normalMap"), 2);
    glUniform1i(glGetUniformLocation(s->getProgramId(), "depthMap"), 3);
    glDebug();
}
Material::Material(){
    m_Diffuse  = Texture::load("#00FF00FF");
    m_Specular = Texture::load("#00FF00FF");
    m_Normal   = Texture::load("normal");
    m_HeightMap    = Texture::load("#808080FF");
    m_Shininess = 5;
}

Material::Material(PathName diffuse, PathName specular, PathName normal, float shininess)
{
    m_Diffuse  = Texture::load(diffuse);
    m_Specular = Texture::load(specular);
    m_Normal   = Texture::load(normal);
    m_HeightMap    = Texture::load("#808080FF");
    m_Shininess = shininess;
}
void Material::setDiffuse(PathName source)
{
    m_Diffuse = Texture::load(source);
}
void  Material::setSpecular(PathName source)
{
    m_Specular = Texture::load(source);
}
void  Material::setNormal(PathName source)
{
    m_Normal= Texture::load(source);
}
void Material::setHeightMap(PathName source)
{
    m_HeightMap = Texture::load(source);
}
void  Material::setShininess(float value)
{
    m_Shininess = value;
}

void Material::updateShader(Shader* s)
{
    s->setUniform("shininess", m_Shininess);
    glGetError();
}

void Material::bind()
{
    glActiveTexture(GL_TEXTURE0);
    m_Diffuse->bind();
    //glBindTexture(GL_TEXTURE_2D, diffuse->getID());
    glActiveTexture(GL_TEXTURE1);
    m_Specular->bind();
    //glBindTexture(GL_TEXTURE_2D, specular->getID());
    glActiveTexture(GL_TEXTURE2);
    m_Normal->bind();
    //glBindTexture(GL_TEXTURE_2D, normal->getID());
    glActiveTexture(GL_TEXTURE3);
    m_HeightMap->bind();

}


void Material::serializeXML(std::ostream &stream,unsigned level) const{
    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"<Material "
        <<SERIALIZE_VAR_NAME_PATH(m_Diffuse,m_Diffuse->filename)
       <<SERIALIZE_VAR_NAME_PATH(m_Specular,m_Specular->filename)
      <<SERIALIZE_VAR_NAME_PATH(m_Normal,m_Normal->filename)
     <<SERIALIZE_VAR_NAME_PATH(m_HeightMap,m_HeightMap->filename)
    <<SERIALIZE_VAR(m_Shininess)
    <<" >\n";

    for(unsigned i=0;i<level;++i)stream<<"\t";
    stream<<"</Material>\n";
}


}
