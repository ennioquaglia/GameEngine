#version 400



uniform vec4 color;

uniform sampler2D depthMap;
uniform sampler2D SpriteMap;
in vec2 tex_coords_frag;
in vec2 screenPos;

void main () {

	vec2 coord=(screenPos.xy+1)*0.5;//vec2((gl_FragCoord.x+1.)*0.5,1.-(gl_FragCoord.y+1.)*0.5);
	
	//if(texture2D(depthMap,(gl_FragCoord.xy+1.)*0.5).x==0.)discard;
	if(texture2D(depthMap,coord).x<gl_FragCoord.z)discard;
   
   vec4 c=texture2D(SpriteMap,tex_coords_frag.xy);  
	if(c.a==0)discard;

	//vec4 frag= c*color;
	//frag.a=c.a;
	
	//gl_FragData[0]=frag;
   gl_FragColor = c*color;
	//gl_FragColor.xyz*=c.a;
   
   //gl_FragColor.a=1.;
   
	//gl_FragData[2]= (c*color);
	
	//gl_FragData[2].xyz*=c.a;
   	//gl_FragData[1] = vec4(gl_FragColor.xyz,1.0); //normal
}