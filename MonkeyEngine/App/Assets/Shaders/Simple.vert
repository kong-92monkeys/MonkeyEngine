#version 460
#pragma shader_stage(vertex)
//? #extension GL_KHR_vulkan_glsl: enable

#include <Shaders/SimpleCommon.glsl>
#include <Shaders/ShaderDataStructures.glsl>
#include <Shaders/Materials/SimpleMaterial.glsl>

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = SUB_LAYER_INSTANCE_INFO_LOCATION) buffer InstanceInfoBuffer
{
    InstanceInfo instanceInfos[];
};

layout(std430, set = SUB_LAYER_DESC_SET_LOCATION, binding = SIMPLE_MATERIAL_DESC_LOCATION) buffer SimpleMaterialBuffer
{
    SimpleMaterial simpleMaterials[];
};

layout(location = VERTEX_ATTRIB_POS_LOCATION) in vec3 inPos;
layout(location = VERTEX_ATTRIB_TEXCOORD_LOCATION) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint materialIndex;
layout(location = 2) out vec4 materialColor;

void main()
{
    gl_Position = vec4(inPos, 1.0f);
    outTexCoord = inTexCoord;

    materialIndex = uint(instanceInfos[gl_InstanceIndex].materialIds[SIMPLE_MATERIAL_SLOT_INDEX]);
    materialColor = simpleMaterials[materialIndex].color;
}