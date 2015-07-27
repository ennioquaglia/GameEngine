

#if GLSL_VERSION>=330
	#version 330
	
	layout(location = 0) out vec4 outColor;
	
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
#define NUM_DIRECTIONAL_LIGHTS 1

struct lightSource
{
  vec4 position;
  vec4 diffuse;
  vec4 specular;
  float constantAttenuation, linearAttenuation, quadraticAttenuation;
  float spotCutoff, spotExponent;
  vec3 dir;
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

uniform lightSource lights[10];
uniform DirectionalLight dirLights[NUM_DIRECTIONAL_LIGHTS];

uniform int numLights;
uniform vec3 EyeWorldPos; 
uniform int useVSM;

vec3 n;

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
    vec3 BumpMapNormal = texture2D(normalMap, TexCoord).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}


float linstep(float min,float max, float v){
	return clamp((v-min)/(max-min),0,1);
}
float ReduceLightBleeding(float p_max, float Amount)
{
return linstep(Amount,1,p_max);
}

float chebyshevUpperBound( sampler2D ShadowMap, vec2 coord, float distance)
{
	// We retrive the two moments previously stored (depth and depth*depth)
	vec2 moments = texture(ShadowMap,coord.xy).xy;
	
	float p = (distance <= moments.x)?1.0:0.0;  
	// Compute variance.  
	float variance= moments.y-(moments.x*moments.x);
	variance=max(variance,0.0000002);
	float d=distance-moments.x;
	float p_max=variance/(variance+d*d);
	p_max=ReduceLightBleeding(p_max,0.5);
	
	return max(p,p_max);
}


float calcShadowVisibility(lightSource l){
	float visibility=1.;

	if (0.0 == l.position.w||l.spotCutoff<=90.){
		if(l.DepthMVP==mat4(1))return 0.0;
		vec4 s_coord = l.DepthMVP*vec4(V.xyz,1.);

		s_coord.xyz/=s_coord.w;
		s_coord.xyz = s_coord.xyz * .5f + vec3(.5f);
		
		
		
		float bias=0.000925;//0.0025;//0.0005;
		/*
		if ( texture( shadowMaps[l.shadowMapID], s_coord.xy ).x  <  s_coord.z-bias){
			if(0.0 == l.position.w)visibility=0.5;
			else
			visibility = 0.1;
		}*/
		
		 bias=0.0009;
		/*
		for (int i=0;i<4;i++){
		  if ( texture( shadowMaps[l.shadowMapID], s_coord.xy + poissonDisk[i]/1200.0 ).z  <   s_coord.z-bias ){
			visibility*=0.8;
		  }
		}*/
		
	}
	return visibility;
}

float calcDirectionalShadowVisibility(DirectionalLight l){

	float visibility=1.;

	for(int c=0;c<NUM_CASCADES;c++)
	{
		vec4 s_coord = l.DepthMVP[c]*vec4(V.xyz,1.);

		s_coord.xyz/=s_coord.w;
		s_coord.xyz = s_coord.xyz * .5f + vec3(.5f);
		
		if(any(greaterThan(s_coord.xyz,vec3(1.-2./1024.)))||any(lessThan(s_coord.xyz,vec3(0.+2./1024.))))continue;
		
		if(useVSM==1)
		visibility=+0.5+chebyshevUpperBound( shadowMaps[l.shadowMapID[c]],s_coord.xy,s_coord.z-0.00002)/2;
		else{
			
			float cosTheta=clamp(dot( n.xyz,l.dir.xyz ),0.,1.);
			float bias = 0.0005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
			bias = clamp(bias, 0,0.01);
			if ( texture( shadowMaps[l.shadowMapID[c]], s_coord.xy ).x  <  s_coord.z-bias)
			visibility=0.5;

		}
		/*
		//float bias=0.000925;
		
		float cosTheta=clamp(dot( n.xyz,l.dir.xyz ),0.,1.);
		float bias = 0.0005*tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
		bias = clamp(bias, 0,0.01);
		bias=0.000925;
		visibility=texture( shadowMaps[l.shadowMapID[c]], s_coord.xy ).x;
		//return VSM(shadowMaps[l.shadowMapID[c]], s_coord.xy, s_coord.z-bias);
		
		if ( texture( shadowMaps[l.shadowMapID[c]], s_coord.xy ).x  <  s_coord.z-bias){
			visibility=0.5;
			//return 1.-1./float(c+1);
		}
		
		/*
		float EPSILON= 0.00001;
		vec4 LightSpacePos = l.DepthMVP[c]*vec4(V.xyz,1.);
		vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;
		vec2 UVCoords;
		UVCoords.x = 0.5 * ProjCoords.x + 0.5;
		UVCoords.y = 0.5 * ProjCoords.y + 0.5;
		float z = 0.5 * ProjCoords.z + 0.5;

		vec2 gMapSize=vec2(1024);
		float xOffset = 1.0/gMapSize.x;
		float yOffset = 1.0/gMapSize.y;

		float Factor = 0.0;

		for (int y = -1 ; y <= 1 ; y++) {
			for (int x = -1 ; x <= 1 ; x++) {
				vec2 Offsets = vec2(x * xOffset, y * yOffset);
				vec3 UVC = vec3(UVCoords + Offsets, z - bias);
				Factor += texture(shadowMaps[l.shadowMapID[c]], UVC);
			}
		}

		return (0.2 + (Factor / 18.0));
		
		
		//else
		//if(!c)
		/*
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
		}*/
		return visibility;
	}
	return visibility;
}



vec4 calcLighting()
{	
	//return texture(shadowMaps[0],TexCoord);
	vec4 diffMat=texture(diffuseMap,TexCoord);
	vec4 specMat=vec4(texture(specularMap,TexCoord).xyz,1.);

	n=CalcBumpedNormal();
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
			float clampedCosine = max(0.0, dot(-lightDirection, normalize(lights[index].dir)));
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
		//if(flag & Shadow)
		visibility=calcDirectionalShadowVisibility(dirLights[i]);
		
		totalLighting =totalLighting + (diffuseReflection + specularReflection)*visibility;
	}
	
	return vec4(totalLighting,1.);
}


void main () {
	
	outColor=calcLighting();//texture(diffuseMap,TexCoord)+vec4(vec3(N*0.01),0.);//Mapvec4(TexCoord,0.,1.)+vec4(0.1);//fragColor;//+texture(normalMap,tex_coords_frag);//+texture(normalMap,tex_coords_frag);//+vec4(tex_coords_frag,0.0,1.0);			

}