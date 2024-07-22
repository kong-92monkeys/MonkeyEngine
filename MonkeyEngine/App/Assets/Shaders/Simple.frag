#version 460
#pragma shader_stage(fragment)

#include <Shaders/Sampler.glsl>
#include <Shaders/Textures.glsl>

layout(location = 0) in vec4 inColor;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = inColor;
}