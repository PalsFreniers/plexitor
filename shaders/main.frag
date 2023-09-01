#version 330 core

#define FONT_WIDTH 128
#define FONT_HEIGHT 64
#define FONT_ROWS 7
#define FONT_COLS 18
#define FONT_CHARACTER_WIDTH  (FONT_WIDTH  / FONT_COLS)
#define FONT_CHARACTER_HEIGHT (FONT_HEIGHT / FONT_ROWS)
#define FONT_CHARACTER_WIDTH_UV  (float(FONT_CHARACTER_WIDTH)  / float(FONT_WIDTH))
#define FONT_CHARACTER_HEIGHT_UV (float(FONT_CHARACTER_HEIGHT) / float(FONT_HEIGHT))
#define ASCII_DISPLAY_LOW 32
#define ASCII_DISPLAY_HIGH 127

uniform sampler2D font;
uniform float time;

in vec2 uv;
flat in int outC;
in vec4 outFGColor;
in vec4 outBGColor;

void main() {
    int C = outC;
    if(!(ASCII_DISPLAY_LOW <= C && C <= ASCII_DISPLAY_HIGH)) C = 63;
    int index = C - ASCII_DISPLAY_LOW;
    float x = float(index % FONT_COLS) * FONT_CHARACTER_WIDTH_UV;
    float y = float(index / FONT_COLS) * FONT_CHARACTER_HEIGHT_UV;
    vec2 t = vec2(x, y + FONT_CHARACTER_HEIGHT_UV) + vec2(FONT_CHARACTER_WIDTH_UV, -FONT_CHARACTER_HEIGHT_UV) * uv;
    vec4 tex = texture(font, t);
    gl_FragColor = outBGColor * (1.0 - tex.x) + tex.x * tex.w * outFGColor;
}
