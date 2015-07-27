#version 120

uniform mat4 PM,CM,MVM;

varying vec3 v; 
varying vec3 N;
varying vec2 tex_coords_frag;

void main() 
{ 

	gl_Position = PM *CM*MVM  * gl_Vertex;

	v = vec3(MVM * gl_Vertex);
	N = normalize(mat3(MVM) * gl_Normal);
	
	tex_coords_frag= gl_MultiTexCoord0.xy;

} 