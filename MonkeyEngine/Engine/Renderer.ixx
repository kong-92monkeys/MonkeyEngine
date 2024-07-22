module;

#include "../Vulkan/Vulkan.h"
#include <shaderc/shaderc.hpp>

export module ntmonkeys.com.Engine.Renderer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Lib.AssetManager;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Graphics.Sampler;
import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.ShaderIncluder;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.FramebufferFactory;
import ntmonkeys.com.Engine.DescriptorUpdater;
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
			const Graphics::PhysicalDevice *pPhysicalDevice{ };
			Graphics::LogicalDevice *pDevice{ };
			Lib::LazyDeleter *pLazyDeleter{ };
			const Lib::AssetManager *pAssetManager{ };
			const RenderPassFactory *pRenderPassFactory{ };
			const Graphics::DescriptorSetLayout *pRenderTargetDescSetLayout{ };
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
		virtual void loadDescriptorInfos(DescriptorUpdater &updater) const noexcept;

		[[nodiscard]]
		virtual const Graphics::DescriptorSetLayout *getSubLayerDescSetLayout() const noexcept;

		[[nodiscard]]
		virtual const Graphics::PipelineLayout &getPipelineLayout() const noexcept = 0;

		virtual void begin(Graphics::CommandBuffer &commandBuffer, const BeginInfo &beginInfo) const noexcept = 0;
		virtual void end(Graphics::CommandBuffer &commandBuffer) const noexcept;

	protected:
		[[nodiscard]]
		std::shared_ptr<Graphics::DescriptorSetLayout> _createDescriptorSetLayout(
			const VkDescriptorSetLayoutCreateFlags flags, const uint32_t bindingCount,
			const VkDescriptorBindingFlags *const pBindingFlags, const VkDescriptorSetLayoutBinding *const pBindings);

		[[nodiscard]]
		std::shared_ptr<Graphics::PipelineLayout> _createPipelineLayout(
			const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
			const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges);

		[[nodiscard]]
		std::shared_ptr<Graphics::Shader> _createShader(const std::string &assetPath) const;

		[[nodiscard]]
		std::shared_ptr<Graphics::Pipeline> _createPipeline(
			const Graphics::LogicalDevice::GraphicsPipelineCreateInfo &createInfo) const;

		[[nodiscard]]
		std::shared_ptr<Graphics::Sampler> _createSampler(
			const Graphics::LogicalDevice::SamplerCreateInfo &createInfo) const;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceLimits &_getDeviceLimits() const noexcept;

		[[nodiscard]]
		const Graphics::RenderPass &_getRenderPass(const RenderPassType type) const noexcept;

		[[nodiscard]]
		constexpr const Graphics::DescriptorSetLayout &_getRenderTargetDescSetLayout() const noexcept;

		template <typename $T>
		void _lazyDelete($T &&garbage) noexcept;

		virtual void _onInit() = 0;

	private:
		const Graphics::PhysicalDevice *__pPhysicalDevice{ };
		Graphics::LogicalDevice *__pDevice{ };
		Lib::LazyDeleter *__pLazyDeleter{ };
		const Lib::AssetManager *__pAssetManager{ };
		const RenderPassFactory *__pRenderPassFactory{ };
		const Graphics::DescriptorSetLayout *__pRenderTargetDescSetLayout{ };

		[[nodiscard]]
		std::vector<uint32_t> __readShaderFile(const std::string &assetPath) const;

		[[nodiscard]]
		shaderc::CompileOptions __makeCopileOptions() const noexcept;
	};

	constexpr const VkPhysicalDeviceLimits &Renderer::_getDeviceLimits() const noexcept
	{
		return __pPhysicalDevice->get10Props().limits;
	}

	constexpr const Graphics::DescriptorSetLayout &Renderer::_getRenderTargetDescSetLayout() const noexcept
	{
		return *__pRenderTargetDescSetLayout;
	}

	template <typename $T>
	void Renderer::_lazyDelete($T &&garbage) noexcept
	{
		__pLazyDeleter->reserve(std::forward<$T>(garbage));
	}
}

module: private;

namespace Engine
{
	void Renderer::init(const InitInfo &info)
	{
		__pPhysicalDevice				= info.pPhysicalDevice;
		__pDevice						= info.pDevice;
		__pLazyDeleter					= info.pLazyDeleter;
		__pAssetManager					= info.pAssetManager;
		__pRenderPassFactory			= info.pRenderPassFactory;
		__pRenderTargetDescSetLayout	= info.pRenderTargetDescSetLayout;

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

	void Renderer::loadDescriptorInfos(DescriptorUpdater &updater) const noexcept
	{}

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

	std::shared_ptr<Graphics::DescriptorSetLayout> Renderer::_createDescriptorSetLayout(
		const VkDescriptorSetLayoutCreateFlags flags, const uint32_t bindingCount,
		const VkDescriptorBindingFlags *const pBindingFlags, const VkDescriptorSetLayoutBinding *const pBindings)
	{
		return std::shared_ptr<Graphics::DescriptorSetLayout>
		{
			__pDevice->createDescriptorSetLayout(flags, bindingCount, pBindingFlags, pBindings)
		};
	}

	std::shared_ptr<Graphics::PipelineLayout> Renderer::_createPipelineLayout(
		const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
		const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges)
	{
		return std::shared_ptr<Graphics::PipelineLayout>
		{
			__pDevice->createPipelineLayout(
				setLayoutCount, pSetLayouts,
				pushConstantRangeCount, pPushConstantRanges)
		};
	}

	std::shared_ptr<Graphics::Shader> Renderer::_createShader(const std::string &assetPath) const
	{
		const auto code{ __readShaderFile(assetPath) };
		return std::shared_ptr<Graphics::Shader>
		{
			__pDevice->createShader(code.size() * sizeof(uint32_t), code.data())
		};
	}

	std::shared_ptr<Graphics::Pipeline> Renderer::_createPipeline(const Graphics::LogicalDevice::GraphicsPipelineCreateInfo &createInfo) const
	{
		return std::shared_ptr<Graphics::Pipeline>{ __pDevice->createPipeline(createInfo) };
	}

	std::shared_ptr<Graphics::Sampler> Renderer::_createSampler(
		const Graphics::LogicalDevice::SamplerCreateInfo &createInfo) const
	{
		return std::shared_ptr<Graphics::Sampler>{ __pDevice->createSampler(createInfo) };
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