#pragma once

#include <stdlib.h>

typedef struct {
    float x, y;
} Vec2f;

typedef struct {
    float x, y, z, w;
} Vec4f;

typedef struct {
    int x, y;
} Vec2i;

Vec2f vec2f(float x, float y);
Vec2f vec2fs(float x);

Vec2f vec2f_add(Vec2f a, Vec2f b);
Vec2f vec2f_sub(Vec2f a, Vec2f b);
Vec2f vec2f_mul(Vec2f a, Vec2f b);
Vec2f vec2f_mul3(Vec2f a, Vec2f b, Vec2f c);
Vec2f vec2f_div(Vec2f a, Vec2f b);

Vec4f vec4f(float x, float y, float z, float w);

Vec4f vec4f_add(Vec4f a, Vec4f b);
Vec4f vec4f_sub(Vec4f a, Vec4f b);
Vec4f vec4f_mul(Vec4f a, Vec4f b);
Vec4f vec4f_div(Vec4f a, Vec4f b);

Vec2i vec2i(int x, int y);
Vec2i vec2is(int x);

Vec2i vec2i_add(Vec2i a, Vec2i b);
Vec2i vec2i_sub(Vec2i a, Vec2i b);
Vec2i vec2i_mul(Vec2i a, Vec2i b);
Vec2i vec2i_div(Vec2i a, Vec2i b);