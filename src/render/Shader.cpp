#include "Shader.h"
#include "Debug.h"
#include "Hash.h"

#include <fstream>
#include <sstream>
#include <conio.h>
#include <list>

namespace Engine {

template <typename T, typename U>
class create_map
{
private:
    std::map<T, U> m_map;
public:
    create_map(const T& key, const U& val)
    {
        m_map[key] = val;
    }

    create_map<T, U>& operator()(const T& key, const U& val)
    {
        m_map[key] = val;
        return *this;
    }

    operator std::map<T, U>()
    {
        return m_map;
    }
};
std::map<int,int> enum_opengl_map = create_map<int, int >(Shader::Vertex,GL_VERTEX_SHADER)(Shader::Fragment,GL_FRAGMENT_SHADER)(Shader::Geometry,GL_GEOMETRY_SHADER)(Shader::Tess_Control,GL_TESS_CONTROL_SHADER)(Shader::Tess_Eval,GL_TESS_EVALUATION_SHADER);


int Shader::getUniformLocation(std::string name) {

    if (locations.find(name) != locations.end())return locations[name];
    else {
        int index = glGetUniformLocation(program, name.data());
        locations[name] = index;
        return index;
    }
}


GLuint Shader::getShaderId(std::string path, GLuint shader_type)
{
    std::string shader_src = Shell::loadTextFile(path);

    if (shader_src.find("#define USE_LIGHT 1") != std::string::npos) {
        use_light = true;
    }
    if(!m_PreprocessorDefinitions.empty()){
        Preprocessor pp(m_PreprocessorDefinitions);
        shader_src= pp.parse(shader_src);
    }
    int v = 120;
    if (shader_src.find("#version") != std::string::npos) {
        sscanf(shader_src.substr(shader_src.find("#version") + 8, 6).data(), "%d", &v);
    }
    if (version < v)version = v;


    GLuint shader = glCreateShader(shader_type);
    char const *shader_src_pointer = shader_src.c_str();
    glShaderSource(shader, 1, &shader_src_pointer, NULL);
    glCompileShader(shader);
    GLint compiled;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        //cout << "\n" << shader_src << "\n";
        is_working = 0;
        GLint blen = 0;
        GLsizei slen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);
        if (blen > 1) {
            GLchar* compiler_log = (GLchar*)malloc(blen);
            glGetShaderInfoLog(shader, blen, &slen, compiler_log);
            QENGINE_ERROR(path + " -> compiler log:\n" + compiler_log);
            QENGINE_INFO(shader_src);
            free(compiler_log);
            QENGINE_PAUSE();
        }
        //exit(1);
    }
    else {
        EString::replaceAll(path,"../",".");
        EString::replaceAll(path,"..",".");
        QENGINE_INFO("Shader at : " + path + " compiled!");
    }
    return shader;
}

bool Shader::operator==(const Shader& s) const
{

    if (programs != s.programs)return 0;


    for(unsigned int i=0;i<6;++i)if(m_Filenames[i]!=s.m_Filenames[i])return 0;

    return 1;
}

bool Shader::compare(Shader *s)
{

    if (programs != s->programs)return 0;

    for(unsigned int i=0;i<6;++i)if(m_Filenames[i]!=s->m_Filenames[i])return 0;

    return 1;
}

Shader::Shader() {
    program = 0;
    programs = 0;
    version = 120;
    use_light = 0;
    is_working = 1;
    useDefaultAttributes = 1;
    for(unsigned int i=0;i<6;++i)m_obj_id[i]=0;

}
Shader::Shader(PathName VertexFilename, PathName FragmentFilename, EString preprocessorDefinitions)
{

    program = 0;

    programs = Vertex | Fragment;
    version = 120;
    use_light = 0;
    is_working = 0;
    useDefaultAttributes = 1;
    m_PreprocessorDefinitions = preprocessorDefinitions.data;

    m_Filenames[0]=VertexFilename.data;
    m_Filenames[1]=FragmentFilename.data;
    for(unsigned int i=0;i<6;++i)m_obj_id[i]=0;
}



GLuint Shader::compile(ShaderType mode){
    if (mode == Default)mode = (ShaderType)programs;
    else programs=(int)mode;
    GLuint programId = glCreateProgram();
    glDebug();


    for(int i=0;i<5;++i){

        if((1<<i) & mode){
            QENGINE_INFO(m_Filenames[i]+" "+enum_opengl_map[Shader::Vertex]+" "+(int)Shader::Vertex+" "+(int)GL_VERTEX_SHADER);
            m_obj_id[i]= getShaderId(m_Filenames[i], enum_opengl_map[1<<i]);

            glAttachShader(programId, m_obj_id[i]);
        }
    }

    glLinkProgram(programId);
    glDebug();

    program = programId;
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        GLchar* compiler_log = (GLchar*)malloc(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, compiler_log);

        QENGINE_ERROR("Shader linker error!  -  " + compiler_log);
        QENGINE_PAUSE();
        free(compiler_log);
        return 0;
    }
    else {
        QENGINE_INFO("Shader linked! (id=" + std::to_string(program) + ")");
        is_working=true;

        Use();
        setupUniformLocations();
        glUseProgram(0);


    }
    return program;

}

void Shader::setShaderFile(PathName filename_, ShaderType shaderType)
{
    std::string filename=filename_.data;

    int i=0;
    while((1<<i)!=shaderType && i<6)++i;

    if(i==6){QENGINE_ERROR((int)shaderType+" shader type doesn't exist");return;}

    QENGINE_INFO("shader type:"+i);
    m_Filenames[i]=filename;


}


void Shader::setupUniformLocations(){

    //todo struct
    //todo fix stoi issue
    for(int i=0;i<6;++i){

        if(m_obj_id[i] && m_Filenames[i].size()){

            std::string path=m_Filenames[i];
            std::string shader_src = Shell::loadTextFile(path);

            if (shader_src.find("#define USE_LIGHT 1") != std::string::npos) {
                use_light = true;
            }
            if(!m_PreprocessorDefinitions.empty()){
                Preprocessor pp(m_PreprocessorDefinitions);
                shader_src= pp.parse(shader_src);
            }
            int v = 120;
            if (shader_src.find("#version") != std::string::npos) {
                sscanf(shader_src.substr(shader_src.find("#version") + 8, 6).data(), "%d", &v);
            }
            if (version < v)version = v;

            std::string str,word;
            unsigned int found=shader_src.find("uniform");

            while(found!=std::string::npos){
                str=shader_src.substr(found+7,std::max((int)(shader_src.find(";",found+7)-found-7),0));
                std::istringstream stream(str);
                stream>>word;

                str=str.substr(str.find(word)+word.length())+",";

                word.clear();
                bool equal=false;
                for(char c:str){
                    if(equal){if(c==',')equal=0;}
                    else{
                        if(c=='='||c==','){
                            if(c=='=')equal=1;
                            word=trim(word);
                            unsigned int bracket=word.find("[");
                            if(bracket!=std::string::npos){
                                int size=0;
                                try{
                                    size= std::stoi(word.substr(bracket+1));
                                }catch(const std::exception& e) {

                                    QENGINE_ERROR(e.what());
                                }
                                word=word.substr(0,bracket);
                                for(int i=0;i<size;++i){
                                    std::string s=word+"["+std::to_string(i)+"]";
                                    m_uniforms[Hash(s)]=glGetUniformLocation(program, s.data());
                                }

                            }else{
                                m_uniforms[Hash(word)]=glGetUniformLocation(program, word.data());
                            }
                            word.clear();
                        }else word+=c;
                    }
                }

                found=shader_src.find("uniform",found+1);

            }


        }
    }
}

}
