module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Texture;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.MemoryAllocator;
import ntmonkeys.com.Lib.LazyDeleter;
import <unordered_map>;
import <memory>;

namespace Engine
{
	export class Texture : public Lib::Unique
	{
	public:
		Texture(const EngineContext &context) noexcept;
		virtual ~Texture() noexcept override;

	private:
		const EngineContext &__context;
	};
}

module: private;

namespace Engine
{
	Texture::Texture(const EngineContext &context) noexcept :
		__context{ context }
	{

	}

	Texture::~Texture() noexcept
	{

	}
}