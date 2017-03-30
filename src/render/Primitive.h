#pragma once


namespace Engine{

extern void drawSphere(GLdouble radius,GLint slices,GLint stacks);
extern void drawCylinder(GLdouble base_radius,GLdouble top_radius,GLdouble height,GLint slices,GLint stacks);
extern void drawCube(GLdouble side);

extern void drawWireSphere(GLdouble radius,GLint slices,GLint stacks);
extern void drawWireCylinder(GLdouble base_radius,GLdouble top_radius,GLdouble height,GLint slices,GLint stacks);
extern void drawWireCube(GLdouble side);


}
