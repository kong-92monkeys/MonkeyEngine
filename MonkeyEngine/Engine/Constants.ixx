export module ntmonkeys.com.Engine.Constants;

import <cstdint>;

export namespace Engine::Constants
{
	// NOTE: Constants.glsl과 동기화 필요

	constexpr size_t DEFAULT_MEMORY_BLOCK_SIZE				{ 16ULL << 20ULL };
	constexpr size_t DEFAULT_BUFFER_BLOCK_SIZE				{ 4ULL << 20ULL };
	constexpr size_t MAX_IN_FLIGHT_FRAME_COUNT_LIMIT		{ 10ULL };

	constexpr uint32_t RENDER_TARGET_DESC_SET_LOCATION		{ 0U };
	// TODO: render target descriptors

	constexpr uint32_t RENDERER_DESC_SET_LOCATION			{ 1U };
	// TODO: renderer descriptors

	constexpr uint32_t SUB_LAYER_DESC_SET_LOCATION			{ 2U };
	constexpr uint32_t SUB_LAYER_INSTANCE_INFO_LOCATION		{ 0U };
	constexpr uint32_t SUB_LAYER_MATERIAL_DESC_LOCATION0	{ 1U };
	constexpr uint32_t SUB_LAYER_MATERIAL_DESC_LOCATION1	{ 2U };
	constexpr uint32_t SUB_LAYER_MATERIAL_DESC_LOCATION2	{ 3U };
	constexpr uint32_t SUB_LAYER_MATERIAL_DESC_LOCATION3	{ 4U };
}