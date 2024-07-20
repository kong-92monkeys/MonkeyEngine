module;

#include "../Vulkan/Vulkan.h"
#include <shaderc/shaderc.hpp>

export module ntmonkeys.com.Engine.Renderer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.AssetManager;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.ShaderIncluder;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.FramebufferFactory;
import <vector>;
import <string>;
import <memory>;
import <typeindex>;
import <stdexcept>;
import <format>;
import <optional>;

namespace Engine
{
	export class Renderer : public Lib::Unique
	{
	public:
		struct InitInfo
		{
		public:
			Graphics::LogicalDevice *pDevice{ };
			const Lib::AssetManager *pAssetManager{ };
			const RenderPassFactory *pRenderPassFactory{ };
		};

		struct BeginInfo
		{
		public:
			const Graphics::ImageView *pSwapchainImageView{ };
			const VkRect2D *pRenderArea{ };
			FramebufferFactory *pFramebufferFactory{ };
		};

		Renderer() = default;
		virtual ~Renderer() noexcept override = default;

		void init(const InitInfo &info);

		[[nodiscard]]
		virtual bool isValidMaterialPack(const MaterialPack &materialPack) const noexcept;

		[[nodiscard]]
		virtual std::optional<uint32_t> getDescriptorLocationOf(const std::type_index &materialType) const noexcept;

		[[nodiscard]]
		virtual const Graphics::DescriptorSetLayout *getSubLayerDescSetLayout() const noexcept;

		[[nodiscard]]
		virtual const Graphics::PipelineLayout &getPipelineLayout() const noexcept = 0;

		virtual void begin(Graphics::CommandBuffer &commandBuffer, const BeginInfo &beginInfo) const noexcept = 0;
		virtual void end(Graphics::CommandBuffer &commandBuffer) const noexcept;

	protected:
		[[nodiscard]]
		std::unique_ptr<Graphics::DescriptorSetLayout> _createDescriptorSetLayout(
			const VkDescriptorSetLayoutCreateFlags flags,
			const uint32_t bindingCount, const VkDescriptorSetLayoutBinding *const pBindings);

		[[nodiscard]]
		std::unique_ptr<Graphics::PipelineLayout> _createPipelineLayout(
			const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
			const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges);

		[[nodiscard]]
		std::unique_ptr<Graphics::Shader> _createShader(const std::string &assetPath) const;

		[[nodiscard]]
		std::unique_ptr<Graphics::Pipeline> _createPipeline(
			const Graphics::LogicalDevice::GraphicsPipelineCreateInfo &createInfo) const;

		[[nodiscard]]
		const Graphics::RenderPass &_getRenderPass(const RenderPassType type) const noexcept;

		virtual void _onInit() = 0;

	private:
		Graphics::LogicalDevice *__pDevice{ };
		const Lib::AssetManager *__pAssetManager{ };
		const RenderPassFactory *__pRenderPassFactory{ };

		[[nodiscard]]
		std::vector<uint32_t> __readShaderFile(const std::string &assetPath) const;

		[[nodiscard]]
		shaderc::CompileOptions __makeCopileOptions() const noexcept;
	};
}

module: private;

namespace Engine
{
	void Renderer::init(const InitInfo &info)
	{
		__pDevice				= info.pDevice;
		__pAssetManager			= info.pAssetManager;
		__pRenderPassFactory	= info.pRenderPassFactory;

		_onInit();
	}

	bool Renderer::isValidMaterialPack(const MaterialPack &materialPack) const noexcept
	{
		return true;
	}

	std::optional<uint32_t> Renderer::getDescriptorLocationOf(const std::type_index &materialType) const noexcept
	{
		return std::nullopt;
	}

	const Graphics::DescriptorSetLayout *Renderer::getSubLayerDescSetLayout() const noexcept
	{
		return nullptr;
	}

	void Renderer::end(Graphics::CommandBuffer &commandBuffer) const noexcept
	{
		const VkSubpassEndInfo subpassEndInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_END_INFO }
		};

		commandBuffer.endRenderPass(subpassEndInfo);
	}

	std::unique_ptr<Graphics::DescriptorSetLayout> Renderer::_createDescriptorSetLayout(
		const VkDescriptorSetLayoutCreateFlags flags,
		const uint32_t bindingCount, const VkDescriptorSetLayoutBinding *const pBindings)
	{
		return std::unique_ptr<Graphics::DescriptorSetLayout>
		{
			__pDevice->createDescriptorSetLayout(flags, bindingCount, pBindings)
		};
	}

	std::unique_ptr<Graphics::PipelineLayout> Renderer::_createPipelineLayout(
		const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
		const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges)
	{
		return std::unique_ptr<Graphics::PipelineLayout>
		{
			__pDevice->createPipelineLayout(
				setLayoutCount, pSetLayouts,
				pushConstantRangeCount, pPushConstantRanges)
		};
	}

	std::unique_ptr<Graphics::Shader> Renderer::_createShader(const std::string &assetPath) const
	{
		const auto code{ __readShaderFile(assetPath) };
		return std::unique_ptr<Graphics::Shader>
		{
			__pDevice->createShader(code.size() * sizeof(uint32_t), code.data())
		};
	}

	std::unique_ptr<Graphics::Pipeline> Renderer::_createPipeline(const Graphics::LogicalDevice::GraphicsPipelineCreateInfo &createInfo) const
	{
		return std::unique_ptr<Graphics::Pipeline>{ __pDevice->createPipeline(createInfo) };
	}

	const Graphics::RenderPass &Renderer::_getRenderPass(const RenderPassType type) const noexcept
	{
		return __pRenderPassFactory->getInstance(type);
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

	shaderc::CompileOptions Renderer::__makeCopileOptions() const noexcept
	{
		shaderc::CompileOptions retVal;

		retVal.SetWarningsAsErrors();
		retVal.SetPreserveBindings(true);

		retVal.SetTargetSpirv(shaderc_spirv_version::shaderc_spirv_version_1_6);
		retVal.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);
		
		retVal.SetTargetEnvironment(
			shaderc_target_env::shaderc_target_env_vulkan,
			shaderc_env_version::shaderc_env_version_vulkan_1_3);

		retVal.SetIncluder(std::unique_ptr<shaderc::CompileOptions::IncluderInterface>{ new ShaderIncluder{ *__pAssetManager } });

		return retVal;
	}
}