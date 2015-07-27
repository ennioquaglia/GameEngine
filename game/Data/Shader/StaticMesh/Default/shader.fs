

#if GLSL_VERSION>=330
	#version 330
	in vec3 V;
	in vec3 N;
	in vec3 T;
	in vec2 TexCoord;
#else
	#version 120
	varying vec3 V;
	varying vec3 N;
	varying vec3 T;
	varying vec2 TexCoord;
#endif
	

	
#define USE_LIGHT 1

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform float shininess; 


//varying vec3 color;


//#define MAX_LIGHTS 32
#define NUM_CASCADES 4


struct lightSource
{
  vec4 position;
  vec4 diffuse;
  vec4 specular;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 spotDirection;
  int shadowMapID;
  mat4 DepthMVP;
};

struct DirectionalLight
{
  vec4 dir;
  vec4 diffuse;
  vec4 specular;
  int shadowMapID[NUM_CASCADES];
  mat4 DepthMVP[NUM_CASCADES];
};

uniform sampler2D shadowMaps[10];

uniform lightSource lights[MAX_LIGHTS];


uniform int numLights;
uniform vec3 EyeWorldPos; 



vec3 CalcBumpedNormal()
{
	if(T==vec3(0.))return normalize(N);
    vec3 Normal = normalize(N);
    vec3 Tangent = normalize(T);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture2D(normalMap, TexCoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

vec4 calcLighting()
{	
	//return texture(shadowMaps[2],TexCoord);
	vec4 diffMat=texture(diffuseMap,TexCoord);
	vec4 specMat=vec4(texture(specularMap,TexCoord).xyz,1.);

	vec3 n=CalcBumpedNormal();
	vec3 v=V;
	//diffMat.w=1.;


	vec3 normalDirection = n.xyz;
	vec3 viewDirection = normalize( EyeWorldPos- v.xyz);
	vec3 lightDirection;
	float attenuation;
	

	// initialize total lighting with ambient lighting
	vec3 totalLighting =vec3(0.);// vec3(scene_ambient) * vec3(frontMaterial.ambient);
 
	for (int index = 0; index < numLights&&index<MAX_LIGHTS; index++) // for all light sources
	{

		
		  vec3 positionToLightSource = vec3(lights[index].position.xyz - v.xyz);
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
		

		vec3 diffuseReflection = attenuation * vec3(lights[index].diffuse) * vec3(diffMat) * max(0.0, dot(normalDirection, lightDirection));

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
		
		float visibility=1.;
		//if (0.0 == lights[index].position.w||lights[index].spotCutoff<=90.)

		//if(flag & Shadow)
		//visibility=calcShadowVisibility(lights[index]);
		
		totalLighting =totalLighting + (diffuseReflection + specularReflection)*visibility;
	}

	
	return vec4(totalLighting,1.);
}


void main () {
	
	gl_FragColor=calcLighting();//texture(diffuseMap,TexCoord)+vec4(vec3(N*0.01),0.);//Mapvec4(TexCoord,0.,1.)+vec4(0.1);//fragColor;//+texture(normalMap,tex_coords_frag);//+texture(normalMap,tex_coords_frag);//+vec4(tex_coords_frag,0.0,1.0);			

}