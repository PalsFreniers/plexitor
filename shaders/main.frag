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
uniform vec2 resolution;

in vec2 uv;
flat in int outC;
in vec4 outFGColor;
in vec4 outBGColor;

vec3 hsl2rgb(vec3 c) {
        vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
        return c.z + c.y * (rgb - 0.5) * (1.0 - abs(2.0 * c.z - 1.0));
}

void main() {
        int C = outC;
        if(!(ASCII_DISPLAY_LOW <= C && C <= ASCII_DISPLAY_HIGH)) C = 63;
        int index = C - ASCII_DISPLAY_LOW;
        float x = float(index % FONT_COLS) * FONT_CHARACTER_WIDTH_UV;
        float y = float(index / FONT_COLS) * FONT_CHARACTER_HEIGHT_UV;
        vec2 pos = vec2(x, y + FONT_CHARACTER_HEIGHT_UV); 
        vec2 size = vec2(FONT_CHARACTER_WIDTH_UV, -FONT_CHARACTER_HEIGHT_UV);
        vec2 t = pos + size * uv;
        vec4 tex = texture(font, t);
        vec2 fragUV = vec2(gl_FragCoord.xy / resolution.xy);
        vec4 rainbow = vec4(hsl2rgb(vec3((time + fragUV.x + fragUV.y) * 0.5, 0.5, 0.5)), 1.0); 
        gl_FragColor = outBGColor * (1.0 - tex.x) + tex.x * tex.w * outFGColor * rainbow;
}
