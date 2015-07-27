#version 120

varying vec2 tc;

void main(){
	gl_Position =  vec4(gl_Vertex.xy,0.,1.);
	
	tc=vec2(gl_MultiTexCoord0.x,gl_MultiTexCoord0.y);

}