#version 330 core
 
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
 
// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;
varying vec4 pos; 
void main(){
 pos=gl_Position =  depthMVP *gl_ModelViewMatrix* gl_Vertex;//vec4(vertexPosition_modelspace,1);
}