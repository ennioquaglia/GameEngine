//#version 120
#version 330 core
#define MAX_LIGHTS 64

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform float shininess;
//uniform vec4 LightPos[MAX_LIGHTS];


struct lightSource
{
  vec4 position;
  vec4 diffuse;
  vec4 specular;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 spotDirection;
  //int shadowMapID;
  //mat4 DepthMVP;
};

uniform lightSource lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 EyeWorldPos;


varying vec4 color;
varying vec2 tex_coords_frag;
varying vec3 v; 
varying vec3 N;
varying vec3 T;


vec3 CalcBumpedNormal()
{
    vec3 Normal = normalize(N);
    vec3 Tangent = normalize(T);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture2D(normalMap, tex_coords_frag).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}


void main () {
	vec4 fragColor=vec4(0.0);
	vec3 n= CalcBumpedNormal() ;
	vec4 diffMat= texture2D(diffuseMap,tex_coords_frag);
	vec4 specMat = texture2D(specularMap,tex_coords_frag);
	/*
	for(int i=0;i<numLights;i++){

		float dist=	length(LightPos[i].xyz- v);
		vec3 L = normalize(LightPos[i].xyz- v);   
		vec3 E = normalize(-v); // we are in Eye Coordinates, so EyePos is (0,0,0)    <--- prima era normalize(-v);
		vec3 R = normalize(-reflect(L,n));  
		
		
		//calculate Diffuse Term:  
		vec4 diffMat= texture2D(diffuseMap,tex_coords_frag);//vec4(0.5,0.5,0.5,0.5);
		vec4 Idiff = diffMat * max(dot(n,L), 0.0);
		Idiff = clamp(Idiff, 0.0, 1.0);     
	   
		// calculate Specular Term:
		vec4 specMat = texture2D(specularMap,tex_coords_frag);
		vec4 Ispec =specMat* pow(max(dot(R,E),0.0),0.3);
		Ispec = clamp(Ispec, 0.0, 1.0); 
		
		//if(dist<LightPos[i].w*2)
		//fragColor+=(Idiff)/(dist/LightPos[i].w);
		
		float att=max(1.0-(dist/LightPos[i].w),0.0);att*=att;
		fragColor+=(Idiff+Ispec)*att;
	}*/
	
	vec3 normalDirection = n;
	vec3 viewDirection = normalize( EyeWorldPos- v);//normalize(LightPos[i].xyz- v);   
	vec3 lightDirection;
	float attenuation;
	


	// initialize total lighting with ambient lighting
	vec3 totalLighting =vec3(0);// vec3(scene_ambient) * vec3(frontMaterial.ambient);
 
	for (int index = 0; index < numLights&&index<MAX_LIGHTS; index++) // for all light sources
	{
	
		
		if (0.0 == lights[index].position.w) // directional light?
		{
		  attenuation = 1.0; // no attenuation
		  lightDirection = normalize(lights[index].position.xyz);
		} 
		else // point light or spotlight (or other kind of light) 
		{
		  vec3 positionToLightSource = vec3(lights[index].position.xyz - v);
		  float distance = length(positionToLightSource);
		  lightDirection = normalize(positionToLightSource);
		  attenuation = 1.0 / (lights[index].constantAttenuation
					   + lights[index].linearAttenuation * distance
					   + lights[index].quadraticAttenuation * distance * distance);

		  if (lights[index].spotCutoff <= 90.0) // spotlight?
		  {
			float clampedCosine = max(0.0, dot(-lightDirection, normalize(lights[index].spotDirection)));
			if (clampedCosine < cos(radians(lights[index].spotCutoff))) // outside of spotlight cone?
			{
			  attenuation = 0.0;
			}
			else
			{
			  attenuation = attenuation * pow(clampedCosine, lights[index].spotExponent);   
			}
		  }
		}

		vec3 diffuseReflection = attenuation 
		* vec3(lights[index].diffuse) * vec3(diffMat)
		* max(0.0, dot(normalDirection, lightDirection));

		vec3 specularReflection;
		if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
		{
			specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
		}
		  else // light source on the right side
		{
			specularReflection = attenuation * vec3(lights[index].specular) * vec3(specMat)
			* pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), shininess);
		}
		

		
		totalLighting +=diffuseReflection + specularReflection;//totalLighting + (diffuseReflection + specularReflection);

	}
	
	gl_FragColor=vec4(totalLighting,1.);//vec4(B,1.0);//+texture(normalMap,tex_coords_frag);//+texture(normalMap,tex_coords_frag);//+vec4(tex_coords_frag,0.0,1.0);

}