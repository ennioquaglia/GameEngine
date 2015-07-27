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

		for(int x=1;x<bufferSize.x;x++){
			for(int y=1;y<bufferSize.y;y++){
					vec2 coord=vec2(x,y)/bufferSize;
					vec3 pos2=texture(PosTex, coord).xyz;
					float dist=distance(pos,pos2);
					if(dist>0)
						vel+=normalize(pos2-pos)*min(1,1./min(dist,0.001))*0.000005;//(1/dist)*0.01;
					
			}
		}

		if(distance(vec3(0),vel)>1)vel=normalize(vel);
		pos+=vel/100;

		if(pos.y>=1 || pos.y<=0 ||pos.x>=1 ||pos.z>=1 ||pos.x<=0 ||pos.z<=0){
			vel=vec3(0);
		}
		

			
		
		gl_FragData[0] = vec4(pos,1);
		gl_FragData[1] = vec4(vel/2.+0.5,1);
	}
	else{//reset
		vec3 vel=vec3(0);

		vec3 pos=texture(NoiseTex,Texcoord+vec2(cos(deltaTime+0.1),sin(deltaTime+0.5))).xyz*0.5;
		gl_FragData[0] = vec4(pos,1);//+vec4(0.5);
		gl_FragData[1] = vec4(vel/2.+0.5,1);
	}
	
	
	
	
	
	/* //Fontain
	if(color==vec3(0,0,0)){

		vec3 pos=texture(PosTex, Texcoord).xyz;
		vec3 vel=texture(VelTex, Texcoord).xyz*2.-1.;
		//pos=texture(NoiseTex,cos(Texcoord+deltaTime)).xyz;
		pos+=vel*deltaTime;
		vel.y-=deltaTime;//apply gravity
		
		if(pos.y<=0&&length(vel)>0.5){
			vel=reflect(vel,vec3(0,1,0))+normalize(texture(NoiseTex,Texcoord+vec2(cos(deltaTime),sin(deltaTime))).xyz*2-1.)*vec3(0.2);;
			vel*=0.5;
			pos.y=0.01;
		}else
		if(pos.y>=1 || pos.y<=0 ||pos.x>=1 ||pos.z>=1 ||pos.x<0 ||pos.z<0||pos==vec3(0)){
			vel=normalize(texture(NoiseTex,Texcoord+vec2(cos(deltaTime),sin(deltaTime))).xyz*2-1.+vec3(0,0.5,1.1));
			vel*=0.3;
			
			pos=vec3(0.5,0.5,0.5);//+emitterPos/4.0;
		}

		gl_FragData[0] = vec4(pos,1);//+vec4(0.5);
		gl_FragData[1] = vec4(vel/2.+0.5,1);
	}
	else{//reset
		vec3 vel=normalize(texture(NoiseTex,Texcoord).xyz*2.-1.);
		vel*=0.3;
		vec3 pos=vec3(0.5,0.5,0.5)+emitterPos/4.0;
		gl_FragData[0] = vec4(pos,1);//+vec4(0.5);
		gl_FragData[1] = vec4(vel/2.+0.5,1);
	}
	*/
}