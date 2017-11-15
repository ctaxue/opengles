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



    /*char vShaderStr[] =
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
    */

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

float height = 1;

void Draw(ESContext *esContext)
{
    clock_t t = clock();
    long sec = t ;
    sec = t % 255;
    height++;
    //printf ( "sec is: %ld\n",sec);

    UserData *userData = esContext->userData;
    GLfloat indexColor =(GLfloat)t;

    float tt = sec;
    float ff = (float) (tt / 255.0);
    GLfloat indexColor2 = (GLfloat) (1.0 - ff );
    printf ( "sec is: %f\n",ff);



    GLfloat color[4] = { 1.0f - indexColor2 , indexColor2, indexColor2, 1.0f };

    // 3 vertices, with (x,y,z) per-vertex
    GLfloat vertexPos[3 * 3] =
            {
                    0.0f,  0.5f, 0.0f, // v0
                    -0.5f, -0.5f, 0.0f, // v1
                    0.5f, -0.5f, 0.0f  // v2
            };


    esContext->height = height;
    glViewport ( 0, 0, esContext->width, esContext->height );

    glClear ( GL_COLOR_BUFFER_BIT );

    glUseProgram ( userData->programObject );

    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vertexPos );
    glEnableVertexAttribArray ( 0 );
    glVertexAttrib4fv ( 1, color );


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

/*
void ESUTIL_API esRegisterUpdateFunc ( ESContext *esContext, void ( ESCALLBACK *updateFunc ) ( ESContext *, float ) )
{
    esContext->updateFunc = updateFunc;
}
*/
