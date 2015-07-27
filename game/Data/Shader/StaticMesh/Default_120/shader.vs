#version 120
 

attribute vec3 vertex_position_modelspace;
attribute vec3 normal_modelspace;
attribute vec2 tex_coords;
attribute vec3 tangent;


uniform mat4 PM,CM,MVM;
uniform mat2 textureMatrix;
 
 
varying vec3 V;
varying vec3 N;
varying vec3 T;
varying vec2 TexCoord;

#define USE_FIXED_INTERFACE

void main()
{
	#ifdef USE_FIXED_INTERFACE
		vec4 position = vec4(vertex_position_modelspace, 1.0);
        
        //gl_Position =  PM * CM *MVM  *  position;
		gl_Position = PM * CM *MVM * gl_Vertex;
		
		TexCoord=textureMatrix*gl_MultiTexCoord0.xy;//tex_coords.xy;
		V= vec3(MVM * gl_Vertex);//vertex_position_modelspace;//gl_Position.xyz;
		N=vec3(MVM * vec4(gl_Normal,0));//(gl_ModelViewProjectionMatrix *vec4(normal_modelspace,0)).xyz;
		T=vec3(MVM * gl_Color);//(gl_ModelViewProjectionMatrix *vec4(tangent,0)).xyz;
	#else
        vec4 position = vec4(vertex_position_modelspace, 1.0);
        
        gl_Position =  PM * CM *MVM  *  position;
		//gl_Position = PM * CM *MVM * gl_Vertex;
		
		TexCoord=textureMatrix*tex_coords.xy;
		V= vec3(MVM * position);//vertex_position_modelspace;//gl_Position.xyz;
		N=normal_modelspace;//(gl_ModelViewProjectionMatrix *vec4(normal_modelspace,0)).xyz;
		T=tangent;//(gl_ModelViewProjectionMatrix *vec4(tangent,0)).xyz;
	
	#endif

}