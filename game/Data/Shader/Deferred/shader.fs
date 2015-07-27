#version 330

varying vec2 tc;


uniform sampler2D normal;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D depth;
//uniform sampler2D rnm;
uniform sampler2D bloom;
uniform sampler2D SSAO_map;
uniform sampler2D top_layer;

uniform float exposure=0.0041;
uniform float decay=0.9994;
uniform float density=0.8407;
uniform float weight=5.65;


uniform vec2 lightPositionOnScreen;
const int NUM_SAMPLES = 50 ;

uniform bool active=true;




//lights


//enum
#define Shadow 1
#define Tessellation 2
#define Light 4

#define MAX_LIGHTS 64
#define NUM_CASCADES 4
#define NUM_DIRECTIONAL_LIGHTS 1


uniform mat4 PM,CM;

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
uniform vec3 EyeWorldPos;

uniform int flag=1;

vec4 v,n;

vec2 poissonDisk[4] = vec2[4](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);


float linstep(float low, float high, float v){
    return clamp((v-low)/(high-low), 0.0, 1.0);
}

float VSM(sampler2D depths, vec2 uv, float fDepth){
		float visibility=1.;
		vec2 moments=texture2D( depths, uv.xy ).xy;
		float bias=0.000925;
		
		if ( moments.x  <  fDepth-bias){
			if(!active)return 0.3;
			
			//visibility=0.5;
			visibility=moments.y-moments.x*moments.x;
			visibility = min( 1.0f, max( 0.0f, visibility + 0.00001f ) );
			
			float mean           = moments.x;
			float d              = fDepth - mean;
			visibility    = visibility / ( visibility + d*d );
			visibility= pow(visibility,2);
			//if(visibility>0.4)visibility=0.4;
			//return 1.-1./float(c+1);
		}
		return visibility;
		
		
		float fPercentLit=1.;
		float  fAvgZ  =texture(depth,uv).x; // Filtered z
        float  fAvgZ2 =texture(depth,uv).y; // Filtered z-squared

        if ( fDepth <= fAvgZ )
        {
			fPercentLit = 1.;
        }
		else
		{
			return 0.5;
			float variance = ( fAvgZ2 ) - ( fAvgZ * fAvgZ );
			variance = min( 1.0f, max( 0.0f, variance + 0.00001f ) );

			float mean           = fAvgZ;
			float d              = fDepth - mean;
			fPercentLit    = variance / ( variance + d*d );
		}
		return fPercentLit;
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

vec4 calcLighting()
{	
	//return texture(shadowMaps[2],tc);
	vec4 diffMat=texture(diffuse,tc);
	vec4 specMat=vec4(texture(specular,tc).xyz,1.);

	if(texture(normal,tc).w==0.)return diffMat;
	
	n=texture(normal,tc)*2.-1.;

	v=inverse(PM*CM)*vec4(vec3(tc,texture(depth,tc).x)*2.-1.,1.);
	v.xyz/=v.w;
	float shininess=50.*diffMat.w;
	//return vec4(shininess);
	//if(shininess>0.5)return vec4(1.);
	diffMat.w=1.;


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
	
	return vec4(totalLighting,1.);
}



/*


const float totStrength = 1.38;
const float strength = 0.07;
const float offset = 18.0;
const float falloff = 0.000002;
const float rad = 0.06;
#define SAMPLES 10 // 10 is good
const float invSamples = -1.38/10.0;

float getSSAO()
{
// these are the random vectors inside a unit sphere
vec3 pSphere[10] = vec3[](vec3(-0.010735935, 0.01647018, 0.0062425877),vec3(-0.06533369, 0.3647007, -0.13746321),vec3(-0.6539235, -0.016726388, -0.53000957),vec3(0.40958285, 0.0052428036, -0.5591124),vec3(-0.1465366, 0.09899267, 0.15571679),vec3(-0.44122112, -0.5458797, 0.04912532),vec3(0.03755566, -0.10961345, -0.33040273),vec3(0.019100213, 0.29652783, 0.066237666),vec3(0.8765323, 0.011236004, 0.28265962),vec3(0.29264435, -0.40794238, 0.15964167));
 
   // grab a normal for reflecting the sample rays later on
   vec3 fres = normalize((texture2D(rnm,tc*offset).xyz*2.0) - vec3(1.0));
 
   vec4 currentPixelSample = PM*CM*texture2D(normal,tc);
 
   float currentPixelDepth = currentPixelSample.a;
 
   // current fragment coords in screen space
   vec3 ep = vec3(tc.xy,currentPixelDepth);
  // get the normal of current fragment
   vec3 norm = currentPixelSample.xyz;
 
   float bl = 0.0;
   // adjust for the depth ( not sure if this is good..)
   float radD = rad/currentPixelDepth;
 
   //vec3 ray, se, occNorm;
   float occluderDepth, depthDifference;
   vec4 occluderFragment;
   vec3 ray;
   for(int i=0; i<SAMPLES;++i)
   {
      // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
      ray = radD*reflect(pSphere[i],fres);
 
      // get the depth of the occluder fragment
      occluderFragment = PM*CM*texture2D(normal,ep.xy + sign(dot(ray,norm) )*ray.xy);
    // if depthDifference is negative = occluder is behind current fragment
      depthDifference = currentPixelDepth-occluderFragment.a;
 
      // calculate the difference between the normals as a weight
 // the falloff equation, starts at falloff and is kind of 1/x^2 falling
      bl += step(falloff,depthDifference)*(1.0-dot(occluderFragment.xyz,norm))*(1.0-smoothstep(falloff,strength,depthDifference));
   }
 
   // output the result
   return 1.0+bl*invSamples;
 
}*/

vec4 getLightScattering(){
	vec2 deltaTextCoord = vec2( tc - lightPositionOnScreen.xy );
	vec2 textCoo =tc;
	deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
	float illuminationDecay = 1.0;
	vec4 color=vec4(0.);

	for(int i=0; i < NUM_SAMPLES ; i++)
	{
			 textCoo -= deltaTextCoord;
			 vec4 sample;// = texture2D(firstPass, textCoo );
			if(texture2D(depth, textCoo ).x<1.)sample=vec4(0.,0.,0.,1.);
			else {
				if(length(textCoo - lightPositionOnScreen.xy)<0.03)sample=vec4(1.);
				else
				sample=vec4(0.,0.,0.1,1.);
			}
			
			 sample *= illuminationDecay * weight;

			 color += sample;

			 illuminationDecay *= decay;
	 }
	color *= exposure;
	return color;
}

void main()
{	
	
	if(texture(normal,tc).w==0.){
		gl_FragColor =texture(diffuse,tc)+getLightScattering()+texture( top_layer,tc);
		return;
	}

	
	if(active)
	gl_FragColor =texture(SSAO_map,tc).a*calcLighting()+texture(bloom,tc)+texture( top_layer,tc);//+getLightScattering();
	else
	gl_FragColor =calcLighting()+texture(bloom,tc)+texture( top_layer,tc);//+getLightScattering()
	
	gl_FragColor.xyz+=texture(SSAO_map,tc).xyz;
	
	return;
	
	gl_FragColor =gl_FragColor+calcLighting();
	float contrast=1.2;
	gl_FragColor.rgb = ((gl_FragColor.rgb - 0.5f) * max(contrast, 0)) + 0.5f;
	//gl_FragColor.xyz=gl_FragColor.xyz*pow(gl_FragColor.xyz,vec3(0.05));//cross(gl_FragColor.xyz,vec3(1.));
	
}
