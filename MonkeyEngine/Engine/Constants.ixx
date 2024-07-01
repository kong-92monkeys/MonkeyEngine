export module ntmonkeys.com.Engine.Constants;

import <cstdint>;

export namespace Engine::Constants
{
	constexpr size_t DEFAULT_MEMORY_BLOCK_SIZE{ 64ULL << 20ULL };
	constexpr size_t DEFAULT_BUFFER_BLOCK_SIZE{ 16ULL << 20ULL };
	constexpr size_t MAX_IN_FLIGHT_FRAME_COUNT_LIMIT{ 10ULL };
}