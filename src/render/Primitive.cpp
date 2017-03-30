#define GLEW_STATIC
#include <GL/glew.h>
#include "Primitive.h"
#include "Debug.h"

namespace Engine{


void drawSphere(GLdouble radius,GLint slices,GLint stacks){
    static auto quadratic = gluNewQuadric();
    if( !quadratic){
        //throw new PROGRAM_EXCEPTION( "Cannot initialize quadartic", NULL);
        QENGINE_ERROR("Cannot initialize quadartic");
        return;
    }
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    //gluQuadricDrawStyle( quadratic, GLU_FILL);
    gluQuadricTexture(quadratic, GL_TRUE); // Tried both GL_TRUE and GL_FALSE



    gluSphere( quadratic, radius, slices, stacks); // Tried r = 0.02f, 0.2f, 2.0f, 20.0f, none works
}


void drawCylinder(GLdouble base_radius,GLdouble top_radius,GLdouble height,GLint slices,GLint stacks){
    static auto quadratic = gluNewQuadric();
    if( !quadratic){
        //throw new PROGRAM_EXCEPTION( "Cannot initialize quadartic", NULL);
        QENGINE_ERROR("Cannot initialize quadartic");
        return;
    }
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    //gluQuadricDrawStyle( quadratic, GLU_FILL);
    gluQuadricTexture(quadratic, GL_TRUE); // Tried both GL_TRUE and GL_FALSE


    gluCylinder( quadratic,base_radius,top_radius,height, slices, stacks); // Tried r = 0.02f, 0.2f, 2.0f, 20.0f, none works
}


void drawCube(GLdouble side){
    side/=2.0;
    glBegin(GL_QUADS);

    glVertex3f(  side, -side, -side );      // P1 is red
    glVertex3f(  side,  side, -side );      // P2 is green
    glVertex3f( -side,  side, -side );      // P3 is blue
    glVertex3f( -side, -side, -side );      // P4 is purple


    glVertex3f(  side, -side, side );
    glVertex3f(  side,  side, side );
    glVertex3f( -side,  side, side );
    glVertex3f( -side, -side, side );


    // Purple side - RIGHT
    glVertex3f( side, -side, -side );
    glVertex3f( side,  side, -side );
    glVertex3f( side,  side,  side );
    glVertex3f( side, -side,  side );


    // Green side - LEFT
    glVertex3f( -side, -side,  side );
    glVertex3f( -side,  side,  side );
    glVertex3f( -side,  side, -side );
    glVertex3f( -side, -side, -side );


    // Blue side - TOP
    glVertex3f(  side,  side,  side );
    glVertex3f(  side,  side, -side );
    glVertex3f( -side,  side, -side );
    glVertex3f( -side,  side,  side );


    // Red side - BOTTOM
    glVertex3f(  side, -side, -side );
    glVertex3f(  side, -side,  side );
    glVertex3f( -side, -side,  side );
    glVertex3f( -side, -side, -side );

    glEnd();

}


void drawWireSphere(GLdouble radius,GLint slices,GLint stacks){
    static auto quadratic = gluNewQuadric();
    if( !quadratic){
        //throw new PROGRAM_EXCEPTION( "Cannot initialize quadartic", NULL);
        QENGINE_ERROR("Cannot initialize quadartic");
        return;
    }
    gluQuadricNormals(quadratic, GLU_LINE);
    //gluQuadricDrawStyle( quadratic, GLU_FILL);
    gluQuadricTexture(quadratic, GL_TRUE); // Tried both GL_TRUE and GL_FALSE



    gluSphere( quadratic, radius, slices, stacks); // Tried r = 0.02f, 0.2f, 2.0f, 20.0f, none works
}


void drawWireCylinder(GLdouble base_radius,GLdouble top_radius,GLdouble height,GLint slices,GLint stacks){
    static auto quadratic = gluNewQuadric();
    if( !quadratic){
        //throw new PROGRAM_EXCEPTION( "Cannot initialize quadartic", NULL);
        QENGINE_ERROR("Cannot initialize quadartic");
        return;
    }
    gluQuadricNormals(quadratic, GLU_LINE);
    //gluQuadricDrawStyle( quadratic, GLU_FILL);
    gluQuadricTexture(quadratic, GL_TRUE); // Tried both GL_TRUE and GL_FALSE


    gluCylinder( quadratic,base_radius,top_radius,height, slices, stacks); // Tried r = 0.02f, 0.2f, 2.0f, 20.0f, none works
}


void drawWireCube(GLdouble side){
    side/=2.0;
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin(GL_QUADS);

    glVertex3f(  side, -side, -side );      // P1 is red
    glVertex3f(  side,  side, -side );      // P2 is green
    glVertex3f( -side,  side, -side );      // P3 is blue
    glVertex3f( -side, -side, -side );      // P4 is purple


    glVertex3f(  side, -side, side );
    glVertex3f(  side,  side, side );
    glVertex3f( -side,  side, side );
    glVertex3f( -side, -side, side );


    // Purple side - RIGHT
    glVertex3f( side, -side, -side );
    glVertex3f( side,  side, -side );
    glVertex3f( side,  side,  side );
    glVertex3f( side, -side,  side );


    // Green side - LEFT
    glVertex3f( -side, -side,  side );
    glVertex3f( -side,  side,  side );
    glVertex3f( -side,  side, -side );
    glVertex3f( -side, -side, -side );


    // Blue side - TOP
    glVertex3f(  side,  side,  side );
    glVertex3f(  side,  side, -side );
    glVertex3f( -side,  side, -side );
    glVertex3f( -side,  side,  side );


    // Red side - BOTTOM
    glVertex3f(  side, -side, -side );
    glVertex3f(  side, -side,  side );
    glVertex3f( -side, -side,  side );
    glVertex3f( -side, -side, -side );

    glEnd();
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

}




}
