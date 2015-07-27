
#if GLSL_VERSION>=330

#version 330

layout(location = 0) in  vec3 vertex_position_modelspace;
layout(location = 1) in  vec3 normal_modelspace;
layout(location = 2) in  vec2 tex_coords;
layout(location = 3) in  vec3 tangent;
layout(location = 4) in  vec4 bone_weights;
layout(location = 5) in  vec4 bone_id;

out vec2 TexCoord;
out vec3 V; 
out vec3 N;
out vec3 T;
out vec4 v_position;

uniform mat4 PM,CM,MVM;
uniform mat4 bone_transforms[MAX_BONE_MATRIX];
uniform mat2 textureMatrix;

 
void main()
{
		
        //If you put W = 0.0 then the translation is nullified.
        mat4 transform0 = bone_weights.x*bone_transforms[int(bone_id.x)];
        mat4 transform1 = bone_weights.y*bone_transforms[int(bone_id.y)];
        mat4 transform2 = bone_weights.z*bone_transforms[int(bone_id.z)];
        mat4 transform3 = bone_weights.w*bone_transforms[int(bone_id.w)];
 
        mat4 transform =transform0+transform1+transform2+transform3;
 
        vec4 position = transform*vec4(vertex_position_modelspace, 1.0);
       
        v_position=gl_Position = PM*CM *MVM * position;
		

		mat3 NormalMatrix=mat3(MVM);
		
		V = vec3(MVM * position);
		N = normalize(NormalMatrix * normal_modelspace);
		T =  normalize(NormalMatrix * tangent);
		
		TexCoord=textureMatrix*tex_coords;
}

#else

#version 120

attribute vec3 vertex_position_modelspace;
attribute vec3 normal_modelspace;
attribute vec2 tex_coords;
attribute vec3 tangent;

varying vec2 TexCoord;
varying vec3 V; 
varying vec3 N;
varying vec3 T;


uniform mat4 PM,CM,MVM;
//uniform mat4 bone_transforms[MAX_BONE_MATRIX];
uniform mat2 textureMatrix;

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




#endif


