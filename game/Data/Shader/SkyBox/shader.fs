#version 400

varying vec3 texcoords;
uniform samplerCube cubeMap;
//out vec4 frag_colour;

void main () {
  //gl_FragColor =texture (cubeMap, texcoords);
  //gl_FragColor.a=0.;
  gl_FragData[1].a=0.;
  gl_FragData[2]=texture (cubeMap, texcoords);
  gl_FragData[3]=gl_FragData[2];
  gl_FragData[4]=vec4(0.,0.,0.3,1.);
}