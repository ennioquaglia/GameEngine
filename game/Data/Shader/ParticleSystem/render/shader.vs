
uniform sampler2D tex;
uniform mat4 PM,CM,MVM;
varying vec3 Texcoord;

void main()
{
	//Texcoord=gl_MultiTexCoord0;
	Texcoord=gl_Vertex.xyz;
	//#define SHOW_TEXTURE
	
	#ifdef SHOW_TEXTURE
	gl_Position =gl_Vertex;
	#else
	vec3 pos=texture2D(tex,gl_Vertex.xy).xyz*20;
	gl_Position =PM*CM*MVM*vec4(pos,1.);
	
	#endif
}



