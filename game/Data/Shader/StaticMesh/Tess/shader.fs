#version 330

//enum
#define Shadow 1
#define Tessellation 2
#define Light 4



//material
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform float shininess;

uniform vec3 EyeWorldPos;

uniform int flag;

//in vec4 color;
in vec2 tex_coords_frag;
in vec3 v;   				//vertex position
in vec3 N;					//normal
in vec3 T;					//tangent

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

void main () {

	vec4 color=vec4(1.);//gl_Color;
	float visibility=1.0;
	vec4 fragColor=vec4(0.0);
	vec3 n=CalcBumpedNormal() ;
	vec4 diffMat= texture2D(diffuseMap,tex_coords_frag);
	vec4 specMat = texture2D(specularMap,tex_coords_frag);
	
	if(diffMat.a<0.1)discard;
	
	
	//gl_FragData[0] = vec4(0.);//vec3(gl_FragCoord.z),1.);//depth 

	
	gl_FragData[1] = vec4((n+1.)*0.5,1.0); //normal
	//if(!isDeferred)gl_FragData[1].w=0.0;
	gl_FragData[2] = diffMat*color;    //texture
	gl_FragData[3] = specMat*color;

	

}