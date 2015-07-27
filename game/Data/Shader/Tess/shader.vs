
layout(location = 0) in vec3 vertex_position_modelspace;
attribute vec3 normal_modelspace;
attribute vec2 tex_coords;
attribute vec3 tangent;

varying vec3 vEye; 
uniform mat4 PM;


//out vec4 vPosition;
out vec2 vTexCoord;
//varying vec3 fColor;
//uniform vec3 EyePos;
uniform mat4 offset;
uniform vec2 texSize;
//varying vec2 tex_coords_frag;
void main()
{
	//fColor=vec3(1.);//distance(EyePos,gl_ModelViewMatrix*vertex_position_modelspace));
    gl_Position=vec4(vertex_position_modelspace,1.);
	
	
	vTexCoord=((gl_ModelViewMatrix*(gl_Vertex)).xz)/texSize;//gl_MultiTexCoord0.xy;
	vTexCoord.x+=0.5;//100/(2*texSize.x);
	vTexCoord.y+=0.5;//100/(2*texSize.y);
	
	//tex_coords_frag=vTexCoord;
}
