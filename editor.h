#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "la.h"
#include "sv.h"

// structures

// line
typedef struct {
    size_t capacity;
    size_t size;
    char *buffer;
} Line;

// editor
typedef struct {
    size_t capacity;
    size_t size;
    Vec2si cursor;
    Line* lines;
} Editor;

// camera
typedef struct {
    Vec2f position;
    Vec2f velocity;
} Camera;

// functions

// line
void LineInsertSizedTextBefore(Line* line, const char* text, size_t size, size_t* col);
void LineInsertTextBefore(Line* line, const char* text, size_t* col);
void LineAppendSizedText(Line* line, const char* str, size_t size);
void LineAppendText(Line* line, const char* str);
void LineBackSpace(Line* line, size_t* col);
void LineDelete(Line* line, size_t* col);

// editor
void EditorInsertTextBeforeCursor(Editor* editor, const char* str);
void EditorInsertLine(Editor* editor);
void EditorBackspace(Editor* editor);
void EditorDelete(Editor* editor);

const char* EditorGetCharUnderCursor(Editor* editor);

void EditorSaveToFile(const Editor* editor, const char* filePath);
void EditorLoadFromFile(Editor* editor, FILE* file);
