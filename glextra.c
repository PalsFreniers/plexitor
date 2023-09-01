#include "glextra.h"

void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    UNUSED(source);
    UNUSED(id);
    UNUSED(length);
    UNUSED(userParam);
    DEBUG_ARG("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

const char* cstrShaderType(GLenum type) {
    switch (type) {
        case GL_VERTEX_SHADER:
            return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER:
            return "GL_FRAGMENT_SHADER";
        default:
            return "not a shader";
    }
}

bool compileShaderSource(const GLchar* source, GLenum shaderType, GLuint* shader) {
    *shader = glCreateShader(shaderType);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLchar message[1024];
        GLsizei messageSize = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &messageSize, message);
        ERROR_ARG("Unable to compile %s", cstrShaderType(shaderType));
        ERROR_ARG("%.*s", messageSize, message);
        return false;
    }
    return true;
}

bool compileShaderFile(const char* path, GLenum shaderType, GLuint* shader){
    char* source = slurpFile(path);
    bool ok = compileShaderSource(source, shaderType, shader);
    if(!ok) ERROR_ARG("Failed to compile %s", path);
    return ok;
}

bool linkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint* program) {
    *program = glCreateProgram();

    glAttachShader(*program, vertexShader);
    glAttachShader(*program, fragmentShader);
    glLinkProgram(*program);

    GLint linked = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if(!linked) {
        GLchar message[1024];
        GLsizei messageSize = 0;

        glGetProgramInfoLog(*program, sizeof(message), &messageSize, message);
        ERROR_ARG("Program linking : %.*s", messageSize, message);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

