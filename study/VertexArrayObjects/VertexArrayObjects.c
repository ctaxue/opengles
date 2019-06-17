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
//https://blog.csdn.net/Blues1021/article/details/51460498
#include <stdio.h>
#include "esUtil.h"
#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1
#define ELEMENT_COUNT         3
typedef struct
{
    // Handle to a program object
    GLuint programObject;

    // VertexBufferObject Ids
    GLuint vboIds[2];
    GLuint vaoId;

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

    // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
    GLfloat vertices[3 * ( VERTEX_POS_SIZE + VERTEX_COLOR_SIZE )] =
            {
                    0.0f,  0.5f, 0.0f,        // v0
                    1.0f,  0.0f, 0.0f, 1.0f,  // c0
                    -0.5f, -0.5f, 0.0f,        // v1
                    0.0f,  1.0f, 0.0f, 1.0f,  // c1
                    0.5f, -0.5f, 0.0f,        // v2
                    0.0f,  0.0f, 1.0f, 1.0f,  // c2
            };
    // Index buffer data
    GLushort indices[3] = { 0, 1, 2 };
    programObject = esLoadProgram(vShaderStr, fShaderStr);
    if ( programObject == 0 )
    {
        return GL_FALSE;
    }

    //GLint positionIndex = glGetAttribLocation(userData->programObject, "a_position");
    //GLint colorIndex = glGetAttribLocation(userData->programObject, "a_color");
    GLint positionIndex = 0;
    GLint colorIndex = 1;
    userData->programObject = programObject;
    glGenBuffers(2, userData->vboIds);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER,sizeof ( vertices ) , vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ( indices ), indices, GL_STATIC_DRAW);

    //一个VAO有多个VBO，它们之间也是通过上下文，只有唯一的激活VAO，在VAO后创建的VBO都属于该VAO
    /*1. Generate Vertex Array Object
    2. Bind Vertex Array Object

    3. Generate Vertex Buffer Object
    4. Bind Vertex Buffer Object*/
    glGenVertexArrays(1, userData->vaoId);
    glBindVertexArray(userData->vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);

    glEnableVertexAttribArray(positionIndex);
    glEnableVertexAttribArray(colorIndex);
    glVertexAttribPointer(positionIndex, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE,
                          (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE) * sizeof(GLfloat), (const void *) 0);
    glVertexAttribPointer(colorIndex, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE,
                          (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE) * sizeof(GLfloat), (const void *) (sizeof(GLfloat)*VERTEX_POS_SIZE));

    glBindVertexArray(0);
    glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
    return GL_TRUE;
}

///
// Draw a triangle using the program object created in Init()
//


void Draw( ESContext *esContext){
    UserData *userData = esContext->userData;
    glViewport ( 0, 0, esContext->width, esContext->height );
    glClear ( GL_COLOR_BUFFER_BIT );
    glUseProgram(userData->programObject);
    glBindVertexArray(userData->vaoId);
    glDrawElements(GL_TRIANGLE_STRIP, ELEMENT_COUNT, GL_UNSIGNED_SHORT, (const void *) 0);
    glBindVertexArray(0);

}

void Shutdown ( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    glDeleteVertexArrays(1, userData->vaoId); // 引用的VBO会变成未绑定的，但不会删除VBO数据块
    glDeleteBuffers(2, userData->vboIds); // 删除VBO缓存区数据块
    glDeleteProgram ( userData->programObject );
    glDeleteBuffers ( 3, userData->vboIds );
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "VertexArray", 640, 480, ES_WINDOW_RGB );

    if ( !Init ( esContext ) )
    {
        return GL_FALSE;
    }

    esRegisterShutdownFunc ( esContext, Shutdown );
    esRegisterDrawFunc ( esContext, Draw );

    return GL_TRUE;
}
