// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// Example_6_3.c
//
//    This example demonstrates using client-side vertex arrays
//    and a constant vertex attribute
//
#include <stdio.h>
#include "esUtil.h"

typedef struct
{
    // Handle to a program object
    GLuint programObject;
    GLuint voidId[4];
} UserData;


///
// Initialize the program object
//
int Init ( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    const char *vShaderStr = file2str("simple_vertex_shader.glsl");
    const char *fShaderStr = file2str("simple_fragment_shader.glsl");
    GLuint programObject;

    // Create the program object
    programObject = esLoadProgram ( vShaderStr, fShaderStr );

    if ( programObject == 0 )
    {
        return GL_FALSE;
    }

    // Store the program object
    userData->programObject = programObject;
    userData->voidId[0] = 0;

    glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
    return GL_TRUE;
}

///
// Draw a triangle using the program object created in Init()
//
#define POSITION_STRIDE  3
#define COLOR_STRIDE  4
void Draw1 ( ESContext *esContext )
{
    const int ELEMENT_STRIDE = POSITION_STRIDE + COLOR_STRIDE;
    UserData *userData = esContext->userData;
    // 3 vertices, with (x,y,z) per-vertex
    GLfloat vertexPos[] =
            {
                    -0.5f,  1.0f, 0.0f, // v0
                    0.0f, 0.0f, 0.0f, 1.0f,
                    -1.0f, 0.0f, 0.0f, // v1
                    1.0f, 1.0f, 0.0f, 1.0f,
                    0.0f, 0.0f, 0.0f,  // v2
                    1.0f, 0.0f, 0.0f, 1.0f
            };

    glViewport ( 0, 0, esContext->width, esContext->height );

    //glClear ( GL_COLOR_BUFFER_BIT );

    glUseProgram ( userData->programObject );

    const  GLsizei stride = sizeof(GLfloat) * ELEMENT_STRIDE;
    GLint positionIndex = glGetAttribLocation(userData->programObject, "a_position");
    glVertexAttribPointer ( positionIndex, POSITION_STRIDE, GL_FLOAT, GL_FALSE, stride, vertexPos );
    glEnableVertexAttribArray ( positionIndex );


    GLint colorIndex = glGetAttribLocation(userData->programObject, "a_color");
    glVertexAttribPointer ( colorIndex, COLOR_STRIDE, GL_FLOAT, GL_FALSE, stride, vertexPos + POSITION_STRIDE );
    glEnableVertexAttribArray ( colorIndex );
    //glVertexAttrib4fv ( colorIndex, color );


    glDrawArrays ( GL_TRIANGLES, 0, 3 );
    glDisableVertexAttribArray ( positionIndex );
    glDisableVertexAttribArray ( colorIndex );
}

void Draw2 ( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat vertexPos[] =
            {
                    0.5f,  0.0f, 0.0f, // v0
                    0.0f, -1.0f, 0.0f, // v1
                    1.0f, -1.0f, 0.0f  // v2
            };

    glViewport ( 0, 0, esContext->width, esContext->height );
    //glClear ( GL_COLOR_BUFFER_BIT );
    glUseProgram ( userData->programObject );

    GLint positionIndex = glGetAttribLocation(userData->programObject, "a_position");
    glVertexAttribPointer ( positionIndex, POSITION_STRIDE, GL_FLOAT, GL_FALSE, 0, vertexPos );
    glEnableVertexAttribArray ( positionIndex );

    GLint colorIndex = glGetAttribLocation(userData->programObject, "a_color");
    glVertexAttrib4fv ( colorIndex, color );


    glDrawArrays ( GL_TRIANGLES, 0, 3 );
    //GLushort indices[3] = { 0, 1, 2};
    //glDrawElements(GL_TRIANGLE_FAN, 3, GL_UNSIGNED_SHORT,indices);
    glDisableVertexAttribArray ( positionIndex );
}
void Draw3 ( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    // GLfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat vertexPos[] =
            {
                    0.0f,  0.5f, 0.0f,        // v0
                    -0.5f, -0.5f, 0.0f,        // v1
                    0.5f, -0.5f, 0.0f         // v2
            };
    GLfloat color[] =
            {
                    1.0f, 0.0f, 0.0f, 1.0f,   // c0
                    0.0f, 1.0f, 0.0f, 1.0f,   // c1
                    0.0f, 0.0f, 1.0f, 1.0f    // c2
            };

    GLushort indices[3] = { 0, 1, 2,0};
    glViewport ( 0, 0, esContext->width, esContext->height );
    //glClear ( GL_COLOR_BUFFER_BIT );
    glUseProgram ( userData->programObject );

    GLint positionIndex = glGetAttribLocation(userData->programObject, "a_position");
    GLint colorIndex = glGetAttribLocation(userData->programObject, "a_color");
    if(userData->voidId[0] == 0){
        glGenBuffers(3, &userData->voidId);
        glBindBuffer(GL_ARRAY_BUFFER, userData->voidId[0]);
        //所有顶点的总长度
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9, vertexPos, GL_STATIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, userData->voidId[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, color, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->voidId[2]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 4, indices, GL_STATIC_DRAW);

    }
    glBindBuffer(GL_ARRAY_BUFFER, userData->voidId[0]);
    glEnableVertexAttribArray ( positionIndex );
    //一个顶点的元素的个数  一个顶点的元素总长度
    glVertexAttribPointer(positionIndex, POSITION_STRIDE, GL_FLOAT, GL_FALSE,sizeof(GLfloat)*POSITION_STRIDE, 0);
    glBindBuffer(GL_ARRAY_BUFFER, userData->voidId[1]);
    glEnableVertexAttribArray ( colorIndex );
    glVertexAttribPointer(colorIndex, COLOR_STRIDE, GL_FLOAT, GL_FALSE,sizeof(GLfloat)*COLOR_STRIDE, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->voidId[2]);


    //glDrawArrays ( GL_TRIANGLES, 0, 3 );

    glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_SHORT,0);
    glDisableVertexAttribArray ( positionIndex );
    glDisableVertexAttribArray ( colorIndex );

    glBindBuffer ( GL_ARRAY_BUFFER, 0 );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Draw( ESContext *esContext){
    glClear ( GL_COLOR_BUFFER_BIT );
    Draw1(esContext);
    //Draw2(esContext);
    //Draw3(esContext);
}
void Shutdown ( ESContext *esContext )
{
    UserData *userData = esContext->userData;

    glDeleteProgram ( userData->programObject );
    glDeleteBuffers ( 1, userData->voidId );
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "Example 6-3", 640, 480, ES_WINDOW_RGB );

    if ( !Init ( esContext ) )
    {
        return GL_FALSE;
    }

    esRegisterShutdownFunc ( esContext, Shutdown );
    esRegisterDrawFunc ( esContext, Draw );

    return GL_TRUE;
}
