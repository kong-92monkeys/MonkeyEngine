#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/Constants.glsl>
#include <Shaders/ShaderDataStructures.glsl>
#include <Shaders/Materials/SimpleMaterial.glsl>

const uint SIMPLE_MATERIAL_DESC_LOCATION = SUB_LAYER_MATERIAL_DESC_LOCATION0;

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = SUB_LAYER_INSTANCE_INFO_LOCATION) buffer InstanceInfoBuffer
{
    InstanceInfo instanceInfos[];
};

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = SIMPLE_MATERIAL_DESC_LOCATION) buffer SimpleMaterialBuffer
{
    SimpleMaterial simpleMaterials[];
};

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;
layout(location = VERTEX_ATTRIB_COLOR_LOCATION) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    gl_Position = vec4(inPos, 1.0f);

    const int materialIndex = instanceInfos[gl_InstanceIndex].materialIds[0];
    outColor = simpleMaterials[materialIndex].color * inColor;
}