module;

#include "../Vulkan/Vulkan.h"
#include <shaderc/shaderc.hpp>

export module ntmonkeys.com.Engine.Renderer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Shader;
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

		struct CreateInfo
		{
		public:
			Graphics::LogicalDevice *pLogicalDevice{ };
			const ShaderInfoMap *pShaderInfoMap;
		};

		explicit Renderer(const CreateInfo &createInfo);
		virtual ~Renderer() noexcept override;

	private:
		Graphics::LogicalDevice &__logicalDevice;

		std::unordered_map<VkShaderStageFlagBits, std::unique_ptr<Graphics::Shader>> __shaderMap;

		void __createShaders(const ShaderInfoMap &shaderInfoMap);

		[[nodiscard]]
		static std::vector<uint32_t> __readShaderFile(const std::string &filePath);

		[[nodiscard]]
		static shaderc::CompileOptions __makeCopileOptions() noexcept;
	};
}

module: private;

namespace Engine
{
	Renderer::Renderer(const CreateInfo &createInfo) :
		__logicalDevice{ *(createInfo.pLogicalDevice) }
	{
		__createShaders(*(createInfo.pShaderInfoMap));
	}

	Renderer::~Renderer() noexcept
	{
		__shaderMap.clear();
	}

	void Renderer::__createShaders(const ShaderInfoMap &shaderInfoMap)
	{
		for (const auto &[stage, filePath] : shaderInfoMap)
		{
			const auto code{ __readShaderFile(filePath) };
			__shaderMap[stage] = __logicalDevice.createShader(code);
		}
	}

	std::vector<uint32_t> Renderer::__readShaderFile(const std::string &filePath)
	{
		std::ifstream fin{ filePath };
		if (!fin)
			throw std::runtime_error{ std::format("Cannot open file: {}", filePath) };

		std::ostringstream oss;
		oss << fin.rdbuf();

		const auto source{ oss.str() };
		auto compileOptions{ __makeCopileOptions() };
		
		compileOptions.AddMacroDefinition("GL_KHR_variable_pointers");

		shaderc::Compiler compiler;
		const auto result
		{
			compiler.CompileGlslToSpv(
				source, shaderc_shader_kind::shaderc_glsl_infer_from_source,
				filePath.c_str(), compileOptions)
		};

		if (result.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
		{
			const auto what{ std::format(R"(Error occurred while compiling "{}". msg: "{}")", filePath, result.GetErrorMessage()) };
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