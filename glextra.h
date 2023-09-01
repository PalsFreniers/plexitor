#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include "macros.h"
#include "file.h"

void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
const char* cstrShaderType(GLenum type); 
bool compileShaderSource(const GLchar* source, GLenum shaderType, GLuint* shader);
bool compileShaderFile(const char* path, GLenum shaderType, GLuint* shader);
bool linkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint* program);
