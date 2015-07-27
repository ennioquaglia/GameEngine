#include "Shader.h"
#include "../common/utility.h"
#include <fstream>
#include <sstream>
#include <conio.h>

using namespace Engine;

namespace Engine
{

	std::string stoi(int number)
	{
		std::stringstream ss;//create a stringstream
		ss << number;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	}
	std::string stof(float number)
	{
		std::stringstream ss;//create a stringstream
		ss << number;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	}
	vec3 getPosFromMatrix(mat4 matrix)
	{
		vec4 p = matrix * vec4(0, 0, 0, 1);
		return vec3(p);///p.w;
	}

	std::string loadTextFile(std::string path)
	{
		std::ifstream in(path.c_str(), std::ifstream::in);
		std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

		if(!contents.size()) {
			error("Error during file loading! Does the file exist?\n");
			contents = "Error during file loading! Does the file exist? \n";
		}
		//cout<<contents;
		return contents;
	}

	void consolePause()
	{
		getch();
	}

}

int Shader::getUniformLocation(string name)
{

	if(locations.find(name) != locations.end())return locations[name];
	else {
		int index = glGetUniformLocation(program, name.data());
		locations[name] = index;
		return index;
	}
}



GLuint Shader::getShaderId(std::string path, GLuint shader_type)
{
	std::string shader_src = Engine::loadTextFile(path);

	if(shader_src.find("#define USE_LIGHT 1") != -1) {
		use_light = true;
	}
	if(preProcessor)shader_src = preProcessor->parse(shader_src);
	//cout<<"\n"<<shader_src<<"\n";
	int v = 120;
	if(shader_src.find("#version") != -1) {
		sscanf(shader_src.substr(shader_src.find("#version") + 8, 6).data(), "%d", &v);
	}
	if(version < v)version = v;



	GLuint shader = glCreateShader(shader_type);
	char const *shader_src_pointer = shader_src.c_str();
	glShaderSource(shader, 1, &shader_src_pointer, NULL);
	glCompileShader(shader);
	GLint compiled;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(!compiled) {
		cout << "\n" << shader_src << "\n";
		is_working = 0;
		GLint blen = 0;
		GLsizei slen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);
		if(blen > 1) {
			GLchar* compiler_log = (GLchar*)malloc(blen);
			glGetShaderInfoLog(shader, blen, &slen, compiler_log);
			error(path + " -> compiler log:\n" + compiler_log);
			free(compiler_log);
			system("PAUSE");
		}
		//exit(1);
	} else {
		debug("Shader at : " + path + " compiled!");
	}
	return shader;
}

bool Shader::operator==(const Shader& s) const
{

	if(programs != s.programs)return 0;

	if(vertex == s.vertex && fragment == s.fragment)return 1;
	else return 0;
}

bool Shader::compare(Shader *s)
{

	if(programs != s->programs)return 0;

	if(vertex == s->vertex && fragment == s->fragment)return 1;
	return 0;
}

Shader::Shader()
{
	program = 0;
	vs = fs = gs = tcs = tes = 0;
	programs = 0;
	version = 120;
	use_light = 0;
	is_working = 1;
	useDefaultAttributes = 1;
	preProcessor = 0;

}
Shader::Shader(std::string VertexFilename, std::string FragmentFilename, Preprocessor *precpu)
{

	program = 0;
	vs = fs = gs = tcs = tes = 0;
	vertex = VertexFilename;
	fragment = FragmentFilename;
	programs = Vertex | Fragment ;
	version = 120;
	use_light = 0;
	is_working = 1;
	useDefaultAttributes = 1;
	preProcessor = precpu;
}

GLuint Shader::Compile(unsigned char mode)
{
	if(mode == Default)mode = programs;
	GLuint programId = glCreateProgram();
	glDebug();

	if(mode & Vertex && vertex.size()) {
		vs = getShaderId(vertex, GL_VERTEX_SHADER);
		glAttachShader(programId, vs);
	}

	if(mode & Fragment && fragment.size()) {
		fs = getShaderId(fragment, GL_FRAGMENT_SHADER);
		glAttachShader(programId, fs);
	}

	if(mode & Geometry && geometry.size()) {
		gs = getShaderId(geometry, GL_GEOMETRY_SHADER);
		glAttachShader(programId, gs);
	}

	if(mode & Tess_Control && tess_c.size()) {
		tcs = getShaderId(tess_c, GL_TESS_CONTROL_SHADER);
		glAttachShader(programId, tcs);
	}

	if(mode & Tess_Eval && tess_e.size()) {
		tes = getShaderId(tess_e, GL_TESS_EVALUATION_SHADER);
		glAttachShader(programId, tes);
	}
	glDebug();
	glLinkProgram(programId);
	glDebug();

	program = programId;

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		
		GLchar* compiler_log = (GLchar*)malloc(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, compiler_log);
		
		//We don't need the program anymore.
		//glDeleteProgram(program);

		Error("Shader error! :(" + compiler_log);
		//Use the infoLog as you see fit.
		system("pause");
		free(compiler_log);

		//In this simple program, we'll just leave
		return 0;
	} else Debug("Shader compiled! (id=" + stoi(program) + ")");
	return program;
}

void Shader::setShaderFile(std::string filename, unsigned char shaderType)
{
	if(!filename.size()) {
		programs |= !shaderType;
		return;
	}

	if(shaderType & Vertex) {
		vertex = filename;
		programs |= Vertex;
	}

	if(shaderType & Fragment) {
		fragment = filename;
		programs |= Fragment;
	}

	if(shaderType & Geometry) {
		geometry = filename;
		programs |= Geometry;
	}

	if(shaderType & Tess_Control) {
		tess_c = filename;
		programs |= Tess_Control;
	}

	if(shaderType & Tess_Eval) {
		tess_e = filename;
		programs |= Tess_Eval;
	}
}
