module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.CommandExecutor;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.CommandBuffer;
import <vector>;
import <functional>;

namespace Engine
{
	export class CommandExecutor : public Lib::Unique
	{
	public:
		using Job = std::function<void(Graphics::CommandBuffer &)>;

		void reserve(Job &&job) noexcept;
		void execute(Graphics::CommandBuffer &commandBuffer) noexcept;

	private:
		std::vector<Job> __jobs;
	};
}

module: private;

namespace Engine
{
	void CommandExecutor::reserve(Job &&job) noexcept
	{
		__jobs.emplace_back(std::move(job));
	}

	void CommandExecutor::execute(Graphics::CommandBuffer &commandBuffer) noexcept
	{
		for (const auto &job : __jobs)
			job(commandBuffer);

		__jobs.clear();
	}
}