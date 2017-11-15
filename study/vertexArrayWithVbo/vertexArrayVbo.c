#include "esUtil.h"
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>


#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1



typedef struct {
    GLuint programObject;
    GLuint vboIds[3];
} UserData;


int Init(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    const char vShaderStr[] =
            "#version 300 es                            \n"
                    "layout(location = 0) in vec4 a_position;   \n"
                    "layout(location = 1) in vec4 a_color;      \n"
                    "out vec4 v_color;                          \n"
                    "void main()                                \n"
                    "{                                          \n"
                    "    v_color = a_color;                     \n"
                    "    gl_Position = a_position;              \n"
                    "}";


    const char fShaderStr[] =
            "#version 300 es            \n"
                    "precision mediump float;   \n"
                    "in vec4 v_color;           \n"
                    "out vec4 o_fragColor;      \n"
                    "void main()                \n"
                    "{                          \n"
                    "    o_fragColor = v_color; \n"
                    "}" ;

    GLuint programObject;

    // Create the program object
    programObject = esLoadProgram ( vShaderStr, fShaderStr );

    if ( programObject == 0 )
    {
        return GL_FALSE;
    }

    // Store the program object
    userData->programObject = programObject;
    userData->vboIds[0] = 0;
    userData->vboIds[1] = 0;
    userData->vboIds[2] = 0;


    glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
    return GL_TRUE;
}


char* file2str(const char *pathname)
{
    int fd;
    if((fd=open(pathname, O_RDONLY)) == -1)
    {
        perror(pathname);
        return NULL;
    }

    char *buf = NULL;
    buf = malloc(0);
    size_t count = 0, n = 0;//count:real number

    do
    {
        buf = realloc(buf, count + 512);
        n = read(fd, buf + count, 512);//each 512

        if(n < 0)//ERROR?
        {
            free(buf);
            buf= NULL;
        }

        count += n;

    }while((n < 512) && (n > 0));//n < 512肯定读完了

    close(fd);

    if(buf)
    {
        if (0 == (count + 1) % 512)//几率很小的溢出
        {
            buf = realloc(buf, count + 1);
        }
        buf[count] = '\0';
    }
    return buf;
}


void DrawPrimitiveWithVBOs(ESContext *esContext,
                           GLint numVertices, GLfloat **vtxBuf,
                           GLint *vtxStrides, GLint numIndices,
                           GLushort *indices )
{
    UserData *userData = esContext->userData;
    if ( userData->vboIds[0] == 0 && userData->vboIds[1] == 0 &&
         userData->vboIds[2] == 0 )
    {
        glGenBuffers(3, userData->vboIds);
        glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, vtxStrides[0] * numIndices, vtxBuf[0], GL_STATIC_DRAW);

        glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[1] );
        glBufferData ( GL_ARRAY_BUFFER, vtxStrides[1] * numVertices,
                       vtxBuf[1], GL_STATIC_DRAW );


        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[2] );
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER,
                       sizeof ( GLushort ) * numIndices,
                       indices, GL_STATIC_DRAW );


    }
    //接下来的操作仅仅针对该bind的内存区域,所以每次draw都要调用bind
    //GL_ARRAY_BUFFER 创建的存储顶点数据的缓冲区对象
    //元素矩阵缓冲区对象是使用 GL_ELEMENT_ARRAY_BUFFER 创建的存储元素索引的缓冲区对象

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray ( VERTEX_POS_INDX );
    glVertexAttribPointer ( VERTEX_POS_INDX, VERTEX_POS_SIZE,
                            GL_FLOAT, GL_FALSE, vtxStrides[0], 0 );

    glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[1] );
    glEnableVertexAttribArray ( VERTEX_COLOR_INDX );
    glVertexAttribPointer ( VERTEX_COLOR_INDX,
                            VERTEX_COLOR_SIZE,
                            GL_FLOAT, GL_FALSE, vtxStrides[1], 0 );


    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[2] );

    glDrawElements ( GL_TRIANGLES, numIndices,
                     GL_UNSIGNED_SHORT, 0 );

    glDisableVertexAttribArray ( VERTEX_POS_INDX );
    glDisableVertexAttribArray ( VERTEX_COLOR_INDX );


    //将当前活动内存设置为空
    glBindBuffer ( GL_ARRAY_BUFFER, 0 );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );

}





void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    // 3 vertices, with (x,y,z) ,(r, g, b, a) per-vertex
    GLfloat vertexPos[3 * VERTEX_POS_SIZE] =
            {
                    0.0f,  0.5f, 0.0f,        // v0
                    -0.5f, -0.5f, 0.0f,        // v1
                    0.5f, -0.5f, 0.0f         // v2
            };
    GLfloat color[4 * VERTEX_COLOR_SIZE] =
            {
                    1.0f, 0.0f, 0.0f, 1.0f,   // c0
                    0.0f, 1.0f, 0.0f, 1.0f,   // c1
                    0.0f, 0.0f, 1.0f, 1.0f    // c2
            };
    GLint vtxStrides[2] =
            {
                    VERTEX_POS_SIZE * sizeof ( GLfloat ),
                    VERTEX_COLOR_SIZE * sizeof ( GLfloat )
            };

    // Index buffer data
    GLushort indices[3] = { 0, 1, 2 };
    GLfloat *vtxBuf[2] = { vertexPos, color };

    glViewport ( 0, 0, esContext->width, esContext->height );
    glClear ( GL_COLOR_BUFFER_BIT );
    glUseProgram ( userData->programObject );

    DrawPrimitiveWithVBOs ( esContext, 3, vtxBuf,
                            vtxStrides, 3, indices );
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    glDeleteProgram ( userData->programObject );
    glDeleteBuffers ( 3, userData->vboIds );
}




int esMain(ESContext *esContext)
{
    esContext->userData = malloc ( sizeof ( UserData ) );

    esCreateWindow ( esContext, "vbo", 320, 240, ES_WINDOW_RGB );

    if ( !Init ( esContext ) )
    {
        return GL_FALSE;
    }

    esRegisterShutdownFunc ( esContext, Shutdown );
    esRegisterDrawFunc ( esContext, Draw );

    return GL_TRUE;
}