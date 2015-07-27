#version 150
varying vec2 Texcoord;
//out vec4 outColor;
uniform sampler2D PosTex;
uniform sampler2D VelTex;
uniform sampler2D NoiseTex;
uniform float deltaTime;
uniform vec3 emitterPos;

varying vec3 color;

uniform vec2 bufferSize;


void main() {
	

	if(color==vec3(0,0,0)){

		vec3 pos=texture(PosTex, Texcoord).xyz;
		vec3 vel=texture(VelTex, Texcoord).xyz*2.-1.;


		
		if(pos.y<=0&&length(vel)>0.5){
			vel=reflect(vel,vec3(0,1,0))+normalize(texture(NoiseTex,Texcoord+vec2(cos(deltaTime),sin(deltaTime))).xyz*2-1.)*vec3(0.2);;
			vel*=0.5;
			pos.y=0.01;
		}else
		if(pos.y>=1 || pos.y<=0 ||pos.x>=1 ||pos.z>=1 ||pos.x<0 ||pos.z<0||pos==vec3(0)){
			vel=normalize(texture(NoiseTex,Texcoord+vec2(cos(deltaTime),sin(deltaTime))).xyz*2-1.+vec3(0,0.5,-1.1));
			vel*=0.3;
			
			pos=vec3(0.5,0.5,1);//+emitterPos/4.0;
		}

				//pos=texture(NoiseTex,cos(Texcoord+deltaTime)).xyz;
		pos+=vel*deltaTime;
		vel.y-=deltaTime;//apply gravity
		
		gl_FragData[0] = vec4(pos,1);//+vec4(0.5);
		gl_FragData[1] = vec4(vel/2.+0.5,1);
	}
	else{//reset
		vec3 vel=normalize(texture(NoiseTex,Texcoord+vec2(cos(deltaTime),sin(deltaTime))).xyz*2-1.+vec3(0,0.5,-1.1));
		vel*=0.3;
		vec3 pos=vec3(0.5,0.5,1);//+emitterPos/4.0;
		gl_FragData[0] = vec4(pos,1);//+vec4(0.5);
		gl_FragData[1] = vec4(vel/2.+0.5,1);
	}
	
}