#version 400
 
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

//uniform vec2 size=vec2(1.0); 

out vec2 tex_coords_frag; 
out vec2 screenPos;

in float Angle[gl_in.length()];
in float scale[gl_in.length()];

void main () {
  for(int i = 0; i < gl_VerticesIn; i++) {



	mat2 Matrix = mat2( cos( Angle[i] ), -sin( Angle[i] ),
						sin( Angle[i] ), cos( Angle[i] ));
	
	vec2 size;

	
	vec4 pos=gl_in[i].gl_Position;
	
	size=Matrix*vec2(-scale[i]);
	gl_Position = pos+vec4(size,0.,0.); 				//bottom right
	tex_coords_frag=vec2(1,1);
	screenPos=gl_Position.xy/gl_Position.w;
    EmitVertex();
	
	size=Matrix*vec2(-scale[i],scale[i]);
	gl_Position = pos+vec4(size,0.,0.); 			//top right
	tex_coords_frag=vec2(1,0);
	screenPos=gl_Position.xy/gl_Position.w;
    EmitVertex();
	
	size=Matrix*vec2(scale[i],-scale[i]);
	gl_Position = pos+vec4(size,0.,0.); 				//bottom left
	tex_coords_frag=vec2(0,1);
	screenPos=gl_Position.xy/gl_Position.w;
    EmitVertex();
	
	size=Matrix*vec2(scale[i],scale[i]);
    gl_Position = pos+vec4(size,0.,0.); 				//top left
	tex_coords_frag=vec2(0,0);
	screenPos=gl_Position.xy/gl_Position.w;
    EmitVertex();
	
	
  }
}