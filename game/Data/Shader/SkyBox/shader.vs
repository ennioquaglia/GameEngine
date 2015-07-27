#version 400


uniform mat4 PM,CM,MVM;
varying vec3 texcoords;

void main () {
  texcoords = gl_Vertex.xyz;
  gl_Position = PM*CM* MVM* gl_Vertex;//vec4 (vp, 1.0);
}