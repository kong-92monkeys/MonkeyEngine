#version 460
#pragma shader_stage(fragment)

#include <Shaders/Textures.glsl>

layout(set = SUB_LAYER_DESC_SET_LOCATION, binding = SUB_LAYER_SAMPLER_LOCATION) uniform sampler albedoTexSampler;

layout(location = 0) in vec4 inColor;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = inColor;
}