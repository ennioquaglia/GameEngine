
#if GLSL_VERSION>=330

#version 330
 

layout(location = 0) in  vec3 vertex_position_modelspace;
layout(location = 2) in  vec2 tex_coords;


uniform mat4 PM,CM,MVM;
uniform mat2 textureMatrix;
out vec2 TexCoord;
out vec4 v_position;
void main()
{
	vec4 pos = vec4(vertex_position_modelspace, 1.0);
	v_position=gl_Position = PM * CM * MVM * pos;
	TexCoord=vec2(tex_coords.x,1.0-tex_coords.y);

}

#else

	
	Comprati una scheda video nuova poveraccio!


#endif