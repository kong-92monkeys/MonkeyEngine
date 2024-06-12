module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Engine;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.DeviceInfo;

namespace Graphics
{
	export class Engine : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const DeviceInfo *pPhysicalDevice{ };
		};

		Engine(const CreateInfo &createInfo);

	private:
		const DeviceInfo &__physicalDevice;
	};
}

module: private;

namespace Graphics
{
	Engine::Engine(const CreateInfo &createInfo) :
		__physicalDevice{ *(createInfo.pPhysicalDevice) }
	{}
}