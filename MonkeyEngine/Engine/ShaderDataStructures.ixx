module;

#include "../Library/GLM.h"

export module ntmonkeys.com.Engine.ShaderDataStructures;

export namespace Engine
{
	struct InstanceInfo
	{
	public:
		glm::ivec4 materialIds{ -1, -1, -1, -1 };
		
		constexpr void reset() noexcept;
	};

	constexpr void InstanceInfo::reset() noexcept
	{
		materialIds[0] = -1;
		materialIds[1] = -1;
		materialIds[2] = -1;
		materialIds[3] = -1;
	}
}