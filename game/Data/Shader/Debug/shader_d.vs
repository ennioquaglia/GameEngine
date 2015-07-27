#version 330 core

varying vec2 tc;

void main(){
	gl_Position =  vec4(gl_Vertex.xy,0.,1.);
	
	tc=gl_MultiTexCoord0.xy;

}