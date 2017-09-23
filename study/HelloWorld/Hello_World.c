#include "esUtil.h"
#include <stdio.h>
typedef struct {
    GLuint programObject;
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
    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "void main()                              \n"
            "{                                        \n"
            "   gl_Position = vPosition;              \n"
            "}                                        \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "void main()                                  \n"
            "{                                            \n"
            "   fragColor = vec4 ( 1.0, 0.0, 1.0, 1.0 );  \n"
            "}                                            \n";


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
    GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                             -0.5f, -0.5f, 0.0f,
                             0.5f, -0.5f, 0.0f
    };

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glDeleteProgram(userData->programObject);
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

    GLint maxVertexAttribs; // n will be >= 8
    //glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

    UserData *userData = esContext->userData;
    glGetProgramiv(userData->programObject, GL_ACTIVE_ATTRIBUTES, &maxVertexAttribs);
    printf("the result is %d\n", maxVertexAttribs);
    return GL_TRUE;
}