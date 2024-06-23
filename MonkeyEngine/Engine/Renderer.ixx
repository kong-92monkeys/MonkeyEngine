module;

#include "../Vulkan/Vulkan.h"
#include <shaderc/shaderc.hpp>

export module ntmonkeys.com.Engine.Renderer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Engine.AssetManager;
import ntmonkeys.com.Engine.ShaderIncluder;
import <unordered_map>;
import <vector>;
import <string>;
import <memory>;
import <fstream>;
import <sstream>;
import <stdexcept>;
import <format>;

namespace Engine
{
	export class Renderer : public Lib::Unique
	{
	public:
		struct DependencyInfo
		{
		public:
			Graphics::LogicalDevice *pLogicalDevice{ };
			const AssetManager *pAssetManager{ };
		};

		Renderer() = default;
		virtual ~Renderer() noexcept override = default;

		constexpr void injectDependencies(const DependencyInfo &info) noexcept;
		virtual void init() = 0;

	protected:
		[[nodiscard]]
		std::unique_ptr<Graphics::DescriptorSetLayout> _createDescriptorSetLayout(
			const uint32_t bindingCount, const VkDescriptorSetLayoutBinding *const pBindings);

		[[nodiscard]]
		std::unique_ptr<Graphics::PipelineLayout> _createPipelineLayout(
			const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
			const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges);

		[[nodiscard]]
		std::unique_ptr<Graphics::Shader> _createShader(const std::string &assetPath) const;

		[[nodiscard]]
		std::unique_ptr<Graphics::Pipeline> _createPipeline(const Graphics::LogicalDevice::GraphicsPipelineCreateInfo &createInfo) const;

		[[nodiscard]]
		virtual const Graphics::Pipeline &_getPipeline() const noexcept = 0;

	private:
		Graphics::LogicalDevice *__pLogicalDevice{ };
		const AssetManager *__pAssetManager{ };

		[[nodiscard]]
		std::vector<uint32_t> __readShaderFile(const std::string &assetPath) const;

		[[nodiscard]]
		static shaderc::CompileOptions __makeCopileOptions() noexcept;
	};

	constexpr void Renderer::injectDependencies(const DependencyInfo &info) noexcept
	{
		__pLogicalDevice = info.pLogicalDevice;
		__pAssetManager = info.pAssetManager;
	}
}

module: private;

namespace Engine
{
	std::unique_ptr<Graphics::DescriptorSetLayout> Renderer::_createDescriptorSetLayout(
		const uint32_t bindingCount, const VkDescriptorSetLayoutBinding *const pBindings)
	{
		return __pLogicalDevice->createDescriptorSetLayout(bindingCount, pBindings);
	}

	std::unique_ptr<Graphics::PipelineLayout> Renderer::_createPipelineLayout(
		const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
		const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges)
	{
		return __pLogicalDevice->createPipelineLayout(setLayoutCount, pSetLayouts, pushConstantRangeCount, pPushConstantRanges);
	}

	std::unique_ptr<Graphics::Shader> Renderer::_createShader(const std::string &assetPath) const
	{
		const auto code{ __readShaderFile(assetPath) };
		return __pLogicalDevice->createShader(code.size() * sizeof(uint32_t), code.data());
	}

	std::unique_ptr<Graphics::Pipeline> Renderer::_createPipeline(const Graphics::LogicalDevice::GraphicsPipelineCreateInfo &createInfo) const
	{
		return __pLogicalDevice->createPipeline(createInfo);
	}

	std::vector<uint32_t> Renderer::__readShaderFile(const std::string &assetPath) const
	{
		const auto source{ __pAssetManager->readString(assetPath) };
		auto compileOptions{ __makeCopileOptions() };
		
		shaderc::Compiler compiler;
		const auto result
		{
			compiler.CompileGlslToSpv(
				source, shaderc_shader_kind::shaderc_glsl_infer_from_source,
				assetPath.c_str(), compileOptions)
		};

		if (result.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
		{
			const auto what{ std::format(R"(Error occurred while compiling "{}". msg: "{}")", assetPath, result.GetErrorMessage()) };
			throw std::runtime_error{ what };
		}

		return { result.begin(), result.end() };
	}

	shaderc::CompileOptions Renderer::__makeCopileOptions() noexcept
	{
		shaderc::CompileOptions retVal;

		retVal.SetWarningsAsErrors();
		retVal.SetPreserveBindings(true);

		retVal.SetTargetSpirv(shaderc_spirv_version::shaderc_spirv_version_1_6);
		retVal.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);
		
		retVal.SetTargetEnvironment(
			shaderc_target_env::shaderc_target_env_vulkan,
			shaderc_env_version::shaderc_env_version_vulkan_1_3);

		retVal.SetIncluder(std::unique_ptr<shaderc::CompileOptions::IncluderInterface>{ new ShaderIncluder });

		return retVal;
	}
}