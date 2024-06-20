module;

#include "../Vulkan/Vulkan.h"
#include <shaderc/shaderc.hpp>

export module ntmonkeys.com.Engine.Renderer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.PipelineLayout;
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
		using ShaderInfoMap = std::unordered_map<VkShaderStageFlagBits, std::string>;

		struct InitInfo
		{
		public:
			Graphics::LogicalDevice *pLogicalDevice{ };
			const AssetManager *pAssetManager{ };
		};

		Renderer() = default;
		virtual ~Renderer() noexcept override;

		void init(const InitInfo &initInfo);

	protected:
		[[nodiscard]]
		constexpr Graphics::LogicalDevice &_getLogicalDevice() const noexcept;

		[[nodiscard]]
		constexpr const AssetManager &_getAssetManager() const noexcept;

		[[nodiscard]]
		virtual std::pair<uint32_t, const VkDescriptorSetLayout *> _getDescriptorSetLayouts() const noexcept;

		[[nodiscard]]
		virtual std::pair<uint32_t, const VkPushConstantRange *> _getPushConstantRanges() const noexcept;

		[[nodiscard]]
		virtual const ShaderInfoMap &_getShaderInfoMap() const noexcept = 0;

		virtual void _onInit() = 0;

	private:
		Graphics::LogicalDevice *__pLogicalDevice{ };
		const AssetManager *__pAssetManager{ };

		std::unique_ptr<Graphics::PipelineLayout> __pPipelineLayout;
		std::unordered_map<VkShaderStageFlagBits, std::unique_ptr<Graphics::Shader>> __shaderMap;

		void __createPipelineLayout();
		void __createShaders();

		[[nodiscard]]
		std::vector<uint32_t> __readShaderFile(const std::string &assetPath) const;

		[[nodiscard]]
		static shaderc::CompileOptions __makeCopileOptions() noexcept;
	};

	constexpr Graphics::LogicalDevice &Renderer::_getLogicalDevice() const noexcept
	{
		return *__pLogicalDevice;
	}

	constexpr const AssetManager &Renderer::_getAssetManager() const noexcept
	{
		return *__pAssetManager;
	}
}

module: private;

namespace Engine
{
	Renderer::~Renderer() noexcept
	{
		__shaderMap.clear();
		__pPipelineLayout = nullptr;
	}

	void Renderer::init(const InitInfo &initInfo)
	{
		__pLogicalDevice	= initInfo.pLogicalDevice;
		__pAssetManager		= initInfo.pAssetManager;

		_onInit();

		__createPipelineLayout();
		__createShaders();
	}

	std::pair<uint32_t, const VkDescriptorSetLayout *> Renderer::_getDescriptorSetLayouts() const noexcept
	{
		return { 0U, nullptr };
	}

	std::pair<uint32_t, const VkPushConstantRange *> Renderer::_getPushConstantRanges() const noexcept
	{
		return { 0U, nullptr };
	}

	void Renderer::__createPipelineLayout()
	{
		const auto [setLayoutCount, pSetLayouts]					{ _getDescriptorSetLayouts() };
		const auto [pushConstantRangeCount, pPushConstantRanges]	{ _getPushConstantRanges() };

		__pPipelineLayout = __pLogicalDevice->createPipelineLayout(
			setLayoutCount, pSetLayouts, pushConstantRangeCount, pPushConstantRanges);
	}

	void Renderer::__createShaders()
	{
		for (const auto &[stage, filePath] : _getShaderInfoMap())
		{
			const auto code{ __readShaderFile(filePath) };
			__shaderMap[stage] = __pLogicalDevice->createShader(code.size() * sizeof(uint32_t), code.data());
		}
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