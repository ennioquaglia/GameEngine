
uniform sampler2D PosTex;
uniform sampler2D VelTex;
varying vec3 Texcoord;

void main()
{
	vec3 vel=texture2D(VelTex, Texcoord.xy).xyz*2.-1.;
	float f=distance(vel,vec3(0))*2;
	vec3 color=mix(vec3(0,1,0),vec3(1,0,0),f);
	//if(color==vec3(0))gl_FragColor=vec4(1.);
	//else 
	gl_FragColor=vec4(color,1);//texture2D(texture,Texcoord.xy);//vec4(1,1,0.6,1);
	
	//gl_FragColor = vec4(Texcoord.xy,0,1);
}