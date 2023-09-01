#version 330 core

#define FONT_WIDTH 128
#define FONT_HEIGHT 64
#define FONT_ROWS 7
#define FONT_COLS 18
#define FONT_CHARACTER_WIDTH  (FONT_WIDTH  / FONT_COLS)
#define FONT_CHARACTER_HEIGHT (FONT_HEIGHT / FONT_ROWS)

layout(location = 0) in ivec2 tile;
layout(location = 1) in int c;
layout(location = 2) in vec4 fgcolor;
layout(location = 3) in vec4 bgcolor;

uniform vec2 resolution;
uniform vec2 scale;

out vec2 uv;
flat out int outC;
out vec4 outFGColor;
out vec4 outBGColor;

vec2 projectPoint(vec2 point) {
    return 2.0 * point / resolution;
}

void main() {
    uv = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
    vec2 charSize = vec2(float(FONT_CHARACTER_WIDTH), float(FONT_CHARACTER_HEIGHT));
    vec2 pos = tile * charSize * scale;
    gl_Position = vec4(projectPoint(uv * charSize * scale + pos), 0.0, 1.0);
    outC = c;
    outFGColor = fgcolor;
    outBGColor = bgcolor;
}
