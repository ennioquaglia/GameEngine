#version 330
 
uniform mat4 PM,CM,MVM;
uniform mat2 textureMatrix;


 
layout(location = 0) in vec3 vertex_position_modelspace;
attribute vec3 normal_modelspace;
attribute vec2 tex_coords;
attribute vec3 tangent;

 
varying vec4 color;
varying vec2 tex_coords_frag;

varying vec3 v; 
varying vec3 vEye; 
varying vec3 N;

varying vec3 T;

varying float toDiscard;
//varying vec3 C;


	
void main()
{

		
		
		mat3 NormalMatrix=mat3(MVM);
        vec4 position = vec4(vertex_position_modelspace, 1.0);
        
        gl_Position = PM * CM * MVM  * position;
		vEye=gl_Position.xyz;
		color=gl_Color;
		tex_coords_frag=textureMatrix*vec2(tex_coords.x,1.0-tex_coords.y);
		
		v = vec3(MVM * position);
		N = normalize(NormalMatrix * normal_modelspace);
		T =  normalize(NormalMatrix* tangent);

		color=gl_Color;


}