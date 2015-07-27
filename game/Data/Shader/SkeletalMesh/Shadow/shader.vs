#version 330
 
uniform mat4 MVM,depthMVP;
uniform mat2 textureMatrix;
uniform mat4 bone_transforms[50];
 
attribute vec3 vertex_position_modelspace;
attribute vec2 tex_coords;
attribute vec4 bone_weights;
attribute vec4 bone_id;

varying vec2 tex_coords_frag;

void main()
{
         //If you put W = 0.0 then the translation is nullified.
        mat4 transform0 = bone_weights.x*bone_transforms[int(bone_id.x)];
        mat4 transform1 = bone_weights.y*bone_transforms[int(bone_id.y)];
        mat4 transform2 = bone_weights.z*bone_transforms[int(bone_id.z)];
        mat4 transform3 = bone_weights.w*bone_transforms[int(bone_id.w)];
 
        mat4 transform = transform0+transform1+transform2+transform3;
		vec4 position = transform*vec4(vertex_position_modelspace, 1.0);
		 
        gl_Position = depthMVP * MVM * position;
		tex_coords_frag=textureMatrix*vec2(tex_coords.x,1.0-tex_coords.y);
}