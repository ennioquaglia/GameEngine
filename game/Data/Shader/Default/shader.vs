#version 120
//#version 330 core

//uniform vec4 LightPos;
uniform mat4 PM,CM,MVM;
uniform float time;

varying vec4 color;
varying vec3 v; 
varying vec3 N,T;
varying vec2 tex_coords_frag;

void main() 
{ 

	gl_Position = PM * CM * MVM * gl_Vertex;

	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
	v = vec3(MVM * gl_Vertex);
	N = normalize(mat3(MVM) * gl_Normal);
	T=vec3(0.);
	color=gl_Color;
	tex_coords_frag= gl_MultiTexCoord0.xy;

} 