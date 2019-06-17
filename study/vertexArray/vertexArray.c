#include "esUtil.h"
#include <stdio.h>

#include<stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<string.h>
#include <time.h>

typedef struct {
    GLuint programObject;
    float angle;
    ESMatrix  mvpMatrix;
} UserData;



GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    if(shader == 0)
    {
        return 0;
    }
    glShaderSource ( shader, 1, &shaderSrc, NULL );
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            GLchar *infoLog = malloc ( sizeof ( char ) * infoLen );
            glGetShaderInfoLog(shader, infoLen, NULL, infoLen);
            esLogMessage ( "Error compiling shader:\n%s\n", infoLog );
            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}


int init(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    char *vShaderStr = file2str("simple_vertex_shader.glsl");
    char *fShaderStr = file2str("simple_fragment_shader.glsl");


    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;


    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    programObject = glCreateProgram();
    if (programObject == 0)
    {
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

    if (!linked)
    {
        GLint infoLen = 0;

        glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

        if (infoLen > 1)
        {
            char *infoLog = malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
            esLogMessage ( "Error linking program:\n%s\n", infoLog );

            free ( infoLog );
        }
        glDeleteProgram ( programObject );
        return FALSE;
    }

    userData->programObject = programObject;
    glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
    return TRUE;

}


void Draw(ESContext *esContext)
{


    UserData *userData = esContext->userData;



    #define VERTEX_POS_SIZE  3  //x y z
    #define VERTEX_COLOR_SIZE 4 //r g b a

    GLfloat input[4*3 + 3*3] = {
            0.0f,  0.5f, 0.0f, // v0
            1.0f, 0.0f, 0.0f, 1.0f ,

            -0.5f, -0.5f, 0.0f, // v1
            0.0f, 1.0f, 0.0f, 1.0f,

            0.5f, -0.5f, 0.0f,  // v2
            0.0f, 0.0f, 1.0f, 1.0f
    };

    glViewport ( 0, 0, esContext->width, esContext->height );

    glClear ( GL_COLOR_BUFFER_BIT );

    glUseProgram ( userData->programObject );


    //pos
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)*sizeof(GLfloat), input );
    glEnableVertexAttribArray ( 0 );

    //color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)*sizeof(GLfloat), input + 3);
    glEnableVertexAttribArray ( 1 );
    //glVertexAttrib4fv ( 1, color );




    glDrawArrays ( GL_TRIANGLES, 0, 3 );

    glDisableVertexAttribArray ( 0 );
    glDisableVertexAttribArray ( 1 );
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glDeleteProgram(userData->programObject);
}


void Update ( ESContext *esContext, float deltaTime )
{
    glVertexAttrib4f ( 1, 1.0f, 1.0f, 1.0f, 1.0f );
}

int esMain(ESContext *esContext)
{
    esContext->userData = malloc(sizeof(UserData));
    esCreateWindow(esContext, "triangle", 640, 480, ES_WINDOW_RGB);
    if ( !init ( esContext ) )
    {
        return GL_FALSE;
    }
    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterDrawFunc ( esContext, Draw );
    //esRegisterUpdateFunc ( esContext,Update);

    GLint maxVertexAttribs; // n will be >= 8

    UserData *userData = esContext->userData;
    glGetProgramiv(userData->programObject, GL_ACTIVE_ATTRIBUTES, &maxVertexAttribs);
    printf("the result is %d\n", maxVertexAttribs);
    return GL_TRUE;
}
