#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stbi.h"
#include "glextra.h"
#include "macros.h"
#include "editor.h"
#include "la.h"

void scc(int code) {
    if(code < 0) {
        ERROR_ARG("SDL Crashed on code with error : %s", SDL_GetError());
        exit(1);
    }
}

void* scp(void* ptr) {
    if(ptr == NULL) {
        ERROR_ARG("SDL Crashed on pointer with error : %s", SDL_GetError());
        exit(1);
    }
    return ptr;
}

SDL_Surface* loadFromFile(const char* file) {
    int w, h, n;
    unsigned char* pixels = stbi_load(file, &w, &h, &n, STBI_rgb_alpha);
    if(pixels == NULL) {
        fprintf(stderr, "Unable to load file %s\nReason : %s\n", file, stbi_failure_reason());
        exit(1);
    }
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const Uint32 rmask = 0xFF000000;
    const Uint32 gmask = 0x00FF0000;
    const Uint32 bmask = 0x0000FF00;
    const Uint32 amask = 0x000000FF;
#else
    const Uint32 rmask = 0x000000FF;
    const Uint32 gmask = 0x0000FF00;
    const Uint32 bmask = 0x00FF0000;
    const Uint32 amask = 0xFF000000;
#endif
    const int d = 32, p = 4*w;

    return scp(SDL_CreateRGBSurfaceFrom((void*)pixels, w, h, d, p, rmask, gmask, bmask, amask));
}

typedef struct {
    SDL_Texture* spritesheet;
    SDL_Rect glyphTable[ASCII_DISPLAY_HIGH - ASCII_DISPLAY_LOW + 1];
} Font;

Font initFontFromFile(SDL_Renderer* renderer, const char* file){
    Font ret = {0};
    SDL_Surface* surface = loadFromFile(file);
    scc(SDL_SetColorKey(surface, SDL_TRUE, 0xFF000000));
    ret.spritesheet = scp(SDL_CreateTextureFromSurface(renderer, surface));
    SDL_FreeSurface(surface);
    for(size_t i = ASCII_DISPLAY_LOW; i <= ASCII_DISPLAY_HIGH; ++i) {
        const size_t index = i - ASCII_DISPLAY_LOW;
        const size_t col = index % FONT_COLS;
        const size_t row = index / FONT_COLS;
        ret.glyphTable[index] = (SDL_Rect) {
            .x = col * FONT_CHARACTER_WIDTH,
                .y = row * FONT_CHARACTER_HEIGHT,
                .w = FONT_CHARACTER_WIDTH,
                .h = FONT_CHARACTER_HEIGHT
        };
    }
    return ret;
}

void renderChar(SDL_Renderer* renderer, Font* font, char c, Vec2f pos, float scale) {
    if(c == '\t') c = ' ';
    assert(c >= ASCII_DISPLAY_LOW);
    const size_t index = c - ASCII_DISPLAY_LOW;
    SDL_Rect dst = (SDL_Rect) {
        .x = (int) floorf(pos.x),
            .y = (int) floorf(pos.y),
            .w = (int) floorf(FONT_CHARACTER_WIDTH * scale),
            .h = (int) floorf(FONT_CHARACTER_HEIGHT * scale)
    };
    scc(SDL_RenderCopy(renderer, font->spritesheet, &(font->glyphTable[index]), &dst));
}

void setTexureColor(SDL_Texture* texture, Uint32 color) {
    scc(SDL_SetTextureColorMod(texture,
                (color >> (8 * 0)) & 0xFF,
                (color >> (8 * 1)) & 0xFF,
                (color >> (8 * 2)) & 0xFF));
    scc(SDL_SetTextureAlphaMod(texture, (color >> (8 * 3)) & 0xFF));
}

void renderSizedText(SDL_Renderer* renderer, Font* font, const char* text, int size, Vec2f pos, Uint32 color, float scale) {
    setTexureColor(font->spritesheet, color);
    Vec2f pen = pos;
    for(int i = 0; i < size; i++){
        renderChar(renderer, font, text[i], pen, scale);
        pen.x += FONT_CHARACTER_WIDTH * scale;
    }
}

void renderText(SDL_Renderer* renderer, Font* font, const char* text, Vec2f pos, Uint32 color, float scale) {
    renderSizedText(renderer, font, text, strlen(text), pos, color, scale);
}

Editor editor = {
    .capacity = 0,
    .cursor = {0, 0},
    .lines = NULL,
    .size = 0,
};
Camera cam = {
    .position = {0, 0},
    .velocity = {0, 0},
};


Vec2f windowSize(SDL_Window* window) {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return vec2f(w, h);
}

Vec2f cameraProjectPoint(SDL_Window* window, Vec2f point) {
            return vec2f_add(
                    vec2f_sub(point, cam.position),
                    vec2f_mul(windowSize(window), vec2fs(0.5f)));
}

void usage(FILE* stream, const char* str) {
    fprintf(stream, "Usage : %s [FILEPATH | OPTION]\noptions :\n\t -h | --help show this usage\n", str);
}
    
void strrev(char *arr, int start, int end)
{
    char temp;

    if (start >= end)
        return;

    temp = *(arr + start);
    *(arr + start) = *(arr + end);
    *(arr + end) = temp;

    start++;
    end--;
    strrev(arr, start, end);
}

char *itoa(int number, char *arr, int base)
{
    int i = 0, r, negative = 0;

    if (number == 0)
    {
        arr[i] = '0';
        arr[i + 1] = '\0';
        return arr;
    }

    if (number < 0 && base == 10)
    {
        number *= -1;
        negative = 1;
    }

    while (number != 0)
    {
        r = number % base;
        arr[i] = (r > 9) ? (r - 10) + 'a' : r + '0';
        i++;
        number /= base;
    }

    if (negative)
    {
        arr[i] = '-';
        i++;
    }

    strrev(arr, 0, i - 1);

    arr[i] = '\0';

    return arr;
}

typedef struct {
    Vec2i tile;
    int c;
    Vec4f fgColor;
    Vec4f bgColor;
} Glyph;

typedef struct {
    size_t offset;
    size_t components;
    GLenum type;
} GlyphAttributeDefinition;

typedef enum {
    GLYPH_ATTRIBUTE_TILE = 0,
    GLYPH_ATTRIBUTE_CHAR,
    GLYPH_ATTRIBUTE_FG_COLOR,
    GLYPH_ATTRIBUTE_BG_COLOR,
    GLYPH_ATTRIBUTE_COUNT,
} GlyphAttribute;

static const GlyphAttributeDefinition GlyphAttributeDefinitions[GLYPH_ATTRIBUTE_COUNT] = {
    [GLYPH_ATTRIBUTE_TILE] = {
        .offset = offsetof(Glyph, tile),
        .components = 2,
        .type = GL_INT,
    },
    [GLYPH_ATTRIBUTE_CHAR]  = {
        .offset = offsetof(Glyph, c),
        .components = 1,
        .type = GL_INT,
    },
    [GLYPH_ATTRIBUTE_FG_COLOR] = {
        .offset = offsetof(Glyph, fgColor),
        .components = 4,
        .type = GL_FLOAT,
    },
    [GLYPH_ATTRIBUTE_BG_COLOR] = {
        .offset = offsetof(Glyph, bgColor),
        .components = 4,
        .type = GL_FLOAT,
    }
};
static_assert(GLYPH_ATTRIBUTE_COUNT == 4, "The amout of glyph attribute haven't been modified correctly");

Glyph glyphBuffer[GLYPH_BUFFER_CAPACITY] = {0};
size_t glyphCount = 0;

void glyphBufferClear() {
	glyphCount = 0;
}

void glyphBufferPush(Glyph glyph) {
    assert(glyphCount < GLYPH_BUFFER_CAPACITY);
    glyphBuffer[glyphCount++] = glyph;
}

void glyphBufferSync() {
    glBufferSubData(GL_ARRAY_BUFFER, 0, glyphCount * sizeof(Glyph), glyphBuffer);
}

void glRenderSizedText(const char* text, size_t textSize, Vec2i tile, Vec4f fgColor, Vec4f bgColor) {
    for(size_t i = 0; i < textSize; i++) {
        glyphBufferPush((Glyph) {
            .tile = vec2i_add(tile, vec2i(i, 0)),
            .c = text[i],
            .fgColor = fgColor,
            .bgColor = bgColor,
        });
    }
}

void glRenderText(const char* text, Vec2i tile, Vec4f fgColor, Vec4f bgColor) {
    glRenderSizedText(text, strlen(text), tile, fgColor, bgColor);
}
Vec4f bgColor = {0};

void renderCursor() {
    const char *c = EditorGetCharUnderCursor(&editor);
    Vec2i tile = vec2i(editor.cursor.x, -(int)editor.cursor.y);
    glRenderSizedText(c ? c : " ", 1, tile, bgColor, vec4f(0.86f, 0.86f, 0.86f, 0.86f));
}

// OPENGL

int main(int argc, char** argv) {
    bgColor = vec4f(0.14f, 0.14f, 0.14f, 1.0f);
    char *filePath = NULL;
    if(argc > 1) {
        if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            usage(stdout, argv[0]);
            exit(0);
        } else filePath = argv[1];
    }

    if(filePath) {
        FILE* f = fopen(filePath, "r");
        if(f != NULL) {
            EditorLoadFromFile(&editor, f);
            fclose(f);
        }
    }
    scc(SDL_Init(SDL_INIT_VIDEO));

    SDL_Window* window = scp(SDL_CreateWindow("Plexitor", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        int major, minor;
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
        LOG_ARG("OpenGL version %d.%d", major, minor);
    }

    scp(SDL_GL_CreateContext(window));
    SDL_Surface* icon = loadFromFile("logo.png");
    SDL_SetWindowIcon(window, icon);

    if(glewInit() != GLEW_OK) {
        ERROR("GLEW Initialisation failed");
        exit(1);
    }

    if(!GLEW_ARB_draw_instanced) {
        ERROR("OpenGL ARB_draw_instanced is not supported Plexitor might not work properly!!");
        exit(1);
    }

    if(!GLEW_ARB_instanced_arrays) {
        WARNING("OpenGL ARB_instanced_arrays is not supported Plexitor might not work properly!!");
        exit(1);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if(GLEW_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(messageCallback, 0);
    } else WARNING("OpenGL debug messages might not appear\nto see them please enable GLEW_ARB_debug_output!");
    GLint timeUniform = 0;
    GLint resolutionUniform = 0;
    GLint scaleUniform = 0;
    GLint cameraUniform = 0;
    {
        GLuint vertexShader = 0;
        if(!compileShaderFile("shaders/main.vert", GL_VERTEX_SHADER, &vertexShader)) {
            ERROR("Unable to compile vertex shader");
            exit(1);
        }
        GLuint fragmentShader = 0;
        if(!compileShaderFile("shaders/main.frag", GL_FRAGMENT_SHADER, &fragmentShader)) {
            ERROR("Unable to compile fragment shader");
            exit(1);
        }

        GLuint program = 0;
        if(!linkProgram(vertexShader, fragmentShader, &program)) {
            ERROR("Unable to link shaders");
            exit(1);
        }

        glUseProgram(program);
        timeUniform = glGetUniformLocation(program, "time");
        resolutionUniform = glGetUniformLocation(program, "resolution");
        scaleUniform = glGetUniformLocation(program, "scale");
        cameraUniform = glGetUniformLocation(program, "camera");
        glUniform2f(scaleUniform, FONT_SCALE, FONT_SCALE);
    }
    
    {
        glActiveTexture(GL_TEXTURE0);
        
        const char* file = "font.png";
        int w, h, n;
        unsigned char* pixels = stbi_load(file, &w, &h, &n, STBI_rgb_alpha);
        if(pixels == NULL) {
            fprintf(stderr, "Unable to load file %s\nReason : %s\n", file, stbi_failure_reason());
            exit(1);
        }

        GLuint fontTexture = 0;
        glGenTextures(1, &fontTexture);
        glBindTexture(GL_TEXTURE_2D, fontTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }

    {
        GLuint VAO = 0;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        
        GLuint VBO = 0;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glyphBuffer), glyphBuffer, GL_DYNAMIC_DRAW);
        for(GlyphAttribute i = 0; i < GLYPH_ATTRIBUTE_COUNT; i++) {
            glEnableVertexAttribArray(i);
            if(GlyphAttributeDefinitions[i].type == GL_FLOAT) glVertexAttribPointer(i, GlyphAttributeDefinitions[i].components, GlyphAttributeDefinitions[i].type, GL_FALSE, sizeof(Glyph), (void*) GlyphAttributeDefinitions[i].offset);
            else if(GlyphAttributeDefinitions[i].type == GL_INT) glVertexAttribIPointer(i, GlyphAttributeDefinitions[i].components,GlyphAttributeDefinitions[i].type, sizeof(Glyph), (void*) GlyphAttributeDefinitions[i].offset);
            else {
                UNREACHABLE();
                exit(1);
            }
            glVertexAttribDivisor(i, 1);
        }
    }



    bool quit = false;
    while(!quit) {
        const Uint32 start = SDL_GetTicks();
        SDL_Event e = {0};
        while(SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    quit = true;
                } break;
                case SDL_KEYDOWN: {
                    switch (e.key.keysym.sym) {
                        case SDLK_F2: {
                            if(filePath) EditorSaveToFile(&editor, filePath);
                        } break;
                         case SDLK_BACKSPACE: {
                            EditorBackspace(&editor);
                        } break;
                        case SDLK_DELETE: {
                            EditorDelete(&editor);
                        } break;
                        case SDLK_RETURN: {
                            EditorInsertLine(&editor);
                        } break;
                        case SDLK_TAB: {
                            EditorInsertTextBeforeCursor(&editor, "    ");
                        } break;
                        case SDLK_LEFT: {
                            if(editor.cursor.x > 0) editor.cursor.x--;
                        } break;
                        case SDLK_RIGHT: {
                            if(editor.size > 0 && editor.cursor.x < editor.lines[editor.cursor.y].size) editor.cursor.x++;
                        } break;
                        case SDLK_UP: {
                            if(editor.cursor.y > 0) editor.cursor.y--;
                        } break;
                        case SDLK_DOWN: {
                            if(editor.cursor.y < editor.size - 1 && editor.size > 0) editor.cursor.y++;
                        } break;
                    }
                } break;
                case SDL_TEXTINPUT: {
                    EditorInsertTextBeforeCursor(&editor, e.text.text);
                } break;
                case SDL_MOUSEBUTTONDOWN: {
                    // TODO : add cursor movement at mouse click
                } break;
            }
        }

        {
            const Vec2f pos = vec2f((float)editor.cursor.x * FONT_CHARACTER_WIDTH * FONT_SCALE,  (float)(-(int)editor.cursor.y) * FONT_CHARACTER_HEIGHT * FONT_SCALE);
            cam.velocity = vec2f_mul(vec2f_sub(pos, cam.position), vec2fs(SPEED));
            cam.position = vec2f_add(cam.position, vec2f_mul(cam.velocity, vec2fs(DELTA_TIME)));
            glUniform2f(cameraUniform, cam.position.x, cam.position.y);
        }

        {
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            glViewport(0, 0, width, height);
            glUniform2f(resolutionUniform, (float)width, (float)height); 
        }

	glyphBufferClear();

        for(size_t row = 0; row < editor.size; row++) {
            const Line* line = editor.lines + row;
            glRenderSizedText(line->buffer, line->size, vec2i(0, -row), vec4f(1.0f, 1.0f, 1.0f, 1.0f), bgColor);
            char num[32] = {0};
            itoa(row + 1, num, 10);
            glRenderText(num, vec2i(-1 - strlen(num), -row), vec4f(1.0f, 1.0f, 1.0f, 1.0f), bgColor); 
        }

        glyphBufferSync();

        glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUniform1f(timeUniform, (float)SDL_GetTicks() / 1000.0f);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, glyphCount);

        glyphBufferClear();
        renderCursor();
        glyphBufferSync();
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, glyphCount);

        SDL_GL_SwapWindow(window);
        const Uint32 delay = SDL_GetTicks() - start;
        const Uint32 deltaTime = 1000/FPS;
        if(delay < deltaTime) SDL_Delay(deltaTime - delay);
    }
    SDL_Quit();
    return 0;
}

#define SV_IMPLEMENTATION
#include "sv.h"

