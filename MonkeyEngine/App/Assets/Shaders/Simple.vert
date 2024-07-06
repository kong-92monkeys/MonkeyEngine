#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/Constants.glsl>

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;
layout(location = VERTEX_ATTRIB_COLOR_LOCATION) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    gl_Position = vec4(inPos, 1.0f);
    outColor = inColor;
}