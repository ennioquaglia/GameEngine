#version 330

//material
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;
uniform samplerCube enviromentMap;
uniform float shininess=0.99;
uniform float metallic=0.;

uniform mat4 PM,CM;

uniform vec3 EyeWorldPos;

uniform int flag;
uniform int isDeferred=1;
uniform vec3 emissiveAmbient=vec3(0.);

varying vec4 color;
varying vec3 normal_camera;
varying vec2 tex_coords_frag;
varying vec4 ShadowCoord;
varying vec3 v;   				//vertex position
varying vec3 N;					//normal
varying vec3 T;					//tangent






//lights

//enum
#define Shadow 1
#define Tessellation 2
#define Light 4

#define MAX_LIGHTS 64
#define NUM_CASCADES 4
#define NUM_DIRECTIONAL_LIGHTS 1

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
uniform DirectionalLight dirLights[NUM_DIRECTIONAL_LIGHTS];

uniform int numLights;


vec2 poissonDisk[4] = vec2[4](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);



vec3 CalcBumpedNormal()
{
	if(T==vec3(0.))return normalize(N);
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

float calcShadowVisibility(lightSource l){
	float visibility=1.;

	if (0.0 == l.position.w||l.spotCutoff<=90.){
	
		vec4 s_coord = l.DepthMVP*vec4(v.xyz,1.);

		s_coord.xyz/=s_coord.w;
		s_coord.xyz = s_coord.xyz * .5f + vec3(.5f);
		
		
		
		float bias=0.000925;//0.0025;//0.0005;

		if ( texture2D( shadowMaps[l.shadowMapID], s_coord.xy ).x  <  s_coord.z-bias){
			if(0.0 == l.position.w)visibility=0.5;
			else
			visibility = 0.;
		}
		 bias=0.0009;
		
		for (int i=0;i<4;i++){
		  if ( texture2D( shadowMaps[l.shadowMapID], s_coord.xy + poissonDisk[i]/1200.0 ).z  <   s_coord.z-bias ){
			visibility*=0.8;
		  }
		}
	}
	return visibility;
}

float calcDirectionalShadowVisibility(DirectionalLight l){

	float visibility=1.;

	for(int c=0;c<NUM_CASCADES;c++)
	{
		vec4 s_coord = l.DepthMVP[c]*vec4(v.xyz,1.);

		s_coord.xyz/=s_coord.w;
		s_coord.xyz = s_coord.xyz * .5f + vec3(.5f);
		
		if(any(greaterThan(s_coord.xyz,vec3(1.)))||any(lessThan(s_coord.xyz,vec3(0.))))continue;
		
		float bias=0.000925;
		
		//return VSM(shadowMaps[l.shadowMapID[c]], s_coord.xy, s_coord.z-bias);
		
		if ( texture2D( shadowMaps[l.shadowMapID[c]], s_coord.xy ).x  <  s_coord.z-bias){
			visibility=0.5;
			//return 1.-1./float(c+1);
		}
		
		//else
		//if(!c)
		{
		
			bias=0.0009;
			for (int i=0;i<4;i++){
			  if ( texture2D( shadowMaps[l.shadowMapID[c]], s_coord.xy + poissonDisk[i]/1000. ).x  <   s_coord.z-bias ){
				visibility*=0.9;
			  }
			  if ( texture2D( shadowMaps[l.shadowMapID[c]], s_coord.xy + poissonDisk[i]/500. ).x  <   s_coord.z-bias ){
				visibility*=0.9;
			  }
			}
		}
		
		
		return visibility;

	}
	return visibility;
}




void main(){

	
	vec4 diffMat= texture2D(diffuseMap,tex_coords_frag);
	if(diffMat.a<0.1)discard;
	diffMat*=color;
	
	float visibility=1.0;
	vec4 fragColor=vec4(0.0);
	vec3 n=CalcBumpedNormal() ;
	vec4 specMat = texture2D(specularMap,tex_coords_frag)*color;
	vec4 emissiveFactor=texture2D(emissiveMap,tex_coords_frag)+vec4(emissiveAmbient,1.);
	
	
	
	
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
		
		float visibility=1.;
		//if (0.0 == lights[index].position.w||lights[index].spotCutoff<=90.)

		if(flag & Shadow)
		visibility=calcShadowVisibility(lights[index]);
		
		totalLighting =totalLighting + (diffuseReflection + specularReflection)*visibility;
	}

	for(int i=0;i<NUM_DIRECTIONAL_LIGHTS;i++)
	{
		lightDirection = normalize(dirLights[i].dir.xyz);
		
		vec3 diffuseReflection =  vec3(dirLights[i].diffuse) * vec3(diffMat)* max(0.0, dot(normalDirection, lightDirection));

		vec3 specularReflection;
		if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
		{
			specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
		}
		  else // light source on the right side
		{
			specularReflection = vec3(dirLights[i].specular) * vec3(specMat)
			* pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), shininess);
		}
		
		float visibility=1.;
		if(flag & Shadow)
		visibility=calcDirectionalShadowVisibility(dirLights[i]);
		
		totalLighting =totalLighting + (diffuseReflection + specularReflection)*visibility;
	}
	
	
	
	vec3 incident_eye = normalize(vec3(CM*vec4(v,1.)));
	vec3 normal =normalize(vec3(CM*vec4(n,0.)));;//vec3 (inverse (CM) * vec4 (n, 0.0));;// normalize (n);

	vec3 reflected = reflect (incident_eye, normal);
	// convert from eye to world space
	reflected = vec3 (inverse (CM) * vec4 (reflected, 0.0));
	
	
	gl_FragColor=(vec4(totalLighting,1.))+metallic*texture (enviromentMap, reflected);//+emissiveFactor;
	
}