#version 330

uniform vec2 pos=vec2(0);
varying vec2 tc;

void main(){
	gl_Position =  gl_ModelViewMatrix*vec4(gl_Vertex.xy,0.,1.);
	//vec4(pos,0.,1.)+
	tc=vec2(gl_MultiTexCoord0.x,1.0-gl_MultiTexCoord0.y);

}