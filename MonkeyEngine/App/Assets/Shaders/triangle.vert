#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

const vec2 positions[] = vec2[]
(
    vec2(0.0f, -0.5f),
    vec2(-0.5f, 0.5f),
    vec2(0.5f, 0.5f)
);

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.5f, 1.0f);
}