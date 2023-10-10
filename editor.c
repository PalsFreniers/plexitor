#include "editor.h"

#define LINE_INIT_CAPACITY 1024
#define EDITOR_INIT_CAPACITY 128

Editor *p = NULL;

static void LineExpand(Line* line, size_t needed) {
    size_t newCapacity = line->capacity;
    assert(line->capacity >= line->size);
    while(newCapacity - line->size < needed) {
        if(line->capacity == 0) newCapacity = LINE_INIT_CAPACITY;
        else newCapacity *= 2;
    }
    if(line->capacity != newCapacity) {
        line->buffer = realloc(line->buffer, newCapacity);
        line->capacity = newCapacity;
    }
}

void LineInsertSizedTextBefore(Line* line, const char* text, size_t size, size_t* col) {
    if(*col > line->size) *col = line->size;
    
    LineExpand(line, size);
    memmove(line->buffer + *col + size,
            line->buffer + *col,
            line->size - *col);
    memcpy(line->buffer + *col, text, size);
    line->size += size;
    *col += size;
}

void LineInsertTextBefore(Line* line, const char* text, size_t* col) {
    LineInsertSizedTextBefore(line, text, strlen(text), col);
}

void LineAppendSizedText(Line *line, const char *str, size_t size) {
    size_t col = line->size;
    LineInsertSizedTextBefore(line, str, size, &col);
}

void LineAppendText(Line* line, const char* str) {
    LineAppendSizedText(line, str, strlen(str));
}

void LineBackSpace(Line* line, size_t* col) {
    if(*col > line->size) *col = line->size;
    if(line->size > 0 && *col > 0) {
        memmove(line->buffer + *col - 1,
                line->buffer + *col,
                line->size - *col);
        line->size--;
        (*col)--;
    }
}

void LineDelete(Line* line, size_t* col) {
    if(*col > line->size) *col = line->size;
    if(line->size > 0 && *col < line->size) {
        memmove(line->buffer + *col,
                line->buffer + *col + 1,
                line->size - *col - 1);
        line->size--;
    }
}

static void EditorExpand(Editor* editor, size_t needed) {
    size_t newCapacity = editor->capacity;
    assert(editor->capacity >= editor->size);
    while(newCapacity - editor->size < needed) {
        if(editor->capacity == 0) newCapacity = EDITOR_INIT_CAPACITY;
        else newCapacity *= 2;
    }
    if(editor->capacity != newCapacity) {
        editor->lines = realloc(editor->lines, newCapacity * sizeof(editor->lines[0]));
        editor->capacity = newCapacity;
    }
}

static void EditorFirstNewLine(Editor* editor) {
    if((size_t)editor->cursor.y >= editor->size) {
        if(editor->size > 0) editor->cursor.y = editor->size - 1;
        else {
            EditorExpand(editor, 1);
            memset(&editor->lines[editor->size], 0, sizeof(editor->lines[0]));
            editor->size++;
        }
    }
}

void EditorInsertTextBeforeCursor(Editor* editor, const char* str) {
    EditorFirstNewLine(editor);
    LineInsertTextBefore(&editor->lines[editor->cursor.y], str, (size_t*)&(editor->cursor.x));
}

void EditorInsertLine(Editor *editor) {
    if((size_t)editor->cursor.y > editor->size) editor->cursor.y = editor->size;
    EditorExpand(editor, 1);
    size_t lineSize = sizeof(editor->lines[0]);
    memmove(editor->lines + editor->cursor.y + 1,
            editor->lines + editor->cursor.y,
            (editor->size - editor->cursor.y) * lineSize);
    memset(&editor->lines[editor->cursor.y+1], 0, lineSize);
    editor->cursor.y += 1;
    editor->cursor.x = 0;
    editor->size += 1;
}

void EditorBackspace(Editor* editor) {
    EditorFirstNewLine(editor);
    LineBackSpace(&editor->lines[editor->cursor.y], (size_t*)&editor->cursor.x);
}

void EditorDelete(Editor* editor) {
    EditorFirstNewLine(editor);
    LineDelete(&editor->lines[editor->cursor.y], (size_t*)&editor->cursor.x);
}

const char* EditorGetCharUnderCursor(Editor *editor) {
    if((size_t)editor->cursor.y < editor->size) {
        if((size_t)editor->cursor.x < editor->lines[editor->cursor.y].size) return &editor->lines[editor->cursor.y].buffer[editor->cursor.x];
    }
    return NULL;
}

void EditorSaveToFile(const Editor* editor, const char* filePath) {
    FILE* f = fopen(filePath, "w");
    if(f == NULL) {
        fprintf(stderr, "ERROR : Unable to open file '%s'\nReason : %s\n", filePath, strerror(errno));
        exit(1);
    }
    for(size_t i = 0; i < editor->size; i++) {
        fwrite(editor->lines[i].buffer, 1, editor->lines[i].size, f);
        fputc('\n', f);
    }
    fclose(f);
}


void EditorLoadFromFile(Editor* editor, FILE* f) {
    assert(editor->lines == NULL && "Unable to load file in not empty editor");
    EditorFirstNewLine(editor);

    static char chunk[1024 * 640];
    while(!feof(f)) {
        size_t len = fread(chunk, 1, sizeof(chunk), f);
        String_View chunkSv = (String_View) {
            .data = chunk,
            .count = len
        };
        while(chunkSv.count > 0) {
            String_View chunkLine = {0};
            Line* line = &editor->lines[editor->size - 1];
            if(sv_try_chop_by_delim(&chunkSv, '\n', &chunkLine)) {
                LineAppendSizedText(line, chunkLine.data, chunkLine.count);
                EditorInsertLine(editor);
            } else {
                LineAppendSizedText(line, chunkSv.data, chunkSv.count);
                chunkSv = SV_NULL;
            }
        }
    }
    editor->cursor.y = 0;
    editor->cursor.x = 0;
}
