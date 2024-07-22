#ifndef TEXTURES_GLSL
#define TEXTURES_GLSL

#include <Shaders/Constants.glsl>

layout(set = SUB_LAYER_DESC_SET_LOCATION, binding = SUB_LAYER_TEXTURES_LOCATION) uniform texture2D textures[];

#endif