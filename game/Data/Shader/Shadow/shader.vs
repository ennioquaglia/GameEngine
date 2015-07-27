#version 330 core
 
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 depthMVP;
uniform mat4 MVM;

attribute vec2 tex_coords;
varying vec2 tex_coords_frag;
uniform mat2 textureMatrix;

void main(){
 gl_Position =  depthMVP*MVM* vec4(vertexPosition_modelspace,1);
 tex_coords_frag=textureMatrix*vec2(tex_coords.x,1.0-tex_coords.y);
}