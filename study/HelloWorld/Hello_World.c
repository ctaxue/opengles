#include "esUtil.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
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
    printf("location is %d\n", strlen(shaderSrc));
    const GLint sourthLength[1] = {strlen(shaderSrc)};
    glShaderSource ( shader, 1, &shaderSrc, sourthLength );
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

float height = 500;


float off = 0.0f;
Bool current = TRUE;

void Draw(ESContext *esContext)
{

    UserData *userData = esContext->userData;
    GLfloat color[4] = { 1.0f  , 0, 1, 1.0f };
    // 3 vertices, with (x,y,z) per-vertex
    GLfloat vertexPos[3 * 3] =
            {
                    0.0f,  0.5f, 0.0f, // v0
                    -0.5f, -0.5f, 0.0f, // v1
                    0.5f, -0.5f, 0.0f  // v2
            };
    if(current)
    {
        off = off + 0.01 / 2;
    }
    else{
        off = off - 0.01 / 2;
    }

    if(off > 1.0f){
        current = FALSE;
    }
    if(off <= 0){
        current = TRUE;
    }

    vertexPos[7] = 0.0f + off;


    glUniform1f(glGetUniformLocation(userData->programObject , "attenuation") , off);
    glViewport ( 0, 0, esContext->width, esContext->height );
    const GLint location = glGetAttribLocation(userData->programObject, "a_color");
    //printf("location is %d\n", location);

    glClear ( GL_COLOR_BUFFER_BIT );

    glUseProgram ( userData->programObject );

    glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 0, vertexPos );
    glEnableVertexAttribArray ( 1 );
    glVertexAttrib4fv ( 0, color );


    glDrawArrays ( GL_TRIANGLES, 0, 3 );

    glDisableVertexAttribArray ( 0 );
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
    //glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

    UserData *userData = esContext->userData;
    glGetProgramiv(userData->programObject, GL_ACTIVE_ATTRIBUTES, &maxVertexAttribs);
    printf("the result is %d\n", maxVertexAttribs);
    return GL_TRUE;
}
