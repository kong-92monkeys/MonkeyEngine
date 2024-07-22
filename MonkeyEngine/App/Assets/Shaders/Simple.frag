#version 460
#pragma shader_stage(fragment)

#extension GL_EXT_nonuniform_qualifier : require

#include <Shaders/SimpleCommon.glsl>
#include <Shaders/Textures.glsl>

layout(set = SUB_LAYER_DESC_SET_LOCATION, binding = SIMPLE_MATERIAL_TEX_LUT_DESC_LOCATION) buffer TextureLUTBuffer
{
    int textureLUT[];
};

layout(set = SUB_LAYER_DESC_SET_LOCATION, binding = SUB_LAYER_SAMPLER_LOCATION) uniform sampler albedoTexSampler;

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in flat uint materialIndex;
layout(location = 2) in vec4 materialColor;

layout(location = 0) out vec4 outColor;

void main()
{
    const uint albedoTexSlotIndex = ((materialIndex * SIMPLE_MATERIAL_TEXTURE_SLOT_COUNT) + SIMPLE_MATERIAL_ALBEDO_TEX_SLOT_INDEX);
    const int albedoTexIndex = textureLUT[albedoTexSlotIndex];

    outColor = materialColor;
    if (albedoTexIndex >= 0)
    {
        const vec4 texColor = texture(sampler2D(textures[albedoTexIndex], albedoTexSampler), inTexCoord);
        outColor *= texColor;
    }
}